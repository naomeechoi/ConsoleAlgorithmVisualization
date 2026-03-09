#pragma once
#include <vector>
class Engine
{
public:
	Engine();
	~Engine();
	void Run();

private:
	void Tick(float deltaTime);
	void Submit();
	void Draw();

private:
	std::pair<int, int> GetRandomCubePos();
	class Renderer* renderer;
	class RDTexture* rdTexture;
	class Plane* plane;
	class Sphere* sphere;
	class Cube* cube;
	class AStar* astar;
	class MouseInput* mouseInput;

	float yaw = 0.0f;
	float pitch = 0.0f;

	int astarX = 0;
	int astarY = 0;
	int astarEndX = 0;
	int astarEndY = 0;
	float astarMoveTimer = 0.0f;
};

