#pragma once
#include <vector>
#include <string>
#include <mutex>

class Maze
{
	struct ReactionDiffusion
	{
		struct Params
		{
			double DA = 0.16;  // 화학물질 A의 확산 속도
			double DB = 0.08;  // 화학물질 B의 확산 속도
			double FEED = 0.035; // 패턴 형성을 위한 공급률
			double KILL = 0.060; // 패턴 형성을 위한 제거율
			double DT = 1.0;     // 프레임당 시뮬레이션 시간 변화량
		};

		ReactionDiffusion(int width, int height);
		~ReactionDiffusion();

		void SetRandomSeed();
		int GetIdx(int x, int y);
		void LoadParams();
		void WatchParamsThread();
		void Update();
		void Submit(std::string& submitBuf);

		int width = 0;
		int height = 0;
		std::vector<double> curA;
		std::vector<double> curB;
		std::vector<double> nextA;
		std::vector<double> nextB;
		
		std::atomic<Params> currentParams;
		std::thread paramsThread;
		std::atomic<bool> running = true;

		std::atomic<bool> isReGenerate = false;
		std::mutex dataMutex;
	};

public:
	Maze(int width, int height);
	~Maze();

	void Upadate(float deltaTime);
	void Submit(std::string& summitBuf);

private:
	bool ValidCheck();

private:
	ReactionDiffusion* rdSystem;
};

