#include "ResourceManager.h"
#include "../Inc/ShaderDefine.h"
#include "../Inc/ResourceDefine.h"
#include "../Inc/ChildResource.h"
#include "../Inc/ASEParser.h"
#include "../Inc/ObjectResourceLoadingClass.h"

#include <vector>
#include <cassert>
#include <utility> // make_pair

#pragma comment(lib, "../Lib/Parser")

class IResource;

ResourceManager::ResourceManager()
	: m_resourceTypeToHandleTable()
	, m_handleToResourceTable()
	, m_IndexBufferSizes()
	, m_handleNumber(ResourceManager::mStartHandleNumber)
	, m_pDevice(nullptr)

	, mASEParser(nullptr)
	, mObjectResourceLoadingClass(nullptr)
{
}

ResourceManager::~ResourceManager()
{
	// dx 자원들을 숨기고 ChildResource로 동적할당 해서 IResource로 관리하고 있던 클래스의 메모리를 해제
	for (auto& it : m_handleToResourceTable)
	{
		delete it.second;
	}
}

void ResourceManager::Initialize(void* pDevice, void* pDeviceContext)
{
	// DXRenderer의 DXDevice로부터 넘어온 포인터는 Release() 하면 안됌.
	// DXDevice에서도 유지하고 있을 것이고 여기서도 유지하고 있을거니까 reference count가 2개인게 맞음
	// 다른 버퍼나, 셰이더 자원의 경우 해당 함수에서 생성하고 함수가 소멸하면 지역변수로 참조가 불가하니까 Release() 하는게 맞고

	ID3D11Device* ptrDevice = reinterpret_cast<ID3D11Device*>(pDevice);
	m_pDevice = ptrDevice;
	m_pDevice->AddRef(); // device 같은 경우 리소스 매니저의 멤버로 들고 있기 때문에 언제든 참조할 수 있어서 AddRef() 함

	ID3D11DeviceContext* ptrDeviceContext = reinterpret_cast<ID3D11DeviceContext*>(pDeviceContext);

	// 리소스 타입과 핸들을 매핑하고
	m_resourceTypeToHandleTable.insert(std::make_pair(EResourceType::Device, m_handleNumber));

	// 핸들과 실제 리소스에 대한 포인터를 멤버로 가지는 class의 동적할당한 포인터를 매핑
	IResource* pResource = new ChildResource<ID3D11Device>(ptrDevice);
	m_handleToResourceTable.insert(std::make_pair(m_handleNumber, pResource));
	++m_handleNumber;

	m_resourceTypeToHandleTable.insert(std::make_pair(EResourceType::DeviceContext, m_handleNumber));
	pResource = new ChildResource<ID3D11DeviceContext>(ptrDeviceContext);
	m_handleToResourceTable.insert(std::make_pair(m_handleNumber, pResource));
	++m_handleNumber;

	CreateBuffers();
	CreateShadersAndInputLayout();
	CreateSampler();
	CreateTexture();

	// mesh object class에 mesh 데이터를 가져와서 자원을 생성하면
	// 그 자원을 resource manager가 관리하는 방식
	mASEParser = new ASEParser();
	bool bIsFileOpen = mASEParser->OpenFile("../ASEFile/box_material.ASE");
	assert(bIsFileOpen);

	mASEParser->ParsingAll();
	mASEParser->OptimizeDataAll(mASEParser->GetMesh(0));
	mASEParser->SetVBResourceType(mASEParser->GetMesh(0), EResourceType::VBBoxMaterial);
	mASEParser->SetIBResourceType(mASEParser->GetMesh(0), EResourceType::IBBoxMaterial);
	mASEParser->SetIBSizeType(mASEParser->GetMesh(0), EResourceType::IBBoxMaterial);

	mObjectResourceLoadingClass = new ObjectResourceLoadingClass();
	mObjectResourceLoadingClass->LoadGeomerty(mASEParser->GetMesh(0), m_pDevice, this);
}

