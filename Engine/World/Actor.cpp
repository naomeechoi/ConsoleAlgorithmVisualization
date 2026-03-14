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
		if (mesh)
			mesh->UpdateTexture();
	}

	void Actor::Draw()
	{
		if(mesh)
			Renderer::Get().Submit(std::move(mesh->GetFinalBuffer()));
	}

	void Actor::SetOwner(Level* newOwner)
	{
		owner = newOwner;
	}

	void Actor::OnDestroy()
	{
	}

	void Actor::SetTexture(const std::vector<double>& texture)
	{
		if (mesh)
			mesh->SetTexture(texture);
	}

	void Actor::Destroy()
	{
		// 삭제 플래그 설정.
		destroyRequested = true;

		// 삭제 이벤트 호출.
		OnDestroy();
	}
}