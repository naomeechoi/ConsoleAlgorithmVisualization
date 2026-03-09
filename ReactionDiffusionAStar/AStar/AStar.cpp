#define NOMINMAX
#include "AStar.h"
#include <vector>
#include <queue>
#include <cmath>
#include <utility>
#include <cstdlib>
#include <algorithm>
const float PI = 3.14159265f;
using namespace std;

float AStar::Heuristic(int sx, int sy, int ex, int ey)
{
	return static_cast<float>((abs(sx - ex) + abs(sy - ey)));
}

void AStar::GetNextPosListAndMinCost(std::pair<int, int> curPos, std::vector<AStar::NextPos>& nextPosList)
{
	vector<vector<int>> list = { {-1, 0}, {1, 0}, {0, -1}, {0, 1}, {-1, -1}, {-1, 1}, {1, -1}, {1, 1} };
	for (int i = 0; i < list.size(); i++)
	{
		AStar::NextPos next;
		next.x = curPos.first + list[i][0];
		next.y = curPos.second + list[i][1];
		next.cost = (abs(list[i][0]) == abs(list[i][1])) ? 1.414f : 1.0f;
		nextPosList.emplace_back(next);
	}
}

std::pair<int, int> AStar::FindNextStepAStar(int sx, int sy, int ex, int ey, const std::vector<double>& densities)
{
	if (sx == ex && sy == ey)
		return { sx, sy };

	priority_queue <Node*, vector<Node*>, CompareNode> openList;
	vector<Node*> nodes;
	vector<float> gScore(width * height, FLT_MAX);
	std::vector<AStar::NextPos> nextPosList;
	GetNextPosListAndMinCost({ sx, sy }, nextPosList);

	nodes.push_back(new Node{ sx, sy, 0.0f, Heuristic(sx, sy, ex, ey), nullptr });
	Node* startNode = nodes[0];
	openList.push(startNode);
	gScore[sy * width + sx] = 0;

	Node* bestNode = startNode;

	pair<int, int> result;
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
			result.first = cur->x;
			result.second = cur->y;
			break;
		}

		if (current->h < bestNode->h)
			bestNode = current;

		nextPosList.clear();
		GetNextPosListAndMinCost({ current->x, current->y }, nextPosList);
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
				nodes.push_back(new Node{ nextPos.x, nextPos.y, newG, Heuristic(nextPos.x, nextPos.y, ex, ey), current });
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

	result.first = idx->x;
	result.second = idx->y;

	for (Node* n : nodes)
		delete n;

	return result;
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

void AStar::SubmitOnSphere(std::vector<CHAR_INFO>& buffer,
	const float radius,
	const std::vector<float>& rot,
	int x, int y, int endX, int endY)
{
	float screenAspectRatio = (float)width / (float)height;
	const float consoleCharAspect = 2.0f;
	float halfWidth = width / 2.0f;
	float halfHeight = height / 2.0f;

	for (int sy = 0; sy < height; sy++)
	{
		for (int sx = 0; sx < width; sx++)
		{
			float nx = ((sx - halfWidth) / halfWidth) * screenAspectRatio / consoleCharAspect;
			float ny = (sy - halfHeight) / halfHeight;
			float lengthSq = nx * nx + ny * ny;

			if (lengthSq > radius * radius) continue;

			float nz = sqrt(radius * radius - lengthSq);

			float nxRot = nx * rot[0] + ny * rot[1] + nz * rot[2];
			float nyRot = nx * rot[3] + ny * rot[4] + nz * rot[5];
			float nzRot = nx * rot[6] + ny * rot[7] + nz * rot[8];

			float u = 0.5f + atan2(nzRot, nxRot) / (2.0f * PI);
			float v = 0.5f - asin(nyRot / radius) / PI;

			int tx = std::clamp(int(u * width), 0, width - 1);
			int ty = std::clamp(int(v * height), 0, height - 1);

			int bufferIdx = sx + sy * width;

			auto getWrappedDist = [&](int tX, int tY, int targetX, int targetY) {
				float dx = fabsf((float)tX - targetX);
				// X축 차이가 화면 너비의 절반보다 크면 반대쪽으로 도는 게 더 가깝다.
				if (dx > width / 2.0f) {
					dx = (float)width - dx;
				}
				float dy = fabsf((float)tY - targetY);
				return sqrtf(dx * dx + dy * dy);
				};

			if (getWrappedDist(tx, ty, endX, endY) < 2.0f)
			{
				buffer[bufferIdx].Char.AsciiChar = '!';
				buffer[bufferIdx].Attributes = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
			}

			if (getWrappedDist(tx, ty, x, y) < 2.0f)
			{
				buffer[bufferIdx].Char.AsciiChar = '@';
				buffer[bufferIdx].Attributes = FOREGROUND_RED | FOREGROUND_INTENSITY;
			}
		}
	}
}