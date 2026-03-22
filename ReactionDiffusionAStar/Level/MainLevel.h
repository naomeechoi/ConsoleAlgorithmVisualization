#pragma once
#include "World/Level.h"
#include "Util/Timer.h"

class MainLevel : public RenderEngine::Level
{
public:
	MainLevel();
	virtual ~MainLevel();

	virtual void OnExit() override;
	virtual void BeginPlay() override;
	virtual void Tick(float deltaTime) override;
	void RepositionAStar();
	void UpdataAStarPos(float deltaTime);
	virtual void Draw() override;

private:
	class ReactionDiffusionSystem* rdSystem = nullptr;
	class AStar* astar = nullptr;
	RenderEngine::Timer astarTimer;
	std::pair<int, int> astarPos;
	std::pair<int, int> astarEndPos;
};

