#pragma once

#include "../Inc/Builder.h"
#include "../Inc/ResourceManager.h"
#include "../Inc/ResourceDefine.h"
#include "../Inc/IObject.h"

// 2023 07 01 ������ home

// �������� mesh�� �ʿ� ���� ������Ʈ(axis, grid ...) ���� builder class
// NonMesh Ÿ������ Ư��ȭ �Ͽ� SRV(texture: diffuse map)�� Sampler �ڵ��� �����ϴ� ���� �ƿ� ������ �ʾ���

class NonMesh;

template <typename ObjectType>
class ObjectPool;

// main class template�� Builder<ObjectType, MeshType>��
// Builder<ObjectType, NonMesh>�� �κ� Ư��ȭ�� class template
template <typename ObjectType>
class Builder<ObjectType, NonMesh>
{
public:
	Builder<ObjectType, NonMesh>(ResourceManager* pResourceManager);
	~Builder<ObjectType, NonMesh>();

	// ObjectType�� ���ø� ���ڷ� �޾ұ� ������
	// builder�� ������Ʈ�� ����� ����� �˰� �ִ�.
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
	// ���� index buffer size type�� ���� enum class�� ���� �־�����
	// IB ������ �� �̹� �ε��� ������ ������ �ֱ� ������ EResourceType�� ����ϱ�� ��
	void SetIndexBufferSize(EResourceType indexBufferSizeType);

	// non mesh�� Ư��ȭ�� ������ SRV, Sampler�� �ʿ� ����.

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

	// ������ �ڵ��� �����ϱ� ���� ������ ���õ� �ڵ��� �ְ� �� �ڵ��� ��ȿ�� �ڵ��̶�� ���� ���� Release()
	if (!bUnsuedHandle)
	{
		_pDevice = mResourceManager->GetResource<ID3D11Device>(mHandleDevice);
		_pDevice->Release();
	}

	mHandleDevice = mResourceManager->GetHandleResource(deviceType);

	// builder�� �ڵ��� ������ �ִ� �̻� �������� �ڿ��� ��û�� �� �ֱ� ������ AddRef()
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
