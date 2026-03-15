#define NOMINMAX
#include "AStar.h"
#include <vector>
#include <queue>
#include <utility>
using namespace std;


void AStar::SetHeuristic(function<float(int, int, int, int)> f)
{
	Heuristic = f;
}

void AStar::SetGetNextPosList(function<vector<vector<float>>(int, int)> f)
{
	GetNextPosList = f;
}

void AStar::SetReposition(std::function<std::pair<int, int>()> f)
{
	Reposition = f;
}

pair<int, int> AStar::FindNextStepAStar(const vector<double>& rdConcentration)
{
	if (startX == endX && startY == endY)
		return { startX, startY };

	priority_queue <Node*, vector<Node*>, CompareNode> openList;
	vector<Node*> nodes;
	vector<float> gScore(width * height, FLT_MAX);

	nodes.push_back(new Node{ startX, startY, 0.0f, Heuristic(startX, startY, endX, endY), nullptr });
	Node* startNode = nodes[0];
	openList.push(startNode);
	gScore[startY * width + startX] = 0;

	Node* bestNode = startNode;

	while (!openList.empty())
	{
		Node* current = openList.top();
		openList.pop();

		if (current->x == endX && current->y == endY)
		{
			bestNode = current;
			break;
		}

		if (current->h < bestNode->h)
			bestNode = current;

		vector<vector<float>> nextPosList = GetNextPosList(current->x, current->y);

		for (vector<float>& nextPos : nextPosList)
		{
			int nextPosX = static_cast<int>(std::round(nextPos[0]));
			int nextPosY = static_cast<int>(std::round(nextPos[1]));
			float nextPosCost = nextPos[2];
			if (nextPosX < 0 || nextPosX >= width || nextPosY < 0 || nextPosY >= height)
				continue;

			int nextIdx = nextPosY * width + nextPosX;
			int endIdx = endY * width + endX;
			if (nextIdx != endIdx  && rdConcentration[nextIdx] > 0.10)
				continue;

			float newG = current->g + nextPosCost;
			if (newG < gScore[nextIdx])
			{
				gScore[nextIdx] = newG;
				nodes.push_back(new Node{ nextPosX, nextPosY, newG, Heuristic(nextPosX, nextPosY, endX, endY), current });
				Node* newNode = nodes.back();
				openList.push(newNode);

				if (newNode->h < bestNode->h)
					bestNode = newNode;
			}
		}
	}

	Node* prev = bestNode->parent;
	while (prev != startNode && prev != nullptr)
	{
		bestNode = prev;
		prev = bestNode->parent;
	}

	startX = bestNode->x;
	startY = bestNode->y;

	for (Node* n : nodes)
		delete n;

	return { startX, startY };
}

std::pair<int, int> AStar::GetEndPos()
{
	return std::pair<int, int>(endX, endY);
}

void AStar::SetPosition()
{
	if (!Reposition)
		return;

	startX = endX;
	startY = endY;

	std::pair<int, int> nextEndPos = Reposition();
	endX = nextEndPos.first;
	endY = nextEndPos.second;
}

/*
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
}*/