#pragma once

#include "../Inc/IObject.h"
#include "../Inc/EgoMatrix4x4.h"

// 2023 06 30 ������ home

// �����ϴ� ������Ʈ�� Non Mesh, Mesh, Skinned Mesh���� �и��Ͽ�
// �׿� �´� builder class�� �����ϱ� ����
// IObject�� Object class ���̿� ����

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