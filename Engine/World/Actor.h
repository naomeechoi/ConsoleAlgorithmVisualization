#pragma once
#include "Common/Common.h"
#include <memory>
#include <vector>
#include <functional>

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
		void SetAstarPos(int x, int y, int endX, int endY);
		virtual std::function<float(int, int, int, int)> MakeHeuristic() = 0;
		virtual std::function<std::vector<std::vector<float>>(int, int)> MakeGetNextPosList() = 0;
		virtual std::function<std::pair<int, int>()> MakeGetAStarPos() = 0;

	private:
		void Destroy();
	protected:
		class Texture* texture;
		Level* owner;
		bool destroyRequested = false;
	};
}

