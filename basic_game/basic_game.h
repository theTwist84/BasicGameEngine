#ifndef BASIC_GAME_H
#define BASIC_GAME_H

#include <windows.h>
#include <string>


int game_main(HINSTANCE instance, HINSTANCE previousInstance, LPSTR commandLine, int showCommand);

void initialize_window(HINSTANCE instance, const std::wstring& className, const std::wstring windowTitle, int showCommand);

#endif