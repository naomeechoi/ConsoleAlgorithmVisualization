#pragma once
#include "World/Actor.h"

class Plane : public RenderEngine::Actor
{
public:
	Plane();
	virtual ~Plane() override;

	virtual void BeginPlay() override;
	virtual void Tick(float deltaTime) override;
	virtual void Draw() override;
	virtual void OnDestroy() override;

};