void ResourceManager::Finalize()
{
	// resource manager가 살아있는 동안에
	// device를 더 이상 사용할 필요가 없어서 미리 해제를 하는 경우도 있을테니
	// if()로 체크해서 여러변 Release() 호출하는 것을 방지
	if (m_pDevice)
	{
		m_pDevice->Release();
		m_pDevice = nullptr;
	}

	delete mASEParser;
	delete mObjectResourceLoadingClass;
}

unsigned int ResourceManager::GetHandleResource(EResourceType eResourceType)
{
	auto it = m_resourceTypeToHandleTable.find(eResourceType);

	// 필요한 자원이 메모리에 없다면 생성해서 만들어 주는것이 되어야 함
	assert(it != m_resourceTypeToHandleTable.end());

	return it->second;
}

unsigned int ResourceManager::GetIndexBufferSize(EResourceType indexBufferSizeType)
{
	auto it = m_IndexBufferSizes.find(indexBufferSizeType);

	assert(it != m_IndexBufferSizes.end());

	return it->second;
}

bool ResourceManager::IsUnusedHandle(unsigned int handle)
{
	// 간단하게 생각해보면 hash table에서 관리하고 있으면 유효한 핸들
	return m_handleToResourceTable.end() == m_handleToResourceTable.find(handle);
}

void ResourceManager::RegisterIndexBufferSize(EResourceType eIndexBufferSizeType, unsigned int indexBufferSize)
{
	m_IndexBufferSizes.insert(std::make_pair(eIndexBufferSizeType, indexBufferSize));
}

void ResourceManager::CreateBuffers()
{
	CreateGridBuffer();
	CreateWorldAxisBuffer();
	CreateConstantBufferAll();
	CreateBoxBuffer();
}

void ResourceManager::CreateShadersAndInputLayout()
{
	CreateVertexShaderAndInputLayoutAll();
	CreatePixelShaderAll();
}

void ResourceManager::CreateGridBuffer()
{
	int pointsOnLine = 21;
	assert(pointsOnLine & 1); // 홀수면 첫비트가 1일테고 1과 bit-and 연산을 하면 1이 되어 true

	ID3D11Buffer* pBuffer;

	// 내가 원하는 크기의 격자 세팅 자동화
	std::vector<ColorVertex>vertices;

	// 격자 좌표계 세팅
	int point = pointsOnLine / 2;
	float y = 0.f;
	for (int z = point; z >= -point; --z)
	{
		for (int x = -point; x <= point; ++x)
		{
			vertices.push_back({ DirectX::XMFLOAT3(static_cast<float>(x), y, static_cast<float>(z)), DirectX::XMFLOAT4(0.f, 0.f, 0.f, 1.f) });
		}
	}

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ColorVertex) * static_cast<unsigned int>(vertices.size());
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA initData;
	ZeroMemory(&initData, sizeof(initData));
	initData.pSysMem = &vertices[0];
	HR(m_pDevice->CreateBuffer(&bd, &initData, &pBuffer));

	m_resourceTypeToHandleTable.insert(std::make_pair(EResourceType::VBGrid, m_handleNumber));
	IResource* pResource = new ChildResource<ID3D11Buffer>(pBuffer);

	// childResource가 만들어지면서 ParentResource의 AddRef()가 호출되었고 여기서는 더이상 참조하지 않을 것이기 때문에 Release()
	// 결국 IResource 내부의 포인터 하나만이 살아 있고 refCnt도 1이다.
	pBuffer->Release();
	m_handleToResourceTable.insert(std::make_pair(m_handleNumber, pResource));
	++m_handleNumber;

	// 격자 인덱스 세팅
	point = pointsOnLine;
	std::vector<WORD>indices;

	// 가로 선
	for (int i = 0; i <= point * (point - 1); i += point)
	{
		for (int j = i; j < i + point - 1; ++j)
		{
			indices.push_back(j);
			indices.push_back(j + 1);
		}
	}

	// 세로 선
	for (int i = 0; i < point; ++i)
	{
		for (int j = i; j < i + point * (point - 1); j += point)
		{
			indices.push_back(j);
			indices.push_back(j + point);
		}
	}

	unsigned int indexBufferSize = static_cast<unsigned int>(indices.size());
	m_IndexBufferSizes.insert(std::make_pair(EResourceType::IBGrid, indexBufferSize));

	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(WORD) * indexBufferSize;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	initData.pSysMem = &indices[0];
	HR(m_pDevice->CreateBuffer(&bd, &initData, &pBuffer));

	m_resourceTypeToHandleTable.insert(std::make_pair(EResourceType::IBGrid, m_handleNumber));
	pResource = new ChildResource<ID3D11Buffer>(pBuffer);
	pBuffer->Release();
	m_handleToResourceTable.insert(std::make_pair(m_handleNumber, pResource));
	++m_handleNumber;
}

