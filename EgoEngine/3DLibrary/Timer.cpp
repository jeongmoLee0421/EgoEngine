#include "Timer.h"

Timer::Timer()
	: m_frequency(0)
	, m_prevTick(0)
	, m_currTick(0)
	, m_deltaTime(0.f)
{
}

Timer::~Timer()
{
}

void Timer::Initialize()
{
	QueryPerformanceFrequency((LARGE_INTEGER*)&m_frequency);
	QueryPerformanceCounter((LARGE_INTEGER*)&m_prevTick);
}

void Timer::Finalize()
{
}

void Timer::Update()
{
	QueryPerformanceCounter((LARGE_INTEGER*)&m_currTick);

	long long tickDiff = m_currTick - m_prevTick;
	float deltaTime = static_cast<float>(tickDiff) / m_frequency;

	m_prevTick = m_currTick;
	m_deltaTime = deltaTime;
}

// 오브젝트 초기화 및 리소스 로드 등이 오래걸리면 첫번째 tick(Initialize)과 두번째 tick(first update)의 차이가 커지는데
// 이는 첫 프레임에 dt의 영향을 받는 오브젝트들이 한번에 너무 많이 움직이는 결과를 발생시킴
// timer를 리셋시켜서 prevTick을 최신화 시켜주자
void Timer::Reset()
{
	QueryPerformanceCounter((LARGE_INTEGER*)&m_prevTick);
}

float Timer::GetDeltaTime()
{
	return m_deltaTime;
}
