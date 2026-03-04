#include "Maze.h"
#include <algorithm>

using std::vector;


Maze::ReactionDiffusion::ReactionDiffusion(int width, int height)
	:width(width), height(height), curA(width * height, 1.0), curB(width * height, 0.0), nextA(width* height), nextB(width* height)
{
}

void Maze::ReactionDiffusion::SetRandomSeed()
{
	curA.assign(width * height, 1.0);
	curB.assign(width * height, 0.0);
	float makeBPercentage = 0.16;
	for (int i = 0; i < static_cast<int>(width * height * makeBPercentage); i++)
	{
		curB[(rand() % height) * width + (rand() % width)] = 1.0;
	}
}

int Maze::ReactionDiffusion::GetIdx(int x, int y)
{
	//return y * width + x;
	return ((y + height) % height) * width + ((x + width) % width);
}

void Maze::ReactionDiffusion::Update()
{
	for (int i = 0; i < width * height; i++)
	{
		int x = i % width;
		int y = i / width;
		double a = curA[i];
		double b = curB[i];

		double lapA = (curA[GetIdx(x + 1, y)] + curA[GetIdx(x - 1, y)] + curA[GetIdx(x, y + 1)] + curA[GetIdx(x, y - 1)]) * 0.2 + curA[i] * -1.0
			+ (curA[GetIdx(x + 1, y + 1)] + curA[GetIdx(x - 1, y - 1)] + curA[GetIdx(x - 1, y + 1)] + curA[GetIdx(x + 1, y - 1)]) * 0.05;

		double lapB = (curA[GetIdx(x + 1, y)] + curA[GetIdx(x - 1, y)] + curA[GetIdx(x, y + 1)] + curA[GetIdx(x, y - 1)]) * 0.2 + curA[i] * -1.0
			+ (curA[GetIdx(x + 1, y + 1)] + curA[GetIdx(x - 1, y - 1)] + curA[GetIdx(x - 1, y + 1)] + curA[GetIdx(x + 1, y - 1)]) * 0.05;
	
		const double DA = 0.16;  // 화학물질 A의 확산 속도
		const double DB = 0.08;  // 화학물질 B의 확산 속도
		const double FEED = 0.035; // 패턴 형성을 위한 공급률
		const double KILL = 0.060; // 패턴 형성을 위한 제거율
		const double DT = 1.0;     // 프레임당 시뮬레이션 시간 변화량
		double abb = a * b * b;
		nextA[i] = a + (DA * lapA - abb + FEED * (1.0f - a)) * DT;
		nextB[i] = b + (DB * lapB + abb - (KILL + FEED) * b) * DT;

		if (nextA[i] < 0)
			nextA[i] = 0;
		else if (nextA[i] > 1)
			nextA[i] = 1;

		if (nextB[i] < 0)
			nextB[i] = 0;
		else if (nextB[i] > 1)
			nextB[i] = 1;
	}
	curA.swap(nextA);
	curB.swap(nextB);
}
