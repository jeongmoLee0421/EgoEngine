#include "MeshObject.h"
#include "../Inc/ShaderDefine.h"
#include "../Inc/ResourceManager.h"
#include "../Inc/ObjectPool.h"

MeshObject::MeshObject(ResourceManager* pResourceManager,
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
	, m_indexBufferSize(indexBufferSize), mHandleTexture(handleTexture), mHandleSampler(handleSamper)
	, mConstantBuffer()
{
	m_worldTM = DirectX::XMMatrixIdentity();
	m_viewTM = DirectX::XMMatrixIdentity();
	m_projTM = DirectX::XMMatrixIdentity();
}

MeshObject::~MeshObject()
{
}

void MeshObject::Initialize()
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

void MeshObject::Finalize()
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

void MeshObject::Update(float dt, EgoMath::EgoMatrix4x4 viewTM, EgoMath::EgoMatrix4x4 projTM)
{
	//m_worldTM = DirectX::XMMatrixIdentity();
	// box_material�� ũ�Ⱑ �ʹ� Ŀ�� �������� �ٿ��µ�
	// �����غ��� ���� ��� ������Ʈ�� ������ ConstantBuffer�� ����ϰ� �־
	// ���� �������� update�Ǵ� box_material�� worldTM���� UpdateSubresource()�� ȣ���ϴ� �ٶ��� ��� ������Ʈ�� ��� �����ϸ��� ����Ǿ���ȴ�.
	// 
	// update�� ���Ƽ� ���� �ʰ� update, render�� ��� �ݺ��ϰų�
	// constant buffer�� ���� �����ϴ���
	// render���� SetConstantBuffers() �ϱ����� UpdateSubresource()ȣ���ؼ� ������ CB ������ �����ϴ���
	m_worldTM = DirectX::XMMatrixScaling(0.1f, 0.1f, 0.1f) * DirectX::XMMatrixTranslation(-5.f, 0.f, 0.f);

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

void MeshObject::Render()
{
	UINT stride = sizeof(PosNormalTexVertex);
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
	pDeviceContext->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R32_UINT, 0); // Mesh Object�� WORD�� �ƴ� UINT�̱� ������ R32�� ����
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	pDeviceContext->VSSetShader(pVertexShader, nullptr, 0);
	pDeviceContext->PSSetShader(pPixelShader, nullptr, 0);

	// ������Ʈ���� worldTM�� �ٸ��� ������ Constant Buffer�� ����ϰ� �ִٺ��� ���� �������� update()�� constant buffer�� ��� ������Ʈ�� ����ǰ� �־���.
	// �׷��� ������Ʈ�� �ڽ��� ����� constant buffer �����͸� ��� �ִٰ� render()�� �� setting �ϴ� ������� �����غ��Ҵ�.
	pDeviceContext->UpdateSubresource(pConstantBuffer, 0, nullptr, &mConstantBuffer, 0, 0);
	//pDeviceContext->VSSetConstantBuffers(0, 1, &pConstantBuffer);

	pDeviceContext->PSSetShaderResources(0, 1, &_pSRV);
	pDeviceContext->PSSetSamplers(0, 1, &_pSamplerState);

	pDeviceContext->DrawIndexed(m_indexBufferSize, 0, 0);
}

void* MeshObject::operator new(size_t size, ObjectPool<MeshObject>* pObjectPool)
{
	// new()�� ȣ��Ǿ� object pool�� �ּҸ� ��ȯ���ָ�
	// ���� �̸� �Ҵ� ���� �� �ּҿ� �����ڸ� �ҷ��� �ʱ�ȭ
	return pObjectPool->Allocate();
}

void MeshObject::operator delete(void* returnAddress, ObjectPool<MeshObject>* pObjectPool)
{
	pObjectPool->DeAllocate(returnAddress);

	return;
}

void MeshObject::operator delete(void* returnAddress)
{
	return;
}
