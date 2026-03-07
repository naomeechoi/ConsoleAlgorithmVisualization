#pragma once
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
	class Renderer* renderer;
	class Maze* maze;
	class Sphere* sphere;
	class MouseInput* mouseInput;

	float yaw = 0.0f;
	float pitch = 0.0f;
};

