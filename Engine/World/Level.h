#pragma once
#include "Common/Common.h"
#include <vector>

namespace RenderEngine
{
	class Actor;
	class NAOMI_API Level
	{
	public:
		Level();
		virtual ~Level();

		virtual void OnExit();
		virtual void BeginPlay();
		virtual void Tick(float deltaTime);
		virtual void Draw();

		// 액터 추가 함수.
		void AddNewActor(Actor* newActor);

		// 액터 추가/제거 처리 함수.
		void ProcessAddAndDestroyActors();

	protected:
		// 액터 배열.
		std::vector<Actor*> actors;

		// 실행 중에 추가 요청된 액터의 배열.
		std::vector<Actor*> addRequestedActors;
	};
}

