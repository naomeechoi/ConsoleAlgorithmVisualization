#include "Core/Engine.h"
#include "Level/PlaneLevel.h"
#include "Level/SphereLevel.h"
using namespace RenderEngine;

int main()
{
	Engine engine;
	//engine.SetLevel(new PlaneLevel());
	engine.SetLevel(new SphereLevel());
	engine.Run();
}