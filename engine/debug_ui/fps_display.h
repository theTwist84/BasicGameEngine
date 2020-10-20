#ifndef ENGINE_DEBUG_UI_FPS_DISPLAY_H
#define ENGINE_DEBUG_UI_FPS_DISPLAY_H

#include "../time/engine_time.h"
#include "../renderer/renderer.h"
#include "../globals/engine_globals.h"
#include "../data_structures/data_array.h"
#include <DirectXMath.h>

namespace DirectX
{
	class SpriteBatch;
	class SpriteFont;
}

namespace engine
{
	class c_fps_display final
	{
	public:

		c_fps_display();
		~c_fps_display();

		void update();
		

	private:
		DirectX::XMFLOAT2 m_text_position;

		int m_frame_count;
		int m_frame_rate;
		double m_last_total_time;
		datum_handle m_debug_text_handle;
	};


}


#endif
