#pragma once
#include "World/Level.h"
#include "Util/Timer.h"
#include "Util/MouseInput.h"

class SphereLevel : public RenderEngine::Level
{
public:
	SphereLevel();
	virtual ~SphereLevel();
	virtual void OnExit() override;
	virtual void BeginPlay() override;
	virtual void Tick(float deltaTime) override;
	void RepositionAStar();
	void UpdataAStarPos(float deltaTime);
	virtual void Draw() override;

private:
	class ReactionDiffusionSystem* rdSystem = nullptr;
	class RenderEngine::MouseInput* mouseInput;
	class AStar* astar = nullptr;
	RenderEngine::Timer astarTimer;
	std::pair<int, int> astarPos;
	std::pair<int, int> astarEndPos;
};

