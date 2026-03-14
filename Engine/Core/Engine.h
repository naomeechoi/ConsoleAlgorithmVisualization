#pragma once
#include "Common/Common.h"
#include <string>

namespace RenderEngine
{
	class NAOMI_API Engine
	{
		struct EngineSetting
		{
			float framerate = 0.0f;
			int width = 0;
			int height = 0;
		};

	public:
		Engine();
		~Engine();
		void Run();
		void Quit() { isQuit = true; };
		void SetLevel();

	protected:
		class Renderer* renderer = nullptr;
		static Engine* instance;
		bool isQuit = false;

	private:
		EngineSetting setting;

	private:
		void LoadSetting();
		std::string trim(const std::string& str);
		void Tick(float deltaTime);
		void Draw();
	};
}