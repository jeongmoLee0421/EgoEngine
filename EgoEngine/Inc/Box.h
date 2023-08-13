#pragma once

#include "../Inc/Mesh.h"
#include "../Inc/DX11Define.h"
#include "../Inc/EgoMatrix4x4.h"
#include "../Inc/ShaderDefine.h"

// 2023 06 28 이정모 home

// 텍스처를 입혀준 박스

// 이전까지 텍스처를 사용하는 오브젝트가 없었고
// box object 생성자에는 Texture를 위한 매개변수가 추가되었음.

class ResourceManager;

template <typename ObjectType>
class ObjectPool;

class Box : public Mesh
{
	// 부모 클래스에서 MeshType이 무엇인지 using문을 사용해서 재정의 했고
	// 자식 클래스에서 부모의 MeshType을 가져와서 같은 이름으로 using문을 사용해 재정의 했음
	// 본인의 mesh type을 밖에서도 참조할 수 있도록 using문을 통해 타입을 재정의한 것이 핵심임
	// 이를 통해 Object Type을 알면 object가 상속받은 MeshType을 가져와서 전용 builder를 생성할 수 있음
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
		
		// box는 mesh object로 texture와 sampler를 사용
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

