#pragma once

#include "../Inc/IObject.h"
#include "../Inc/EgoMatrix4x4.h"

// 2023 07 01 ������ home

// �����ϴ� ������Ʈ�� Non Mesh, Mesh, Skinned Mesh���� �и��Ͽ�
// �׿� �´� builder class�� �����ϱ� ����
// IObject�� Object class ���̿� ����

class NonMesh : public IObject
{
public:
	using MeshType = NonMesh;

public:
	NonMesh() {}
	~NonMesh() {}

public:
	virtual void Initialize() = 0;
	virtual void Finalize() = 0;

public:
	virtual void Update(float dt, EgoMath::EgoMatrix4x4 viewTM, EgoMath::EgoMatrix4x4 projTM) = 0;
	virtual void Render() = 0;
};