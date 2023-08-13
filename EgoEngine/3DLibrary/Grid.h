#pragma once

#include "../Inc/NonMesh.h"
#include "../Inc/DX11Define.h"
#include "../Inc/EgoMatrix4x4.h"
#include "../Inc/ShaderDefine.h"

// 2023 06 11 ������ home

// 3d �������� ������ �Ǵ� ����

class ResourceManager;

template <typename ObjectType>
class ObjectPool;

class Grid : public NonMesh
{
	// ������ ��� ���� mesh type�� ������ �����ν� �ܺο��� �̸� ������ �� �ֵ��� using���� ��������
	// �̸� ���� object type�� mesh type�� �´� ���� builder�� ������ �� �ְԵ�
public:
	using MeshType = MeshType;

public:
	Grid(ResourceManager* pResourceManager,
		unsigned int handleDevice,
		unsigned int handleDeviceContext,
		unsigned int handleVertexShader,
		unsigned int handleInputLayout,
		unsigned int handlePixelShader,
		unsigned int handleVertexBuffer,
		unsigned int handleIndexBuffer,
		unsigned int handleConstantBuffer,
		unsigned int indexBufferSize
		// grid�� non mesh type�̶� texture�� sampler�� ���� �ʿ䰡 ����
	);
	~Grid();

public:
	void Initialize();
	void Finalize();

public:
	void Update(float dt, EgoMath::EgoMatrix4x4 viewTM, EgoMath::EgoMatrix4x4 projTM);
	void Render();

	// object pool�� �̸� �Ҵ�� �ּҿ� ����/�Ҹ��� �ϱ� ����
	// new, delete ������ �����ε�
public:
	// class user-define new()
	void* operator new(size_t size, ObjectPool<Grid>* pObjectPool);

	// class user-define delete()
	// operator new() ȣ�� �� �����ڿ��� ���ܰ� �߻��ϸ� ¦�� �´� �� delete()�� ȣ��ǰ�
	void operator delete(void* returnAddress, ObjectPool<Grid>* pObjectPool);

	// class usual delete()
	// ���������� �ʱ�ȭ�ǰ� �Ҹ�Ǹ� �� delete()�� ȣ��
	void operator delete(void* returnAddress);

private:
	ResourceManager* m_pResourceManager;

private:
	unsigned int m_handleDevice;
	unsigned int m_handleDeviceContext;

	unsigned int m_handleVertexShader;
	unsigned int m_handlePixelShader;
	unsigned int m_handleInputLayout;

	unsigned int m_handleVertexBuffer;
	unsigned int m_handleIndexBuffer;
	unsigned int m_handleConstantBuffer;
	unsigned int m_indexBufferSize;

private:
	DirectX::XMMATRIX m_worldTM;
	DirectX::XMMATRIX m_viewTM;
	DirectX::XMMATRIX m_projTM;
	ConstantBufferWVP mConstantBuffer;
};

