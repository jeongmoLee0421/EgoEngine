#pragma once

#include "../Inc/NonMesh.h"
#include "../Inc/DX11Define.h"
#include "../Inc/EgoMatrix4x4.h"
#include "../Inc/ShaderDefine.h"

// 2023 06 11 이정모 home

// 3d 공간에서 기준이 되는 격자

class ResourceManager;

template <typename ObjectType>
class ObjectPool;

class Grid : public NonMesh
{
	// 본인이 상속 받은 mesh type을 재정의 함으로써 외부에서 이를 참조할 수 있도록 using문을 선언했음
	// 이를 통해 object type과 mesh type에 맞는 전용 builder가 생성될 수 있게됨
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
		// grid는 non mesh type이라서 texture나 sampler를 받을 필요가 없음
	);
	~Grid();

public:
	void Initialize();
	void Finalize();

public:
	void Update(float dt, EgoMath::EgoMatrix4x4 viewTM, EgoMath::EgoMatrix4x4 projTM);
	void Render();

	// object pool에 미리 할당된 주소에 생성/소멸을 하기 위한
	// new, delete 연산자 오버로딩
public:
	// class user-define new()
	void* operator new(size_t size, ObjectPool<Grid>* pObjectPool);

	// class user-define delete()
	// operator new() 호출 후 생성자에서 예외가 발생하면 짝이 맞는 이 delete()가 호출되고
	void operator delete(void* returnAddress, ObjectPool<Grid>* pObjectPool);

	// class usual delete()
	// 정상적으로 초기화되고 소멸되면 이 delete()가 호출
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

