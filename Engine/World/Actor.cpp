#include "Actor.h"
#include "Core/Engine.h"
#include "Renderer/Renderer.h"
#include "World/Level.h"
#include "World/Mesh.h"

namespace RenderEngine
{
	Actor::Actor()
	{
	}

	Actor::~Actor()
	{
	}

	void Actor::BeginPlay()
	{
	}

	void Actor::Tick(float deltaTime)
	{
	}

	void Actor::Draw()
	{
		std::weak_ptr<Mesh> mesh;

		if (auto locked = mesh.lock())
		{
			Renderer::Get().Submit(std::move(locked->GetFinalBuffer()));
			locked->GetFinalBuffer();
		}
	}

	void Actor::SetOwner(std::weak_ptr<Level> newOwner)
	{
		owner = newOwner;
	}
}