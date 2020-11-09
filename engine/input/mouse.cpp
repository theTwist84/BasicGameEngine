#include "mouse.h"

#include "../engine_debug.h"
#include "windowsx.h"

namespace engine
{
	c_mouse::c_mouse()
	{
		memset(m_current_state, 0, k_number_mouse_buttons);
		memset(m_last_state, 0, k_number_mouse_buttons);
		m_x = 0;
		m_y = 0;
		m_wheel = 0;

		m_x_last = 0;
		m_y_last = 0;
		m_wheel_last = 0;
		m_x_current = 0;
		m_y_current = 0;
		m_wheel_current = 0;

		m_mouse_wheel_handle = CreateEventEx(nullptr, nullptr, CREATE_EVENT_MANUAL_RESET, EVENT_MODIFY_STATE | SYNCHRONIZE);

		if (!m_mouse_wheel_handle)
			debug_printf("Failed to create mouse wheel handle");
	}

	c_mouse::~c_mouse() {}

	void c_mouse::update()
	{
		memcpy(m_last_state, m_current_state, k_number_mouse_buttons);
		// memset(m_current_state, 0, k_number_mouse_buttons);

		m_x_last = m_x_current;
		m_y_last = m_y_current;

		//m_x = 0;
		//m_y = 0;
		//m_wheel = 0;
	}

	void c_mouse::process_state(RAWMOUSE* const raw_mouse)
	{
		if (raw_mouse->usFlags & MOUSE_MOVE_ABSOLUTE)
		{
			bool isVirtualDesktop = (raw_mouse->usFlags & MOUSE_VIRTUAL_DESKTOP) > 0;

			int32 width = GetSystemMetrics(isVirtualDesktop ? SM_CXVIRTUALSCREEN : SM_CXSCREEN);
			int32 height = GetSystemMetrics(isVirtualDesktop ? SM_CYVIRTUALSCREEN : SM_CYSCREEN);

			m_x_current = int32((raw_mouse->lLastX / 65535.0f) * width);
			m_y_current = int32((raw_mouse->lLastY / 65535.0f) * height);

			m_x = m_x_current - m_x_last;
			m_y = m_y_current - m_y_last;
		}
		else if (raw_mouse->lLastX != 0 || raw_mouse->lLastY != 0)
		{
			m_x = raw_mouse->lLastX;
			m_y = raw_mouse->lLastY;
		}

		auto button_flags = raw_mouse->usButtonFlags;

		if (button_flags & RI_MOUSE_WHEEL)
			m_wheel = (int16)raw_mouse->usButtonData;
		else
			m_wheel = 0;

		if (button_flags & RI_MOUSE_LEFT_BUTTON_DOWN)
			m_current_state[(byte)e_mouse_buttons::_lmb] = true;

		if (button_flags & RI_MOUSE_LEFT_BUTTON_DOWN)
			m_current_state[(byte)e_mouse_buttons::_lmb] = true;
		
		if (button_flags & RI_MOUSE_LEFT_BUTTON_UP)
			m_current_state[(byte)e_mouse_buttons::_lmb] = false;

		if (button_flags & RI_MOUSE_RIGHT_BUTTON_DOWN)
			m_current_state[(byte)e_mouse_buttons::_rmb] = true;

		if (button_flags & RI_MOUSE_RIGHT_BUTTON_UP)
			m_current_state[(byte)e_mouse_buttons::_rmb] = false;

		if (button_flags & RI_MOUSE_MIDDLE_BUTTON_DOWN)
			m_current_state[(byte)e_mouse_buttons::_mmb] = true;

		if (button_flags & RI_MOUSE_MIDDLE_BUTTON_UP)
			m_current_state[(byte)e_mouse_buttons::_mmb] = false;

		if (button_flags & RI_MOUSE_BUTTON_4_DOWN)
			m_current_state[(byte)e_mouse_buttons::_x_button_1] = true;

		if (button_flags & RI_MOUSE_BUTTON_4_UP)
			m_current_state[(byte)e_mouse_buttons::_x_button_1] = false;

		if (button_flags & RI_MOUSE_BUTTON_5_DOWN)
			m_current_state[(byte)e_mouse_buttons::_x_button_2] = true;

		if (button_flags & RI_MOUSE_BUTTON_5_UP)
			m_current_state[(byte)e_mouse_buttons::_x_button_2] = false;

		// debug_printf("mouse input: x: %d, y: %d, wheel : %d\n", m_x, m_y, m_wheel);
	}

	bool c_mouse::is_button_down(e_mouse_buttons button) const 
	{
		byte button_state = m_current_state[(byte)button];
		return button_state > 0;
	}

	bool c_mouse::was_button_down(e_mouse_buttons button) const
	{
		byte button_state = m_last_state[(byte)button];
		return button_state > 0;
	}

	bool c_mouse::was_button_pressed_this_frame(e_mouse_buttons button) const
	{
		return !was_button_down(button) && is_button_down(button);
	}

	bool c_mouse::was_button_released_this_frame(e_mouse_buttons button) const
	{
		return was_button_down(button) && !is_button_down(button);
	}

	bool c_mouse::is_button_held_down(e_mouse_buttons button) const
	{
		return was_button_down(button) && is_button_down(button);
	}

	bool c_mouse::any_button_down() const
	{
		for (int i = 0; i < k_number_mouse_buttons; i++)
			if (is_button_down((e_mouse_buttons)i))
				return true;
		return false;
	}
}