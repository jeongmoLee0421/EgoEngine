#include "Box.h"
#include "../Inc/ShaderDefine.h"
#include "../Inc/ResourceManager.h"
#include "../Inc/ObjectPool.h"

Box::Box(ResourceManager* pResourceManager,
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
	unsigned int handleSamper)
	: m_pResourceManager(pResourceManager)
	, m_handleDevice(handleDevice), m_handleDeviceContext(handleDeviceContext)
	, m_handleVertexShader(handleVertexShader), m_handleInputLayout(handleInputLayout), m_handlePixelShader(handlePixelShader)
	, m_handleVertexBuffer(handleVertexBuffer), m_handleIndexBuffer(handleIndexBuffer), m_handleConstantBuffer(handleConstantBuffer)
	, m_indexBufferSize(indexBufferSize), mConstantBuffer(), mHandleTexture(handleTexture), mHandleSampler(handleSamper)
	, mYRotateAngle(0.f), mRotateSpeed(15.f)
{
	m_worldTM = DirectX::XMMatrixIdentity();
	m_viewTM = DirectX::XMMatrixIdentity();
	m_projTM = DirectX::XMMatrixIdentity();
}

Box::~Box()
{
}

void Box::Initialize()
{
	ID3D11Device* pDevice = m_pResourceManager->GetResource<ID3D11Device>(m_handleDevice);
	pDevice->AddRef();

	ID3D11DeviceContext* pDeviceContext = m_pResourceManager->GetResource<ID3D11DeviceContext>(m_handleDeviceContext);
	pDeviceContext->AddRef();

	ID3D11VertexShader* pVertexShader = m_pResourceManager->GetResource<ID3D11VertexShader>(m_handleVertexShader);
	pVertexShader->AddRef();

	ID3D11PixelShader* pPixelShader = m_pResourceManager->GetResource<ID3D11PixelShader>(m_handlePixelShader);
	pPixelShader->AddRef();

	ID3D11InputLayout* pInputLayout = m_pResourceManager->GetResource<ID3D11InputLayout>(m_handleInputLayout);
	pInputLayout->AddRef();

	ID3D11Buffer* pVertexBuffer = m_pResourceManager->GetResource<ID3D11Buffer>(m_handleVertexBuffer);
	pVertexBuffer->AddRef();

	ID3D11Buffer* pIndexBuffer = m_pResourceManager->GetResource<ID3D11Buffer>(m_handleIndexBuffer);
	pIndexBuffer->AddRef();

	ID3D11Buffer* pConstantBuffer = m_pResourceManager->GetResource<ID3D11Buffer>(m_handleConstantBuffer);
	pConstantBuffer->AddRef();

	ID3D11ShaderResourceView* _pSRV = m_pResourceManager->GetResource<ID3D11ShaderResourceView>(mHandleTexture);
	_pSRV->AddRef();

	ID3D11SamplerState* _pSampler = m_pResourceManager->GetResource<ID3D11SamplerState>(mHandleSampler);
	_pSampler->AddRef();
}

void Box::Finalize()
{
	ID3D11Device* pDevice = m_pResourceManager->GetResource<ID3D11Device>(m_handleDevice);
	pDevice->Release();

	ID3D11DeviceContext* pDeviceContext = m_pResourceManager->GetResource<ID3D11DeviceContext>(m_handleDeviceContext);
	pDeviceContext->Release();

	ID3D11VertexShader* pVertexShader = m_pResourceManager->GetResource<ID3D11VertexShader>(m_handleVertexShader);
	pVertexShader->Release();

	ID3D11PixelShader* pPixelShader = m_pResourceManager->GetResource<ID3D11PixelShader>(m_handlePixelShader);
	pPixelShader->Release();

	ID3D11InputLayout* pInputLayout = m_pResourceManager->GetResource<ID3D11InputLayout>(m_handleInputLayout);
	pInputLayout->Release();

	ID3D11Buffer* pVertexBuffer = m_pResourceManager->GetResource<ID3D11Buffer>(m_handleVertexBuffer);
	pVertexBuffer->Release();

	ID3D11Buffer* pIndexBuffer = m_pResourceManager->GetResource<ID3D11Buffer>(m_handleIndexBuffer);
	pIndexBuffer->Release();

	ID3D11Buffer* pConstantBuffer = m_pResourceManager->GetResource<ID3D11Buffer>(m_handleConstantBuffer);
	pConstantBuffer->Release();

	ID3D11ShaderResourceView* _pSRV = m_pResourceManager->GetResource<ID3D11ShaderResourceView>(mHandleTexture);
	_pSRV->Release();

	ID3D11SamplerState* _pSampler = m_pResourceManager->GetResource<ID3D11SamplerState>(mHandleSampler);
	_pSampler->Release();
}

