#pragma once

#include "../Inc/EgoMatrix4x4.h"

// 2023/06/10 ������ home

// ���� ������Ʈ�� interface
// �߻�ȭ�ؼ� �θ� �����ͷ� ����

class IObject
{
public:
	IObject() {}
	virtual ~IObject() = 0 {}

public:
	virtual void Initialize() = 0;
	virtual void Finalize() = 0;

public:
	virtual void Update(float dt, EgoMath::EgoMatrix4x4 viewTM, EgoMath::EgoMatrix4x4 projTM) = 0;
	virtual void Render() = 0;
};