#pragma once
#include "Common/Common.h"
#include <memory>

namespace RenderEngine
{
	class Level;

	class NAOMI_API Actor
	{
	public:
		Actor();
		virtual ~Actor();

		virtual void BeginPlay();
		virtual void Tick(float deltaTime);
		virtual void Draw();

		void SetOwner(std::weak_ptr<Level> newOwner);
	private:
		std::weak_ptr<class Mesh> mesh;
		std::weak_ptr<Level> owner;
	};
}

