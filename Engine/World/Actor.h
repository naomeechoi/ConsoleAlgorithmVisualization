#pragma once
#include "Common/Common.h"
#include <memory>
#include <vector>

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

		void SetOwner(Level* newOwner);
		inline bool DestroyRequested() const
		{
			return destroyRequested;
		}
		virtual void OnDestroy();

		void SetTexture(const std::vector<double>& texture);

	private:
		void Destroy();
	protected:
		class Mesh* mesh;
		Level* owner;
		bool destroyRequested = false;
	};
}

