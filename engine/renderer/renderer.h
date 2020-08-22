#ifndef ENGINE_RENDERER_RENDERER_H 
#define ENGINE_RENDERER_RENDERER_H

#include <d3d11_4.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <dxgidebug.h>

#include "..\engine_definitions.h"
#include "..\engine_settings.h"
#include "..\engine_debug.h"

namespace rendering
{
	void release_d3d_object(IUnknown* unknown);


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

		DirectX::XMVECTORF32 m_background_color;
	protected:

		const bool m_debug;
		bool m_initialized;
		
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

	};

}


#endif