void Box::Update(float dt, EgoMath::EgoMatrix4x4 viewTM, EgoMath::EgoMatrix4x4 projTM)
{
	m_worldTM = DirectX::XMMatrixRotationY(DirectX::XMConvertToRadians(mYRotateAngle));
	mYRotateAngle += mRotateSpeed * dt;

	DirectX::XMFLOAT4X4 convertedTM;

	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			convertedTM(i, j) = viewTM(i, j);
		}
	}
	m_viewTM = DirectX::XMLoadFloat4x4(&convertedTM);

	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			convertedTM(i, j) = projTM(i, j);
		}
	}
	m_projTM = DirectX::XMLoadFloat4x4(&convertedTM);

	mConstantBuffer.WorldViewProj = DirectX::XMMatrixTranspose(m_worldTM * m_viewTM * m_projTM);
}

void Box::Render()
{
	UINT stride = sizeof(TextureVertex);
	UINT offset = 0;

	ID3D11DeviceContext* pDeviceContext = m_pResourceManager->GetResource<ID3D11DeviceContext>(m_handleDeviceContext);
	ID3D11Buffer* pVertexBuffer = m_pResourceManager->GetResource<ID3D11Buffer>(m_handleVertexBuffer);
	ID3D11InputLayout* pInputLayout = m_pResourceManager->GetResource<ID3D11InputLayout>(m_handleInputLayout);
	ID3D11Buffer* pIndexBuffer = m_pResourceManager->GetResource<ID3D11Buffer>(m_handleIndexBuffer);

	ID3D11VertexShader* pVertexShader = m_pResourceManager->GetResource<ID3D11VertexShader>(m_handleVertexShader);
	ID3D11Buffer* pConstantBuffer = m_pResourceManager->GetResource<ID3D11Buffer>(m_handleConstantBuffer);
	ID3D11PixelShader* pPixelShader = m_pResourceManager->GetResource<ID3D11PixelShader>(m_handlePixelShader);

	ID3D11ShaderResourceView* _pSRV = m_pResourceManager->GetResource<ID3D11ShaderResourceView>(mHandleTexture);
	ID3D11SamplerState* _pSamplerState = m_pResourceManager->GetResource<ID3D11SamplerState>(mHandleSampler);

	pDeviceContext->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride, &offset);
	pDeviceContext->IASetInputLayout(pInputLayout);
	// box의 인덱스 1개의 크기를 WORD(unsigned short)로 정의했기 때문에 R16_UINT로 포맷을 알려줌
	pDeviceContext->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	pDeviceContext->VSSetShader(pVertexShader, nullptr, 0);
	pDeviceContext->PSSetShader(pPixelShader, nullptr, 0);

	pDeviceContext->UpdateSubresource(pConstantBuffer, 0, nullptr, &mConstantBuffer, 0, 0);
	//pDeviceContext->VSSetConstantBuffers(0, 1, &pConstantBuffer);

	pDeviceContext->PSSetShaderResources(0, 1, &_pSRV);
	pDeviceContext->PSSetSamplers(0, 1, &_pSamplerState);

	pDeviceContext->DrawIndexed(m_indexBufferSize, 0, 0);
}

void* Box::operator new(size_t size, ObjectPool<Box>* pObjectPool)
{
	return pObjectPool->Allocate();
}

void Box::operator delete(void* returnAddress, ObjectPool<Box>* pObjectPool)
{
	pObjectPool->DeAllocate(returnAddress);

	return;
}

void Box::operator delete(void* returnAddress)
{
	return;
}
