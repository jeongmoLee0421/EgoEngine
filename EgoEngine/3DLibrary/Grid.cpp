#include "Grid.h"
#include "../Inc/ResourceManager.h"
#include "../Inc/ShaderDefine.h"
#include "../Inc/ObjectPool.h"

Grid::Grid(ResourceManager* pResourceManager,
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

Grid::~Grid()
{

}

// ������ �κп����� ��������� ���� �⺻ ���ø� �ϰ�
// �ʱ� �ܰ迡�� �ؾ��� ���� Initialize()���� ����
// ���̾ �ϳ� �� ������ ���� �Ű� ����� ������ ���� ������ �۾��� �����ϱ� ����.
void Grid::Initialize()
{
	// �ڿ��� ���� �ڵ��� �����Դٴ� �� �������� �ڿ��� ���� �����͸� ���� �� �ִٴ� �Ŵϱ� AddRef()

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

void Grid::Finalize()
{
	// grid�� �Ҹ��ϸ� �� ��ü�� ��� ������ �� �̻� ������ �� ����
	// �ᱹ �� �̻� �ڿ��� ���� �����͸� ������ �� ���� ������ Release()

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

void Grid::Update(float dt, EgoMath::EgoMatrix4x4 viewTM, EgoMath::EgoMatrix4x4 projTM)
{
	m_worldTM = DirectX::XMMatrixIdentity();

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

void Grid::Render()
{
	UINT stride = sizeof(ColorVertex);
	UINT offset = 0;

	// �ڵ��� ���� �ڿ��� ���� �����͸� �������� ���� �׳� ������ ����� �����ٰ� �����ϴµ�
	// �߿��Ѱ� �����͸� ������ �� ParentResource()�� dynamic_cast<>()�� �ϱ� ������ �̰� �� ���ϰ� ���� ������ ����
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

	pDeviceContext->UpdateSubresource(pConstantBuffer, 0, nullptr, &mConstantBuffer, 0, 0);
	pDeviceContext->VSSetConstantBuffers(0, 1, &pConstantBuffer);
	
	pDeviceContext->DrawIndexed(m_indexBufferSize, 0, 0);
}

// new �����ڿ��� object pool�� ���� �ּҸ� �Ҵ� ���� ��
// �ش� �ּҿ� ������ ȣ��
void* Grid::operator new(size_t size, ObjectPool<Grid>* pObjectPool)
{
	return pObjectPool->Allocate();
}

// operator new() ȣ�� �� �����ڿ��� ���ܰ� �߻��� ��� ¦�� �´� �� delete�� ȣ��
void Grid::operator delete(void* returnAddress, ObjectPool<Grid>* pObjectPool)
{
	// ������ ȣ�� �� �ʱ�ȭ �߿� ���ܰ� �߻��ߴٸ�
	// �� �޸𸮴� ������ �ʱ�ȭ�� ��ü�� �ƴϱ� ������ �ٷ� ȸ��
	pObjectPool->DeAllocate(returnAddress);

	return;
}

// ���������� ����/�Ҹ�� �� �Ҹ��� delete
void Grid::operator delete(void* returnAddress)
{
	// ���������� delete�� �ҷȴٸ� �ּҴ� ���� �ܰ�(ReturnObject() -> DeAllocate())���� �̹� ȸ�� ����
	// �� �� delete�� �ڽ� �Ҹ��ں��� �θ� �Ҹ��ڱ��� ȣ���ϱ� ���� ����

	return;
}
