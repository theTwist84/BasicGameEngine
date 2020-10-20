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
		m_text_position = { 0 , 20 };
		m_last_total_time = 0.0f;
		m_debug_text_handle = get_engine_globals()->debug_graphics->register_text_entry();

		// TODO: init debug_text
		s_debug_text* text = get_engine_globals()->debug_graphics->get_text_entry(m_debug_text_handle);
		text->box_max_height = 100.0f;
		text->box_max_width = 200.0f;

		auto font_name = L"Lucida Console";
		auto locale_name = L"en-GB";
		wcsncpy_s(text->font_name, font_name, k_maximum_string_length);
		wcsncpy_s(text->locale_name, locale_name, k_maximum_string_length);

		text->font_collection = nullptr;
		text->font_weight = DWRITE_FONT_WEIGHT_LIGHT;
		text->font_style = DWRITE_FONT_STYLE_NORMAL;
		text->font_stretch = DWRITE_FONT_STRETCH_NORMAL;
		text->font_size = 12.0f;
		text->paragraph_alignment = DWRITE_PARAGRAPH_ALIGNMENT_NEAR;
		text->text_alignment = DWRITE_TEXT_ALIGNMENT_LEADING;
		text->brush_color = D2D1::ColorF(D2D1::ColorF::White);
		text->position = D2D1::Point2F(2.0f, 5.0f);
	}

	c_fps_display::~c_fps_display()
	{
		if (m_debug_text_handle != -1)
			get_engine_globals()->debug_graphics->unregister_text_entry(m_debug_text_handle);
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

		wostringstream fps_sstring;
		fps_sstring << "fps: " << m_frame_rate << " elapsed time: " << time->total_time() / 1000.0;
		wstring fps_string = fps_sstring.str();
		s_debug_text* text = get_engine_globals()->debug_graphics->get_text_entry(m_debug_text_handle);
		wcsncpy_s(text->message, fps_string.c_str(), k_maximum_long_string_length);
		text->actual_message_length = (int32)fps_string.size();
	}
}