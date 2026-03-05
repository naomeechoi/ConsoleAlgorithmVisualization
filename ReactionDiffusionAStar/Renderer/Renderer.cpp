#include "Renderer.h"
#include <string>
#include <Windows.h>
#include <iostream>

Renderer::Renderer(int x, int y, int width, int height)
	:x(static_cast<short>(x)), y(static_cast<short>(y)), width(width), height(height), bufferStr(std::string(width * height, ' '))
{
	CONSOLE_CURSOR_INFO info = {};
	info.dwSize = 1;
	info.bVisible = false;
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);
}

Renderer::~Renderer()
{
}


void Renderer::Summit(int x, int y, char c)
{
	bufferStr[x + width * y] = c;
}

void Renderer::SetWholeBuffer(std::string& buf)
{
	bufferStr = std::move(buf);
}

void Renderer::Draw()
{
    static HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleCursorPosition(handle, { 0, 0 }); // 항상 화면 맨 위에서 시작

    for (int i = 0; i < height; i++)
    {
        DWORD written;
        WriteConsoleA(handle, bufferStr.data() + i * width, width, &written, nullptr);
        WriteConsoleA(handle, "\r\n", 2, &written, nullptr); // 정확한 CR+LF 줄 바꿈
    }
}