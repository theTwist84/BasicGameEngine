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
		memset(m_current_state, 0, vk_count);

		for (int i = 0; i < vk_count; i++)
			m_current_state[i] = GetAsyncKeyState(i) >> 15;
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
	}
}