#pragma once

#include "../Inc/EgoMatrix4x4.h"

// 2023/06/10 이정모 home

// 게임 오브젝트의 interface
// 추상화해서 부모 포인터로 관리

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