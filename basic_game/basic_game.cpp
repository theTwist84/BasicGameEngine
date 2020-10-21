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

UINT mScreenWidth = 1024;
UINT mScreenHeight = 768;

HWND m_window_handle;
WNDCLASSEX mWindow;

using namespace engine;

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
	settings.width = mScreenWidth;
	settings.height = mScreenHeight;


	// init renderer
	if (!g_renderer.initialize(m_window_handle, &settings))
	{
		shutdown(window_class_name);
		return -1;
	}

	// init globals

	c_engine_clock clock = c_engine_clock();
	c_engine_time time = c_engine_time();
	s_engine_globals* g_engine = get_engine_globals();

	g_engine->engine_clock = &clock;
	g_engine->engine_time = &time;
	g_engine->renderer = &g_renderer;

	// init debug
	c_debug_graphics debug_graphics = c_debug_graphics();
	initialize_global_debug_graphics(&debug_graphics);

	// init services
	c_fps_display fps_display = c_fps_display();



	// main loop
	MSG message;
	ZeroMemory(&message, sizeof(message));

	clock.reset();

	while (message.message != WM_QUIT)
	{
		
		if (PeekMessage(&message, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&message);
			DispatchMessage(&message);
		}
		else
		{
			clock.update_engine_time(&time);

			g_renderer.clear_views();
			
			// game stuff
			fps_display.update();


			get_debug_graphics()->draw();
			g_renderer.render();
			// Sleep(1000/60);
		}
	}

	shutdown(window_class_name);
	return 0;
}

void initialize_window(HINSTANCE instance, const std::wstring& className, const std::wstring windowTitle, int showCommand)
{
	ZeroMemory(&mWindow, sizeof(mWindow));
	mWindow.cbSize = sizeof(WNDCLASSEX);
	mWindow.style = CS_CLASSDC;
	mWindow.lpfnWndProc = WndProc;
	mWindow.hInstance = instance;
	mWindow.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
	mWindow.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);
	mWindow.hCursor = LoadCursor(nullptr, IDC_ARROW);
	mWindow.hbrBackground = GetSysColorBrush(COLOR_BTNFACE);
	mWindow.lpszClassName = className.c_str();

	RECT windowRectangle = { 0, 0, (long)mScreenWidth, (long)mScreenHeight };
	AdjustWindowRect(&windowRectangle, WS_OVERLAPPEDWINDOW, FALSE);

	RegisterClassEx(&mWindow);
	POINT center = CenterWindow(mScreenWidth, mScreenHeight);
	m_window_handle = CreateWindow(className.c_str(), windowTitle.c_str(), WS_OVERLAPPEDWINDOW, center.x, center.y, windowRectangle.right - windowRectangle.left, windowRectangle.bottom - windowRectangle.top, nullptr, nullptr, instance, nullptr);

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
	UnregisterClass(className.c_str(), mWindow.hInstance);
}