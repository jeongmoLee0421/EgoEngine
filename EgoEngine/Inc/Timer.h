#pragma once

#include <Windows.h>

// 2023 06 11 이정모 home
// 한 틱당 얼마만큼의 시간이 경과했는지 계산하는 타이머

class Timer
{
public:
	Timer();
	~Timer();

public:
	void Initialize();
	void Finalize();

	void Update();
	void Reset();
	float GetDeltaTime();

private:
	long long m_frequency; // 1초당 틱이 얼마나 되는가?
	long long m_prevTick;
	long long m_currTick;
	float m_deltaTime;
};

