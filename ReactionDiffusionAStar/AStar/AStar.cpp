#define NOMINMAX
#include "AStar.h"
#include <vector>
#include <queue>
#include <cmath>
#include <utility>
#include <cstdlib>
#include <algorithm>
#include <Mesh/Cube/Cube.h>
const float PI = 3.14159265f;
using namespace std;

int AStar::GetFace(int x, int y)
{
	int fx = x / faceSize();
	int fy = y / faceSize();

	// 십자가 전개도의 중앙 가로줄
	if (fy == 1) {
		if (fx == 0) return 1; // FACE_NEG_X (왼쪽)
		if (fx == 1) return 4; // FACE_Z (정면)
		if (fx == 2) return 0; // FACE_X (오른쪽)
		if (fx == 3) return 5; // FACE_NEG_Z (뒷면)
	}

	// 위/아래 면
	if (fx == 1 && fy == 0) return 2; // FACE_Y (위쪽)
	if (fx == 1 && fy == 2) return 3; // FACE_NEG_Y (아래쪽)

	return -1;
}

float AStar::Heuristic(int sx, int sy, int ex, int ey)
{
	return static_cast<float>((abs(sx - ex) + abs(sy - ey)));
}

bool AStar::IsCubeAtlasValid(int x, int y) const {
	if (type != 2) return true;
	int fx = x / (width / 4);
	int fy = y / (height / 3);
	if (fy == 1) return true;
	if (fx == 1 && (fy == 0 || fy == 2)) return true;
	return false;
}

