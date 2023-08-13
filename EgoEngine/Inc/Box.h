#pragma once

#include "../Inc/Mesh.h"
#include "../Inc/DX11Define.h"
#include "../Inc/EgoMatrix4x4.h"
#include "../Inc/ShaderDefine.h"

// 2023 06 28 ������ home

// �ؽ�ó�� ������ �ڽ�

// �������� �ؽ�ó�� ����ϴ� ������Ʈ�� ������
// box object �����ڿ��� Texture�� ���� �Ű������� �߰��Ǿ���.

class ResourceManager;

template <typename ObjectType>
class ObjectPool;

class Box : public Mesh
{
	// �θ� Ŭ�������� MeshType�� �������� using���� ����ؼ� ������ �߰�
	// �ڽ� Ŭ�������� �θ��� MeshType�� �����ͼ� ���� �̸����� using���� ����� ������ ����
	// ������ mesh type�� �ۿ����� ������ �� �ֵ��� using���� ���� Ÿ���� �������� ���� �ٽ���
	// �̸� ���� Object Type�� �˸� object�� ��ӹ��� MeshType�� �����ͼ� ���� builder�� ������ �� ����
public:
	using MeshType = MeshType; // Box::MeshType = Mesh::MeshType

public:
	Box(ResourceManager* pResourceManager,
		unsigned int handleDevice,
		unsigned int handleDeviceContext,
		unsigned int handleVertexShader,
		unsigned int handleInputLayout,
		unsigned int handlePixelShader,
		unsigned int handleVertexBuffer,
		unsigned int handleIndexBuffer,
		unsigned int handleConstantBuffer,
		unsigned int indexBufferSize,
		
		// box�� mesh object�� texture�� sampler�� ���
		unsigned int handleTexture,
		unsigned int handleSampler);
	~Box();

public:
	void Initialize();
	void Finalize();

public:
	void Update(float dt, EgoMath::EgoMatrix4x4 viewTM, EgoMath::EgoMatrix4x4 projTM);
	void Render();

public:
	void* operator new(size_t size, ObjectPool<Box>* pObjectPool);
	void operator delete(void* returnAddress, ObjectPool<Box>* pObjectPool);
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

	unsigned int mHandleTexture;
	unsigned int mHandleSampler;

private:
	DirectX::XMMATRIX m_worldTM;
	DirectX::XMMATRIX m_viewTM;
	DirectX::XMMATRIX m_projTM;

	ConstantBufferWVP mConstantBuffer;
	
	float mYRotateAngle;
	float mRotateSpeed;
};

