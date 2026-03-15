#define NOMINMAX
#include "Engine.h"
#include "AlgorithmSystem/ReactionDiffusionSystem.h"
#include "Texture/PlaneTexture.h"
#include "Texture/Sphere.h"
#include "Texture/Cube.h"
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
    rdTexture = new ReactionDiffusionSystem(WIDTH, HEIGHT);
    //plane = new PlaneMesh(WIDTH, HEIGHT);
    sphere = new Sphere(WIDTH, HEIGHT, 1.0f);
    cube = new Cube(WIDTH, HEIGHT, 1.0f);
    astar = new AStar(WIDTH, HEIGHT);
    mouseInput = new MouseInput();

    //std::pair<int, int> startPos = GetRandomCubePos();
    //astarX = startPos.first;
    //astarY = startPos.second;

    //std::pair<int, int> endPos = GetRandomCubePos();
    //astarEndX = endPos.first;
    //astarEndY = endPos.second;
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

/*
std::pair<int, int> Engine::GetRandomCubePos()
{
    int x, y;
    do {
        x = RandomIndex(0, WIDTH - 1);
        y = RandomIndex(0, HEIGHT - 1);
    } while (!astar->IsCubeAtlasValid(x, y));
    return { x, y };
}*/

void Engine::Tick(float deltaTime)
{
    rdTexture->Update(deltaTime);
    mouseInput->Update();

    const float SENSITIVITY = 0.01f;
    float dx = mouseInput->GetYawDelta() * SENSITIVITY;
    float dy = mouseInput->GetPitchDelta() * SENSITIVITY;

    if (dx != 0.0f || dy != 0.0f) {
        //sphere->SetRotation(-dx, dy);
        cube->SetRotation(dx, -dy);
    }

    mouseInput->PostUpdate();
    //plane->SetTexture(rdTexture->GetConcentration());
    //sphere->SetTexture(rdTexture->GetConcentration());
    cube->SetTexture(rdTexture->GetConcentration());

    
    if (astarX == astarEndX && astarY == astarEndY)
    {
        //std::pair<int, int> endPos = GetRandomCubePos();
        //astarEndX = endPos.first;
        //astarEndY = endPos.second;
    }

    astarMoveTimer += deltaTime;
    if (astarMoveTimer >= ASTAR_MOVE_INTERVAL)
    {
        //astarMoveTimer = 0.0f;
        //rdTexture->TestBufferClear();
       // std::pair<int, int> nextPos = astar->FindNextStepAStar(astarX, astarY, astarEndX, astarEndY, rdTexture->GetConcentration());
        //astarX = nextPos.first;
        //astarY = nextPos.second;
    }
}

void Engine::Submit()
{
    //auto& buf = renderer->GetBuffer();
    //plane->Submit(buf);
    //sphere->Submit(buf, WIDTH, HEIGHT);
    //cube->Submit(buf, WIDTH, HEIGHT, astarX, astarY, astarEndX, astarEndY);
    //astar->SubmitOnCube(buf, cube, astarX, astarY, astarEndX, astarEndY);
    //astar->SubmitOnSphere(buf, sphere->GetRadius(), sphere->GetRotation(), astarX, astarY, astarEndX, astarEndY);
}

void Engine::Draw()
{
    //renderer->Draw();
}