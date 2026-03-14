#pragma once
#include "World/Level.h"

class MainLevel : public RenderEngine::Level
{
public:
	MainLevel();
	virtual ~MainLevel();

	virtual void OnExit() override;
	virtual void BeginPlay() override;
	virtual void Tick(float deltaTime) override;
	virtual void Draw() override;

private:
	class RDTexture* rdTexture = nullptr;
};

