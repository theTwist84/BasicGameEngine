#include "debug_graphics.h"

namespace engine
{
	c_debug_graphics* g_debug_graphics = nullptr;

	c_debug_graphics* const get_debug_graphics() { return g_debug_graphics; }

	void initialize_global_debug_graphics(c_debug_graphics* graphics_instance)
	{
		if(g_debug_graphics == nullptr)
			g_debug_graphics = graphics_instance;
	}

	c_debug_graphics::c_debug_graphics()
	{
		m_debug_text_queue = create_new_queue<s_debug_text>(0x100, g_heap_allocator());

		IDWriteFactory* write_factory = get_engine_globals()->renderer->dwrite_factory();

		m_debug_text_format = nullptr;
		bool success = SUCCEEDED(write_factory->CreateTextFormat(L"Lucida Console", nullptr, DWRITE_FONT_WEIGHT_LIGHT, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, m_font_size, L"en-GB", &m_debug_text_format));
		success &= SUCCEEDED(m_debug_text_format->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING));
		success &= SUCCEEDED(m_debug_text_format->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR));

		if(!success)
		{
			debug_printf("Failed DWriteFactory::CreateTextFormat().\n");
			m_debug_text_format = nullptr;
		}
			

	}

	c_debug_graphics::~c_debug_graphics()
	{
		m_debug_text_queue->dispose();
		rendering::release_unknown_object(m_debug_text_format);
	}

	void c_debug_graphics::print_string(D2D1_POINT_2F position, D2D1_COLOR_F color, const wchar_t* format, ...)
	{
		s_debug_text debug_text;
		va_list va;
		va_start(va, format);
		vswprintf_s(debug_text.message, _TRUNCATE, format, va);
		va_end(va);

		debug_text.actual_message_length = (int32)wcslen(debug_text.message);
		debug_text.brush_color = color;
		debug_text.position = position;

		m_debug_text_queue->put(&debug_text);
	}

	void c_debug_graphics::draw()
	{
		if (m_debug_text_queue->is_empty())
			return;

		ID2D1DeviceContext1* d2d_device_context = get_engine_globals()->renderer->d2d_device_context();
		IDWriteFactory* write_factory = get_engine_globals()->renderer->dwrite_factory();

		d2d_device_context->BeginDraw();

		// draw text

		s_debug_text* debug_text;
		while (!m_debug_text_queue->is_empty())
		{
			ID2D1SolidColorBrush* brush = nullptr;
			IDWriteTextLayout* layout = nullptr;

			debug_text = m_debug_text_queue->front();

			if (SUCCEEDED(d2d_device_context->CreateSolidColorBrush(debug_text->brush_color, &brush)))
			{
				if (SUCCEEDED(write_factory->CreateTextLayout(debug_text->message, debug_text->actual_message_length, m_debug_text_format, 1000.0f, 1000.0f, &layout)))
				{
					d2d_device_context->DrawTextLayout(debug_text->position, layout, brush);
				}
				else
					debug_printf("Failed DWriteFactory::CreateTextLayout().\n");
			}
			else
				debug_printf("Failed ID2D1DeviceContext1::CreateSolidColorBrush().\n");

			m_debug_text_queue->pop();

			rendering::release_unknown_object(brush);
			rendering::release_unknown_object(layout);
		}

		// draw other stuff

		d2d_device_context->EndDraw();
	}
}