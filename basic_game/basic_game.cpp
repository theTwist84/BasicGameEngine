#include "basic_game.h"
#include "engine_exception.h"
#include "renderer/renderer.h"
#include <iostream>
#include "engine_debug.h"
#include "time/engine_time.h"
#include "time/engine_clock.h"
#include "debug_ui/fps_display.h"
#include "globals/engine_globals.h"
#include "debug_graphics/debug_graphics.h"
#include "input/input_manager.h"
#include "hidusage.h"

UINT m_windowed_screen_width = 1440;
UINT m_windowed_screen_height = 810;

HWND m_window_handle;
WNDCLASSEX m_window;

using namespace engine;


void update_window_full_screen_windowed(HWND window_handle, rendering::c_renderer* renderer)
{
	int32 style = GetWindowLong(window_handle, GWL_STYLE);

	WINDOWPLACEMENT previous_placement;
	MONITORINFO monitor_info;

	memset(&previous_placement, 0, sizeof(WINDOWPLACEMENT));
	memset(&monitor_info, 0, sizeof(MONITORINFO));
	monitor_info.cbSize = sizeof(MONITORINFO);
	if (style & WS_OVERLAPPEDWINDOW)
	{
		if (GetWindowPlacement(window_handle, &previous_placement))
			if (GetMonitorInfo(MonitorFromWindow(window_handle, MONITOR_DEFAULTTOPRIMARY), &monitor_info))
			{
				SetWindowLong(window_handle, GWL_STYLE, style & ~WS_OVERLAPPEDWINDOW);
				int32 width = monitor_info.rcMonitor.right - monitor_info.rcMonitor.left;
				int32 height = monitor_info.rcMonitor.bottom - monitor_info.rcMonitor.top;

				SetWindowPos(window_handle, HWND_TOP, monitor_info.rcMonitor.left, monitor_info.rcMonitor.top,
					width,
					height,
					SWP_NOOWNERZORDER | SWP_FRAMECHANGED);

				renderer->resize_views(width, height);
			}
	}
	/*else
	{
		SetWindowLong(window_handle, GWL_STYLE, style | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(window_handle, &previous_placement);
		SetWindowPos(window_handle, nullptr, 0, 0, 0, 0, SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
	}*/
}


int WINAPI WinMain(HINSTANCE instance, HINSTANCE previous_instance, LPSTR command_line, int show_command)
{
	if (ENGINE_DEBUG)
	{
		AllocConsole();
		freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
	}
	
	int return_code = game_main(instance, previous_instance, command_line, show_command);

	exit(return_code);
}

int game_main(HINSTANCE instance, HINSTANCE previous_instance, LPSTR command_line, int show_command)
{
	debug_printf("Starting up basic_game...\n");

	std::wstring window_class_name = L"BasicGame";
	initialize_window(instance, window_class_name, L"Basic Game", show_command);


	
	rendering::c_renderer g_renderer = rendering::c_renderer(m_window_handle, true);

	

	// build settings
	engine::s_renderer_settings settings;
	memset(&settings, 0, sizeof(settings));
	settings.window_type = settings._window_type_windowed;
	settings.anti_aliasing_type = settings._anti_aliasing_off;
	settings.multi_sampling_quality_level = 1;
	settings.multi_sampling_sample_count = 1;
	settings.use_vsync = true;
	settings.width = m_windowed_screen_width;
	settings.height = m_windowed_screen_height;


	// init renderer
	if (!g_renderer.initialize(m_window_handle, &settings))
	{
		shutdown(window_class_name);
		return -1;
	}

	// init globals
	c_engine_clock clock = c_engine_clock();
	c_engine_time time = c_engine_time();
	c_input_manager input_manager = c_input_manager();

	s_engine_globals* g_engine = get_engine_globals();

	g_engine->engine_clock = &clock;
	g_engine->engine_time = &time;
	g_engine->renderer = &g_renderer;
	g_engine->input_manager = &input_manager;

	// init debug
	c_debug_graphics debug_graphics = c_debug_graphics();
	initialize_global_debug_graphics(&debug_graphics);

	// init services
	c_fps_display fps_display = c_fps_display();
	g_engine->input_manager->init(m_window_handle);


	// main loop
	bool terminate = false;
	bool skip_game_update = false;
	clock.reset();

	MSG message;
	memset(&message, 0, sizeof(message));

	while (!terminate)
	{
		skip_game_update = false;

		clock.update_engine_time(&time);
		g_renderer.clear_views();
		g_engine->input_manager->update();

		// message pump
		while (PeekMessage(&message, nullptr, 0, 0, PM_REMOVE))
		{
			if (message.message == WM_QUIT)
				terminate = true;
			else
			{
				TranslateMessage(&message);
	
				switch (message.message)
				{
				// don't handle sys keys
				case WM_SYSKEYUP:
				case WM_SYSKEYDOWN:
					continue;

				case WM_INPUT:
					g_engine->input_manager->process_raw_input(&message);
					break;
				case WM_INPUT_DEVICE_CHANGE:
					g_engine->input_manager->process_new_RID(&message);
					break;

				case WM_SIZE:
				case WM_SIZING:
					continue;
				}

				DispatchMessage(&message);
			}

		}

		if (terminate)
			break;

		if (skip_game_update)
			continue;

		if (g_engine->input_manager->mouse()->is_button_down(e_mouse_buttons::_lmb))
		{
			update_window_full_screen_windowed(m_window_handle, &g_renderer);
		}

		// update game components
		fps_display.update();

		// draw and render
		get_debug_graphics()->draw();
		g_renderer.render();
		Sleep(1000 / 60);
	}

	shutdown(window_class_name);
	return 0;
}

void initialize_window(HINSTANCE instance, const std::wstring& className, const std::wstring windowTitle, int showCommand)
{
	ZeroMemory(&m_window, sizeof(m_window));
	m_window.cbSize = sizeof(WNDCLASSEX);
	m_window.style = CS_CLASSDC;
	m_window.lpfnWndProc = WndProc;
	m_window.hInstance = instance;
	m_window.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
	m_window.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);
	m_window.hCursor = LoadCursor(nullptr, IDC_ARROW);
	m_window.hbrBackground = GetSysColorBrush(COLOR_BTNFACE);
	m_window.lpszClassName = className.c_str();

	RECT windowRectangle = { 0, 0, (long)m_windowed_screen_width, (long)m_windowed_screen_height };
	AdjustWindowRect(&windowRectangle, WS_OVERLAPPEDWINDOW, FALSE);

	RegisterClassEx(&m_window);
	POINT center = CenterWindow(m_windowed_screen_width, m_windowed_screen_height);
	m_window_handle = CreateWindow(className.c_str(), windowTitle.c_str(), WS_OVERLAPPED | WS_MINIMIZEBOX | WS_SYSMENU, center.x, center.y, windowRectangle.right - windowRectangle.left, windowRectangle.bottom - windowRectangle.top, nullptr, nullptr, instance, nullptr);

	ShowWindow(m_window_handle, showCommand);
	UpdateWindow(m_window_handle);
}

LRESULT WINAPI WndProc(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(windowHandle, message, wParam, lParam);
}

POINT CenterWindow(int windowWidth, int windowHeight)
{
	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);

	POINT center;
	center.x = (screenWidth - windowWidth) / 2;
	center.y = (screenHeight - windowHeight) / 2;

	return center;
}

void shutdown(const std::wstring& className)
{
	UnregisterClass(className.c_str(), m_window.hInstance);
}