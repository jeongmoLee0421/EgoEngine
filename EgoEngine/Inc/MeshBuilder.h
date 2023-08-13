#pragma once

#include "../Inc/Builder.h"
#include "../Inc/ResourceManager.h"
#include "../Inc/ResourceDefine.h"
#include "../Inc/IObject.h"

// 2023 07 01 이정모 home

// 렌더링시 mesh(SRV(diffuse map), sampler)가 필요한 오브젝트(box ...) 전용 builder class

class Mesh;

template <typename ObjectType>
class ObjectPool;

// main class template인 Builder<ObjectType, MeshType>을
// Builder<ObjectType, Mesh>로 부분 특수화한 class template
template <typename ObjectType>
class Builder<ObjectType, Mesh>
{
public:
	Builder<ObjectType, Mesh>(ResourceManager* pResourceManager);
	~Builder<ObjectType, Mesh>();

public:
	// 이제는 object pool이 지정해준 주소에 생성자를 호출할 것임
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

	// mesh로 특수화된 빌더는 SRV, Sampler를 세팅하는 함수를 제공
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
	// factory 소멸 전 finalize()에서 빌더가 소멸되고 이후부터 이 빌더에서는 자원에 대한 포인터를 얻어올 수 없기 때문에 Release()를 해주자

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

// 템플릿 인자를 통해 object type을 알고 있는 빌더는 object를 만드는 방법을 알고 있음
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

			// non mesh에는 없는 자원에 대한 핸들
			mHandleTexture,
			mHandleSampler
		);
	}
	catch (...)
	{
		// 생성자 호출 후 객체 초기화에서 예외가 발생한 경우
		// ObjectPool을 인자로 가지는 class user-define delete를 컴파일러가 호출해서 메모리를 회수
		return nullptr;
	}

	// 나는 initialize()에서 복사된 자원의 핸들에 대한 reference counting을 하는데
	// 이걸 빼먹었으니 AddRef()가 되지 않았을테고
	// Release()를 더 많이 하게 되어 에러가 발생함
	pObj->Initialize();

	return pObj;
}

template<typename ObjectType>
inline void Builder<ObjectType, Mesh>::SetHandleDevice(EResourceType deviceType)
{
	// 앞서 세팅되어 있는 핸들이 유효한지 아닌지 체크
	bool bUnsuedHandle = mResourceManager->IsUnusedHandle(mHandleDevice);
	ID3D11Device* _pDevice = nullptr;

	// 빌더에 핸들을 세팅하기 전에 이전에 세팅된 핸들이 있고 그 핸들이 유효한(사용 가능한) 핸들이라면 세팅 전에 Release()
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
