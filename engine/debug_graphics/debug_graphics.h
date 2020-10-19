#ifndef ENGINE_DEBUG_GRAPHICS_DEBUG_GRAPHICS_H
#define ENGINE_DEBUG_GRAPHICS_DEBUG_GRAPHICS_H

#include "../renderer/renderer.h"
#include "../globals/engine_globals.h"
#include "../memory/data_array.h"
#include <string>

namespace engine
{
	struct s_debug_text : s_datum_header
	{
		D2D1_COLOR_F brush_color;
		wchar_t font_name[k_maximum_string_length];
		IDWriteFontCollection* font_collection;
		DWRITE_FONT_WEIGHT font_weight;
		DWRITE_FONT_STYLE font_style;
		DWRITE_FONT_STRETCH font_stretch;
		DWRITE_TEXT_ALIGNMENT text_alignment;
		DWRITE_PARAGRAPH_ALIGNMENT paragraph_alignment;

		float32 font_size;
		wchar_t locale_name[k_maximum_string_length];
		D2D1_POINT_2F position;

		float32 box_max_width;
		float32 box_max_height;

		wchar_t message[k_maximum_long_string_length];
		int32 actual_message_length;
	};


	class c_debug_graphics
	{
	public:

		c_debug_graphics(s_engine_globals* engine_globals);
		~c_debug_graphics();


		void draw(s_engine_globals* engine_globals);

	private:
		c_data_array<s_debug_text>* m_debug_text_array;
	};
}

#endif