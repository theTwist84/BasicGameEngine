#pragma once

#include <windows.h>
#include <exception>
#include <cassert>
#include <string>
#include <vector>
#include <map>
#include <stack>
#include <memory>

#include <d3d11_1.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

#define DeleteObject(object) if((object) != nullptr) { delete object; object = nullptr; }
#define DeleteObjects(objects) if((objects) != nullptr) { delete[] objects; objects = nullptr; }
#define ReleaseObject(object) if((object) != nullptr) { object->Release(); object = nullptr; }

namespace engine
{
    typedef unsigned char byte;
}

using namespace DirectX;
using namespace DirectX::PackedVector;

int game_main(HINSTANCE instance, HINSTANCE previousInstance, LPSTR commandLine, int showCommand);


void initialize_window(HINSTANCE instance, const std::wstring& className, const std::wstring windowTitle, int showCommand);
LRESULT WINAPI WndProc(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam);
POINT CenterWindow(int windowWidth, int windowHeight);
void shutdown(const std::wstring& className);