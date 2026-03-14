#pragma once
#include "Common/Common.h"

namespace RenderEngine
{
	class NAOMI_API Level
	{
	public:
		Level();
		virtual ~Level();

		virtual void OnExit();
		virtual void BeginPlay();
		virtual void Tick(float deltaTime);
		virtual void Draw();
	};
}

