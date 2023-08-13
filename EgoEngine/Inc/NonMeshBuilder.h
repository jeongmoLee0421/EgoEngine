#pragma once

#include "../Inc/Builder.h"
#include "../Inc/ResourceManager.h"
#include "../Inc/ResourceDefine.h"
#include "../Inc/IObject.h"

// 2023 07 01 이정모 home

// 렌더링시 mesh가 필요 없는 오브젝트(axis, grid ...) 전용 builder class
// NonMesh 타입으로 특수화 하여 SRV(texture: diffuse map)나 Sampler 핸들을 세팅하는 것을 아예 만들지 않았음

class NonMesh;

template <typename ObjectType>
class ObjectPool;

// main class template인 Builder<ObjectType, MeshType>을
// Builder<ObjectType, NonMesh>로 부분 특수화한 class template
template <typename ObjectType>
class Builder<ObjectType, NonMesh>
{
public:
	Builder<ObjectType, NonMesh>(ResourceManager* pResourceManager);
	~Builder<ObjectType, NonMesh>();

	// ObjectType을 템플릿 인자로 받았기 때문에
	// builder는 오브젝트를 만드는 방법을 알고 있다.
public:
	IObject* Build(ObjectPool<ObjectType>* pObjectPool);

public:
	void SetHandleDevice(EResourceType deviceType);
	void SetHandleDeviceContext(EResourceType deviceContextType);

	void SetHandleVertexShader(EResourceType vertexShaderType);
	void SetHandlePixelShader(EResourceType pixelShaderType);
	void SetHandleInputLayout(EResourceType inputLayoutType);

	void SetHandleVertexBuffer(EResourceType vertexBufferType);
	void SetHandleIndexBuffer(EResourceType indexBufferType);
	void SetHandleConstantBuffer(EResourceType constantBufferType);
	// 원래 index buffer size type에 대한 enum class가 따로 있었지만
	// IB 생성할 때 이미 인덱스 개수는 정해져 있기 때문에 EResourceType을 사용하기로 함
	void SetIndexBufferSize(EResourceType indexBufferSizeType);

	// non mesh로 특수화된 빌더는 SRV, Sampler가 필요 없다.

private:
	ResourceManager* mResourceManager;

private:
	unsigned int mHandleDevice;
	unsigned int mHandleDeviceContext;

	unsigned int mHandleVertexShader;
	unsigned int mHandlePixelShader;
	unsigned int mHandleInputLayout;

	unsigned int mHandleVertexBuffer;
	unsigned int mHandleIndexBuffer;
	unsigned int mHandleConstantBuffer;
	unsigned int mIndexBufferSize;
};

template<typename ObjectType>
inline Builder<ObjectType, NonMesh>::Builder(ResourceManager* pResourceManager)
	: mResourceManager(pResourceManager)
	, mHandleDevice(0), mHandleDeviceContext(0)
	, mHandleVertexShader(0), mHandlePixelShader(0), mHandleInputLayout(0)
	, mHandleVertexBuffer(0), mHandleIndexBuffer(0), mHandleConstantBuffer(0), mIndexBufferSize(0)
{
}

template<typename ObjectType>
inline Builder<ObjectType, NonMesh>::~Builder()
{
	ID3D11Device* _pDevice = mResourceManager->GetResource<ID3D11Device>(mHandleDevice);
	_pDevice->Release();

	ID3D11DeviceContext* _pDeviceContext = mResourceManager->GetResource<ID3D11DeviceContext>(mHandleDeviceContext);
	_pDeviceContext->Release();

	ID3D11VertexShader* _pVertexShader = mResourceManager->GetResource<ID3D11VertexShader>(mHandleVertexShader);
	_pVertexShader->Release();

	ID3D11PixelShader* _pPixelShader = mResourceManager->GetResource<ID3D11PixelShader>(mHandlePixelShader);
	_pPixelShader->Release();

	ID3D11InputLayout* _pInputLayout = mResourceManager->GetResource<ID3D11InputLayout>(mHandleInputLayout);
	_pInputLayout->Release();

	ID3D11Buffer* _pVertexBuffer = mResourceManager->GetResource<ID3D11Buffer>(mHandleVertexBuffer);
	_pVertexBuffer->Release();

	ID3D11Buffer* _pIndexBuffer = mResourceManager->GetResource<ID3D11Buffer>(mHandleIndexBuffer);
	_pIndexBuffer->Release();

	ID3D11Buffer* _pConstantBuffer = mResourceManager->GetResource<ID3D11Buffer>(mHandleConstantBuffer);
	_pConstantBuffer->Release();
}

