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
#include "IO/config.h"
#include "IO/utils.h"
#include "common/camera.h"
#include <math.h>
#include "cube_demo.h"

using namespace engine;

enum window_state : byte
{
	_windowed,
	_fullscreen_windowed,
	_fullscreen
};

const std::wstring m_window_class_name = L"BasicGame";
const std::wstring m_window_title = L"Basic Game";

HWND m_window_handle;
WNDCLASSEX m_window;
byte m_current_window_state;

WINDOWPLACEMENT m_window_placement;
int window_width;
int window_height;

int shutdown_ok()
{
	UnregisterClass(m_window_class_name.c_str(), m_window.hInstance);
	return 0;
}

int shutdown_failure()
{
	UnregisterClass(m_window_class_name.c_str(), m_window.hInstance);
	return -1;
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

bool build_default_settings(s_config* const default_config)
{
	if (!default_config)
		return false;

	default_config->window_mode = _windowed;
	default_config->resolution_width = GetSystemMetrics(SM_CXSCREEN);
	default_config->resolution_height = GetSystemMetrics(SM_CYSCREEN);

	return true;
}

void load_settings(s_config* const config, const s_config* const default_config)
{
	auto dir = executable_directory();
	std::wstring file_path;
	path_join(file_path, dir, L"config.json");

	// read config always return a valid config, given a valid default_config. If it fails to read, write a valid config over.
	if (!read_config(file_path, config, default_config))
		write_config(file_path, config);

	int32 max_width = GetSystemMetrics(SM_CXSCREEN);
	int32 max_height = GetSystemMetrics(SM_CYSCREEN);

	if (config->resolution_width > max_width)
		config->resolution_width = max_width;

	if (config->resolution_height > max_height)
		config->resolution_height = max_height;


}

void set_windowed(HWND window_handle, rendering::c_renderer* renderer)
{
	m_current_window_state = _windowed;
}

void set_full_screen_windowed(HWND window_handle, rendering::c_renderer* renderer)
{
	if (m_current_window_state == _fullscreen_windowed)
		return;

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

	m_current_window_state = _fullscreen_windowed;
}

void update_window_mode(HWND window_handle, rendering::c_renderer* renderer, s_config* config)
{
	if (m_current_window_state == config->window_mode)
		return;


	int32 style = GetWindowLong(window_handle, GWL_STYLE);

	MONITORINFO monitor_info;

	memset(&monitor_info, 0, sizeof(MONITORINFO));
	monitor_info.cbSize = sizeof(MONITORINFO);

	if ((style & WS_OVERLAPPEDWINDOW) && config->window_mode == _fullscreen_windowed)
	{
		GetMonitorInfo(MonitorFromWindow(window_handle, MONITOR_DEFAULTTOPRIMARY), &monitor_info);
		SetWindowLong(window_handle, GWL_STYLE, style & ~WS_OVERLAPPEDWINDOW);
		int32 width = monitor_info.rcMonitor.right - monitor_info.rcMonitor.left;
		int32 height = monitor_info.rcMonitor.bottom - monitor_info.rcMonitor.top;

		SetWindowPos(window_handle, HWND_TOP, monitor_info.rcMonitor.left, monitor_info.rcMonitor.top,
			width,
			height,
			SWP_NOOWNERZORDER | SWP_FRAMECHANGED);

		renderer->resize_views(width, height);
	}
	else if (config->window_mode == _windowed)
	{
		SetWindowLong(window_handle, GWL_STYLE, style | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(window_handle, &m_window_placement);
		SetWindowPos(window_handle, nullptr, 0, 0, 0, 0, SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
		renderer->resize_views(config->resolution_width, config->resolution_height);
	}

	m_current_window_state = config->window_mode;
}

int game_main(HINSTANCE instance, HINSTANCE previous_instance, LPSTR command_line, int show_command)
{
	debug_printf("Starting up basic_game...\n");

	// load settings

	s_config default_config, config;
	if (!build_default_settings(&default_config))
	{
		debug_printf("Failed to generate default settings!\n");
		return -1;
	}

	load_settings(&config, &default_config);

	// initialize window
	initialize_window(instance, m_window_class_name, m_window_title, show_command);
	// hide cursor
	// ShowCursor(false);

	rendering::c_renderer g_renderer = rendering::c_renderer(m_window_handle, true);

	// build settings
	engine::s_renderer_settings settings;
	memset(&settings, 0, sizeof(settings));
	settings.window_type = settings._window_type_windowed;
	settings.anti_aliasing_type = settings._anti_aliasing_off;
	settings.multi_sampling_quality_level = 1;
	settings.multi_sampling_sample_count = 1;
	settings.use_vsync = true;

	if (m_current_window_state == _windowed)
	{
		settings.width = window_width;
		settings.height = window_height;
	}
	else
	{
		settings.width = config.resolution_width;
		settings.height = config.resolution_height;
	}
	


	// init renderer
	if (!g_renderer.initialize(m_window_handle, &settings))
	{
		return shutdown_failure();
	}

	// set window type according to setting
	update_window_mode(m_window_handle, &g_renderer, &config);


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

	// init camera
	c_camera camera = c_camera(16.0f/9.0f, 1.570796f, 0.001f, 1000.0f);
	g_engine->camera = &camera;

	XMFLOAT3 camera_position = XMFLOAT3(10, 10, 10);
	camera.set_position(&camera_position);
	camera.init();

	

	// init example
	c_cube_demo demo = c_cube_demo();

	demo.init();
	

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

		if (g_engine->input_manager->keyboard()->is_key_down(e_keyboard_keys::_escape))
		{
			terminate = true;
		}

		if (g_engine->input_manager->mouse()->is_button_down(e_mouse_buttons::_rmb))
		{
			debug_printf("Mouse button pressed!");
		}

		// process input and create transformation
		camera.apply_input_transformation();


		// update game components
		camera.update();
		fps_display.update();
		demo.update();

		// draw and render
		demo.draw();

		// debug graphics last
		get_debug_graphics()->draw();
		
		g_renderer.render();
		Sleep(1000 / 60);
	}

	return shutdown_ok();
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

	RegisterClassEx(&m_window);


	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);

	window_width = (int)(0.75 * screenWidth);
	window_height = (int)(0.75 * screenHeight);

	RECT windowRectangle = { 0, 0, window_width, window_height };
	AdjustWindowRect(&windowRectangle, WS_OVERLAPPEDWINDOW, FALSE);

	POINT center;
	center.x = (screenWidth - window_width) / 2;
	center.y = (screenHeight - window_height) / 2;

	m_window_handle = CreateWindow(className.c_str(), windowTitle.c_str(), WS_OVERLAPPED | WS_SYSMENU, center.x, center.y, windowRectangle.right - windowRectangle.left, windowRectangle.bottom - windowRectangle.top, nullptr, nullptr, instance, nullptr);
	m_current_window_state = _windowed;

	ShowWindow(m_window_handle, showCommand);
	UpdateWindow(m_window_handle);

	memset(&m_window_placement, 0, sizeof(WINDOWPLACEMENT));
	GetWindowPlacement(m_window_handle, &m_window_placement);
}
