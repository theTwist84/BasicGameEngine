#include "renderer.h"
#include <winerror.h>
#include <string>

namespace rendering
{
	void release_unknown_object(IUnknown* unknown)
	{
		if (unknown != nullptr)
		{
			unknown->Release();
			unknown = nullptr;
		}
	}

	c_renderer::c_renderer(HWND window_handle, bool debug) : m_feature_level(), m_d3d_device(nullptr), m_d3d_device_context(nullptr),
		m_debug(debug), m_window_handle(), m_swap_chain(nullptr), m_settings(), m_d3d_initialized(false),
		m_render_target_view(nullptr), m_depth_stencil_view(nullptr), m_depth_stencil_buffer(nullptr), m_initialized(false), m_dxgi_debug(nullptr)
	{
		m_background_color = { 0.3f, 0.3f, 0.3f, 1.0f };
		m_d2d_device = nullptr;
		m_d2d_device_context = nullptr;
		m_yellow_brush = nullptr;
		m_write_factory = nullptr;
		m_text_format = nullptr;
	}

	void c_renderer::init_dxgi_swap_chain_full_screen_desc(DXGI_SWAP_CHAIN_FULLSCREEN_DESC* full_screen_desc)
	{
		memset(full_screen_desc, 0, sizeof(*full_screen_desc));
		full_screen_desc->RefreshRate.Numerator = 60;
		full_screen_desc->RefreshRate.Denominator = 1;
		full_screen_desc->Windowed = true;
	}

	void c_renderer::init_dxgi_swap_chain_desc(DXGI_SWAP_CHAIN_DESC1* swap_chain_desc)
	{
		memset(swap_chain_desc, 0, sizeof(*swap_chain_desc));
		swap_chain_desc->Width = m_settings.width;
		swap_chain_desc->Height = m_settings.height;
		swap_chain_desc->Format = m_swap_chain_format;
		swap_chain_desc->Stereo = false;
		swap_chain_desc->SampleDesc.Count = m_settings.multi_sampling_sample_count;
		swap_chain_desc->SampleDesc.Quality = m_settings.multi_sampling_quality_level - 1;
		swap_chain_desc->BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swap_chain_desc->BufferCount = 1;

		swap_chain_desc->Scaling = DXGI_SCALING_STRETCH;
		swap_chain_desc->SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		swap_chain_desc->AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	}

	void c_renderer::init_viewport(D3D11_VIEWPORT* viewport)
	{
		memset(viewport, 0, sizeof(*viewport));
		viewport->TopLeftX = 0.0f;
		viewport->TopLeftY = 0.0f;
		viewport->Width = static_cast<float>(m_settings.width);
		viewport->Height = static_cast<float>(m_settings.height);
		viewport->MinDepth = 0.0f;
		viewport->MaxDepth = 1.0f;
	}

	void c_renderer::init_depth_stencil_buffer(D3D11_TEXTURE2D_DESC* texture)
	{
		memset(texture, 0, sizeof(*texture));
		texture->Width = m_settings.width;
		texture->Height = m_settings.height;
		texture->MipLevels = 1;
		texture->ArraySize = 1;
		texture->Format = m_depth_stencil_format;
		texture->BindFlags = D3D11_BIND_DEPTH_STENCIL;
		texture->Usage = D3D11_USAGE_DEFAULT;
		texture->SampleDesc.Count = m_settings.multi_sampling_sample_count;
		texture->SampleDesc.Quality = m_settings.multi_sampling_quality_level - 1;
	}

	bool c_renderer::init_d3d_device()
	{
		D3D_FEATURE_LEVEL feature_levels[] = {
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0
		};

		int32 feature_level_count = 2;

		uint32 create_device_flags = 0;
		if (m_debug)
			create_device_flags |= D3D11_CREATE_DEVICE_DEBUG;

		// to allow D2D to work
		create_device_flags |= D3D11_CREATE_DEVICE_BGRA_SUPPORT;

		ID3D11Device* d3d_device = nullptr;
		ID3D11DeviceContext* d3d_device_context = nullptr;

		bool init_success = false;

		if (SUCCEEDED(D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, create_device_flags, feature_levels, feature_level_count, D3D11_SDK_VERSION, &d3d_device, &m_feature_level, &d3d_device_context)))
		{
			if (SUCCEEDED(d3d_device->QueryInterface(__uuidof(ID3D11Device1), reinterpret_cast<void**>(&m_d3d_device))))
			{
				if (SUCCEEDED(d3d_device_context->QueryInterface(__uuidof(ID3D11DeviceContext1), reinterpret_cast<void**>(&m_d3d_device_context))))
					init_success = true;
				else
					debug_printf("Failed ID3D11DeviceContext::QueryInterface() for ID3D11DeviceContext1.\n");
			}
			else
				debug_printf("Failed ID3D11Device::QueryInterface() for  ID3D11Device1.\n");
		}	
		else
			debug_printf("Failed D3D11CreateDevice().\n");
				
