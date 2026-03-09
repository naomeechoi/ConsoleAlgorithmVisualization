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


std::pair<int, int> AStar::FindNextStepAStar(int sx, int sy, int ex, int ey, const std::vector<double>& densities, const std::vector<NextPos>& NextPosList, float minCost)
{
	if (sx == ex && sy == ey)
		return { sx, sy };

	priority_queue <Node*, vector<Node*>, CompareNode> openList;
	vector<Node> nodes;
	vector<float> gScore(width * height, FLT_MAX);

	nodes.push_back({ sx, sy, 0.0f, Heuristic(sx, sy, ex, ey, minCost), nullptr });
	Node* startNode = &nodes[0];
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

		for (const auto& nextPos : NextPosList)
		{
			if (nextPos.x < 0 || nextPos.x >= width || nextPos.y < 0 || nextPos.y >= height)
				continue;

			int nextIdx = nextPos.y * width + nextPos.x;
			if (densities[nextIdx] > 0.10)
				continue;

			float newG = current->g + nextPos.cost;
			if (newG < gScore[nextIdx])
			{
				gScore[nextIdx] = newG;
				nodes.push_back({ nextPos.x, nextPos.y, newG, Heuristic(nextPos.x, nextPos.y, ex, ey, minCost), current });
				Node* newNode = &nodes.back();
				openList.push(newNode);
			}

		}

		if (current->h < bestNode->h)
			bestNode = current;
	}

	Node* idx = bestNode;
	Node* prev = idx->parent;
	while (prev != startNode && prev != nullptr)
	{
		idx = prev;
		prev = idx->parent;
	}
	return { idx->x, idx->y };
}