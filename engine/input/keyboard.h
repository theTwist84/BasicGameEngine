#ifndef __ENGINE_INPUT_KEYBOARD_H
#define __ENGINE_INPUT_KEYBOARD_H

#include "../engine_definitions.h"
#include <windows.h>

// Taken from
// https://docs.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes
//

namespace engine
{
	enum class e_keyboard_keys : byte
	{
		_back_space = VK_BACK,
		_tab = VK_TAB,
		_clear = VK_CLEAR,
		_return = VK_RETURN,
		_shift = VK_SHIFT,
		_control = VK_CONTROL,
		_menu = VK_MENU,
		_pause = VK_PAUSE,
		_capital = VK_CAPITAL,
		_escape = VK_ESCAPE,
		_space = VK_SPACE,
		_page_up = VK_PRIOR,
		_page_down = VK_NEXT,
		_end = VK_END,
		_home = VK_HOME,
		_left = VK_LEFT,
		_up = VK_UP,
		_right = VK_RIGHT,
		_down = VK_DOWN,
		_select = VK_SELECT,
		_print = VK_PRINT,
		_execute = VK_EXECUTE,
		_print_screen = VK_SNAPSHOT,
		_insert = VK_INSERT,
		_delete = VK_DELETE,
		_help = VK_HELP,
		_0 = 0x30,
		_1 = 0x31,
		_2 = 0x32,
		_3 = 0x33,
		_4 = 0x34,
		_5 = 0x35,
		_6 = 0x36,
		_7 = 0x37,
		_8 = 0x38,
		_9 = 0x39,
		_A = 0x41,
		_B = 0x42,
		_C = 0x43,
		_D = 0x44,
		_E = 0x45,
		_F = 0x46,
		_G = 0x47,
		_H = 0x48,
		_I = 0x49,
		_J = 0x4A,
		_K = 0x4B,
		_L = 0x4C,
		_M = 0x4D,
		_N = 0x4E,
		_O = 0x4F,
		_P = 0x50,
		_Q = 0x51,
		_R = 0x52,
		_S = 0x53,
		_T = 0x54,
		_U = 0x55,
		_V = 0x56,
		_W = 0x57,
		_X = 0x58,
		_Y = 0x59,
		_Z = 0x5A,
		_left_windows = VK_LWIN,
		_right_windows = VK_RWIN,
		_apps = VK_APPS,
		_sleep = VK_SLEEP,
		_num_0 = VK_NUMPAD0,
		_num_1 = VK_NUMPAD1,
		_num_2 = VK_NUMPAD2,
		_num_3 = VK_NUMPAD3,
		_num_4 = VK_NUMPAD4,
		_num_5 = VK_NUMPAD5,
		_num_6 = VK_NUMPAD6,
		_num_7 = VK_NUMPAD7,
		_num_8 = VK_NUMPAD8,
		_num_9 = VK_NUMPAD9,
		_multiply = VK_MULTIPLY,
		_add = VK_ADD,
		_separator = VK_SEPARATOR,
		_subtract = VK_SUBTRACT,
		_decimal = VK_DECIMAL,
		_divide = VK_DIVIDE,
		_F1 = VK_F1,
		_F2 = VK_F2,
		_F3 = VK_F3,
		_F4 = VK_F4,
		_F5 = VK_F5,
		_F6 = VK_F6,
		_F7 = VK_F7,
		_F8 = VK_F8,
		_F9 = VK_F9,
		_F10 = VK_F10,
		_F11 = VK_F11,
		_F12 = VK_F12,
		_F13 = VK_F13,
		_F14 = VK_F14,
		_F15 = VK_F15,
		_F16 = VK_F16,
		_F17 = VK_F17,
		_F18 = VK_F18,
		_F19 = VK_F19,
		_F20 = VK_F20,
		_F21 = VK_F21,
		_F22 = VK_F22,
		_F23 = VK_F23,
		_F24 = VK_F24,
		_num_lock = VK_NUMLOCK,
		_scroll_lock = VK_SCROLL,
		_left_shift = VK_LSHIFT,
		_right_shift = VK_RSHIFT,
		_left_control = VK_LCONTROL,
		_right_control = VK_RCONTROL,
		_semi_colon_colo = VK_OEM_1,
		_plus = VK_OEM_PLUS,
		_comma = VK_OEM_COMMA,
		_minus = VK_OEM_MINUS,
		_period = VK_OEM_PERIOD,
		_front_slash = VK_OEM_2,
		_tilda = VK_OEM_3,
		_left_bracket = VK_OEM_4,
		_back_slash = VK_OEM_5,
		_right_bracket = VK_OEM_6,
	};


	class c_keyboard
	{
	public:

		c_keyboard();
		~c_keyboard();

		void update();

		bool is_key_down(e_keyboard_keys key) const;
		bool was_key_down(e_keyboard_keys key) const;
		bool was_key_pressed_this_frame(e_keyboard_keys key) const;
		bool was_key_released_this_frame(e_keyboard_keys key) const;
		bool is_key_held_down(e_keyboard_keys key) const;

		bool any_key_down() const;

		inline bool is_key_up(e_keyboard_keys key) const { return !is_key_down(key); }
		inline bool was_key_up(e_keyboard_keys key) const { return !was_key_down(key); }

	private:

		const static int32 vk_count = 256;

		byte m_current_state[vk_count];
		byte m_last_state[vk_count];
	};
}


#endif