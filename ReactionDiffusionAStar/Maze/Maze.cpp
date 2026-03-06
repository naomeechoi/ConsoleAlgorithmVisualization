#define NOMINMAX
#include "Maze.h"
#include <algorithm>
#include <fstream>
#include <Windows.h>

using std::vector;
using std::string;

Maze::ReactionDiffusion::ReactionDiffusion(int width, int height)
	:width(width), height(height), curA(width * height, 1.0), curB(width * height, 0.0), nextA(width* height), nextB(width* height)
{
	Params initial;
	currentParams.store(initial);
	LoadParams();

	paramsThread = std::thread(&Maze::ReactionDiffusion::WatchParamsThread, this);
}

Maze::ReactionDiffusion::~ReactionDiffusion()
{
	running = false;
	if (paramsThread.joinable()) {
		paramsThread.join();
	}
}

void Maze::ReactionDiffusion::SetRandomSeed()
{
	curA.assign(width * height, 1.0);
	curB.assign(width * height, 0.0);
	float makeBPercentage = 0.10f;
	for (int i = 0; i < static_cast<int>(width * height * makeBPercentage); )
	{
		int x = rand() % width;
		int y = rand() % height;
		int idx = y * width + x;
		if (curB[idx] == 0.0)
		{
			curB[idx] = 1.0;
			i++;
		}
	}
}

int Maze::ReactionDiffusion::GetIdx(int x, int y)
{
	//return y * width + x;
	return ((y + height) % height) * width + ((x + width) % width);
}

void Maze::ReactionDiffusion::LoadParams()
{
	std::ifstream file("Params.txt");
	if (!file.is_open())
		return;

	Params prevParams = currentParams.load();
	Params newParams;
	string line;
	constexpr double EPS = 1e-6;
	while (std::getline(file, line))
	{
		auto pos = line.find('=');
		if (pos == string::npos)
			continue;

		string key = std::move(line.substr(0, pos));
		double value = std::stod(std::move(line.substr(pos + 1)));
		if (key == "DA")
		{
			newParams.DA = value;
		}
		else if (key == "DB")
		{
			newParams.DB = value;
		}
		else if (key == "FEED")
		{
			newParams.FEED = value;
		}
		else if (key == "KILL")
		{
			newParams.KILL = value;
		}
		else if (key == "DT")
		{
			newParams.DT = value;
		}
	}

	if (std::abs(prevParams.DA - newParams.DA) > EPS) isReGenerate.store(true);
	if (std::abs(prevParams.DB - newParams.DB) > EPS) isReGenerate.store(true);
	if (std::abs(prevParams.FEED - newParams.FEED) > EPS) isReGenerate.store(true);
	if (std::abs(prevParams.KILL - newParams.KILL) > EPS) isReGenerate.store(true);
	if (std::abs(prevParams.DT - newParams.DT) > EPS) isReGenerate.store(true);

	currentParams.exchange(newParams);
}

void Maze::ReactionDiffusion::WatchParamsThread()
{
	while (running)
	{
		LoadParams();
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}
}

void Maze::ReactionDiffusion::Update()
{
	Params p = currentParams.load();

	if (isReGenerate.exchange(false))
	{
		SetRandomSeed();
	}

	for (int i = 0; i < width * height; i++)
	{
		int x = i % width;
		int y = i / width;
		double a = curA[i];
		double b = curB[i];

		double lapA = (curA[GetIdx(x + 1, y)] + curA[GetIdx(x - 1, y)] + curA[GetIdx(x, y + 1)] + curA[GetIdx(x, y - 1)]) * 0.2
			+ (curA[GetIdx(x + 1, y + 1)] + curA[GetIdx(x - 1, y - 1)] + curA[GetIdx(x - 1, y + 1)] + curA[GetIdx(x + 1, y - 1)]) * 0.05
			- a;

		double lapB = (curB[GetIdx(x + 1, y)] + curB[GetIdx(x - 1, y)] + curB[GetIdx(x, y + 1)] + curB[GetIdx(x, y - 1)]) * 0.2
			+ (curB[GetIdx(x + 1, y + 1)] + curB[GetIdx(x - 1, y - 1)] + curB[GetIdx(x - 1, y + 1)] + curB[GetIdx(x + 1, y - 1)]) * 0.05
			- b;

		double abb = a * b * b;

		// subDT 사용
		double nextValA = a + (p.DA * lapA - abb + p.FEED * (1.0 - a)) * p.DT;
		double nextValB = b + (p.DB * lapB + abb - (p.KILL + p.FEED) * b) * p.DT;

		nextA[i] = std::max(0.0, std::min(1.0, nextValA));
		nextB[i] = std::max(0.0, std::min(1.0, nextValB));
	}

	{
		std::lock_guard<std::mutex> lock(dataMutex);

		// 매 서브스텝마다 데이터를 교체해줘야 다음 계산이 가능함
		// 이 때는 외부에 보여줄 필요가 없으므로 lock 없이 내부 swap
		curA.swap(nextA);
		curB.swap(nextB);
		nextA = curA;
		nextB = curB;
	}
}

void Maze::ReactionDiffusion::Submit(string& submitBuf)
{
	std::lock_guard<std::mutex> lock(dataMutex);
	for (int i = 0; i < width * height; i++)
	{
		double d = curB[i];
		submitBuf[i] = (d > 0.35) ? '&' : (d > 0.25) ? '#' : (d > 0.185) ? '*' : (d > 0.10) ? '.' : ' ';
	}
}

Maze::Maze(int width, int height)
{
	rdSystem = new ReactionDiffusion(width, height);
	rdSystem->SetRandomSeed();
}

Maze::~Maze()
{
	if (rdSystem)
		delete rdSystem;
	rdSystem = nullptr;
}

void Maze::Upadate(float deltaTime)
{
	if (!ValidCheck())
		return;

	rdSystem->Update();
}

void Maze::Submit(std::string& submitBuf)
{
	if (!ValidCheck())
		return;

	rdSystem->Submit(submitBuf);	
}

bool Maze::ValidCheck()
{
	if (!rdSystem)
		return false;
	return true;
}
