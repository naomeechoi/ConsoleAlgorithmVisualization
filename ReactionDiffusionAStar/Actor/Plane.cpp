#include "Plane.h"
#include "Renderer/Renderer.h"
#include "Texture/PlaneTexture.h"
#include "Util/Random.h"

using namespace RenderEngine;

Plane::Plane()
{
	texture = new PlaneTexture(
		Renderer::Get().GetScreenWidth(),
		Renderer::Get().GetScreenHeight());
}

Plane::~Plane()
{
}

void Plane::BeginPlay()
{
	Actor::BeginPlay();
}

void Plane::Tick(float deltaTime)
{
	Actor::Tick(deltaTime);
}

void Plane::Draw()
{
	Actor::Draw();
}

void Plane::OnDestroy()
{
	Actor::OnDestroy();
}

std::function<float(int, int, int, int)> Plane::MakeHeuristic()
{
	return [](int sx, int sy, int ex, int ey) -> float
	{
			return static_cast<float>((abs(sx - ex) + abs(sy - ey)));
	};
}

std::function<std::vector<std::vector<float>>(int, int)> Plane::MakeGetNextPosList()
{
	return [](int x, int y) -> std::vector<std::vector<float>> {
		std::vector<std::vector<float>> list;
		// 직선 4방향
		std::vector<std::vector<int>> straight = { {-1,0}, {1,0}, {0,-1}, {0,1} };
		for (auto& off : straight)
		{
			int nx = x + off[0];
			int ny = y + off[1];
			if (nx < 0 || ny < 0 || 
				nx >= Renderer::Get().GetScreenWidth() ||
				ny >= Renderer::Get().GetScreenHeight())
				continue;

			list.push_back({ static_cast<float>(nx), static_cast<float>(ny), 1.0f });
		}

		// 대각선 4방향
		std::vector<std::vector<int>> diagonal = { {-1,-1}, {-1,1}, {1,-1}, {1,1} };
		for (auto& off : diagonal)
		{
			int nx = x + off[0];
			int ny = y + off[1];
			if (nx < 0 || ny < 0 ||
				nx >= Renderer::Get().GetScreenWidth() ||
				ny >= Renderer::Get().GetScreenHeight())
				continue;
			list.push_back({ static_cast<float>(nx), static_cast<float>(ny), 1.414f });
		}

		return list;
	};
}

std::function<std::pair<int, int>()> Plane::MakeGetAStarPos()
{
	return []() -> std::pair<int, int> {
		return std::make_pair(
			Random::Random(0, Renderer::Get().GetScreenWidth() - 1),
			Random::Random(0, Renderer::Get().GetScreenHeight() - 1));
	};
}
