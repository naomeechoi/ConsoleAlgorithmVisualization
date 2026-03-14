#pragma once
#include "Common/Common.h"
#include <string>
#include <Windows.h>
#include <vector>

namespace RenderEngine
{
	class NAOMI_API Renderer
	{
	public:
		Renderer(int width, int height);
		~Renderer();

		void Submit(std::vector<CHAR_INFO> finalBuffer);
		void Draw();
		std::vector<CHAR_INFO>& GetBuffer() { return buffer; }
		static Renderer& Get();
		inline int GetScreenWidth() { return width; };
		inline int GetScreenHeight() { return height; };
	private:
		int width = 0;
		int height = 0;
		std::vector<CHAR_INFO> buffer;

		static Renderer* instance;
	};
}
