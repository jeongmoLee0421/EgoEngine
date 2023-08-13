#pragma once

#include "../Inc/Mesh.h"
#include "../Inc/DX11Define.h"
#include "../Inc/EgoMatrix4x4.h"
#include "../Inc/ShaderDefine.h"

// 2023 07 13 ������ home

// �޽ð� �ִ� ������Ʈ�� �����ϴ� Ŭ����
// ����� mesh object class�� object �ϳ��ϳ��� �����ǵ��� ���������
// ���߿��� mesh object�� �������ϱ� ���� �ʿ��� �ڿ� VB, IB, shader ���� �ε��ϴ� class�� �����ؼ�
// ��� mesh object�� �ڿ��� �� MeshObject class�� ���ư��鼭 �ö���� �׷����� ������� �����Ѵ�.

class ResourceManager;

template <typename ObjectType>
class ObjectPool;

class MeshObject : public Mesh
{
public:
	using MeshType = MeshType;

public:
	MeshObject(ResourceManager* pResourceManager,
		unsigned int handleDevice,
		unsigned int handleDeviceContext,
		unsigned int handleVertexShader,
		unsigned int handleInputLayout,
		unsigned int handlePixelShader,
		unsigned int handleVertexBuffer,
		unsigned int handleIndexBuffer,
		unsigned int handleConstantBuffer,
		unsigned int indexBufferSize,
		unsigned int handleTexture,
		unsigned int handleSampler);
	~MeshObject();

public:
	void Initialize();
	void Finalize();

public:
	void Update(float dt, EgoMath::EgoMatrix4x4 viewTM, EgoMath::EgoMatrix4x4 projTM);
	void Render();

public:
	void* operator new(size_t size, ObjectPool<MeshObject>* pObjectPool);
	void operator delete(void* returnAddress, ObjectPool<MeshObject>* pObjectPool);
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

	// ��� ������Ʈ�� update()���� �ڽ��� �����ϴ� constant buffer�� �����ϴµ�
	// ���� ��� ������Ʈ�� ���� constant buffer�� ����ϰ� �ֱ� ������ ���� �������� update�� constant buffer�� ��ü�� �׸��� �־���.
	// ��� ������ constant buffer�� �����صΰ� SetConstantBuffers()�� ȣ���ϱ� �ٷ� ������ UpdateSubresource()�� ȣ���غ���
	ConstantBufferWVP mConstantBuffer;
};

