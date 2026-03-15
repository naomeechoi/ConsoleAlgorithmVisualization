#pragma once
#include <vector>
#include <string>
#include <mutex>
#include <filesystem>
#include <Windows.h>

class ReactionDiffusionSystem
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

		void SetRandomParams();

		void SetRandomSeed();
		int GetIdx(int x, int y);
		bool IsFileUpdated(const std::string& path);
		void LoadParams(const std::string& path);
		void WatchParamsThread();
		void Update();
		const std::vector<double>& GetConcentration();

		int width = 0;
		int height = 0;
		std::vector<double> curA;
		std::vector<double> curB;
		std::vector<double> nextA;
		std::vector<double> nextB;
		
		std::vector<Params> paramsList;
		std::atomic<Params> currentParams;
		std::thread paramsThread;
		std::atomic<bool> running{ true };


		std::atomic<bool> isReGenerate = false;
		std::mutex dataMutex;
		std::mutex paramsMutex;

		std::filesystem::file_time_type lastWriteTime;
	};

public:
	ReactionDiffusionSystem(int width, int height);
	~ReactionDiffusionSystem();

	void Update(float deltaTime);
	void Submit(std::vector<CHAR_INFO>& submitBuf);
	const std::vector<double>& GetConcentration() const {
		return mixedConcentration;
	}
	void TestBufferClear()
	{
		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				int idx = j + i * width;
				mixedConcentration[idx] = 0.0f;
			}
		}
	}

private:
	double ComputeLighting(int x, int y);
	void MixRdSystem(float deltaTime);
	bool ValidCheck();

private:
	int width = 0;
	int height = 0;
	std::vector<double> mixedConcentration;
	ReactionDiffusion* rdSystem1;
	ReactionDiffusion* rdSystem2;

	int simulationSteps = 0;
	bool transitioning = false;
	float transitionTime = 0.0f;
};

