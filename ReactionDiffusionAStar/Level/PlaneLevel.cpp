#include "PlaneLevel.h"
#include "World/Level.h"
#include "Actor/Plane.h"
#include "AlgorithmSystem/ReactionDiffusionSystem.h"
#include "Renderer/Renderer.h"
#include "AStar/AStar.h"

using namespace RenderEngine;
const float ASTAR_INTERVAL = 0.1f;

PlaneLevel::PlaneLevel()
{
	int width = Renderer::Get().GetScreenWidth();
	int height = Renderer::Get().GetScreenHeight();
	rdSystem = new ReactionDiffusionSystem(width, height);
	astar = new AStar(width, height);
}

PlaneLevel::~PlaneLevel()
{
	delete rdSystem;
	delete astar;
}

void PlaneLevel::OnExit()
{
	Level::OnExit();
}

void PlaneLevel::BeginPlay()
{
	if (hasBegan)
		return;

	hasBegan = true;

	Level::BeginPlay();

	AddNewActorImmediately(new Plane());
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

void PlaneLevel::Tick(float deltaTime)
{
	Level::Tick(deltaTime);

	if (!rdSystem)
		return;

	rdSystem->Update(deltaTime);
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

void PlaneLevel::RepositionAStar()
{
	if (astarPos.first == astarEndPos.first && astarPos.second == astarEndPos.second)
		astar->SetPosition();
}

void PlaneLevel::UpdataAStarPos(float deltaTime)
{
	astarTimer.Tick(deltaTime);
	if (!astarTimer.IsTimeOut())
		return;

	astarTimer.Reset();
	astarPos = astar->FindNextStepAStar(rdSystem->GetConcentration());
	astarEndPos = astar->GetEndPos();
}

void PlaneLevel::Draw()
{
	Level::Draw();
}
