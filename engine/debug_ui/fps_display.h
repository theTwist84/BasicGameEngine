#ifndef ENGINE_DEBUG_UI_FPS_DISPLAY_H
#define ENGINE_DEBUG_UI_FPS_DISPLAY_H

#include "../time/engine_time.h"
#include "../renderer/renderer.h"
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

		void init(const rendering::c_renderer* const renderer);
		void update(const c_engine_time* time);
		void draw(const c_engine_time* time);
		

	private:
		DirectX::SpriteBatch* m_sprite_batch;
		DirectX::SpriteFont* m_sprite_font;

		DirectX::XMFLOAT2 m_text_position;

		int m_frame_count;
		int m_frame_rate;
		double m_last_total_time;

	};


}


#endif
