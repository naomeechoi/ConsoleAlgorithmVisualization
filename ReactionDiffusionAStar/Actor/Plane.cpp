#include "Plane.h"
#include "Renderer/Renderer.h"
#include "Mesh/Plane/PlaneMesh.h"

using namespace RenderEngine;

Plane::Plane()
{
	mesh = new PlaneMesh(
		Renderer::Get().GetScreenWidth(),
		Renderer::Get().GetScreenHeight());
}

Plane::~Plane()
{
}

void Plane::BeginPlay()
{
	Actor::BeginPlay();
}

void Plane::Tick(float deltaTime)
{
	Actor::Tick(deltaTime);
}

void Plane::Draw()
{
	Actor::Draw();
}

void Plane::OnDestroy()
{
	Actor::OnDestroy();
}
