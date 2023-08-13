#pragma once

#include "../Inc/Mesh.h"
#include "../Inc/DX11Define.h"
#include "../Inc/EgoMatrix4x4.h"
#include "../Inc/ShaderDefine.h"

// 2023 07 13 이정모 home

// 메시가 있는 오브젝트를 정의하는 클래스
// 현재는 mesh object class가 object 하나하나에 대응되도록 만들었지만
// 나중에는 mesh object를 렌더링하기 위해 필요한 자원 VB, IB, shader 등을 로드하는 class로 변경해서
// 모든 mesh object의 자원이 이 MeshObject class에 돌아가면서 올라오고 그려지는 방식으로 가야한다.

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

	// 모든 오브젝트가 update()에서 자신이 참조하는 constant buffer를 갱신하는데
	// 현재 모든 오브젝트가 같은 constant buffer를 사용하고 있기 때문에 가장 마지막에 update한 constant buffer로 물체를 그리고 있었다.
	// 멤버 변수로 constant buffer를 정의해두고서 SetConstantBuffers()를 호출하기 바로 직전에 UpdateSubresource()를 호출해보자
	ConstantBufferWVP mConstantBuffer;
};

