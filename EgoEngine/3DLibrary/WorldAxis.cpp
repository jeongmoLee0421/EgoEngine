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

	// IObject의 추상화를 위해서 EgoMatrix4x4를 인자로 받았고
	// 내부 행렬 곱셈에는 연산 최적화 된 XMMATRIX를 사용했다
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

	// update()에서 WorldViewProj 행렬 변경에 대한 계산만 먼저 해두고
	// render()에서 DrawIndexed()하기 전에 constant buffer에 대해 UpdateSubresource() 호출
	mConstantBuffer.WorldViewProj = DirectX::XMMatrixTranspose(m_worldTM * m_viewTM * m_projTM); // 전치해서 넣어야 벡터와 행렬의 한 행의 계산을 내적 명령어 하나로 처리 가능
}

void WorldAxis::Render()
{
	UINT stride = sizeof(ColorVertex);
	UINT offset = 0;

	// 자원에 대한 포인터를 얻어와서 이 함수에서만 쓰고 더이상 참조하지 않을 것이기 때문에 AddRef()를 하지 않는다.
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

	// UpdateSubresource()는 cpu에서 gpu 메모리로 데이터를 쓰는 연산인거 같은데
	// 만약 constant buffer에 수정된 데이터가 없다면 UpdateSubresource()를 불필요하게 호출하지 않아도 된다.
	// 이런 방식으로 cpu에서 gpu 메모리에 데이터를 쓰는 시간도 최적화가 가능하다.
	pDeviceContext->UpdateSubresource(pConstantBuffer, 0, nullptr, &mConstantBuffer, 0, 0);

	// 모든 오브젝트가 0번 slot에 동일한 ConstantBuffer를 사용하기 때문에 첫 오브젝트에서 한번만 세팅해도 된다.
	//pDeviceContext->VSSetConstantBuffers(0, 1, &pConstantBuffer);
	
	pDeviceContext->DrawIndexed(m_indexBufferSize, 0, 0);

	// PS에서 상수 버퍼 접근하고 싶으면 PS에 대해서 다시 세팅해야함
	//m_pImmediateContext->PSSetConstantBuffers(0, 1, &m_pConstantBuffer);
}

void* WorldAxis::operator new(size_t size, ObjectPool<WorldAxis>* pObjectPool)
{
	return pObjectPool->Allocate();
}

void WorldAxis::operator delete(void* returnAddress, ObjectPool<WorldAxis>* pObjectPool)
{
	// 일반적으로 delete()가 수행되기 전에 소멸자가 모두 수행되서 메모리에 대한 정리가 끝나고 나면
	// delete() 외부에서 메모리 회수를 해도 문제가 없지만(factory 사용자가 올바른 object의 주소를 받으면 이를 반납처리하면 되기 때문),
	// 만약 생성자에서 예외를 throw하면 컴파일러가 자동으로 짝이 맞는 delete 연산자를 호출해주고
	// 이 때는 delete() 내부에서 메모리를 반드시 회수해야 함
	// 생성자에서 예외가 발생했기 때문에 factory 사용자는 object의 반환값을 nullptr로 받아서
	// 어떤 메모리 주소에서 예외가 발생했는지 알 수 없기 때문에 delete를 호출할 수도 없음.
	// 그리고 object 초기화가 제대로 되지 않았는데 여기에 delete()를 호출해서 소멸자를 부르는 것도 예기치 않은 동작을 발생시킬 수 있음
	// 또한 게임 엔진 내부에서 예외가 발생한 것은 당연히 게임 엔진 내부에서 책임을 져야지,
	// 외부 factory 사용자에게 이를 처리하도록 하는 것은 좋지 못함
	pObjectPool->DeAllocate(returnAddress);
	return;
}

void WorldAxis::operator delete(void* returnAddress)
{
	return;
}