template<typename ObjectType>
inline IObject* Builder<ObjectType, NonMesh>::Build(ObjectPool<ObjectType>* pObjectPool)
{
	IObject* pObj = nullptr;

	try
	{
		pObj = new(pObjectPool) ObjectType(
			mResourceManager,
			mHandleDevice,
			mHandleDeviceContext,
			mHandleVertexShader,
			mHandleInputLayout,
			mHandlePixelShader,
			mHandleVertexBuffer,
			mHandleIndexBuffer,
			mHandleConstantBuffer,
			mIndexBufferSize
		);
	}
	catch (...)
	{
		return nullptr;
	}

	pObj->Initialize();
	return pObj;
}

template<typename ObjectType>
inline void Builder<ObjectType, NonMesh>::SetHandleDevice(EResourceType deviceType)
{
	bool bUnsuedHandle = mResourceManager->IsUnusedHandle(mHandleDevice);
	ID3D11Device* _pDevice = nullptr;

	// 빌더에 핸들을 세팅하기 전에 이전에 세팅된 핸들이 있고 그 핸들이 유효한 핸들이라면 세팅 전에 Release()
	if (!bUnsuedHandle)
	{
		_pDevice = mResourceManager->GetResource<ID3D11Device>(mHandleDevice);
		_pDevice->Release();
	}

	mHandleDevice = mResourceManager->GetHandleResource(deviceType);

	// builder가 핸들을 가지고 있는 이상 언제든지 자원을 요청할 수 있기 때문에 AddRef()
	_pDevice = mResourceManager->GetResource<ID3D11Device>(mHandleDevice);
	_pDevice->AddRef();
}

template<typename ObjectType>
inline void Builder<ObjectType, NonMesh>::SetHandleDeviceContext(EResourceType deviceContextType)
{
	bool bUnsuedHandle = mResourceManager->IsUnusedHandle(mHandleDeviceContext);
	ID3D11DeviceContext* _pDeviceContext = nullptr;

	if (!bUnsuedHandle)
	{
		_pDeviceContext = mResourceManager->GetResource<ID3D11DeviceContext>(mHandleDeviceContext);
		_pDeviceContext->Release();
	}

	mHandleDeviceContext = mResourceManager->GetHandleResource(deviceContextType);

	_pDeviceContext = mResourceManager->GetResource<ID3D11DeviceContext>(mHandleDeviceContext);
	_pDeviceContext->AddRef();
}

template<typename ObjectType>
inline void Builder<ObjectType, NonMesh>::SetHandleVertexShader(EResourceType vertexShaderType)
{
	bool bUnusedHandle = mResourceManager->IsUnusedHandle(mHandleVertexShader);
	ID3D11VertexShader* _pVertexShader = nullptr;

	if (!bUnusedHandle)
	{
		_pVertexShader = mResourceManager->GetResource<ID3D11VertexShader>(mHandleVertexShader);
		_pVertexShader->Release();
	}

	mHandleVertexShader = mResourceManager->GetHandleResource(vertexShaderType);

	_pVertexShader = mResourceManager->GetResource<ID3D11VertexShader>(mHandleVertexShader);
	_pVertexShader->AddRef();
}

