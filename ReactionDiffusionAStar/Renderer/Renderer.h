#pragma once
#include <string>
#include <Windows.h>
#include <vector>

class Renderer
{
public:
	Renderer(int x, int y, int width, int height);
	~Renderer();

	void Submit(int x, int y, char c, WORD color);
	void Draw();
	std::vector<CHAR_INFO>& GetBuffer() { return buffer; }
private:
	short x = 0;
	short y = 0;
	int width = 0;
	int height = 0;
	std::vector<CHAR_INFO> buffer;
};

