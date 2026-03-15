#pragma once
#include "World/Actor.h"
#include <functional>

class Plane : public RenderEngine::Actor
{
public:
	Plane();
	virtual ~Plane() override;

	virtual void BeginPlay() override;
	virtual void Tick(float deltaTime) override;
	virtual void Draw() override;
	virtual void OnDestroy() override;

	virtual std::function<float(int, int, int, int)> MakeHeuristic() override;
	virtual std::function<std::vector<std::vector<float>>(int, int)> MakeGetNextPosList() override;
	virtual std::function<std::pair<int, int>()> MakeGetAStarPos() override;

};