void ResourceManager::CreateWorldAxisBuffer()
{
	float axisLength = 20.f;
	ID3D11Buffer* pBuffer;

	ColorVertex vertices[] =
	{
		{DirectX::XMFLOAT3{0.f, 0.f, 0.f}, DirectX::XMFLOAT4{1.f, 0.f, 0.f, 1.f}},
		{DirectX::XMFLOAT3{axisLength, 0.f, 0.f}, DirectX::XMFLOAT4{1.f, 0.f, 0.f, 1.f}},

		{DirectX::XMFLOAT3{0.f, 0.f, 0.f}, DirectX::XMFLOAT4{0.f, 1.f, 0.f, 1.f}},
		{DirectX::XMFLOAT3{0.f, axisLength, 0.f}, DirectX::XMFLOAT4{0.f, 1.f, 0.f, 1.f}},

		{DirectX::XMFLOAT3{0.f, 0.f, 0.f}, DirectX::XMFLOAT4{0.f, 0.f, 1.f, 1.f}},
		{DirectX::XMFLOAT3{0.f, 0.f, axisLength}, DirectX::XMFLOAT4{0.f, 0.f, 1.f, 1.f}},
	};

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT; // 버퍼 크기가 동적으로 변해야하면 D3D11_USAGE_DYNAMIC 세팅하고 map/unmap
	bd.ByteWidth = sizeof(ColorVertex) * 6;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA initData;
	ZeroMemory(&initData, sizeof(initData));
	initData.pSysMem = vertices;
	HR(m_pDevice->CreateBuffer(&bd, &initData, &pBuffer));

	m_resourceTypeToHandleTable.insert(std::make_pair(EResourceType::VBWorldAxis, m_handleNumber));
	IResource* pResource = new ChildResource<ID3D11Buffer>(pBuffer);
	pBuffer->Release();
	m_handleToResourceTable.insert(std::make_pair(m_handleNumber, pResource));
	++m_handleNumber;

	WORD indices[] =
	{
		0,1,
		2,3,
		4,5,
	};

	unsigned int worldAxisIndexBufferSize = static_cast<unsigned int>(sizeof(indices) / sizeof(indices[0]));
	m_IndexBufferSizes.insert(std::make_pair(EResourceType::IBWorldAxis, worldAxisIndexBufferSize));

	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(WORD) * worldAxisIndexBufferSize;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	initData.pSysMem = indices;
	HR(m_pDevice->CreateBuffer(&bd, &initData, &pBuffer));

	m_resourceTypeToHandleTable.insert(std::make_pair(EResourceType::IBWorldAxis, m_handleNumber));
	pResource = new ChildResource<ID3D11Buffer>(pBuffer);
	pBuffer->Release();
	m_handleToResourceTable.insert(std::make_pair(m_handleNumber, pResource));
	++m_handleNumber;
}

