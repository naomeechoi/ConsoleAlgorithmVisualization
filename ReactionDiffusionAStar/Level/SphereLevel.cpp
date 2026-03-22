#include "SphereLevel.h"
#include "World/Level.h"
#include "Actor/Sphere.h"
#include "AlgorithmSystem/ReactionDiffusionSystem.h"
#include "Renderer/Renderer.h"
#include "AStar/AStar.h"

using namespace RenderEngine;
const float ASTAR_INTERVAL = 0.1f;

SphereLevel::SphereLevel()
{
	int width = Renderer::Get().GetScreenWidth();
	int height = Renderer::Get().GetScreenHeight();
	rdSystem = new ReactionDiffusionSystem(width, height);
	astar = new AStar(width, height);
	mouseInput = new MouseInput();
}

SphereLevel::~SphereLevel()
{
	delete rdSystem;
	delete astar;
	delete mouseInput;
}

void SphereLevel::OnExit()
{
	Level::OnExit();
}

void SphereLevel::BeginPlay()
{
	if (hasBegan)
		return;

	hasBegan = true;

	Level::BeginPlay();

	AddNewActorImmediately(new Sphere());
	for (Actor* const actor : actors)
	{
		if (!actor)
			continue;

		actor->SetTexture(rdSystem->GetConcentration());
		astar->SetHeuristic(actor->MakeHeuristic());
		astar->SetGetNextPosList(actor->MakeGetNextPosList());
		astar->SetReposition(actor->MakeGetAStarPos());
		astar->SetPosition();
	}
	astarTimer.SetTargetTime(ASTAR_INTERVAL);
}

void SphereLevel::Tick(float deltaTime)
{
	Level::Tick(deltaTime);

	if (!rdSystem)
		return;

	rdSystem->Update(deltaTime);

	mouseInput->Update();

	const float SENSITIVITY = 0.01f;
	float dx = mouseInput->GetYawDelta() * SENSITIVITY;
	float dy = mouseInput->GetPitchDelta() * SENSITIVITY;

	if (dx != 0.0f || dy != 0.0f) {
		for (Actor* const actor : actors)
		{
			if (!actor)
				continue;
			Sphere* sphere = dynamic_cast<Sphere*>(actor);
			
			if (!sphere)
				continue;

			sphere->SetRotation(-dx, dy);
		}
	}

	mouseInput->PostUpdate();
	UpdataAStarPos(deltaTime);

	for (Actor* const actor : actors)
	{
		if (!actor)
			continue;

		actor->SetTexture(rdSystem->GetConcentration());
		actor->SetAstarPos(astarPos.first, astarPos.second, astarEndPos.first, astarEndPos.second);
	}

	RepositionAStar();
}

void SphereLevel::RepositionAStar()
{
	if (astarPos.first == astarEndPos.first && astarPos.second == astarEndPos.second)
		astar->SetPosition();
}

void SphereLevel::UpdataAStarPos(float deltaTime)
{
	astarTimer.Tick(deltaTime);
	if (!astarTimer.IsTimeOut())
		return;

	astarTimer.Reset();
	astarPos = astar->FindNextStepAStar(rdSystem->GetConcentration());
	astarEndPos = astar->GetEndPos();
}

void SphereLevel::Draw()
{
	Level::Draw();
}
