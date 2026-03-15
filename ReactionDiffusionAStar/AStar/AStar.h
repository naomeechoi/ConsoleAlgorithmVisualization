#pragma once
#include <Windows.h>
#include <vector>
#include <functional>
#include <utility>

class AStar
{
	struct Node
	{
		Node(int x, int y, float g, float h, Node* parent)
			:x(x), y(y), g(g), h(h), parent(parent)
		{
		};

		float f() const { return g + h; }

		int x = 0;
		int y = 0;
		float g = 0.0f;
		float h = 0.0f;
		Node* parent = nullptr;
	};

	struct CompareNode {
		bool operator()(Node* a, Node* b) { return a->f() > b->f(); }
	};

public:
	AStar(int width, int height)
		:width(width), height(height)
	{
	};
	~AStar() = default;

	void SetHeuristic(std::function<float(int, int, int, int)> f);
	void SetGetNextPosList(std::function<std::vector<std::vector<float>>(int, int)> f);
	void SetReposition(std::function<std::pair<int, int>()> f);
	std::pair<int, int> FindNextStepAStar(const std::vector<double>& densities);
	std::pair<int, int> GetEndPos();
	
	void SetPosition();

private:
	std::function<float(int, int, int, int)> Heuristic = nullptr;
	std::function<std::vector<std::vector<float>>(int, int)> GetNextPosList = nullptr;
	std::function<std::pair<int, int>()> Reposition = nullptr;
private:
	int width = 0;
	int height = 0;

	int startX = 0;
	int startY = 0;
	int endX = 0;
	int endY = 0;
};