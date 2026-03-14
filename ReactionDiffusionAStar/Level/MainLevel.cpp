#include "MainLevel.h"
#include "World/Level.h"
#include "Actor/Plane.h"
#include "Texture/RDTexture.h"
#include "Renderer/Renderer.h"

using namespace RenderEngine;

MainLevel::MainLevel()
{
	int width = Renderer::Get().GetScreenWidth();
	int height = Renderer::Get().GetScreenWidth();
	rdTexture = new RDTexture(width, height);
	AddNewActor(new Plane());
	for (Actor* const actor : actors)
	{
		if (actor)
			actor->SetTexture(rdTexture->GetConcentration());
	}

}

MainLevel::~MainLevel()
{
	delete rdTexture;
}

void MainLevel::OnExit()
{
	Level::OnExit();
}

void MainLevel::BeginPlay()
{
	Level::BeginPlay();
}

void MainLevel::Tick(float deltaTime)
{
	Level::Tick(deltaTime);

	if (!rdTexture)
		return;

	rdTexture->Update(deltaTime);
	for (Actor* const actor : actors)
	{
		if (actor)
			actor->SetTexture(rdTexture->GetConcentration());
	}
}

void MainLevel::Draw()
{
	Level::Draw();
}
