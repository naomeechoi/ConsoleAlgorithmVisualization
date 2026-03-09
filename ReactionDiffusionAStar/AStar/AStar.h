#pragma once
#include <vector>
#include <Windows.h>
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

	struct NextPos
	{
		int x = 0;
		int y = 0;
		float cost = 0.0f;
	};

	struct CompareNode {
		bool operator()(Node* a, Node* b) { return a->f() > b->f(); }
	};

public:
	AStar(int width, int height, int type)
		:width(width), height(height), type(type)
	{
	};
	~AStar() = default;

	std::pair<int, int> FindNextStepAStar(int sx, int sy,
		int ex, int ey,
		const std::vector<double>& densities);
	void Submit(std::vector<CHAR_INFO>& buffer,int x, int y, int endX, int endY);
	void SubmitOnSphere(std::vector<CHAR_INFO>& buffer, const float radius, const std::vector<float>& rot, int x, int y, int endX, int endY);
	void SetRotation(float dx, float dy);
private:
	float Heuristic(int sx, int sy, int ex, int ey);

	void GetNextPosListAndMinCost(std::pair<int, int> curPos, std::vector<AStar::NextPos>& nextPosList);

private:
	int width = 0;
	int height = 0;
	int type = 0;
};