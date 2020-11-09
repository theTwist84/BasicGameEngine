#ifndef __ENGINE_INPUT_MOUSE_H
#define __ENGINE_INPUT_MOUSE_H

#include "../engine_definitions.h"
#include "windows.h"

namespace engine
{
	enum class e_mouse_buttons : byte
	{
		_lmb,
		_rmb,
		_mmb,
		_x_button_1,
		_x_button_2,
		_button_count,
	};


	class c_mouse
	{
	public:
		c_mouse();
		~c_mouse();

		void process_state(RAWMOUSE* const raw_mouse);
		void update();

		bool is_button_down(e_mouse_buttons button) const;
		bool was_button_down(e_mouse_buttons button) const;
		bool was_button_pressed_this_frame(e_mouse_buttons button) const;
		bool was_button_released_this_frame(e_mouse_buttons button) const;
		bool is_button_held_down(e_mouse_buttons button) const;
		bool any_button_down() const;

		inline bool is_button_up(e_mouse_buttons button) const { return !is_button_down(button); }
		inline bool was_button_up(e_mouse_buttons button) const { return !was_button_down(button); }

		inline int32 x() const { return m_x; }
		inline int32 y() const { return m_y; }
		inline int32 wheel() const { return m_wheel; }

	private:

		const static int32 k_number_mouse_buttons = (int32)e_mouse_buttons::_button_count;

		bool m_current_state[k_number_mouse_buttons];
		bool m_last_state[k_number_mouse_buttons];

		int32 m_x;
		int32 m_y;
		int32 m_wheel;

		int32 m_x_last;
		int32 m_y_last;
		int32 m_wheel_last;
		int32 m_x_current;
		int32 m_y_current;
		int32 m_wheel_current;

		HANDLE m_mouse_wheel_handle;

	};
}

#endif
