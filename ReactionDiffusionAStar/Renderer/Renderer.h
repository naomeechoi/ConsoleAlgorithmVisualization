#pragma once
#include <string>
class Renderer
{
public:
	Renderer(int x, int y, int width, int height);
	~Renderer();

	void Summit(int x, int y, char c);
	void Draw();
private:
	short x = 0;
	short y = 0;
	int width = 0;
	int height = 0;
	std::string bufferStr;
};