template<typename ObjectType>
inline void Builder<ObjectType, NonMesh>::SetHandlePixelShader(EResourceType pixelShaderType)
{
	bool bUnusedHandle = mResourceManager->IsUnusedHandle(mHandlePixelShader);
	ID3D11PixelShader* _pPixelShader = nullptr;

	if (!bUnusedHandle)
	{
		_pPixelShader = mResourceManager->GetResource<ID3D11PixelShader>(mHandlePixelShader);
		_pPixelShader->Release();
	}

	mHandlePixelShader = mResourceManager->GetHandleResource(pixelShaderType);

	_pPixelShader = mResourceManager->GetResource<ID3D11PixelShader>(mHandlePixelShader);
	_pPixelShader->AddRef();
}

template<typename ObjectType>
inline void Builder<ObjectType, NonMesh>::SetHandleInputLayout(EResourceType inputLayoutType)
{
	bool bUnusedHandle = mResourceManager->IsUnusedHandle(mHandleInputLayout);
	ID3D11InputLayout* _pInputLayout = nullptr;

	if (!bUnusedHandle)
	{
		_pInputLayout = mResourceManager->GetResource<ID3D11InputLayout>(mHandleInputLayout);
		_pInputLayout->Release();
	}

	mHandleInputLayout = mResourceManager->GetHandleResource(inputLayoutType);

	_pInputLayout = mResourceManager->GetResource<ID3D11InputLayout>(mHandleInputLayout);
	_pInputLayout->AddRef();
}

template<typename ObjectType>
inline void Builder<ObjectType, NonMesh>::SetHandleVertexBuffer(EResourceType vertexBufferType)
{
	bool bUnusedHandle = mResourceManager->IsUnusedHandle(mHandleVertexBuffer);
	ID3D11Buffer* _pVertexBuffer = nullptr;

	if (!bUnusedHandle)
	{
		_pVertexBuffer = mResourceManager->GetResource<ID3D11Buffer>(mHandleVertexBuffer);
		_pVertexBuffer->Release();
	}

	mHandleVertexBuffer = mResourceManager->GetHandleResource(vertexBufferType);

	_pVertexBuffer = mResourceManager->GetResource<ID3D11Buffer>(mHandleVertexBuffer);
	_pVertexBuffer->AddRef();
}

template<typename ObjectType>
inline void Builder<ObjectType, NonMesh>::SetHandleIndexBuffer(EResourceType indexBufferType)
{
	bool bUnusedHandle = mResourceManager->IsUnusedHandle(mHandleIndexBuffer);
	ID3D11Buffer* _pIndexBuffer = nullptr;

	if (!bUnusedHandle)
	{
		_pIndexBuffer = mResourceManager->GetResource<ID3D11Buffer>(mHandleIndexBuffer);
		_pIndexBuffer->Release();
	}

	mHandleIndexBuffer = mResourceManager->GetHandleResource(indexBufferType);

	_pIndexBuffer = mResourceManager->GetResource<ID3D11Buffer>(mHandleIndexBuffer);
	_pIndexBuffer->AddRef();
}

template<typename ObjectType>
inline void Builder<ObjectType, NonMesh>::SetHandleConstantBuffer(EResourceType constantBufferType)
{
	bool bUnusedHandle = mResourceManager->IsUnusedHandle(mHandleConstantBuffer);
	ID3D11Buffer* _pConstantBuffer = nullptr;

	if (!bUnusedHandle)
	{
		_pConstantBuffer = mResourceManager->GetResource<ID3D11Buffer>(mHandleConstantBuffer);
		_pConstantBuffer->Release();
	}

	mHandleConstantBuffer = mResourceManager->GetHandleResource(constantBufferType);

	_pConstantBuffer = mResourceManager->GetResource<ID3D11Buffer>(mHandleConstantBuffer);
	_pConstantBuffer->AddRef();
}

template<typename ObjectType>
inline void Builder<ObjectType, NonMesh>::SetIndexBufferSize(EResourceType indexBufferSizeType)
{
	mIndexBufferSize = mResourceManager->GetIndexBufferSize(indexBufferSizeType);
}
