#include "Builder.h"
#include "../Inc/ResourceManager.h"

Builder::Builder(ResourceManager* pResourceManager)
	: m_pResourceManager(pResourceManager)
	, m_handleDevice(0), m_handleDeviceContext(0)
	, m_handleVertexShader(0), m_handlePixelShader(0), m_handleInputLayout(0)
	, m_handleVertexBuffer(0), m_handleIndexBuffer(0), m_handleConstantBuffer(0)
	, m_indexBufferSize(0), mHandleTextureDiffuseMap(0), mHandleSampler(0)
{
}

Builder::~Builder()
{
	// ������ �Ҹ��� �� ��� ���� �ڵ�� ���̻� �ڿ��� ���� ������ ���� ���� ���̹Ƿ� release()�� ȣ��

	// �ڵ� ġ�鼭 ������ ������ �Ź� ��밡���� �ڵ����� ���θ� if�� Ȯ���ϰ� ����ϴ� ���� ���� ���� �� ����.
	// �ʿ� ���� �ڿ��̸� �� �޴°� ����. �ֳ��ϸ� ������ ������ ���°Ű� �� ������ �� ���ٴ� �� ��� ���� �Ǳ� ����
	// �׷��ٸ� resource manager���� ��ȿ�� �ڵ��ΰ��� ��� ������ ����
	if (!m_pResourceManager->IsUnusedHandle(m_handleDevice))
	{
		ID3D11Device* pDevice = m_pResourceManager->GetResource<ID3D11Device>(m_handleDevice);
		pDevice->Release();
	}

	if (!m_pResourceManager->IsUnusedHandle(m_handleDeviceContext))
	{
		ID3D11DeviceContext* pDeviceContext = m_pResourceManager->GetResource<ID3D11DeviceContext>(m_handleDeviceContext);
		pDeviceContext->Release();
	}

	if (!m_pResourceManager->IsUnusedHandle(m_handleVertexShader))
	{
		ID3D11VertexShader* pVertexShader = m_pResourceManager->GetResource<ID3D11VertexShader>(m_handleVertexShader);
		pVertexShader->Release();
	}

	if (!m_pResourceManager->IsUnusedHandle(m_handlePixelShader))
	{
		ID3D11PixelShader* pPixelShader = m_pResourceManager->GetResource<ID3D11PixelShader>(m_handlePixelShader);
		pPixelShader->Release();
	}

	if (!m_pResourceManager->IsUnusedHandle(m_handleInputLayout))
	{
		ID3D11InputLayout* pInputLayout = m_pResourceManager->GetResource<ID3D11InputLayout>(m_handleInputLayout);
		pInputLayout->Release();
	}

	if (!m_pResourceManager->IsUnusedHandle(m_handleVertexBuffer))
	{
		ID3D11Buffer* pVertexBuffer = m_pResourceManager->GetResource<ID3D11Buffer>(m_handleVertexBuffer);
		pVertexBuffer->Release();
	}

	if (!m_pResourceManager->IsUnusedHandle(m_handleIndexBuffer))
	{
		ID3D11Buffer* pIndexBuffer = m_pResourceManager->GetResource<ID3D11Buffer>(m_handleIndexBuffer);
		pIndexBuffer->Release();
	}

	if (!m_pResourceManager->IsUnusedHandle(m_handleConstantBuffer))
	{
		ID3D11Buffer* pConstantBuffer = m_pResourceManager->GetResource<ID3D11Buffer>(m_handleConstantBuffer);
		pConstantBuffer->Release();
	}

	// �ڿ��� �ø� �ü��� ������ �����ؾ� �ϴ� �͵� �þ��.
	// �������� �ִ� �ڿ��鳢�� ���ļ� Ŭ������ �����ϴ���
	// �ƴϸ� ������ ������ ���� Ư�� �ڿ��� ���ؼ��� Set�̳� Get�� �ƿ� �� �� ������ ����
	// ������ �ʿ䰡 �����״�
	if (!m_pResourceManager->IsUnusedHandle(mHandleTextureDiffuseMap))
	{
		ID3D11ShaderResourceView* _pSRV = m_pResourceManager->GetResource<ID3D11ShaderResourceView>(mHandleTextureDiffuseMap);
		_pSRV->Release();
	}

	if (!m_pResourceManager->IsUnusedHandle(mHandleSampler))
	{
		ID3D11SamplerState* _pSampler = m_pResourceManager->GetResource<ID3D11SamplerState>(mHandleSampler);
		_pSampler->Release();
	}
}

void Builder::SetHandleDevice(EResourceType deviceType)
{
	if (deviceType == EResourceType::None)
	{
		m_handleDevice = ResourceManager::mUnusedHandleNumber;
		return;
	}

	m_handleDevice = m_pResourceManager->GetHandleResource(deviceType);

	// ������ ����ִ� ���ȿ��� �ڵ�� �ڿ��� ��� ������ �� �ֱ� ������ AddRef()
	ID3D11Device* pDevice = m_pResourceManager->GetResource<ID3D11Device>(m_handleDevice);
	pDevice->AddRef();
}

void Builder::SetHandleDeviceContext(EResourceType deviceContextType)
{
	if (deviceContextType == EResourceType::None)
	{
		m_handleDeviceContext = ResourceManager::mUnusedHandleNumber;
		return;
	}

	m_handleDeviceContext = m_pResourceManager->GetHandleResource(deviceContextType);

	ID3D11DeviceContext* pDeviceContext = m_pResourceManager->GetResource<ID3D11DeviceContext>(m_handleDeviceContext);
	pDeviceContext->AddRef();
}

