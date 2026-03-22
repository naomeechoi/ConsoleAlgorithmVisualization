#include "Sphere.h"
#include "Renderer/Renderer.h"
#include "Texture/SphereTexture.h"
#include "Util/Random.h"

using namespace RenderEngine;

Sphere::Sphere()
{
	texture = new SphereTexture(
		Renderer::Get().GetScreenWidth(),
		Renderer::Get().GetScreenHeight());

	rot[0] = 1.0f; rot[1] = 0.0f; rot[2] = 0.0f;
	rot[3] = 0.0f; rot[4] = 1.0f; rot[5] = 0.0f;
	rot[6] = 0.0f; rot[7] = 0.0f; rot[8] = 1.0f;

	SphereTexture* curTexture = dynamic_cast<SphereTexture*>(texture);
	if (curTexture)
		curTexture->SetRotation(&rot);
	/*
	[1, 0, 0]
	[0, 1, 0]
	[0, 0, 1]
	의 행렬이다. 어떤 (3, 4, 1) 이라는 벡터에 이 행렬을 곱하면
	동일하게 (3, 4, 1)이 나온다. 즉 아무 회전도 들어가지 않은 default 회전 행렬이라고 생각하면 된다.*/
}

Sphere::~Sphere()
{
}

void Sphere::BeginPlay()
{
	Actor::BeginPlay();
}

void Sphere::Tick(float deltaTime)
{
	Actor::Tick(deltaTime);
}

void Sphere::Draw()
{
	Actor::Draw();
}

void Sphere::OnDestroy()
{
	Actor::OnDestroy();
}

std::function<float(int, int, int, int)> Sphere::MakeHeuristic()
{
	return [](int sx, int sy, int ex, int ey) -> float
		{
			return static_cast<float>((abs(sx - ex) + abs(sy - ey)));
		};
}

std::function<std::vector<std::vector<float>>(int, int)> Sphere::MakeGetNextPosList()
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

std::function<std::pair<int, int>()> Sphere::MakeGetAStarPos()
{
	return []() -> std::pair<int, int> {
		return std::make_pair(
			Random::Random(0, Renderer::Get().GetScreenWidth() - 1),
			Random::Random(0, Renderer::Get().GetScreenHeight() - 1));
		};
}

void Sphere::SetRotation(float dx, float dy)
{
	this->rot = rot;
	float cosX = cos(dx), sinX = sin(dx);
	float cosY = cos(dy), sinY = sin(dy);

	std::vector<float> prev(rot.begin(), rot.end());

	/* y축 기준 회전
	[cosθ, 0,  sinθ]
	[0,     1,   0   ]
	[−sinθ, 0, cosθ]
	 */
	rot[0] = prev[0] * cosX - prev[2] * sinX;
	rot[1] = prev[1];
	rot[2] = prev[0] * sinX + prev[2] * cosX;

	rot[3] = prev[3] * cosX - prev[5] * sinX;
	rot[4] = prev[4];
	rot[5] = prev[3] * sinX + prev[5] * cosX;

	rot[6] = prev[6] * cosX - prev[8] * sinX;
	rot[7] = prev[7];
	rot[8] = prev[6] * sinX + prev[8] * cosX;

	// 누적
	prev.assign(rot.begin(), rot.end());

	/* x축 기준 회전
	[1,  0,    0   ]
	[0,cosθ,−sinθ]
	[0,sinθ, cosθ]​
	*/
	rot[0] = prev[0];
	rot[1] = prev[1] * cosY + prev[2] * sinY;
	rot[2] = prev[1] * sinY * -1.0f + prev[2] * cosY;

	rot[3] = prev[3];
	rot[4] = prev[4] * cosY + prev[5] * sinY;
	rot[5] = prev[4] * sinY * -1.0f + prev[5] * cosY;

	rot[6] = prev[6];
	rot[7] = prev[7] * cosY + prev[8] * sinY;
	rot[8] = prev[7] * sinY * -1.0f + prev[8] * cosY;

	// 행렬곱이 많아지면 부동소수점 오차가 생길 수 있어, 이를 막기 위해 정규화 한다.
	// x축 방향 벡터, y축 방향 벡터, z축 방향 백터 각각 정규화하기 위해 3번 룹을 돈다.
	for (int i = 0; i < 3; i++) {
		int idx = i * 3;
		float len = sqrt(rot[idx] * rot[idx]
			+ rot[idx + 1] * rot[idx + 1]
			+ rot[idx + 2] * rot[idx + 2]);

		rot[idx] /= len;
		rot[idx + 1] /= len;
		rot[idx + 2] /= len;
	}

	SphereTexture*  curTexture = dynamic_cast<SphereTexture*>(texture);
	if(curTexture)
		curTexture->SetRotation(&rot);
}
