#pragma once

#include "../Inc/IObject.h"
#include "../Inc/EgoMatrix4x4.h"

// 2023 06 30 ������ home

// �����ϴ� ������Ʈ�� Non Mesh, Mesh, Skinned Mesh���� �и��Ͽ�
// �׿� �´� builder class�� �����ϱ� ����
// IObject�� Object class ���̿� ����

class Mesh : public IObject
{
	// Mesh�� ��ӹ��� object�� MeshType�̶� �����ǵ� �̸����� �θ��� Mesh Type�� ���� ����
public:
	using MeshType = Mesh;

public:
	Mesh() {}
	~Mesh() {}

public:
	virtual void Initialize() = 0;
	virtual void Finalize() = 0;

public:
	virtual void Update(float dt, EgoMath::EgoMatrix4x4 viewTM, EgoMath::EgoMatrix4x4 projTM) = 0;
	virtual void Render() = 0;
};