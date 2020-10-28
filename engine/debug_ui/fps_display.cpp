#include "fps_display.h"
#include <iomanip>
#include <sstream>
#include "../debug_graphics/debug_graphics.h"

using namespace std;
using namespace std::literals;
using namespace DirectX;

namespace engine
{
	D2D1::ColorF get_fps_display_color(int32 framerate)
	{
		if (framerate < 30)
			return D2D1::ColorF(D2D1::ColorF::Red);
		else if (framerate < 60)
			return D2D1::ColorF(D2D1::ColorF::Orange);
		else
			return D2D1::ColorF(D2D1::ColorF::LimeGreen);
	}


	c_fps_display::c_fps_display()
	{
		m_frame_count = 0;
		m_frame_rate = 0;
		m_last_total_time = 0.0f;
	}

	c_fps_display::~c_fps_display()
	{
	
	}


	void c_fps_display::update()
	{
		c_engine_time* time = get_engine_globals()->engine_time;

		// count how many times this is called and extrapolate frame rate
		if (time->total_time() - m_last_total_time >= 250.0)
		{
			m_last_total_time = time->total_time();
			m_frame_rate = m_frame_count * 4;
			m_frame_count = 0;
		}
		m_frame_count++;

		get_debug_graphics()->print_string(get_fps_display_color(m_frame_rate), L"FPS: %d", m_frame_rate);
		get_debug_graphics()->print_string(D2D1::ColorF(D2D1::ColorF::White), L"Elapsed  time: %.3f", time->total_time() / 1000);
	}
}