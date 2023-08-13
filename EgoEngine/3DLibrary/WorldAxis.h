#pragma once

#include "../Inc/NonMesh.h"
#include "../Inc/DX11Define.h"
#include "../Inc/EgoMatrix4x4.h"
#include "../Inc/ShaderDefine.h"

// 2023/06/10 이정모 home

// 3d 공간의 기준이 되는 월드 기준 x,y,z 축

class ResourceManager;

template <typename ObjectType>
class ObjectPool;

// 원래는 IObject를 바로 상속받았으나
// mesh type에 대한 전용 builder 세팅을 위해 interface와 object 사이에 mesh type 레이어를 하나 추가함
class WorldAxis: public NonMesh
{
public:
	using MeshType = MeshType;

public:
	WorldAxis(ResourceManager* pResourceManager,
		unsigned int handleDevice,
		unsigned int handleDeviceContext,
		unsigned int handleVertexShader,
		unsigned int handleInputLayout,
		unsigned int handlePixelShader,
		unsigned int handleVertexBuffer,
		unsigned int handleIndexBuffer,
		unsigned int handleConstantBuffer,
		unsigned int indexBufferSize
		// world axis 역시 texture가 필요 없는 non mesh
	);
	~WorldAxis();

public:
	void Initialize();
	void Finalize();

public:
	void Update(float dt, EgoMath::EgoMatrix4x4 viewTM, EgoMath::EgoMatrix4x4 projTM);
	void Render();

public:
	void* operator new(size_t size, ObjectPool<WorldAxis>* pObjectPool);
	void operator delete(void* returnAddress, ObjectPool<WorldAxis>* pObjectPool);
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

