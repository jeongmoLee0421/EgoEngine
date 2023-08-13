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

// ������Ʈ �ʱ�ȭ �� ���ҽ� �ε� ���� �����ɸ��� ù��° tick(Initialize)�� �ι�° tick(first update)�� ���̰� Ŀ���µ�
// �̴� ù �����ӿ� dt�� ������ �޴� ������Ʈ���� �ѹ��� �ʹ� ���� �����̴� ����� �߻���Ŵ
// timer�� ���½��Ѽ� prevTick�� �ֽ�ȭ ��������
void Timer::Reset()
{
	QueryPerformanceCounter((LARGE_INTEGER*)&m_prevTick);
}

float Timer::GetDeltaTime()
{
	return m_deltaTime;
}
