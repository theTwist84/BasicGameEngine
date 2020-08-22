#include "renderer.h"
#include <winerror.h>

namespace rendering
{
	void release_d3d_object(IUnknown* unknown)
	{
		if (unknown != nullptr)
			unknown->Release();
	}

	void init_dxgi_swap_chain_full_screen_desc(DXGI_SWAP_CHAIN_FULLSCREEN_DESC* full_screen_desc, const engine::s_renderer_settings* const settings)
	{
		memset(full_screen_desc, 0, sizeof(*full_screen_desc));
		full_screen_desc->RefreshRate.Numerator = 60;
		full_screen_desc->RefreshRate.Denominator = 1;
		full_screen_desc->Windowed = true;
	}

	void init_dxgi_swap_chain_desc(DXGI_SWAP_CHAIN_DESC1* swap_chain_desc, const engine::s_renderer_settings* const settings)
	{
		memset(swap_chain_desc, 0, sizeof(*swap_chain_desc));
		swap_chain_desc->Width = settings->width;
		swap_chain_desc->Height = settings->height;
		swap_chain_desc->Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swap_chain_desc->Stereo = false;
		swap_chain_desc->SampleDesc.Count = settings->multi_sampling_sample_count;
		swap_chain_desc->SampleDesc.Quality = settings->multi_sampling_quality_level - 1;
		swap_chain_desc->BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swap_chain_desc->BufferCount = 1;

		swap_chain_desc->Scaling = DXGI_SCALING_STRETCH;
		swap_chain_desc->SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		swap_chain_desc->AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	}


	c_renderer::c_renderer(HWND window_handle, bool debug) : m_feature_level(), m_d3d_device(nullptr), m_d3d_device_context(nullptr),
		m_debug(debug), m_window_handle(), m_swap_chain(nullptr), m_settings(),
		m_render_target_view(nullptr), m_depth_stencil_view(nullptr), m_depth_stencil_buffer(nullptr), m_initialized(false), m_dxgi_debug(nullptr)
	{
		m_background_color = { 0.392f, 0.584f, 0.929f, 1.0f };
	}

	void c_renderer::set_default_settings(engine::s_renderer_settings::e_window_type window_type)
	{
		engine::s_renderer_settings default_settings;

		// TODO: build default settings
		default_settings.window_type = window_type;
		default_settings.use_vsync = true;

		default_settings.anti_aliasing_type = engine::s_renderer_settings::e_anti_aliasing_type::_anti_aliasing_off;
		default_settings.multi_sampling_sample_count = 1;
		
		m_d3d_device->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, default_settings.multi_sampling_sample_count, &default_settings.multi_sampling_quality_level);

		if (default_settings.multi_sampling_quality_level == 0)
		{
			debug_printf("Mutlisampling with %d samples not supported.\n", default_settings.multi_sampling_sample_count);
			default_settings.multi_sampling_sample_count = 1;
		}


		if (window_type == engine::s_renderer_settings::e_window_type::_window_type_fullscreen_windowed || window_type == engine::s_renderer_settings::e_window_type::_window_type_windowed)
		{
			RECT window_rectangle;
			GetClientRect(m_window_handle, &window_rectangle);
			default_settings.width = window_rectangle.right - window_rectangle.left;
			default_settings.height = window_rectangle.bottom - window_rectangle.top;
		}
		else
		{
			IDXGIOutput* output = nullptr;
			IDXGIOutput4* output4 = nullptr;
			if (SUCCEEDED(m_swap_chain->GetContainingOutput(&output)))
			{
				if (SUCCEEDED(output->QueryInterface(__uuidof(IDXGIOutput4), reinterpret_cast<void**>(&output4))))
				{
					uint32 num = 0;
					output4->GetDisplayModeList1(DXGI_FORMAT_R32G32B32A32_FLOAT, 0, &num, 0);

					DXGI_MODE_DESC1* descs = new DXGI_MODE_DESC1[num];
					output4->GetDisplayModeList1(DXGI_FORMAT_R32G32B32A32_FLOAT, 0, &num, descs);

					if (num > 0)
					{
						DXGI_MODE_DESC1 desc = descs[0];
						default_settings.width = desc.Width;
						default_settings.height = desc.Height;
					}
				}

			}

			release_d3d_object(output);
			release_d3d_object(output4);
		}
		
