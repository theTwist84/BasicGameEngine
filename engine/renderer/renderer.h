#ifndef ENGINE_RENDERER_RENDERER_H 
#define ENGINE_RENDERER_RENDERER_H

#include <d3d11_4.h>
#include <d2d1_3.h>
#include <dwrite_3.h>

#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <dxgidebug.h>

#include "..\engine_definitions.h"
#include "..\engine_settings.h"
#include "..\engine_debug.h"

namespace rendering
{
	void release_unknown_object(IUnknown* unknown);


	class c_renderer
	{
	public:
		c_renderer(HWND window_handle, bool debug);
		~c_renderer();

		bool update_settings(const engine::s_renderer_settings* settings);
		bool verify_settings(const engine::s_renderer_settings* settings);

		void set_default_settings(engine::s_renderer_settings::e_window_type window_type);
		void get_settings(const engine::s_renderer_settings* settings);

		bool initialize(HWND window_handle, const engine::s_renderer_settings* const settings);
		void clear_views();
		bool render();

		bool resize_views(int32 width, int32 height);

		DirectX::XMVECTORF32 m_background_color;

		inline ID3D11Device1* d3d_device() const { return m_d3d_device; }
		inline ID2D1Device1* d2d_device() const { return m_d2d_device; }
		inline ID3D11DeviceContext1* d3d_device_context() const { return m_d3d_device_context; }
		inline ID2D1DeviceContext1* d2d_device_context() const { return m_d2d_device_context; }
		inline IDWriteFactory* dwrite_factory() const { return m_write_factory; }

	private:

		const bool m_debug;
		bool m_initialized;
		bool m_d3d_initialized;
		const DXGI_FORMAT m_swap_chain_format = DXGI_FORMAT_R8G8B8A8_UNORM;
		const DXGI_FORMAT m_depth_stencil_format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		const DXGI_FORMAT m_screen_format = DXGI_FORMAT_R32G32B32A32_FLOAT;

		engine::s_renderer_settings m_settings;

		HWND m_window_handle;
		D3D_FEATURE_LEVEL m_feature_level;
		ID3D11Device1* m_d3d_device;
		ID3D11DeviceContext1* m_d3d_device_context;
		IDXGISwapChain1* m_swap_chain;
		ID3D11Texture2D* m_depth_stencil_buffer;
		ID3D11DepthStencilView* m_depth_stencil_view;
		ID3D11RenderTargetView* m_render_target_view;
		IDXGIDebug* m_dxgi_debug;

		ID2D1Device1* m_d2d_device;
		ID2D1DeviceContext1* m_d2d_device_context;

		IDWriteFactory* m_write_factory;


		void init_dxgi_swap_chain_full_screen_desc(DXGI_SWAP_CHAIN_FULLSCREEN_DESC* full_screen_desc);
		void init_dxgi_swap_chain_desc(DXGI_SWAP_CHAIN_DESC1* swap_chain_desc);
		void init_viewport(D3D11_VIEWPORT* viewport);
		void init_depth_stencil_buffer(D3D11_TEXTURE2D_DESC* texture);
		void init_d2d_bitmap(D2D1_BITMAP_PROPERTIES1* const bitmap_properties);
		bool init_d3d_device();

		bool init_d2d();
	};

	void release_unknown_object(IUnknown* unknown);

}


#endif