void AStar::GetNextPosListAndMinCost(std::pair<int, int> curPos, std::vector<AStar::NextPos>& nextPosList)
{
	int fs = faceSize();
	int cx = curPos.first;
	int cy = curPos.second;
	int curFace = GetFace(cx, cy);
	int lx = cx % fs; // 면 내부 x (0 ~ fs-1)
	int ly = cy % fs; // 면 내부 y (0 ~ fs-1)


	// 8방향 탐색
	vector<vector<int>> list = { {-1,0},{1,0},{0,-1},{0,1},{-1,-1},{-1,1},{1,-1},{1,1} };

	for (int i = 0; i < list.size(); i++)
	{
		AStar::NextPos res;
		res.cost = (abs(list[i][0]) == abs(list[i][1])) ? 1.414f : 1.0f;

		int nx = cx + list[i][0];
		int ny = cy + list[i][1];

		if (type == 2)
		{
			if (abs(cy - height) < 3 && cx >= width / 4 && cx < width / 2) { // 파랑색 면(중앙)으로 올라가려는 시도라면

				float t = 1.0f - (cx - width / 4.0f) / (width / 4.0f);

				float newX = (width / 4.0f) * t + width / 4.0f * 3.0f;
				float newY = (height / 3.0f * 2.0f) - 5;

				nx = (int)newX;
				ny = (int)newY;
				res.cost = 0;
				res.x = nx;
				res.y = ny;
				nextPosList.emplace_back(res);
				return;
			}

			if (list[i][0] == 1)
			{
				//초록 오룬쪽 모서리에서 빨강 위쪽
				if (abs(cx - width / 4 * 2) < 2 && cy >= 0 && cy <= height / 3)
				{
					float t = 1.0f - (cy / (height / 3.0f));

					float newX = width / 4.0f * 2 + t * (width / 4.0f);
					float newY = height / 3.0f;
					nx = (int)newX;
					ny = (int)newY;
				}
				// 연초록 오른쪽에서 빨강 아래쪽
				else if (abs(cx - width / 4 * 2) < 2 && cy >= height / 3 * 2 && cy < height)
				{
					float t = 1.0f - ((cy - height * 2 / 3) / (height / 3));

					float newX = width / 4.0f * 2 + t * (width / 4.0f);
					float newY = height / 3.0f * 2;
					nx = (int)newX;
					ny = (int)newY;
				}
				// 연파랑에서 연빨강
				else if (abs(cx - width) < 2 && cy >= height / 3 && cy < height / 3 * 2)
				{
					nx = 0;
					ny = cy;
				}
			}
			else if (list[i][0] == -1)
			{
				// 초록 왼쪽에서 연빨강 위쪽
				if (abs(cx - width / 4) < 2 && cy >= 0 && cy <= height / 3)
				{
					float t = (cy / (height / 3.0f));

					float newX = 0 + t * (width / 4.0f);
					float newY = height / 3.0f;

					nx = (int)newX;
					ny = (int)newY;
				}
				// 연초록 왼쪽에서 연빨강 아래쪽
				else if (abs(cx - width / 4) < 2 && cy >= height / 3 * 2 && cy < height)
				{
					float t = ((cy - height * 2 / 3) / (height / 3));

					float newX = t * (width / 4.0f);
					float newY = height / 3.0f * 2;
					nx = (int)newX;
					ny = (int)newY;
				}
				// 연빨강에서 연파랑
				else if (abs(cx - 0) < 2 && cy >= height / 3 && cy < height / 3 * 2)
				{
					nx = width - 1;
					ny = cy;
				}
			}

			if (list[i][1] == -1)
			{
				// 빨강 위쪽 모서리에서 초록색 오른쪽
				if (abs(cy - height / 3) < 2 && cx >= width / 4 * 2 && cx <= width / 4 * 3)
				{
					float t = 1.0f - ((cx - width / 4.0f * 2) / (width / 4.0f));

					float newX = width * 2 / 4;
					float newY = t * (height / 3.0f);
					nx = (int)newX;
					ny = (int)newY;
				}
				// 연빨강 위쪽에서 초록색 왼쪽
				else if (abs(cy - height / 3) < 2 && cx >= 0 && cx <= width / 4)
				{
					float t = (cx / (width / 4.0f));

					float newX = width / 4.0f;
					float newY = 0 + t * (height / 3.0f);

					nx = (int)newX;
					ny = (int)newY;
				}
				// 연파랑 위쪽에서 초록 위쪽
				else if (abs(cy - height / 3) < 2 && cx >= width / 4 * 3 && cx < width)
				{
					float t = 1.0f - ((cx - width / 4.0f * 3.0f) / (width / 4.0f));

					float newX = (width / 4.0f) * t + width / 4.0f;
					float newY = 0;

					nx = (int)newX;
					ny = (int)newY;
				}
				else if (abs(cy - 0) < 2 && cx >= width / 4 && cx < width / 2)
				{
					float t = 1.0f - ((cx - width / 4.0f) / (width / 4.0f));

					float newX = width / 4.0f * 3 + t * (width / 4.0f);
					float newY = height / 3.0f;

					nx = (int)newX;
					ny = (int)newY;
				}
				// 연초록(NEG_Y) 면의 위쪽 경계 (y = height * 2/3 근처)에서 파랑 하드 코딩
				else if (abs(cy - (height / 3 * 2)) < 2 && cx >= width / 4 && cx < width / 2)
				{
					// 연초록의 x좌표 비율을 그대로 파랑의 x좌표로 전달
					float t = (cx - (width / 4.0f)) / (width / 4.0f);

					nx = (int)(width / 4.0f + t * (width / 4.0f));
					ny = (int)(height / 3.0f * 2.0f - 1); // 파랑의 맨 아랫줄로 진입
				}

			}
			else if (list[i][1] == 1)
			{
				// 빨강 아래쪽에서 연초록 오른쪽
				if (abs(cy - height / 3 * 2) < 2 && cx >= width / 2 && cx < width / 4 * 3)
				{
					float t = 1.0f - ((cx - width / 2.0f) / (width / 4.0f));

					float newX = width / 2.0f;
					float newY = height / 3.0f * 2 + t * (height / 3.0f);

					nx = (int)newX;
					ny = (int)newY;
				}
				// 연빨강 아래쪽에서 연초록 왼쪽
				else if (abs(cy - height / 3 * 2) < 2 && cx >= 0 && cx < width / 4)
				{
					float t = (cx / (width / 4.0f));

					float newX = width / 4.0f;
					float newY = height / 3.0f * 2 + t * (height / 3.0f);

					nx = (int)newX;
					ny = (int)newY;
				}
				// 연파랑 아래쪽에서 연초록 아래쪽
				else if (abs(cy - height / 3 * 2) < 2 && cx >= width / 4 * 3 && cx < width)
				{
					float t = ((cx - width / 4.0f * 3.0f) / (width / 4.0f));

					float newX = (width / 4.0f) * t + width / 4.0f;
					float newY = height - 5;

					nx = (int)newX;
					ny = (int)newY;
				}
				// 연초록 아래쪽 연파랑 아래쪽
				else if (abs(cy - height) < 2 && cx >= width / 4 && cx < width / 4 * 2)
				{
					float t = 1.0f - (cx - width / 4.0f) / (width / 4.0f);

					float newX = (width / 4.0f) * t + width / 4.0f * 3.0f;
					float newY = (height / 3.0f * 2.0f) - 5;

					nx = (int)newX;
					ny = (int)newY;
				}
				// 파랑에서 연초록 하드코딩
				else if (abs(cy - (height / 3 * 2)) < 2 && cx >= width / 4 && cx < width / 2)
				{
					// 파랑의 x좌표 비율을 그대로 연초록의 x좌표로 전달
					float t = (cx - (width / 4.0f)) / (width / 4.0f);

					nx = (int)(width / 4.0f + t * (width / 4.0f));
					ny = (int)(height / 3.0f * 2.0f + 1); // 연초록의 맨 윗줄로 진입
				}
			}
		}

		//if (type == 2 && !IsCubeAtlasValid(nx, ny))
		//	continue;
		// 최종 유효성 검사
		if (nx < 0 || ny < 0 || nx >= width || ny >= height)
			continue;

		
		res.x = nx;
		res.y = ny;
		nextPosList.emplace_back(res);
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