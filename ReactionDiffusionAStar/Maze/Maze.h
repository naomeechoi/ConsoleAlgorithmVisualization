#pragma once
#include <vector>

class Maze
{
	struct ReactionDiffusion
	{
		ReactionDiffusion(int width, int height);
		~ReactionDiffusion() = default;

		void SetRandomSeed();
		int GetIdx(int x, int y);
		void Update();

		int width = 0;
		int height = 0;
		std::vector<double> curA;
		std::vector<double> curB;
		std::vector<double> nextA;
		std::vector<double> nextB;
	};
};