		release_unknown_object(d3d_device);
		release_unknown_object(d3d_device_context);
		m_d3d_initialized = init_success;
		return init_success;
	}

	void c_renderer::set_default_settings(engine::s_renderer_settings::e_window_type window_type)
	{
		if (!m_d3d_initialized)
		{
			debug_printf("Cannot create default settings if D3D has not been initialized");
			return;
		}

		engine::s_renderer_settings default_settings;

		// TODO: build default settings
		default_settings.window_type = window_type;
		default_settings.use_vsync = true;

		default_settings.anti_aliasing_type = engine::s_renderer_settings::e_anti_aliasing_type::_anti_aliasing_off;
		default_settings.multi_sampling_sample_count = 1;
		
		m_d3d_device->CheckMultisampleQualityLevels(m_swap_chain_format, default_settings.multi_sampling_sample_count, &default_settings.multi_sampling_quality_level);

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
					output4->GetDisplayModeList1(m_screen_format, 0, &num, 0);

					DXGI_MODE_DESC1* descs = new DXGI_MODE_DESC1[num];
					output4->GetDisplayModeList1(m_screen_format, 0, &num, descs);

					if (num > 0)
					{
						DXGI_MODE_DESC1 desc = descs[0];
						default_settings.width = desc.Width;
						default_settings.height = desc.Height;
					}
				}

			}

			release_unknown_object(output);
			release_unknown_object(output4);
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

		if (!init_d3d_device())
			return false;
		
		bool init_success = false;
		IDXGIDevice3* dxgi_device = nullptr;
		IDXGIAdapter3* dxgi_adapter = nullptr;
		IDXGIFactory4* dxgi_factory = nullptr;
		if (SUCCEEDED(m_d3d_device->QueryInterface(__uuidof(IDXGIDevice3), reinterpret_cast<void**>(&dxgi_device))))
		{
			if (SUCCEEDED(dxgi_device->GetParent(__uuidof(IDXGIAdapter3), reinterpret_cast<void**>(&dxgi_adapter))))
			{
				if (SUCCEEDED(dxgi_adapter->GetParent(__uuidof(IDXGIFactory4), reinterpret_cast<void**>(&dxgi_factory))))
					init_success = true;
				else
					debug_printf("Failed IDXGIDevice3::GetParent() for IDXGIFactory4.\n");
			}
			else
				debug_printf("Failed IDXGIDevice3::GetParent() for IDXGIAdapter3.\n");
		}
		else
			debug_printf("Failed ID3D11Device1::QueryInterface() for IDXGIDevice3.\n");

		if (!init_success)
		{
			release_unknown_object(dxgi_device);
			release_unknown_object(dxgi_adapter);
			release_unknown_object(dxgi_factory);
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
		init_dxgi_swap_chain_full_screen_desc(&full_screen_desc);


		DXGI_SWAP_CHAIN_DESC1 swap_chain_desc;
		init_dxgi_swap_chain_desc(&swap_chain_desc);

		
		if (FAILED(dxgi_factory->CreateSwapChainForHwnd(dxgi_device, m_window_handle, &swap_chain_desc, &full_screen_desc, nullptr, &m_swap_chain)))
		{
			release_unknown_object(dxgi_device);
			release_unknown_object(dxgi_adapter);
			release_unknown_object(dxgi_factory);
			debug_printf("Failed in IDXGIFactory::CreateSwapChainForHwnd()\n");
			return false;
		}

		release_unknown_object(dxgi_device);
		release_unknown_object(dxgi_adapter);
		release_unknown_object(dxgi_factory);

		ID3D11Texture2D* back_buffer = nullptr;
		
		// acquire back buffer resource
		if (FAILED(m_swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&back_buffer))))
		{
			release_unknown_object(back_buffer);
			debug_printf("Failed IDXGISwapChain::GetBuffer(0)\n");
			return false;
		}
		// create resource view for render target from the back buffer
		if (FAILED(m_d3d_device->CreateRenderTargetView(back_buffer, nullptr, &m_render_target_view)))
		{
			release_unknown_object(back_buffer);
			debug_printf("Failed ID3D11Device::CreateRenderTargetView() for back buffer\n");
			return false;
		}

		release_unknown_object(back_buffer);

		// create resource depth stencil buffer

		D3D11_TEXTURE2D_DESC depth_stencil_desc;
		init_depth_stencil_buffer(&depth_stencil_desc);

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
		init_viewport(&viewport);

		m_d3d_device_context->OMSetRenderTargets(1, &m_render_target_view, m_depth_stencil_view);
		m_d3d_device_context->RSSetViewports(1, &viewport);

		// initialize D2D
		if (!c_renderer::init_d2d())
			return false;

		if (!c_renderer::init_d2d_brushes_fonts())
			return false;

		m_initialized = true;

		return true;
	}

	bool c_renderer::verify_settings(const engine::s_renderer_settings* const settings)
	{
		if (!m_d3d_initialized)
		{
			debug_printf("Cannot verify settings if D3D has not been initialized");
			return false;
		}

		if (settings == nullptr)
			return false;

		// verify super sampling, resolution, and other features

		uint32 quality_level = 0;
		m_d3d_device->CheckMultisampleQualityLevels(m_swap_chain_format, settings->multi_sampling_sample_count, &quality_level);

		if (quality_level == 0 || settings->multi_sampling_quality_level != quality_level)
		{
			debug_printf("Mutlisampling with %d samples not supported.\n", settings->multi_sampling_sample_count);
			return false;
		}

		return true;
	}

	void c_renderer::clear_views()
	{
		if (!m_d3d_initialized)
		{
			debug_printf("Cannot clear views if D3D is initialized");
			return;
		}

		m_d3d_device_context->ClearRenderTargetView(m_render_target_view, reinterpret_cast<const float*>(&m_background_color));
		m_d3d_device_context->ClearDepthStencilView(m_depth_stencil_view, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	}

	bool c_renderer::render()
	{
		if (!m_d3d_initialized)
		{
			debug_printf("Cannot present if D3D is not initialized");
			return false;
		}

		return SUCCEEDED(m_swap_chain->Present(0, 0));
	}

	c_renderer::~c_renderer()
	{
		release_unknown_object(m_swap_chain);
		release_unknown_object(m_render_target_view);
		release_unknown_object(m_depth_stencil_buffer);
		release_unknown_object(m_depth_stencil_view);
		release_unknown_object(m_dxgi_debug);


		release_unknown_object(m_yellow_brush);
		release_unknown_object(m_write_factory);
		release_unknown_object(m_text_format);

		release_unknown_object(m_d2d_device_context);
		release_unknown_object(m_d2d_device);

		if (m_d3d_device_context != nullptr)
			m_d3d_device_context->ClearState();

		release_unknown_object(m_d3d_device_context);
		release_unknown_object(m_d3d_device);
	}

	bool c_renderer::init_d2d()
	{
		bool init_success = false;
		ID2D1Factory2* d2d_factory = nullptr;
		IDXGIDevice3* dxgi_device = nullptr;

		D2D1_FACTORY_OPTIONS factory_options;
		factory_options.debugLevel = D2D1_DEBUG_LEVEL_NONE;
		if (m_debug)
			factory_options.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;

		// initialize D2D device and device context

		if (SUCCEEDED(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&m_write_factory))))
		{
			if (SUCCEEDED(D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, __uuidof(ID2D1Factory2), &factory_options, reinterpret_cast<void**>(&d2d_factory))))
			{
				
				if (SUCCEEDED(m_d3d_device->QueryInterface(__uuidof(IDXGIDevice3), reinterpret_cast<void**>(&dxgi_device))))
				{
					if (SUCCEEDED(d2d_factory->CreateDevice(dxgi_device, &m_d2d_device)))
					{
						if (SUCCEEDED(m_d2d_device->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_ENABLE_MULTITHREADED_OPTIMIZATIONS, &m_d2d_device_context)))
							init_success = true;
						else
							debug_printf("Failed ID2D1Device1::CreateDeviceContext() for ID2D1DeviceContext1.\n");
					}
					else
						debug_printf("Failed ID2D1Factory2::CreateDevice() for ID2D1Device1.\n");
				}
				else
					debug_printf("Failed ID3D11Device1::QueryInterface() for IDXGIDevice3.\n");
			}
			else
				debug_printf("Failed D2D1CreateFactory() for ID2D1Factory2.\n");
		}
		else
			debug_printf("Failed DWriteCreateFactory() for IDWriteFactory.\n");

		release_unknown_object(d2d_factory);
		release_unknown_object(dxgi_device);

		if (!init_success)
			return false;

		init_success = false;

		// initialize bitmap properties for D2D

		D2D1_BITMAP_PROPERTIES1 bitmap_properties;

		bitmap_properties.pixelFormat.format = m_swap_chain_format;
		bitmap_properties.pixelFormat.alphaMode = D2D1_ALPHA_MODE_IGNORE;
		bitmap_properties.dpiX = 96.0f;
		bitmap_properties.dpiY = 96.0f;
		bitmap_properties.bitmapOptions = D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW;
		bitmap_properties.colorContext = nullptr;
		
		IDXGISurface1* dxgi_buffer = nullptr;
		ID2D1Bitmap1* bitmap = nullptr;

		if (SUCCEEDED(m_swap_chain->GetBuffer(0, __uuidof(IDXGISurface1), reinterpret_cast<void**>(&dxgi_buffer))))
		{
			if (SUCCEEDED(m_d2d_device_context->CreateBitmapFromDxgiSurface(dxgi_buffer, &bitmap_properties, &bitmap)))
			{
				m_d2d_device_context->SetTarget(bitmap);
				init_success = true;
			}
			else
				debug_printf("Failed ID2D1DeviceContext1::CreateBitmapFromDxgiSurface() for ID2D1Bitmap1.\n");
		}
		else
			debug_printf("Failed IDXGISwapChain1::GetBuffer() for IDXGISurface1.\n");

		release_unknown_object(dxgi_buffer);
		release_unknown_object(bitmap);

		return init_success;
	}

	bool c_renderer::init_d2d_brushes_fonts()
	{
		auto debug_font = L"Lucida Console";
		IDWriteFontCollection* font_collection = nullptr;	// use system font collection
		DWRITE_FONT_WEIGHT font_weight = DWRITE_FONT_WEIGHT_LIGHT;
		DWRITE_FONT_STYLE font_style = DWRITE_FONT_STYLE_NORMAL;
		DWRITE_FONT_STRETCH font_stretch = DWRITE_FONT_STRETCH_NORMAL;
		float32 font_size = 12.0f;
		auto locale_name = L"en-GB";

		if (SUCCEEDED(m_d2d_device_context->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Yellow), &m_yellow_brush)))
		{
			if (SUCCEEDED(m_write_factory->CreateTextFormat(debug_font, font_collection, font_weight, font_style, font_stretch, font_size, locale_name, &m_text_format)))
			{
				if (SUCCEEDED(m_text_format->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING)))
				{
					if (SUCCEEDED(m_text_format->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR)))
					{
						return true;
					}
					else
						debug_printf("Failed DWriteFactory::SetParagraphAlignment().\n");
				}
				else
					debug_printf("Failed DWriteFactory::SetTextAlignment().\n");
			}
			else
				debug_printf("Failed DWriteFactory::CreateTextFormat().\n");
		}
		else
			debug_printf("Failed ID2D1DeviceContext1::CreateSolidColorBrush().\n");



		return false;
		
	}

	bool c_renderer::test_d2d()
	{
		// first, create the text layout with actual text
		std::wstring message = L"Hello there";
		IDWriteTextLayout* layout = nullptr;

		// temporary float values set, either use the window size or a predefined box size
		m_write_factory->CreateTextLayout(message.c_str(), (uint32)message.size(), m_text_format, 100.0f, 100.0f, &layout);

		// unsafe drawing, must have result checks
		m_d2d_device_context->BeginDraw();
		m_d2d_device_context->DrawTextLayout(D2D1::Point2F(2.0f, 5.0f), layout, m_yellow_brush);
		m_d2d_device_context->EndDraw();

		return true;
	}

}