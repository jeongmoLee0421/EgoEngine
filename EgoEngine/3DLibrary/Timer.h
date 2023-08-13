#pragma once

#include <Windows.h>

// 2023 06 11 ������ home
// �� ƽ�� �󸶸�ŭ�� �ð��� ����ߴ��� ����ϴ� Ÿ�̸�

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
	long long m_frequency; // 1�ʴ� ƽ�� �󸶳� �Ǵ°�?
	long long m_prevTick;
	long long m_currTick;
	float m_deltaTime;
};

