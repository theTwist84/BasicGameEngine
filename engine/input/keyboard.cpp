#include "keyboard.h"
#include "../engine_debug.h"

namespace engine
{
	c_keyboard::c_keyboard()
	{
		memset(m_current_state, 0, vk_count);
		memset(m_last_state , 0, vk_count);
	}

	c_keyboard::~c_keyboard() { }

	void c_keyboard::update()
	{
		memcpy(m_last_state, m_current_state, vk_count);
		// memset(m_current_state, 0, vk_count);
	}

	void c_keyboard::process_state(RAWKEYBOARD* const raw_keyboard)
	{
		byte virtual_key = (byte)raw_keyboard->VKey;
		bool state = (raw_keyboard->Flags & RI_KEY_BREAK) > 0 ? false : true;
		

		if (virtual_key == VK_CONTROL)
		{
			if ((raw_keyboard->Flags & RI_KEY_E0) > 0)
				virtual_key = VK_RCONTROL;
			else
				virtual_key = VK_LCONTROL;
		}
		else if (virtual_key == VK_MENU)
		{
			if ((raw_keyboard->Flags & RI_KEY_E0) > 0)
				virtual_key = VK_RMENU;
			else
				virtual_key = VK_LMENU;
		}
		
		m_current_state[virtual_key] = state;

		if (raw_keyboard->Message == WM_SYSKEYDOWN)
			raw_keyboard->Message = WM_KEYDOWN;
		else if (raw_keyboard->Message == WM_SYSKEYUP)
			raw_keyboard->Message = WM_KEYUP;

		if (state)
			debug_printf("keyboard key 0x%02x pressed\n", virtual_key);
		else
			debug_printf("keyboard key 0x%02x released\n", virtual_key);
	}

	bool c_keyboard::is_key_down(e_keyboard_keys key) const
	{
		byte key_state = m_current_state[(byte)key];
		return key_state > 0;
	}

	bool c_keyboard::was_key_down(e_keyboard_keys key) const
	{
		byte key_state = m_last_state[(byte)key];
		return key_state > 0;
	}

	bool c_keyboard::was_key_pressed_this_frame(e_keyboard_keys key) const
	{
		return is_key_down(key) && !was_key_down(key);
	}

	bool c_keyboard::was_key_released_this_frame(e_keyboard_keys key) const
	{
		return !is_key_down(key) && was_key_down(key);
	}

	bool c_keyboard::is_key_held_down(e_keyboard_keys key) const
	{
		return is_key_down(key) && was_key_down(key);
	}

	bool c_keyboard::any_key_down() const
	{
		for (int i = 0; i < vk_count; i++)
			if (is_key_down((e_keyboard_keys)i))
				return true;
		return false;
	}
}