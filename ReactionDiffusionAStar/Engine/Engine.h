#pragma once
class Engine
{
public:
	Engine();
	~Engine();
	void Run();

private:
	void Tick(float deltaTime);
	void Draw();

private:
	class Renderer* renderer;
};

