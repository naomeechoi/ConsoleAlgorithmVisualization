#pragma once
#include <vector>
class AStar
{
	struct Node
	{
		int x = 0;
		int y = 0;
		float g = 0.0f;
		float h = 0.0f;
		Node* parent = nullptr;

		Node(int x, int y, float g, float h, Node* parent)
			:x(x), y(y), g(g), h(h), parent(parent)
		{
		};
		float f() const { return g + h; }
	};

	struct NextPos
	{
		int x = 0;
		int y = 0;
		float cost = 0.0f;
	};

	struct CompareNode {
		bool operator()(Node* a, Node* b) { return a->f() > b->f(); }
	};

	AStar(int width, int height)
		:width(width), height(height)
	{
	};
	~AStar() = default;

	float Heuristic(int sx, int sy, int ex, int ey, float minCost);

	std::pair<int, int> FindNextStepAStar(int sx, int sy,
		int ex, int ey,
		const std::vector<double>& densities,
		const std::vector<NextPos>& directions,
		float minCost);

private:
	int width = 0;
	int height = 0;
};