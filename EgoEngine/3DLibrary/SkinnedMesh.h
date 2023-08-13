#pragma once

#include "../Inc/IObject.h"
#include "../Inc/EgoMatrix4x4.h"

// 2023 06 30 이정모 home

// 생성하는 오브젝트가 Non Mesh, Mesh, Skinned Mesh인지 분리하여
// 그에 맞는 builder class를 생성하기 위함
// IObject와 Object class 사이에 존재

class SkinnedMesh : public IObject
{
public:
	using MeshType = SkinnedMesh;

public:
	SkinnedMesh() {}
	~SkinnedMesh() {}

public:
	virtual void Initialize() = 0;
	virtual void Finalize() = 0;

public:
	virtual void Update(float dt, EgoMath::EgoMatrix4x4 viewTM, EgoMath::EgoMatrix4x4 projTM) = 0;
	virtual void Render() = 0;
};