#ifndef ENGINE_DEBUG_GRAPHICS_DEBUG_GRAPHICS_H
#define ENGINE_DEBUG_GRAPHICS_DEBUG_GRAPHICS_H

#include "../renderer/renderer.h"
#include "../globals/engine_globals.h"
#include "../data_structures/queue.h"
#include <string>

namespace engine
{
	struct s_debug_text
	{
		D2D1_COLOR_F brush_color;
		D2D1_POINT_2F position;
		wchar_t message[1024];
		int32 actual_message_length;
	};


	class c_debug_graphics
	{
	public:

		c_debug_graphics();
		~c_debug_graphics();

		void draw();

		void print_string(D2D1_POINT_2F position, D2D1_COLOR_F color, const wchar_t* format, ...);

		inline float get_line_spacing() { return m_font_size + 4.0f; }

	private:

		IDWriteTextFormat* m_debug_text_format;
		c_queue<s_debug_text>* m_debug_text_queue;

		const float m_font_size = 12.0f;
	};

	c_debug_graphics* const get_debug_graphics();
	void initialize_global_debug_graphics(c_debug_graphics* graphics_instance);
}

#endif