#include "Actor.h"
#include "Core/Engine.h"
#include "Renderer/Renderer.h"
#include "World/Level.h"
#include "World/Texture.h"

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
		if (texture)
			texture->UpdateTexture();
	}

	void Actor::Draw()
	{
		if(texture)
			Renderer::Get().Submit(std::move(texture->GetFinalBuffer()));
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
		if (this->texture)
			this->texture->SetTexture(texture);
	}

	void Actor::SetAstarPos(int x, int y, int endX, int endY)
	{
		if (this->texture)
			this->texture->SetAStarPos(x, y, endX, endY);
	}

	void Actor::Destroy()
	{
		// 삭제 플래그 설정.
		destroyRequested = true;

		// 삭제 이벤트 호출.
		OnDestroy();
	}
}