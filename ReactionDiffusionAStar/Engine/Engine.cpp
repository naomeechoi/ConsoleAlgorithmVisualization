#include "Engine.h"
#include "Maze/Maze.h"
#include <Windows.h>
#include <stdint.h>
#include "Renderer/Renderer.h"

const int WIDTH = 70;
const int HEIGHT = 30;

using std::string;

Engine::Engine()
{
	renderer = new Renderer(0, 0, WIDTH, HEIGHT);
	maze = new Maze(WIDTH, HEIGHT);
}

Engine::~Engine()
{
	if (renderer)
	{
		delete renderer;
		renderer = nullptr;
	}

	if (maze)
	{
		delete maze;
		maze = nullptr;
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

	float oneFrameTime = 1.0f / 200.f; //TODO 하드 코딩 수정
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
			Submit();
			Draw();
			previousTime = currentTime;
		}
	}
}

void Engine::Tick(float deltaTime)
{
	maze->Upadate(deltaTime);
}

void Engine::Submit()
{
	string buf = std::move(renderer->GetBuffer());
	maze->Submit(buf);
	renderer->SetWholeBuffer(buf);
}

void Engine::Draw()
{
	renderer->Draw();
}
