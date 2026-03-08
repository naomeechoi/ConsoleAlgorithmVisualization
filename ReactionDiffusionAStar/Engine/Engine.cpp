#define NOMINMAX
#include "Engine.h"
#include "Texture/RDTexture.h"
#include "Mesh/Sphere/Sphere.h"
#include "Mesh/Cube/Cube.h"
#include "AStar/AStar.h"
#include "Input/MouseInput.h"
#include <Windows.h>
#include <stdint.h>
#include "Renderer/Renderer.h"

const int WIDTH = 120;
const int HEIGHT = 50;

Engine::Engine()
{
    renderer = new Renderer(0, 0, WIDTH, HEIGHT);
    rdTexture = new RDTexture(WIDTH, HEIGHT);
    sphere = new Sphere(WIDTH, HEIGHT, 1.0f);
    cube = new Cube(WIDTH, HEIGHT, 1.0f);
    mouseInput = new MouseInput();
}

Engine::~Engine()
{
    delete renderer;
    delete rdTexture;
    delete sphere;
    delete cube;
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
    rdTexture->Update(deltaTime);
    mouseInput->Update();

    const float SENSITIVITY = 0.01f;
    float dx = mouseInput->GetYawDelta() * SENSITIVITY;
    float dy = mouseInput->GetPitchDelta() * SENSITIVITY;

    if (dx != 0.0f || dy != 0.0f) {
        //sphere->SetRotation(-dx, dy);
        //cube->SetRotation(dx, -dy);
    }

    //mouseInput->PostUpdate();
    //sphere->SetTexture(rdTexture->GetConcentration());
    //cube->SetTexture(rdTexture->GetConcentration());
}

void Engine::Submit()
{
    auto& buf = renderer->GetBuffer();
    rdTexture->Submit(buf);
    //sphere->Submit(buf, WIDTH, HEIGHT);
    //cube->Submit(buf, WIDTH, HEIGHT);
}

void Engine::Draw()
{
    renderer->Draw();
}