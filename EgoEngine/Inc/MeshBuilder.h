#pragma once

#include "../Inc/Builder.h"
#include "../Inc/ResourceManager.h"
#include "../Inc/ResourceDefine.h"
#include "../Inc/IObject.h"

// 2023 07 01 ������ home

// �������� mesh(SRV(diffuse map), sampler)�� �ʿ��� ������Ʈ(box ...) ���� builder class

class Mesh;

template <typename ObjectType>
class ObjectPool;

// main class template�� Builder<ObjectType, MeshType>��
// Builder<ObjectType, Mesh>�� �κ� Ư��ȭ�� class template
template <typename ObjectType>
class Builder<ObjectType, Mesh>
{
public:
	Builder<ObjectType, Mesh>(ResourceManager* pResourceManager);
	~Builder<ObjectType, Mesh>();

public:
	// ������ object pool�� �������� �ּҿ� �����ڸ� ȣ���� ����
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
	void SetIndexBufferSize(EResourceType indexBufferSizeType);

	// mesh�� Ư��ȭ�� ������ SRV, Sampler�� �����ϴ� �Լ��� ����
	void SetHandleTexture(EResourceType textureType);
	void SetHandleSampler(EResourceType samplerType);

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

	unsigned int mHandleTexture;
	unsigned int mHandleSampler;
};



template<typename ObjectType>
inline Builder<ObjectType, Mesh>::Builder(ResourceManager* pResourceManager)
	: mResourceManager(pResourceManager)
	, mHandleDevice(0), mHandleDeviceContext(0)
	, mHandleVertexShader(0), mHandlePixelShader(0), mHandleInputLayout(0)
	, mHandleVertexBuffer(0), mHandleIndexBuffer(0), mHandleConstantBuffer(0), mIndexBufferSize(0)
	, mHandleTexture(0), mHandleSampler(0)
{
}

template<typename ObjectType>
inline Builder<ObjectType, Mesh>::~Builder()
{
	// factory �Ҹ� �� finalize()���� ������ �Ҹ�ǰ� ���ĺ��� �� ���������� �ڿ��� ���� �����͸� ���� �� ���� ������ Release()�� ������

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

	ID3D11ShaderResourceView* _pSRV = mResourceManager->GetResource<ID3D11ShaderResourceView>(mHandleTexture);
	_pSRV->Release();

	ID3D11SamplerState* _pSampler = mResourceManager->GetResource<ID3D11SamplerState>(mHandleSampler);
	_pSampler->Release();
}

// ���ø� ���ڸ� ���� object type�� �˰� �ִ� ������ object�� ����� ����� �˰� ����
template<typename ObjectType>
inline IObject* Builder<ObjectType, Mesh>::Build(ObjectPool<ObjectType>* pObjectPool)
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
			mIndexBufferSize,

			// non mesh���� ���� �ڿ��� ���� �ڵ�
			mHandleTexture,
			mHandleSampler
		);
	}
	catch (...)
	{
		// ������ ȣ�� �� ��ü �ʱ�ȭ���� ���ܰ� �߻��� ���
		// ObjectPool�� ���ڷ� ������ class user-define delete�� �����Ϸ��� ȣ���ؼ� �޸𸮸� ȸ��
		return nullptr;
	}

	// ���� initialize()���� ����� �ڿ��� �ڵ鿡 ���� reference counting�� �ϴµ�
	// �̰� ���Ծ����� AddRef()�� ���� �ʾ����װ�
	// Release()�� �� ���� �ϰ� �Ǿ� ������ �߻���
	pObj->Initialize();

	return pObj;
}

template<typename ObjectType>
inline void Builder<ObjectType, Mesh>::SetHandleDevice(EResourceType deviceType)
{
	// �ռ� ���õǾ� �ִ� �ڵ��� ��ȿ���� �ƴ��� üũ
	bool bUnsuedHandle = mResourceManager->IsUnusedHandle(mHandleDevice);
	ID3D11Device* _pDevice = nullptr;

	// ������ �ڵ��� �����ϱ� ���� ������ ���õ� �ڵ��� �ְ� �� �ڵ��� ��ȿ��(��� ������) �ڵ��̶�� ���� ���� Release()
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
inline void Builder<ObjectType, Mesh>::SetHandleDeviceContext(EResourceType deviceContextType)
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
inline void Builder<ObjectType, Mesh>::SetHandleVertexShader(EResourceType vertexShaderType)
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
inline void Builder<ObjectType, Mesh>::SetHandlePixelShader(EResourceType pixelShaderType)
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
inline void Builder<ObjectType, Mesh>::SetHandleInputLayout(EResourceType inputLayoutType)
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
inline void Builder<ObjectType, Mesh>::SetHandleVertexBuffer(EResourceType vertexBufferType)
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
inline void Builder<ObjectType, Mesh>::SetHandleIndexBuffer(EResourceType indexBufferType)
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
inline void Builder<ObjectType, Mesh>::SetHandleConstantBuffer(EResourceType constantBufferType)
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
inline void Builder<ObjectType, Mesh>::SetIndexBufferSize(EResourceType indexBufferSizeType)
{
	mIndexBufferSize = mResourceManager->GetIndexBufferSize(indexBufferSizeType);
}

template<typename ObjectType>
inline void Builder<ObjectType, Mesh>::SetHandleTexture(EResourceType textureType)
{
	bool bUnusedHandle = mResourceManager->IsUnusedHandle(mHandleTexture);
	ID3D11ShaderResourceView* _pSRV = nullptr;

	if (!bUnusedHandle)
	{
		_pSRV = mResourceManager->GetResource<ID3D11ShaderResourceView>(mHandleTexture);
		_pSRV->Release();
	}

	mHandleTexture = mResourceManager->GetHandleResource(textureType);

	_pSRV = mResourceManager->GetResource<ID3D11ShaderResourceView>(mHandleTexture);
	_pSRV->AddRef();
}

template<typename ObjectType>
inline void Builder<ObjectType, Mesh>::SetHandleSampler(EResourceType samplerType)
{
	bool bUnusedHandle = mResourceManager->IsUnusedHandle(mHandleSampler);
	ID3D11SamplerState* _pSampler = nullptr;

	if (!bUnusedHandle)
	{
		_pSampler = mResourceManager->GetResource<ID3D11SamplerState>(mHandleSampler);
		_pSampler->Release();
	}

	mHandleSampler = mResourceManager->GetHandleResource(samplerType);

	_pSampler = mResourceManager->GetResource<ID3D11SamplerState>(mHandleSampler);
	_pSampler->AddRef();
}