void ResourceManager::CreateBoxBuffer()
{
	ID3D11Buffer* pBuffer;

	// box는 텍스처를 사용하기 때문에 정점에 텍스처 좌표가 들어가서 면마다 개별적인 점을 정의해줘야 함
	TextureVertex vertices[] =
	{
		//clock wise

		// front
		{DirectX::XMFLOAT3(-1.f, 1.f, -1.f), DirectX::XMFLOAT2(0.f, 0.f)},
		{DirectX::XMFLOAT3(1.f, 1.f, -1.f), DirectX::XMFLOAT2(1.f, 0.f)},
		{DirectX::XMFLOAT3(1.f, -1.f, -1.f), DirectX::XMFLOAT2(1.f, 1.f)},
		{DirectX::XMFLOAT3(-1.f, -1.f, -1.f), DirectX::XMFLOAT2(0.f, 1.f)},

		// back
		{DirectX::XMFLOAT3(1.f, 1.f, 1.f), DirectX::XMFLOAT2(0.f, 0.f)},
		{DirectX::XMFLOAT3(-1.f, 1.f, 1.f), DirectX::XMFLOAT2(1.f, 0.f)},
		{DirectX::XMFLOAT3(-1.f, -1.f, 1.f), DirectX::XMFLOAT2(1.f, 1.f)},
		{DirectX::XMFLOAT3(1.f, -1.f, 1.f), DirectX::XMFLOAT2(0.f, 1.f)},

		// up
		{DirectX::XMFLOAT3(-1.f, 1.f, 1.f), DirectX::XMFLOAT2(0.f, 0.f)},
		{DirectX::XMFLOAT3(1.f, 1.f, 1.f), DirectX::XMFLOAT2(1.f, 0.f)},
		{DirectX::XMFLOAT3(1.f, 1.f, -1.f), DirectX::XMFLOAT2(1.f, 1.f)},
		{DirectX::XMFLOAT3(-1.f, 1.f, -1.f), DirectX::XMFLOAT2(0.f, 1.f)},

		// down
		{DirectX::XMFLOAT3(-1.f, -1.f, -1.f), DirectX::XMFLOAT2(0.f, 0.f)},
		{DirectX::XMFLOAT3(1.f, -1.f, -1.f), DirectX::XMFLOAT2(1.f, 0.f)},
		{DirectX::XMFLOAT3(1.f, -1.f, 1.f), DirectX::XMFLOAT2(1.f, 1.f)},
		{DirectX::XMFLOAT3(-1.f, -1.f, 1.f), DirectX::XMFLOAT2(0.f, 1.f)},

		// left
		{DirectX::XMFLOAT3(-1.f, 1.f, 1.f), DirectX::XMFLOAT2(0.f, 0.f)},
		{DirectX::XMFLOAT3(-1.f, 1.f, -1.f), DirectX::XMFLOAT2(1.f, 0.f)},
		{DirectX::XMFLOAT3(-1.f, -1.f, -1.f), DirectX::XMFLOAT2(1.f, 1.f)},
		{DirectX::XMFLOAT3(-1.f, -1.f, 1.f), DirectX::XMFLOAT2(0.f, 1.f)},

		// right
		{DirectX::XMFLOAT3(1.f, 1.f, -1.f), DirectX::XMFLOAT2(0.f, 0.f)},
		{DirectX::XMFLOAT3(1.f, 1.f, 1.f), DirectX::XMFLOAT2(1.f, 0.f)},
		{DirectX::XMFLOAT3(1.f, -1.f, 1.f), DirectX::XMFLOAT2(1.f, 1.f)},
		{DirectX::XMFLOAT3(1.f, -1.f, -1.f), DirectX::XMFLOAT2(0.f, 1.f)},
	};

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ColorVertex) * 24;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA initData;
	ZeroMemory(&initData, sizeof(initData));
	initData.pSysMem = vertices;
	HR(m_pDevice->CreateBuffer(&bd, &initData, &pBuffer));

	m_resourceTypeToHandleTable.insert(std::make_pair(EResourceType::VBTextureBox, m_handleNumber));
	IResource* pResource = new ChildResource<ID3D11Buffer>(pBuffer);
	pBuffer->Release(); // parentResource가 ptr을 복사해가면서 AddRef()를 했고 여기서는 더이상 참조하지 않을 것이기 때문에 Release()
	m_handleToResourceTable.insert(std::make_pair(m_handleNumber, pResource));
	++m_handleNumber;

	WORD indices[] =
	{
		// front
		0,1,2,
		2,3,0,

		// back
		4,5,6,
		6,7,4,

		// up
		8,9,10,
		10,11,8,

		// down
		12,13,14,
		14,15,12,

		// left
		16,17,18,
		18,19,16,

		// right
		20,21,22,
		22,23,20
	};

	unsigned int boxIndexBufferSize = static_cast<unsigned int>(sizeof(indices) / sizeof(indices[0]));
	m_IndexBufferSizes.insert(std::make_pair(EResourceType::IBTextureBox, boxIndexBufferSize));

	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(WORD) * boxIndexBufferSize;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	initData.pSysMem = indices;
	HR(m_pDevice->CreateBuffer(&bd, &initData, &pBuffer));

	m_resourceTypeToHandleTable.insert(std::make_pair(EResourceType::IBTextureBox, m_handleNumber));
	pResource = new ChildResource<ID3D11Buffer>(pBuffer);
	pBuffer->Release();
	m_handleToResourceTable.insert(std::make_pair(m_handleNumber, pResource));
	++m_handleNumber;
}

