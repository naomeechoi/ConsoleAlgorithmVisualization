#pragma once
#include <vector>
#include <Windows.h>
class AStar
{
	enum Face
	{
		FACE_X,
		FACE_NEG_X,
		FACE_Y,
		FACE_NEG_Y,
		FACE_Z,
		FACE_NEG_Z,
	};

	struct Node
	{
		Node(int x, int y, float g, float h, Node* parent, int face = 0)
			:x(x), y(y), g(g), h(h), parent(parent), face(face)
		{
		};

		float f() const { return g + h; }

		int x = 0;
		int y = 0;
		int face = 0;
		float g = 0.0f;
		float h = 0.0f;
		Node* parent = nullptr;
	};

	struct NextPos
	{
		int x = 0;
		int y = 0;
		int face = 0;
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
	bool IsCubeAtlasValid(int x, int y) const;
private:
	std::tuple<float, float, float>  AtlasTo3D(int x, int y);
	int faceSize() const { return std::min(width / 4, height / 3); }
	int GetFace(int x, int y);
	float Heuristic(int sx, int sy, int ex, int ey);
	bool ValidateTransition(int fromX, int fromY, int toX, int toY);

	void GetNextPosListAndMinCost(std::pair<int, int> curPos, std::vector<AStar::NextPos>& nextPosList);

private:
	int width = 0;
	int height = 0;
	int type = 0;

	std::vector<std::vector<int>> cubeOffsets = {
			   {2, 1}, // FACE_X
			   {0, 1}, // FACE_NEG_X
			   {1, 0}, // FACE_Y
			   {1, 2}, // FACE_NEG_Y
			   {1, 1}, // FACE_Z
			   {3, 1}  // FACE_NEG_Z
	};
};