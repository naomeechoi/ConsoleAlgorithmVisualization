#include "Renderer.h"
#include <string>
#include <Windows.h>
#include <iostream>
#include <cassert>

namespace RenderEngine
{
    Renderer* Renderer::instance = nullptr;

    Renderer::Renderer(int width, int height)
        : width(width), height(height)
    {
        assert(!instance);
        instance = this;

        HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);
        DWORD mode = 0;
        GetConsoleMode(hInput, &mode);

        mode &= ~ENABLE_QUICK_EDIT_MODE;   // QuickEdit 끄기
        mode &= ~ENABLE_INSERT_MODE;       // Insert 끄기
        mode |= ENABLE_MOUSE_INPUT;        // 마우스 입력 활성화
        SetConsoleMode(hInput, mode);

        CONSOLE_CURSOR_INFO info = {};
        info.dwSize = 1;
        info.bVisible = false;
        SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);
        buffer = std::vector<CHAR_INFO>(width * height);
    }

    Renderer::~Renderer()
    {
    }


    void Renderer::Submit(std::vector<CHAR_INFO> finalBuffer)
    {
        buffer = std::move(finalBuffer);
    }

    void Renderer::Draw()
    {
        HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);

        COORD bufferSize = { (short)width, (short)height };
        COORD bufferCoord = { 0,0 };

        SMALL_RECT region;
        region.Left = 0;
        region.Top = 0;
        region.Right = width - 1;
        region.Bottom = height - 1;

        WriteConsoleOutputA(
            handle,
            buffer.data(),
            bufferSize,
            bufferCoord,
            &region
        );
    }

    Renderer& Renderer::Get()
    {
        assert(instance);
        return *instance;
    }
}