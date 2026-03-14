#include "Core/Engine.h"
#include "Level/MainLevel.h"
using namespace RenderEngine;

int main()
{
	Engine engine;
	engine.SetLevel(new MainLevel());
	engine.Run();
}