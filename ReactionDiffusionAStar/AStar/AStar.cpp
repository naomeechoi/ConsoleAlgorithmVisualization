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

std::tuple<float, float, float> AStar::AtlasTo3D(int x, int y)
{
	int face = GetFace(x, y);
	int fs = faceSize();
	int lx = x % fs;
	int ly = y % fs;

	// [0,fs-1] -> [-1, 1] 범위로 변환
	float u = (float)lx / (fs - 1) * 2.0f - 1.0f;
	float v = (float)ly / (fs - 1) * 2.0f - 1.0f;

	// 각 면이 3D 큐브 모서리에서 완벽히 맞닿도록 v 부호 수정 (-v 적용)
	switch (face) {
	case 0: return { 1.0f, -v, -u };     // FACE_X
	case 1: return { -1.0f, -v, u };     // FACE_NEG_X  
	case 2: return { u, 1.0f, v };       // FACE_Y
	case 3: return { u, -1.0f, -v };     // FACE_NEG_Y
	case 4: return { u, -v, 1.0f };      // FACE_Z
	case 5: return { -u, -v, -1.0f };    // FACE_NEG_Z
	default: return { 0, 0, 0 };
	}
}
bool AStar::ValidateTransition(int fromX, int fromY, int toX, int toY)
{
	if (type != 2) return true; // 큐브 모드가 아니면 항상 유효

	auto [x1, y1, z1] = AtlasTo3D(fromX, fromY);
	auto [x2, y2, z2] = AtlasTo3D(toX, toY);

	// 3D 공간에서 실제 거리 계산
	float dist3D = sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1) + (z2 - z1) * (z2 - z1));

	// 인접한 픽셀은 3D에서도 가까워야 함 (임계값 조정 가능)
	return dist3D < 0.5f;
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
		int nx = cx + list[i][0];
		int ny = cy + list[i][1];

		// 아틀라스 범위를 벗어나거나 '빈 공간(Invalid)'으로 가는 경우 면 전환 처리
		if (!IsCubeAtlasValid(nx, ny))
		{
			switch (curFace)
			{
			case 1: // FACE_NEG_X (0, 1)
				if (nx < 0) { nx = 4 * fs - 1; ny = cy; } // 왼쪽 -> -Z 오른쪽 끝
				else if (ny < fs) { nx = fs; ny = lx; } // 위 -> Y 왼쪽 끝
				else if (ny >= 2 * fs) { nx = fs; ny = 3 * fs - 1 - lx; } // 아래 -> -Y 왼쪽 끝
				break;

			case 4: // FACE_Z (1, 1)
				// 정면은 상하좌우가 모두 유효한 면으로 둘러싸여 있어 대각선 이탈만 발생함
				continue;

			case 0: // FACE_X (2, 1)
				if (nx >= 3 * fs) { nx = 3 * fs; ny = cy; } // 오른쪽 -> -Z 왼쪽 끝
				else if (ny < fs) { nx = 2 * fs - 1; ny = fs - 1 - lx; } // 위 -> Y 오른쪽 끝
				else if (ny >= 2 * fs) { nx = 2 * fs - 1; ny = 2 * fs + lx; } // 아래 -> -Y 오른쪽 끝
				break;

			case 5: // FACE_NEG_Z (3, 1)
				if (nx >= 4 * fs) { nx = 0; ny = cy; } // 오른쪽 -> -X 왼쪽 끝
				else if (ny < fs) { nx = 2 * fs - 1 - lx; ny = 0; } // 위 -> Y 위쪽 끝
				else if (ny >= 2 * fs) { nx = 2 * fs - 1 - lx; ny = 3 * fs - 1; } // 아래 -> -Y 아래쪽 끝
				break;

			case 2: // FACE_Y (1, 0)
				if (ny < 0) { nx = 4 * fs - 1 - lx; ny = fs; } // 위 -> -Z 위쪽 끝 (뒤집힘)
				else if (nx < fs) { nx = ly; ny = fs; } // 왼쪽 -> -X 위쪽 끝
				else if (nx >= 2 * fs) { nx = 2 * fs - 1 - ly; ny = fs; } // 오른쪽 -> X 위쪽 끝
				break;

			case 3: // FACE_NEG_Y (1, 2)
				if (ny >= 3 * fs) { nx = 4 * fs - 1 - lx; ny = 2 * fs - 1; } // 아래 -> -Z 아래쪽 끝
				else if (nx < fs) { nx = 2 * fs - 1 - ly; ny = 2 * fs - 1; } // 왼쪽 -> -X 아래쪽 끝
				else if (nx >= 2 * fs) { nx = fs + ly; ny = 2 * fs - 1; } // 오른쪽 -> X 아래쪽 끝
				break;
			}
		}

		// 최종 유효성 검사
		if (nx < 0 || ny < 0 || nx >= width || ny >= height || !IsCubeAtlasValid(nx, ny))
			continue;

		AStar::NextPos res;
		res.x = nx;
		res.y = ny;
		res.cost = (abs(list[i][0]) == abs(list[i][1])) ? 1.414f : 1.0f;
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