		c_renderer::update_settings(&default_settings);
	}

	void c_renderer::get_settings(const engine::s_renderer_settings* settings)
	{
		if (settings != nullptr)
			memcpy((void*)settings , &m_settings, sizeof(*settings));
	}

	bool c_renderer::update_settings(const engine::s_renderer_settings* settings)
	{
		if (!c_renderer::verify_settings(settings))
		{
			debug_printf("s_renderer_settings failed verification.\n");
			return false;
		}
			
		
		memcpy(&m_settings, settings, sizeof(*settings));
		return true;
	}

	bool c_renderer::initialize(HWND window_handle, const engine::s_renderer_settings* const settings)
	{
		m_window_handle = window_handle;

		D3D_FEATURE_LEVEL feature_levels[] = {
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		};

		int32 feature_level_count = 4;

		uint32 create_device_flags = 0;
		if(m_debug)
			create_device_flags |= D3D11_CREATE_DEVICE_DEBUG;

		ID3D11Device* d3d_device = nullptr;
		ID3D11DeviceContext* d3d_device_context = nullptr;
		bool init_sucess = false;

		if (SUCCEEDED(D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, create_device_flags, feature_levels, feature_level_count, D3D11_SDK_VERSION, &d3d_device, &m_feature_level, &d3d_device_context)))
			if (SUCCEEDED(d3d_device->QueryInterface(__uuidof(ID3D11Device1), reinterpret_cast<void**>(&m_d3d_device))))
				if (SUCCEEDED(d3d_device_context->QueryInterface(__uuidof(ID3D11DeviceContext1), reinterpret_cast<void**>(&m_d3d_device_context))))
					init_sucess = true;

		release_d3d_object(d3d_device);
		release_d3d_object(d3d_device_context);

		if (!init_sucess)
		{
			debug_printf("Failed to initialize ID3D11Device or ID3D11DeviceContext.\n");
			return false;
		}
			

		
		init_sucess = false;
		IDXGIDevice3* dxgi_device = nullptr;
		IDXGIAdapter3* dxgi_adapter = nullptr;
		IDXGIFactory4* dxgi_factory = nullptr;
		if (SUCCEEDED(m_d3d_device->QueryInterface(__uuidof(IDXGIDevice3), reinterpret_cast<void**>(&dxgi_device))))
		{
			if (SUCCEEDED(dxgi_device->GetParent(__uuidof(IDXGIAdapter3), reinterpret_cast<void**>(&dxgi_adapter))))
			{
				if (SUCCEEDED(dxgi_adapter->GetParent(__uuidof(IDXGIFactory4), reinterpret_cast<void**>(&dxgi_factory))))
				{
					init_sucess = true;
				}
			}
		}

		if (!init_sucess)
		{
			release_d3d_object(dxgi_device);
			release_d3d_object(dxgi_adapter);
			release_d3d_object(dxgi_factory);
			debug_printf("Failed to obtain DXGI interfaces.\n");
			return false;
		}

		if (!c_renderer::update_settings(settings))
			return false;
		
		if (m_debug)
		{
			auto debug_module = GetModuleHandle(L"Dxgidebug.dll");
			typedef HRESULT(*DXGIGetDebugInterface)(REFIID riid, void** ppDebug);
			auto get_debug_interface = (DXGIGetDebugInterface)GetProcAddress(debug_module, "DXGIGetDebugInterface");
			get_debug_interface(__uuidof(IDXGIDebug), reinterpret_cast<void**>(&m_dxgi_debug));
		}

		DXGI_SWAP_CHAIN_FULLSCREEN_DESC full_screen_desc;
		init_dxgi_swap_chain_full_screen_desc(&full_screen_desc, &m_settings);


		DXGI_SWAP_CHAIN_DESC1 swap_chain_desc;
		init_dxgi_swap_chain_desc(&swap_chain_desc, &m_settings);

		
		if (FAILED(dxgi_factory->CreateSwapChainForHwnd(dxgi_device, m_window_handle, &swap_chain_desc, &full_screen_desc, nullptr, &m_swap_chain)))
		{
			release_d3d_object(dxgi_device);
			release_d3d_object(dxgi_adapter);
			release_d3d_object(dxgi_factory);
			debug_printf("Failed in IDXGIFactory::CreateSwapChainForHwnd()\n");
			return false;
		}

		release_d3d_object(dxgi_device);
		release_d3d_object(dxgi_adapter);
		release_d3d_object(dxgi_factory);


		ID3D11Texture2D* back_buffer = nullptr;
		
		// acquire back buffer resource
		if (FAILED(m_swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&back_buffer))))
		{
			release_d3d_object(back_buffer);
			debug_printf("Failed IDXGISwapChain::GetBuffer(0)\n");
			return false;
		}
		// create resource view for render target from the back buffer
		if (FAILED(m_d3d_device->CreateRenderTargetView(back_buffer, nullptr, &m_render_target_view)))
		{
			release_d3d_object(back_buffer);
			debug_printf("Failed ID3D11Device::CreateRenderTargetView() for back buffer\n");
			return false;
		}

		release_d3d_object(back_buffer);

		// create resource depth stencil buffer

		D3D11_TEXTURE2D_DESC depth_stencil_desc;
		memset(&depth_stencil_desc, 0, sizeof(depth_stencil_desc));
		depth_stencil_desc.Width = m_settings.width;
		depth_stencil_desc.Height = m_settings.height;
		depth_stencil_desc.MipLevels = 1;
		depth_stencil_desc.ArraySize = 1;
		depth_stencil_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depth_stencil_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depth_stencil_desc.Usage = D3D11_USAGE_DEFAULT;
		depth_stencil_desc.SampleDesc.Count = m_settings.multi_sampling_sample_count;
		depth_stencil_desc.SampleDesc.Quality = m_settings.multi_sampling_quality_level - 1;


		if (FAILED(m_d3d_device->CreateTexture2D(&depth_stencil_desc, nullptr, &m_depth_stencil_buffer)))
		{
			debug_printf("Failed ID3D11Device::CreateTexture2D() for depth stencil buffer\n");
			return false;
		}
			

		if (FAILED(m_d3d_device->CreateDepthStencilView(m_depth_stencil_buffer, nullptr, &m_depth_stencil_view)))
		{
			debug_printf("Failed ID3D11Device::CreateDepthStencilView()\n");
			return false;
		}
			


		D3D11_VIEWPORT viewport;
		viewport.TopLeftX = 0.0f;
		viewport.TopLeftY = 0.0f;
		viewport.Width = static_cast<float>(m_settings.width);
		viewport.Height = static_cast<float>(m_settings.height);
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;

		m_d3d_device_context->OMSetRenderTargets(1, &m_render_target_view, m_depth_stencil_view);
		m_d3d_device_context->RSSetViewports(1, &viewport);
		m_initialized = true;

		return true;
	}

	bool c_renderer::verify_settings(const engine::s_renderer_settings* const settings)
	{
		if (settings == nullptr)
			return false;

		// verify super sampling, resolution, and other features


		return true;
	}

	void c_renderer::clear_views()
	{
		m_d3d_device_context->ClearRenderTargetView(m_render_target_view, reinterpret_cast<const float*>(&m_background_color));
		m_d3d_device_context->ClearDepthStencilView(m_depth_stencil_view, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	}

	bool c_renderer::render()
	{
		return SUCCEEDED(m_swap_chain->Present(0, 0));
	}

	c_renderer::~c_renderer()
	{
		release_d3d_object(m_swap_chain);
		release_d3d_object(m_render_target_view);
		release_d3d_object(m_depth_stencil_buffer);
		release_d3d_object(m_depth_stencil_view);
		release_d3d_object(m_dxgi_debug);
		if (m_d3d_device_context != nullptr)
			m_d3d_device_context->ClearState();


		release_d3d_object(m_d3d_device_context);
		release_d3d_object(m_d3d_device);
	}
}