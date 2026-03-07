#define NOMINMAX
#include "Maze.h"
#include <algorithm>
#include <fstream>
#include <cmath>
#include <ctime>

using std::vector;
using std::string;
const int MAX_SIMULATIOIN_STEPS = 500;
const float MAX_TRANSITION_DURATION = 2.5f;

static void InitialRand()
{
	static bool initialized = false;
	if (initialized)
		return;
	
	srand(static_cast<unsigned int>(std::time(0)));
	initialized = true;
}

static int RandomIndex(int min, int max)
{
	InitialRand();
	return min + rand() % (max - min + 1);
}

Maze::ReactionDiffusion::ReactionDiffusion(int width, int height)
	:width(width), height(height), curA(width * height, 1.0), curB(width * height, 0.0), nextA(width* height), nextB(width* height)
{
	Params initial;
	currentParams.store(initial);
	LoadParams("Params.txt");

	paramsThread = std::thread(&Maze::ReactionDiffusion::WatchParamsThread, this);
}

Maze::ReactionDiffusion::~ReactionDiffusion()
{
	running = false;
	if (paramsThread.joinable()) {
		paramsThread.join();
	}
}

void Maze::ReactionDiffusion::SetRandomParams()
{
	std::lock_guard<std::mutex> lock(paramsMutex);
	if (!paramsList.empty())
	{
		currentParams.store(paramsList[RandomIndex(0, paramsList.size() - 1)]);
	}
	else
	{
		currentParams.store(Params());
	}
}

void Maze::ReactionDiffusion::SetRandomSeed()
{
	SetRandomParams();

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

bool Maze::ReactionDiffusion::IsFileUpdated(const string& path)
{
	auto ftime = std::filesystem::last_write_time(path);
	if (ftime != lastWriteTime)
	{
		lastWriteTime = ftime;
		return true; // 파일이 변경됨
	}
	return false; // 변경 없음
}

void Maze::ReactionDiffusion::LoadParams(const std::string& path)
{
	std::ifstream file("Params.txt");
	if (!file.is_open())
		return;

	std::vector<Params> newParamsList;
	string line;

	Params currentCase;
	while (std::getline(file, line))
	{
		if (line.empty() || line[0] == '#')
			continue;

		auto pos = line.find('=');
		if (pos == string::npos)
			continue;

		string key = line.substr(0, pos);
		double value = std::stod(line.substr(pos + 1));

		if (key == "DA")       currentCase.DA = value;
		else if (key == "DB")  currentCase.DB = value;
		else if (key == "FEED") currentCase.FEED = value;
		else if (key == "KILL") currentCase.KILL = value;
		else if (key == "DT")   currentCase.DT = value;

		if (key == "DT")
		{
			newParamsList.push_back(currentCase);
			currentCase = Params();
		}
	}

	if (!newParamsList.empty())
	{
		std::lock_guard<std::mutex> lock(paramsMutex);
		paramsList = std::move(newParamsList);
		isReGenerate.store(true);
	}
}

void Maze::ReactionDiffusion::WatchParamsThread()
{
	while (running)
	{
		if (IsFileUpdated("Params.txt"))
		{
			LoadParams("Params.txt");
			std::this_thread::sleep_for(std::chrono::milliseconds(500));
		}
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

const std::vector<double>& Maze::ReactionDiffusion::GetConcentration()
{
	std::lock_guard<std::mutex> lock(dataMutex);
	return curB;
}

Maze::Maze(int width, int height)
	: width(width), height(height)
{
	rdSystem1 = new ReactionDiffusion(width, height);
	rdSystem1->SetRandomSeed();

	rdSystem2 = new ReactionDiffusion(width, height);
	rdSystem2->SetRandomSeed();

	mixedConcentration = vector<double>(width * height, 0.0);
}

Maze::~Maze()
{
	if (rdSystem1)
		delete rdSystem1;
	rdSystem1 = nullptr;

	if (rdSystem2)
		delete rdSystem2;
	rdSystem2 = nullptr;
}

void Maze::Update(float deltaTime)
{
	if (!ValidCheck())
		return;

	rdSystem1->Update();
	rdSystem2->Update();
	MixRdSystem(deltaTime);
}

void Maze::Submit(std::vector<CHAR_INFO>& submitBuf)
{
	if (!ValidCheck())
		return;

	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			int i = y * width + x;

			double d = mixedConcentration[i];
			//double light = ComputeLighting(x, y);

			//double shade = d * 0.7 + light * 0.3;

			char c;
			int color;
			if (d > 0.35)
			{
				c = '&';
				color = 15;
			}
			else if (d > 0.25)
			{
				c = '#';
				color = 14;
			}
			else if (d > 0.185)
			{
				c = '*';
				color = 11;
			}
			else if (d > 0.10)
			{
				c = '.';
				color = 8;
			}
			else
			{
				c = ' ';
				color = 0;
			}

			submitBuf[i].Char.AsciiChar = c;
			submitBuf[i].Attributes = color;
		}
	}
}

double Maze::ComputeLighting(int x, int y)
{
	int idx = y * width + x;

	double hL = mixedConcentration[rdSystem1->GetIdx(x - 1, y)];
	double hR = mixedConcentration[rdSystem1->GetIdx(x + 1, y)];
	double hD = mixedConcentration[rdSystem1->GetIdx(x, y - 1)];
	double hU = mixedConcentration[rdSystem1->GetIdx(x, y + 1)];

	double dx = hR - hL;
	double dy = hU - hD;

	// 간단한 light direction
	double lightX = -0.6;
	double lightY = -0.6;
	double lightZ = 0.5;

	double normalZ = 1.0;

	double dot = dx * lightX + dy * lightY + normalZ * lightZ;

	return std::max(0.0, std::min(1.0, dot));
}

void Maze::MixRdSystem(float deltaTime)
{
	if (!ValidCheck())
		return;

	simulationSteps++;
	if (simulationSteps >= MAX_SIMULATIOIN_STEPS && !transitioning)
	{
		transitioning = true;
		transitionTime = 0.0f;
	}

	std::vector<double> concentration1 = rdSystem1->GetConcentration();
	if (transitioning)
	{
		transitionTime += deltaTime;
		float t = transitionTime / MAX_TRANSITION_DURATION;

		std::vector<double> concentration2 = rdSystem2->GetConcentration();
		for (int i = 0; i < width * height; i++)
		{
			mixedConcentration[i] = concentration1[i] * (1.0f - t) + concentration2[i] * t;
		}

		if (t >= 1.0f)
		{
			std::swap(rdSystem1, rdSystem2);
			simulationSteps = 0;
			transitioning = false;
			transitionTime = 0.0f;
			rdSystem2->SetRandomSeed();
		}
	}
	else
	{
		for (int i = 0; i < width * height; i++)
		{
			mixedConcentration[i] = concentration1[i];
		}
	}
}

bool Maze::ValidCheck()
{
	if (!rdSystem1)
		return false;

	if (!rdSystem2)
		return false;

	return true;
}