void ResourceManager::CreateVertexShaderAndInputLayoutAll()
{
	// color_vs ----------------------------------------------------------------------------------------------
	Microsoft::WRL::ComPtr<ID3DBlob> pVSBlob; // comptr이라서 알아서 해제
	ID3D11VertexShader* pVS;

	HR(D3DReadFileToBlob(L"../Fx/color_vs.cso", pVSBlob.ReleaseAndGetAddressOf()));

	HR(m_pDevice->CreateVertexShader(pVSBlob->GetBufferPointer(),
		pVSBlob->GetBufferSize(),
		nullptr,
		&pVS)); // pVS refCnt++

	m_resourceTypeToHandleTable.insert(std::make_pair(EResourceType::VSColor, m_handleNumber));
	IResource* pResource = new ChildResource<ID3D11VertexShader>(pVS); // pVS refCnt++
	pVS->Release(); // pVS refCnt--
	m_handleToResourceTable.insert(std::make_pair(m_handleNumber, pResource));
	++m_handleNumber;

	// gpu가 정점 처리할 때 정점이 어떤 구조로 되어있는지 알려주기 위함
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	UINT numElements = ARRAYSIZE(layout);

	ID3D11InputLayout* pLayout;

	HR(m_pDevice->CreateInputLayout(layout, numElements,
		pVSBlob->GetBufferPointer(),
		pVSBlob->GetBufferSize(),
		&pLayout));

	m_resourceTypeToHandleTable.insert(std::make_pair(EResourceType::ILColor, m_handleNumber));
	pResource = new ChildResource<ID3D11InputLayout>(pLayout);
	pLayout->Release();
	m_handleToResourceTable.insert(std::make_pair(m_handleNumber, pResource));
	++m_handleNumber;

	// texture_vs ------------------------------------------------------------------------------------------------
	HR(D3DReadFileToBlob(L"../Fx/texture_vs.cso", pVSBlob.ReleaseAndGetAddressOf()));

	HR(m_pDevice->CreateVertexShader(pVSBlob->GetBufferPointer(),
		pVSBlob->GetBufferSize(),
		nullptr,
		&pVS));

	m_resourceTypeToHandleTable.insert(std::make_pair(EResourceType::VSTexture, m_handleNumber));
	pResource = new ChildResource<ID3D11VertexShader>(pVS);
	pVS->Release();
	m_handleToResourceTable.insert(std::make_pair(m_handleNumber, pResource));
	++m_handleNumber;

	D3D11_INPUT_ELEMENT_DESC textureLayout[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	numElements = ARRAYSIZE(textureLayout);

	HR(m_pDevice->CreateInputLayout(textureLayout, numElements,
		pVSBlob->GetBufferPointer(),
		pVSBlob->GetBufferSize(),
		&pLayout));

	m_resourceTypeToHandleTable.insert(std::make_pair(EResourceType::ILTexture, m_handleNumber));
	pResource = new ChildResource<ID3D11InputLayout>(pLayout);
	pLayout->Release();
	m_handleToResourceTable.insert(std::make_pair(m_handleNumber, pResource));
	++m_handleNumber;

	// normal_texture_vs ------------------------------------------------------------------------------------------------
	HR(D3DReadFileToBlob(L"../Fx/normal_texture_vs.cso", pVSBlob.ReleaseAndGetAddressOf()));

	HR(m_pDevice->CreateVertexShader(pVSBlob->GetBufferPointer(),
		pVSBlob->GetBufferSize(),
		nullptr,
		&pVS));

	m_resourceTypeToHandleTable.insert(std::make_pair(EResourceType::VSNormalTexture, m_handleNumber));
	pResource = new ChildResource<ID3D11VertexShader>(pVS);
	pVS->Release();
	m_handleToResourceTable.insert(std::make_pair(m_handleNumber, pResource));
	++m_handleNumber;

	D3D11_INPUT_ELEMENT_DESC normalTextureLayout[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	numElements = ARRAYSIZE(normalTextureLayout);

	HR(m_pDevice->CreateInputLayout(normalTextureLayout, numElements,
		pVSBlob->GetBufferPointer(),
		pVSBlob->GetBufferSize(),
		&pLayout));

	m_resourceTypeToHandleTable.insert(std::make_pair(EResourceType::ILNormalTexture, m_handleNumber));
	pResource = new ChildResource<ID3D11InputLayout>(pLayout);
	pLayout->Release();
	m_handleToResourceTable.insert(std::make_pair(m_handleNumber, pResource));
	++m_handleNumber;
}

void ResourceManager::CreatePixelShaderAll()
{
	Microsoft::WRL::ComPtr<ID3DBlob> pPSBlob;
	ID3D11PixelShader* pPS;

	// color_ps ----------------------------------------------------------------------------------------------
	HR(D3DReadFileToBlob(L"../Fx/color_ps.cso", pPSBlob.ReleaseAndGetAddressOf()));

	HR(m_pDevice->CreatePixelShader(pPSBlob->GetBufferPointer(),
		pPSBlob->GetBufferSize(),
		NULL,
		&pPS));

	m_resourceTypeToHandleTable.insert(std::make_pair(EResourceType::PSColor, m_handleNumber));
	IResource* pResource = new ChildResource<ID3D11PixelShader>(pPS);
	pPS->Release();
	m_handleToResourceTable.insert(std::make_pair(m_handleNumber, pResource));
	++m_handleNumber;

	// texture_ps ----------------------------------------------------------------------------------------------
	HR(D3DReadFileToBlob(L"../Fx/texture_ps.cso", pPSBlob.ReleaseAndGetAddressOf()));

	HR(m_pDevice->CreatePixelShader(pPSBlob->GetBufferPointer(),
		pPSBlob->GetBufferSize(),
		NULL,
		&pPS));

	m_resourceTypeToHandleTable.insert(std::make_pair(EResourceType::PSTexture, m_handleNumber));
	pResource = new ChildResource<ID3D11PixelShader>(pPS);
	pPS->Release();
	m_handleToResourceTable.insert(std::make_pair(m_handleNumber, pResource));
	++m_handleNumber;

	// normal_texture_ps ----------------------------------------------------------------------------------------------

	HR(D3DReadFileToBlob(L"../Fx/normal_texture_ps.cso", pPSBlob.ReleaseAndGetAddressOf()));

	HR(m_pDevice->CreatePixelShader(pPSBlob->GetBufferPointer(),
		pPSBlob->GetBufferSize(),
		NULL,
		&pPS));

	m_resourceTypeToHandleTable.insert(std::make_pair(EResourceType::PSNormalTexture, m_handleNumber));
	pResource = new ChildResource<ID3D11PixelShader>(pPS);
	pPS->Release();
	m_handleToResourceTable.insert(std::make_pair(m_handleNumber, pResource));
	++m_handleNumber;
}

void ResourceManager::CreateSampler()
{
	ID3D11SamplerState* _pSamplerLinear;

	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP; // 0.0 ~ 1.0 사이의 텍스처 좌표를 넘어섰을 때 텍스처를 반복해서 매핑하겠다는 의미
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	HR(m_pDevice->CreateSamplerState(&sampDesc, &_pSamplerLinear));

	m_resourceTypeToHandleTable.insert(std::make_pair(EResourceType::SamplerLinear, m_handleNumber));
	IResource* _pResource = new ChildResource<ID3D11SamplerState>(_pSamplerLinear);
	_pSamplerLinear->Release();
	m_handleToResourceTable.insert(std::make_pair(m_handleNumber, _pResource));
	++m_handleNumber;
}

void ResourceManager::CreateTexture()
{
	DirectX::ScratchImage _img;

	// tex dark brick ---------------------------------------------------------------------------------------------------------------
	DirectX::LoadFromDDSFile(L"../Textures/darkbrickdxt1.dds", DirectX::DDS_FLAGS_NONE, nullptr, _img);

	ID3D11ShaderResourceView* _pSRV;

	// 텍스처에 대한 셰이더 리소스 뷰를 만들면 셰이더에서 참조할 수 있음
	DirectX::CreateShaderResourceView(m_pDevice, _img.GetImages(), _img.GetImageCount(), _img.GetMetadata(), &_pSRV);
	m_resourceTypeToHandleTable.insert(std::make_pair(EResourceType::TexDarkBrick, m_handleNumber));
	IResource* _pResource = new ChildResource<ID3D11ShaderResourceView>(_pSRV);
	_pSRV->Release();
	m_handleToResourceTable.insert(std::make_pair(m_handleNumber, _pResource));
	++m_handleNumber;

	// tex wood crate01 ---------------------------------------------------------------------------------------------------------------
	// 이 texture는 중간에 DarkBrick으로 변경되는 mip map이 적용되어 있음
	// directx texture tool로 texture를 열어보면 mip 2 of 10에서 DarkBrick이고 나머지는 WoodCrate
	DirectX::LoadFromDDSFile(L"../Textures/WoodCrate01.dds", DirectX::DDS_FLAGS_NONE, nullptr, _img);
	DirectX::CreateShaderResourceView(m_pDevice, _img.GetImages(), _img.GetImageCount(), _img.GetMetadata(), &_pSRV);
	m_resourceTypeToHandleTable.insert(std::make_pair(EResourceType::TexWoodCrate01, m_handleNumber));
	_pResource = new ChildResource<ID3D11ShaderResourceView>(_pSRV);
	_pSRV->Release();
	m_handleToResourceTable.insert(std::make_pair(m_handleNumber, _pResource));
	++m_handleNumber;
}

void ResourceManager::CreateConstantBufferAll()
{
	ID3D11Buffer* pBuffer;

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ConstantBufferWVP);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	HR(m_pDevice->CreateBuffer(&bd, NULL, &pBuffer));

	m_resourceTypeToHandleTable.insert(std::make_pair(EResourceType::CBWorldViewProj, m_handleNumber));
	IResource* pResource = new ChildResource<ID3D11Buffer>(pBuffer);
	pBuffer->Release();
	m_handleToResourceTable.insert(std::make_pair(m_handleNumber, pResource));
	++m_handleNumber;
}

