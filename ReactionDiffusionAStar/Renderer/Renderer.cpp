#include "Renderer.h"
#include <string>
#include <Windows.h>
#include <iostream>

Renderer::Renderer(int x, int y, int width, int height)
	:x(static_cast<short>(x)), y(static_cast<short>(y)), width(width), height(height), bufferStr(std::string((width + 1)* height, ' '))
{
}

Renderer::~Renderer()
{
}


void Renderer::Summit(int x, int y, char c)
{
	bufferStr[x + (width + 1) * y] = c;
}

void Renderer::Draw()
{
	static HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleCursorPosition(handle, COORD{ x,y });
	std::cout << bufferStr;
}