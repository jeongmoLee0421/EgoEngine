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

// 생성자 부분에서는 멤버변수에 대한 기본 세팅만 하고
// 초기 단계에서 해야할 일은 Initialize()에서 하자
// 레이어가 하나 더 있으면 물론 신경 써줘야 하지만 나눠 놓으면 작업을 분할하기 좋다.
void Grid::Initialize()
{
	// 자원에 대한 핸들을 가져왔다는 건 언제든지 자원에 대한 포인터를 얻어올 수 있다는 거니까 AddRef()

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
	// grid가 소멸하면 이 객체의 멤버 변수는 더 이상 접근할 수 없고
	// 결국 더 이상 자원에 대한 포인터를 가져올 수 없기 때문에 Release()

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

	// 핸들을 통해 자원에 대한 포인터를 가져오는 것은 그냥 포인터 복사라 괜찮다고 생각하는데
	// 중요한건 포인터를 가져올 때 ParentResource()로 dynamic_cast<>()를 하기 때문에 이게 좀 부하가 있지 않을까 생각
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

// new 연산자에서 object pool을 통해 주소를 할당 받은 뒤
// 해당 주소에 생성자 호출
void* Grid::operator new(size_t size, ObjectPool<Grid>* pObjectPool)
{
	return pObjectPool->Allocate();
}

// operator new() 호출 후 생성자에서 예외가 발생한 경우 짝이 맞는 이 delete가 호출
void Grid::operator delete(void* returnAddress, ObjectPool<Grid>* pObjectPool)
{
	// 생성자 호출 후 초기화 중에 예외가 발생했다면
	// 그 메모리는 온전히 초기화된 객체가 아니기 때문에 바로 회수
	pObjectPool->DeAllocate(returnAddress);

	return;
}

// 정상적으로 생성/소멸될 때 불리는 delete
void Grid::operator delete(void* returnAddress)
{
	// 정상적으로 delete가 불렸다면 주소는 이전 단계(ReturnObject() -> DeAllocate())에서 이미 회수 했음
	// 즉 이 delete는 자식 소멸자부터 부모 소멸자까지 호출하기 위한 목적

	return;
}
