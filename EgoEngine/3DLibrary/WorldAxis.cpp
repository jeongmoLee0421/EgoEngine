#include "WorldAxis.h"
#include "../Inc/ShaderDefine.h"
#include "../Inc/ResourceManager.h"
#include "../Inc/ObjectPool.h"

WorldAxis::WorldAxis(ResourceManager* pResourceManager,
	unsigned int handleDevice,
	unsigned int handleDeviceContext,
	unsigned int handleVertexShader,
	unsigned int handleInputLayout,
	unsigned int handlePixelShader,
	unsigned int handleVertexBuffer,
	unsigned int handleIndexBuffer,
	unsigned int handleConstantBuffer,
	unsigned int indexBufferSize)
	: m_pResourceManager(pResourceManager)
	, m_handleDevice(handleDevice), m_handleDeviceContext(handleDeviceContext)
	, m_handleVertexShader(handleVertexShader), m_handleInputLayout(handleInputLayout), m_handlePixelShader(handlePixelShader)
	, m_handleVertexBuffer(handleVertexBuffer), m_handleIndexBuffer(handleIndexBuffer), m_handleConstantBuffer(handleConstantBuffer)
	, m_indexBufferSize(indexBufferSize), mConstantBuffer()
{
	m_worldTM = DirectX::XMMatrixIdentity();
	m_viewTM = DirectX::XMMatrixIdentity();
	m_projTM = DirectX::XMMatrixIdentity();
}

WorldAxis::~WorldAxis()
{

}

void WorldAxis::Initialize()
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
}

void WorldAxis::Finalize()
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
}

void WorldAxis::Update(float dt, EgoMath::EgoMatrix4x4 viewTM, EgoMath::EgoMatrix4x4 projTM)
{
	m_worldTM = DirectX::XMMatrixIdentity();

	// IObject�� �߻�ȭ�� ���ؼ� EgoMatrix4x4�� ���ڷ� �޾Ұ�
	// ���� ��� �������� ���� ����ȭ �� XMMATRIX�� ����ߴ�
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

	// update()���� WorldViewProj ��� ���濡 ���� ��길 ���� �صΰ�
	// render()���� DrawIndexed()�ϱ� ���� constant buffer�� ���� UpdateSubresource() ȣ��
	mConstantBuffer.WorldViewProj = DirectX::XMMatrixTranspose(m_worldTM * m_viewTM * m_projTM); // ��ġ�ؼ� �־�� ���Ϳ� ����� �� ���� ����� ���� ��ɾ� �ϳ��� ó�� ����
}

void WorldAxis::Render()
{
	UINT stride = sizeof(ColorVertex);
	UINT offset = 0;

	// �ڿ��� ���� �����͸� ���ͼ� �� �Լ������� ���� ���̻� �������� ���� ���̱� ������ AddRef()�� ���� �ʴ´�.
	ID3D11DeviceContext* pDeviceContext = m_pResourceManager->GetResource<ID3D11DeviceContext>(m_handleDeviceContext);
	ID3D11Buffer* pVertexBuffer = m_pResourceManager->GetResource<ID3D11Buffer>(m_handleVertexBuffer);
	ID3D11InputLayout* pInputLayout = m_pResourceManager->GetResource<ID3D11InputLayout>(m_handleInputLayout);
	ID3D11Buffer* pIndexBuffer = m_pResourceManager->GetResource<ID3D11Buffer>(m_handleIndexBuffer);

	ID3D11VertexShader* pVertexShader = m_pResourceManager->GetResource<ID3D11VertexShader>(m_handleVertexShader);
	ID3D11Buffer* pConstantBuffer = m_pResourceManager->GetResource<ID3D11Buffer>(m_handleConstantBuffer);
	ID3D11PixelShader* pPixelShader = m_pResourceManager->GetResource<ID3D11PixelShader>(m_handlePixelShader);

	pDeviceContext->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride, &offset);
	pDeviceContext->IASetInputLayout(pInputLayout);
	pDeviceContext->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	pDeviceContext->VSSetShader(pVertexShader, nullptr, 0);
	pDeviceContext->PSSetShader(pPixelShader, nullptr, 0);

	// UpdateSubresource()�� cpu���� gpu �޸𸮷� �����͸� ���� �����ΰ� ������
	// ���� constant buffer�� ������ �����Ͱ� ���ٸ� UpdateSubresource()�� ���ʿ��ϰ� ȣ������ �ʾƵ� �ȴ�.
	// �̷� ������� cpu���� gpu �޸𸮿� �����͸� ���� �ð��� ����ȭ�� �����ϴ�.
	pDeviceContext->UpdateSubresource(pConstantBuffer, 0, nullptr, &mConstantBuffer, 0, 0);

	// ��� ������Ʈ�� 0�� slot�� ������ ConstantBuffer�� ����ϱ� ������ ù ������Ʈ���� �ѹ��� �����ص� �ȴ�.
	//pDeviceContext->VSSetConstantBuffers(0, 1, &pConstantBuffer);
	
	pDeviceContext->DrawIndexed(m_indexBufferSize, 0, 0);

	// PS���� ��� ���� �����ϰ� ������ PS�� ���ؼ� �ٽ� �����ؾ���
	//m_pImmediateContext->PSSetConstantBuffers(0, 1, &m_pConstantBuffer);
}

void* WorldAxis::operator new(size_t size, ObjectPool<WorldAxis>* pObjectPool)
{
	return pObjectPool->Allocate();
}

void WorldAxis::operator delete(void* returnAddress, ObjectPool<WorldAxis>* pObjectPool)
{
	// �Ϲ������� delete()�� ����Ǳ� ���� �Ҹ��ڰ� ��� ����Ǽ� �޸𸮿� ���� ������ ������ ����
	// delete() �ܺο��� �޸� ȸ���� �ص� ������ ������(factory ����ڰ� �ùٸ� object�� �ּҸ� ������ �̸� �ݳ�ó���ϸ� �Ǳ� ����),
	// ���� �����ڿ��� ���ܸ� throw�ϸ� �����Ϸ��� �ڵ����� ¦�� �´� delete �����ڸ� ȣ�����ְ�
	// �� ���� delete() ���ο��� �޸𸮸� �ݵ�� ȸ���ؾ� ��
	// �����ڿ��� ���ܰ� �߻��߱� ������ factory ����ڴ� object�� ��ȯ���� nullptr�� �޾Ƽ�
	// � �޸� �ּҿ��� ���ܰ� �߻��ߴ��� �� �� ���� ������ delete�� ȣ���� ���� ����.
	// �׸��� object �ʱ�ȭ�� ����� ���� �ʾҴµ� ���⿡ delete()�� ȣ���ؼ� �Ҹ��ڸ� �θ��� �͵� ����ġ ���� ������ �߻���ų �� ����
	// ���� ���� ���� ���ο��� ���ܰ� �߻��� ���� �翬�� ���� ���� ���ο��� å���� ������,
	// �ܺ� factory ����ڿ��� �̸� ó���ϵ��� �ϴ� ���� ���� ����
	pObjectPool->DeAllocate(returnAddress);
	return;
}

void WorldAxis::operator delete(void* returnAddress)
{
	return;
}
