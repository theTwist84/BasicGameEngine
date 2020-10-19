#include "debug_graphics.h"

namespace engine
{
	c_debug_graphics::c_debug_graphics(s_engine_globals* engine_globals)
	{
		m_debug_text_array = create_new_data_array<s_debug_text>("debug_ui_text_array", 0x100);
	}

	c_debug_graphics::~c_debug_graphics()
	{
		m_debug_text_array->dispose_data_array();
	}


	bool initialize_d2d_text_info(ID2D1DeviceContext1* d2d_device_context, IDWriteFactory* write_factory, s_debug_text* debug_text, ID2D1SolidColorBrush* brush, IDWriteTextFormat* text_format)
	{
		if (SUCCEEDED(d2d_device_context->CreateSolidColorBrush(debug_text->brush_color, &brush)))
		{
			if (SUCCEEDED(write_factory->CreateTextFormat(debug_text->font_name, debug_text->font_collection, debug_text->font_weight, debug_text->font_style, debug_text->font_stretch, debug_text->font_size, debug_text->locale_name, &text_format)))
			{
				if (SUCCEEDED(text_format->SetTextAlignment(debug_text->text_alignment)))
				{
					if (SUCCEEDED(text_format->SetParagraphAlignment(debug_text->paragraph_alignment)))
					{
						return true;
					}
					else
						debug_printf("Failed DWriteFactory::SetParagraphAlignment().\n");
				}
				else
					debug_printf("Failed DWriteFactory::SetTextAlignment().\n");
			}
			else
				debug_printf("Failed DWriteFactory::CreateTextFormat().\n");
		}
		else
			debug_printf("Failed ID2D1DeviceContext1::CreateSolidColorBrush().\n");

		return false;
	}

	void c_debug_graphics::draw(s_engine_globals* engine_globals)
	{
		c_data_array_iterator<s_debug_text> iterator = c_data_array_iterator<s_debug_text>(m_debug_text_array);
		ID2D1DeviceContext1* d2d_device_context = engine_globals->renderer->d2d_device_context();
		IDWriteFactory* write_factory = engine_globals->renderer->dwrite_factory();

		d2d_device_context->BeginDraw();

		s_debug_text* debug_text;
		while (debug_text = iterator.data_iterator_next(), debug_text != nullptr)
		{
			// TODO: init text format, layout and brush
			ID2D1SolidColorBrush* brush = nullptr;
			IDWriteTextFormat* text_format = nullptr;

			IDWriteTextLayout* layout = nullptr;

			// temporary float values set, either use the window size or a predefined box size
			write_factory->CreateTextLayout(debug_text->message, debug_text->actual_message_length, text_format, debug_text->box_max_width, debug_text->box_max_height, &layout);
			
			d2d_device_context->DrawTextLayout(debug_text->position, layout, brush);
			
		}

		d2d_device_context->EndDraw();
	}
}