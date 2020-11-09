#ifndef ENGINE_INPUT_INPUT_MANAGER_H
#define ENGINE_INPUT_INPUT_MANAGER_H

#include "Windows.h"
#include "../engine_definitions.h"
#include "../memory/allocator.h"
#include "keyboard.h"
#include "mouse.h"

namespace engine
{
	class c_input_manager
	{
	public:
		c_input_manager();
		~c_input_manager();

		void init(HWND m_window_handle);

		void update();

		bool process_raw_input(MSG* message);
		bool process_new_RID(MSG* message);

		inline const c_keyboard* const keyboard() { return &m_keyboard; }
		inline const c_mouse* const mouse() { return &m_mouse; }

	private:
		byte* m_input_data;
		const static byte RID_count = 2;

		c_keyboard m_keyboard;
		c_mouse m_mouse;

	};

}

#endif