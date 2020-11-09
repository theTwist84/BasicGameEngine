#include "input_manager.h"
#include "../engine_debug.h"
#include "../globals/engine_globals.h"
#include "hidusage.h"

namespace engine
{



	// TODO: add class to manage this and access keyboard/ mouse classes if they exist
	// TODO: add fixed size memory for input data, allocated in the class on creation and removed on destruction

	c_input_manager::c_input_manager()
	{
		m_input_data = (byte*)g_heap_allocator()->allocate(sizeof(RAWINPUT));
		m_keyboard = c_keyboard();
		m_mouse = c_mouse();
	}

	c_input_manager::~c_input_manager()
	{
		g_heap_allocator()->free(m_input_data);
	}

	void c_input_manager::update()
	{
		m_keyboard.update();
		m_mouse.update();
	}

	void c_input_manager::init(HWND m_window_handle)
	{
		RAWINPUTDEVICE* raw_input_devices = (RAWINPUTDEVICE*)g_heap_allocator()->allocate(RID_count * sizeof(RAWINPUTDEVICE));

		RAWINPUTDEVICE* raw_keyboard = &raw_input_devices[0];
		RAWINPUTDEVICE* raw_mouse = &raw_input_devices[1];

		memset(raw_keyboard, 0, sizeof(RAWINPUTDEVICE));
		memset(raw_mouse, 0, sizeof(RAWINPUTDEVICE));

		raw_keyboard->hwndTarget = m_window_handle;
		raw_mouse->hwndTarget = m_window_handle;

		raw_keyboard->usUsage = HID_USAGE_GENERIC_KEYBOARD;
		raw_keyboard->usUsagePage = HID_USAGE_PAGE_GENERIC;

		raw_mouse->usUsage = HID_USAGE_GENERIC_MOUSE;
		raw_mouse->usUsagePage = HID_USAGE_PAGE_GENERIC;

		bool result = RegisterRawInputDevices(raw_input_devices, RID_count, sizeof(RAWINPUTDEVICE));

		g_heap_allocator()->free(raw_input_devices);
	}

	bool c_input_manager::process_raw_input(MSG* message)
	{
		if (message->message != WM_INPUT)
			return false;

		uint32 input_size;

		GetRawInputData((HRAWINPUT)message->lParam, RID_INPUT, m_input_data, &input_size, sizeof(RAWINPUTHEADER));

		RAWINPUT* raw = (RAWINPUT*)m_input_data;

		s_engine_globals* globals = get_engine_globals();

		if (raw->header.dwType == RIM_TYPEKEYBOARD)		
			m_keyboard.process_state(&raw->data.keyboard);
		else if (raw->header.dwType == RIM_TYPEMOUSE)
			m_mouse.process_state(&raw->data.mouse);
		else
			return false;

		return true;
	}

	bool c_input_manager::process_new_RID(MSG* message)
	{
		return true;
	}
}