#define NOMINMAX
#include "Engine.h"
#include "Maze/Maze.h"
#include "Sphere/Sphere.h"
#include "Input/MouseInput.h"
#include <Windows.h>
#include <stdint.h>
#include "Renderer/Renderer.h"

const int WIDTH = 120;
const int HEIGHT = 50;

Engine::Engine()
{
    renderer = new Renderer(0, 0, WIDTH, HEIGHT);
    maze = new Maze(WIDTH, HEIGHT);
    sphere = new Sphere(WIDTH, HEIGHT, 1.0f); // radius = 20
    mouseInput = new MouseInput();
}

Engine::~Engine()
{
    delete renderer;
    delete maze;
    delete sphere;
    delete mouseInput;
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

    float oneFrameTime = 1.0f / 60.f;

    while (true)
    {
        QueryPerformanceCounter(&time);
        currentTime = time.QuadPart;

        float deltaTime = static_cast<float>(currentTime - previousTime) / static_cast<float>(frequency.QuadPart);

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
    maze->Update(deltaTime);
    mouseInput->Update();

    const float SENSITIVITY = 0.01f;
    float dx = mouseInput->GetYawDelta() * SENSITIVITY;
    float dy = mouseInput->GetPitchDelta() * SENSITIVITY;

    if (dx != 0.0f || dy != 0.0f) {
        sphere->SetRotation(-dx, dy);
    }

    mouseInput->PostUpdate();
    sphere->SetTexture(maze->GetConcentration());
}

void Engine::Submit()
{
    auto& buf = renderer->GetBuffer();
    int sphereSize = std::min(WIDTH, HEIGHT);
    sphere->Submit(buf, WIDTH, HEIGHT);
}

void Engine::Draw()
{
    renderer->Draw();
}