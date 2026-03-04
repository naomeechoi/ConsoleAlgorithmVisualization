#include "Engine.h"
#include <Windows.h>
#include <stdint.h>
#include "Renderer/Renderer.h"

Engine::Engine()
{
	renderer = new Renderer(10, 10, 100, 100);
}

Engine::~Engine()
{
	if (renderer)
	{
		delete renderer;
		renderer = nullptr;
	}
}

void Engine::Run()
{
	LARGE_INTEGER frequency;
	QueryPerformanceFrequency(&frequency);

	int64_t currentTime = 0;
	int64_t previousTime = 0;

	LARGE_INTEGER time;
	QueryPerformanceCounter(&time);

	currentTime = time.QuadPart;
	previousTime = currentTime;

	float oneFrameTime = 1.0f / 60.f; //TODO 하드 코딩 수정
	while (true) // TODO 종료 시점 정하기
	{
		QueryPerformanceCounter(&time);
		currentTime = time.QuadPart;

		float deltaTime
			= static_cast<float>(currentTime - previousTime);

		deltaTime = deltaTime
			/ static_cast<float>(frequency.QuadPart);

		if (deltaTime >= oneFrameTime)
		{
			Tick(deltaTime);
			Draw();
			previousTime = currentTime;
		}
	}
}

void Engine::Tick(float deltaTime)
{
}

void Engine::Draw()
{
}
