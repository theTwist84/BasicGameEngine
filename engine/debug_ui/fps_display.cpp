#include "fps_display.h"
#include <iomanip>
#include <sstream>
#include "../debug_graphics/debug_graphics.h"

using namespace std;
using namespace std::literals;
using namespace DirectX;

namespace engine
{

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
		// count how many times this function is called (assuming 1 per frame) in 1 second record that number
		if (time->total_time() - m_last_total_time >= 1000.0)
		{
			m_last_total_time = time->total_time();
			m_frame_rate = m_frame_count;
			m_frame_count = 0;
		}
		m_frame_count++;

		get_debug_graphics()->print_string(D2D1::ColorF(D2D1::ColorF::Yellow), L"fps: %d elapsed  time: %.3f", m_frame_rate, time->total_time() / 1000);
	}
}