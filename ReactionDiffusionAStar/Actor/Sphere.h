#pragma once
#include "World/Actor.h"
#include <functional>
#include <vector>

class Sphere : public RenderEngine::Actor
{
public:
	Sphere();
	virtual ~Sphere() override;

	virtual void BeginPlay() override;
	virtual void Tick(float deltaTime) override;
	virtual void Draw() override;
	virtual void OnDestroy() override;

	virtual std::function<float(int, int, int, int)> MakeHeuristic() override;
	virtual std::function<std::vector<std::vector<float>>(int, int)> MakeGetNextPosList() override;
	virtual std::function<std::pair<int, int>()> MakeGetAStarPos() override;


	void SetRotation(float yaw, float pitch);

private:
	std::vector<float> rot = { 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f };
};