void Builder::SetHandleVertexShader(EResourceType vertexShaderType)
{
	if (vertexShaderType == EResourceType::None)
	{
		m_handleVertexShader = ResourceManager::mUnusedHandleNumber;
		return;
	}

	m_handleVertexShader = m_pResourceManager->GetHandleResource(vertexShaderType);

	ID3D11VertexShader* pVertexShader = m_pResourceManager->GetResource<ID3D11VertexShader>(m_handleVertexShader);
	pVertexShader->AddRef();
}

void Builder::SetHandlePixelShader(EResourceType pixelShaderType)
{
	if (pixelShaderType == EResourceType::None)
	{
		m_handlePixelShader = ResourceManager::mUnusedHandleNumber;
		return;
	}

	m_handlePixelShader = m_pResourceManager->GetHandleResource(pixelShaderType);

	ID3D11PixelShader* pPixelShader = m_pResourceManager->GetResource<ID3D11PixelShader>(m_handlePixelShader);
	pPixelShader->AddRef();
}

void Builder::SetHandleInputLayout(EResourceType inputLayoutType)
{
	if (inputLayoutType == EResourceType::None)
	{
		m_handleInputLayout = ResourceManager::mUnusedHandleNumber;
		return;
	}

	m_handleInputLayout = m_pResourceManager->GetHandleResource(inputLayoutType);

	ID3D11InputLayout* pInputLayout = m_pResourceManager->GetResource<ID3D11InputLayout>(m_handleInputLayout);
	pInputLayout->AddRef();
}

void Builder::SetHandleVertexBuffer(EResourceType VertexBufferType)
{
	if (VertexBufferType == EResourceType::None)
	{
		m_handleVertexBuffer = ResourceManager::mUnusedHandleNumber;
		return;
	}

	m_handleVertexBuffer = m_pResourceManager->GetHandleResource(VertexBufferType);

	ID3D11Buffer* pVertexBuffer = m_pResourceManager->GetResource<ID3D11Buffer>(m_handleVertexBuffer);
	pVertexBuffer->AddRef();
}

void Builder::SetHandleIndexBuffer(EResourceType IndexBufferType)
{
	if (IndexBufferType == EResourceType::None)
	{
		m_handleIndexBuffer = ResourceManager::mUnusedHandleNumber;
		return;
	}

	m_handleIndexBuffer = m_pResourceManager->GetHandleResource(IndexBufferType);

	ID3D11Buffer* pIndexBuffer = m_pResourceManager->GetResource<ID3D11Buffer>(m_handleIndexBuffer);
	pIndexBuffer->AddRef();
}

void Builder::SetHandleConstantBuffer(EResourceType ConstantBufferType)
{
	if (ConstantBufferType == EResourceType::None)
	{
		m_handleConstantBuffer = ResourceManager::mUnusedHandleNumber;
		return;
	}

	m_handleConstantBuffer = m_pResourceManager->GetHandleResource(ConstantBufferType);

	ID3D11Buffer* pConstantBuffer = m_pResourceManager->GetResource<ID3D11Buffer>(m_handleConstantBuffer);
	pConstantBuffer->AddRef();
}

void Builder::SetIndexBufferSize(EIndexBufferSizeType eIndexBufferSizeType)
{
	m_indexBufferSize = m_pResourceManager->GetIndexBufferSize(eIndexBufferSizeType);
}

void Builder::SetHandleTextureDiffuseMap(EResourceType textureDiffuseMapType)
{
	if (textureDiffuseMapType == EResourceType::None)
	{
		mHandleTextureDiffuseMap = ResourceManager::mUnusedHandleNumber;
		return;
	}

	mHandleTextureDiffuseMap = m_pResourceManager->GetHandleResource(textureDiffuseMapType);

	ID3D11ShaderResourceView* _pSRV = m_pResourceManager->GetResource<ID3D11ShaderResourceView>(mHandleTextureDiffuseMap);
	_pSRV->AddRef();
}

void Builder::SetHandleSampler(EResourceType samplerType)
{
	if (samplerType == EResourceType::None)
	{
		mHandleSampler = ResourceManager::mUnusedHandleNumber;
		return;
	}

	mHandleSampler = m_pResourceManager->GetHandleResource(samplerType);

	ID3D11SamplerState* _pSamplerState = m_pResourceManager->GetResource<ID3D11SamplerState>(mHandleSampler);
	_pSamplerState->AddRef();
}

unsigned int Builder::GetHandleDevice()
{
	return m_handleDevice;
}

unsigned int Builder::GetHandleDeviceContext()
{
	return m_handleDeviceContext;
}

unsigned int Builder::GetHandleVertexShader()
{
	return m_handleVertexShader;
}

unsigned int Builder::GetHandlePixelShader()
{
	return m_handlePixelShader;
}

unsigned int Builder::GetHandleInputLayout()
{
	return m_handleInputLayout;
}

unsigned int Builder::GetHandleVertexBuffer()
{
	return m_handleVertexBuffer;
}

unsigned int Builder::GetHandleIndexBuffer()
{
	return m_handleIndexBuffer;
}

unsigned int Builder::GetHandleConstantBuffer()
{
	return m_handleConstantBuffer;
}

unsigned int Builder::GetIndexBufferSize()
{
	return m_indexBufferSize;
}

unsigned int Builder::GetHandleTextureDiffuseMap()
{
	return mHandleTextureDiffuseMap;
}

unsigned int Builder::GetHandleSampler()
{
	return mHandleSampler;
}
