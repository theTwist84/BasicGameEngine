#include "fps_display.h"
#include "SpriteBatch.h"
#include "SpriteFont.h"
#include <iomanip>
#include <sstream>

using namespace std;
using namespace std::literals;
using namespace DirectX;

namespace engine
{

	c_fps_display::c_fps_display()
	{
		m_frame_count = 0;
		m_frame_rate = 0;
		m_sprite_batch = nullptr;
		m_sprite_font = nullptr;
		m_text_position = { 0 , 20 };
		m_last_total_time = 0.0f;
	}

	c_fps_display::~c_fps_display()
	{
		if (m_sprite_batch != nullptr)
			delete(m_sprite_batch);

		if (m_sprite_font != nullptr)
			delete(m_sprite_font);
	}

	void c_fps_display::init(const rendering::c_renderer* const renderer)
	{
		m_sprite_batch = new SpriteBatch(renderer->d3d_device_context());
		m_sprite_font = new SpriteFont(renderer->d3d_device(), L"..\\engine\\default_content\\fonts\\arial_14_regular.spritefont");
	}

	void c_fps_display::update(const c_engine_time* time)
	{
		// count how many times this function is called (assuming 1 per frame) in 1 second record that number
		if (time->total_time() - m_last_total_time >= 1000.0)
		{
			m_last_total_time = time->total_time();
			m_frame_rate = m_frame_count;
			m_frame_count = 0;
		}
		m_frame_count++;
	}

	void c_fps_display::draw(const c_engine_time* time)
	{
		m_sprite_batch->Begin();

		wostringstream fps_label;
		fps_label << setprecision(10) << L"FPS: " << m_frame_rate << L"    Total Elapsed Time: " << time->total_time() / 1000.0;
		m_sprite_font->DrawString(m_sprite_batch, fps_label.str().c_str(), m_text_position);

		m_sprite_batch->End();
	}
}