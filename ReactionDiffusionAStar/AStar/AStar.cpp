#include "AStar.h"
#include <vector>
#include <queue>
#include <cmath>
#include <utility>
#include <cstdlib>

using namespace std;

float AStar::Heuristic(int sx, int sy, int ex, int ey, float minCost)
{
	return static_cast<float>((abs(sx - ex) + abs(sy - ey)) * minCost);
}

void AStar::GetNextPosListAndMinCost(int meshType, std::pair<int, int> curPos, std::vector<AStar::NextPos>&nextPosList, float& minCost)
{
	if (meshType == 0)
	{
		vector<vector<int>> list = { {-1, 0}, {1, 0}, {0, -1}, {0, 1}, {-1, -1}, {-1, 1}, {1, -1}, {1, 1} };
		for (int i = 0; i < list.size(); i++)
		{
			AStar::NextPos next;
			next.x = curPos.first + list[i][0];
			next.y = curPos.second + list[i][1];
			next.cost = (abs(list[i][0]) == abs(list[i][1])) ? 1.414f : 1.0f;
			nextPosList.emplace_back(next);
			minCost = min(minCost, next.cost);
		}
	}
}


std::pair<int, int> AStar::FindNextStepAStar(int sx, int sy, int ex, int ey, const std::vector<double>& densities, int type)
{
	if (sx == ex && sy == ey)
		return { sx, sy };

	priority_queue <Node*, vector<Node*>, CompareNode> openList;
	vector<Node*> nodes;
	vector<float> gScore(width * height, FLT_MAX);
	std::vector<AStar::NextPos> nextPosList;
	float minCost = FLT_MAX;
	GetNextPosListAndMinCost(type, { sx, sy }, nextPosList, minCost);

	nodes.push_back(new Node{ sx, sy, 0.0f, Heuristic(sx, sy, ex, ey, minCost), nullptr });
	Node* startNode = nodes[0];
	openList.push(startNode);
	gScore[sy * width + sx] = 0;

	Node* bestNode = startNode;
	while (!openList.empty())
	{
		Node* current = openList.top();
		openList.pop();

		if (current->x == ex && current->y == ey)
		{
			Node* cur = current;
			Node* prev = cur->parent;
			while (prev != startNode && prev != nullptr)
			{
				cur = prev;
				prev = cur->parent;
			}
			return { cur->x, cur->y };
		}

		if (current->h < bestNode->h)
			bestNode = current;

		nextPosList.clear();
		GetNextPosListAndMinCost(type, { current->x, current->y }, nextPosList, minCost);
		for (const auto& nextPos : nextPosList)
		{
			if (nextPos.x < 0 || nextPos.x >= width || nextPos.y < 0 || nextPos.y >= height)
				continue;

			int nextIdx = nextPos.y * width + nextPos.x;
			int endIdx = ey * width + ex;
			if (nextIdx != endIdx && densities[nextIdx] > 0.10)
				continue;

			float newG = current->g + nextPos.cost;
			if (newG < gScore[nextIdx])
			{
				gScore[nextIdx] = newG;
				nodes.push_back(new Node{ nextPos.x, nextPos.y, newG, Heuristic(nextPos.x, nextPos.y, ex, ey, minCost), current });
				Node* newNode = nodes.back();
				openList.push(newNode);

				if (newNode->h < bestNode->h)
					bestNode = newNode;
			}
		}
	}

	Node* idx = bestNode;
	Node* prev = idx->parent;
	while (prev != startNode && prev != nullptr)
	{
		idx = prev;
		prev = idx->parent;
	}

	int rx = idx->x;
	int ry = idx->y;

	for (Node* n : nodes)
		delete n;

	return { rx, ry };
}

void AStar::Submit(std::vector<CHAR_INFO>& buffer, int x, int y, int endX, int endY)
{
	int idx = y * width + x;
	int endIdx = endY * width + endX;

	vector<vector<int>> list = { {-1, 0}, {1, 0}, {0, -1}, {0, 1}, {-1, -1}, {-1, 1}, {1, -1}, {1, 1} };
	for (int i = 0; i < list.size(); i++)
	{
		int nextX = endX + list[i][0];
		int nextY = endY + list[i][1];
		if (nextX < 0 || nextX >= width || nextY < 0 || nextY >= height)
			continue;

		int nextNeighborIdx = nextY * width + nextX;
		buffer[nextNeighborIdx].Attributes |= BACKGROUND_GREEN;
	}
	buffer[endIdx].Char.AsciiChar = '!';
	buffer[endIdx].Attributes = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | BACKGROUND_GREEN;

	buffer[idx].Char.AsciiChar = '@';
	buffer[idx].Attributes = FOREGROUND_RED;
}