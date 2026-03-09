#define NOMINMAX
#include "Engine.h"
#include "Texture/RDTexture.h"
#include "Mesh/Plane/Plane.h"
#include "Mesh/Sphere/Sphere.h"
#include "Mesh/Cube/Cube.h"
#include "AStar/AStar.h"
#include "Input/MouseInput.h"
#include <Windows.h>
#include <stdint.h>
#include "Renderer/Renderer.h"
#include <ctime>

using std::vector;
using std::string;
const int MAX_SIMULATIOIN_STEPS = 500;
const float MAX_TRANSITION_DURATION = 2.5f;
const float ASTAR_MOVE_INTERVAL = 0.1f;

static void InitialRand()
{
    static bool initialized = false;
    if (initialized)
        return;

    srand(static_cast<unsigned int>(std::time(0)));
    initialized = true;
}

static int RandomIndex(int min, int max)
{
    InitialRand();
    return min + rand() % (max - min + 1);
}

const int WIDTH = 120;
const int HEIGHT = 40;

Engine::Engine()
{
    renderer = new Renderer(0, 0, WIDTH, HEIGHT);
    rdTexture = new RDTexture(WIDTH, HEIGHT);
    plane = new Plane(WIDTH, HEIGHT);
    sphere = new Sphere(WIDTH, HEIGHT, 1.0f);
    cube = new Cube(WIDTH, HEIGHT, 1.0f);
    astar = new AStar(WIDTH, HEIGHT);
    mouseInput = new MouseInput();

    astarEndX = RandomIndex(0, WIDTH - 1);
    astarEndY = RandomIndex(0, HEIGHT - 1);
}

Engine::~Engine()
{
    delete renderer;
    delete rdTexture;
    delete plane;
    delete sphere;
    delete cube;
    delete astar;
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
        sphere->SetRotation(-dx, dy);
        //cube->SetRotation(dx, -dy);
    }

    mouseInput->PostUpdate();
    //plane->SetTexture(rdTexture->GetConcentration());
    sphere->SetTexture(rdTexture->GetConcentration());
    //cube->SetTexture(rdTexture->GetConcentration());

    /*
    if (astarX == astarEndX && astarY == astarEndY)
    {
        astarEndX = RandomIndex(0, WIDTH - 1);
        astarEndY = RandomIndex(0, HEIGHT - 1);
    }

    astarMoveTimer += deltaTime;
    if (astarMoveTimer >= ASTAR_MOVE_INTERVAL)
    {
        astarMoveTimer = 0.0f;
        std::pair<int, int> nextPos = astar->FindNextStepAStar(astarX, astarY, astarEndX, astarEndY, rdTexture->GetConcentration(), 0);
        astarX = nextPos.first;
        astarY = nextPos.second;
    }*/
}

void Engine::Submit()
{
    auto& buf = renderer->GetBuffer();
    //plane->Submit(buf);
    sphere->Submit(buf, WIDTH, HEIGHT);
    //cube->Submit(buf, WIDTH, HEIGHT);
   // astar->Submit(buf, astarX, astarY, astarEndX, astarEndY);
}

void Engine::Draw()
{
    renderer->Draw();
}