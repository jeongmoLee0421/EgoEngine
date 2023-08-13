#include "GameProcess.h"

#include "../Inc/DXRenderer.h"
#include "../Inc/Timer.h"
#include "../Inc/DXCamera.h"
#include "../Inc/Factory.h"
#include "../Inc/ResourceManager.h"
#include "../Inc/ResourceDefine.h"

#include "../Inc/IObject.h"
#include "../Inc/Grid.h"
#include "../Inc/WorldAxis.h"
#include "../Inc/Box.h"
#include "../Inc/MeshObject.h"
//#include "../Inc/IFactory.h"

#include <windowsx.h>

#pragma comment(lib, "../Lib/3DRenderer")
#pragma comment(lib, "../Lib/3DLibrary")

ICamera* GameProcess::m_pDXCameraInstance = nullptr;

GameProcess::GameProcess()
	: m_hInstance(nullptr)
	, m_hWnd(nullptr)
	, m_msg()

	, m_pDXRenderer(nullptr)
	, m_pTimer(nullptr)
	, m_pDXCamera(nullptr)
	, m_pResourceManager(nullptr)

	, mAllObjects(), mGridObjects(), mWorldAxisObjects(), mBoxObjects(), mMeshObjects()
	, mGridFactory(nullptr), mWorldAxisFactory(nullptr), mBoxFactory(nullptr)
	, mMeshObjectFactory(nullptr)

	, m_clientWidth(0)
	, m_clientHeight(0)
{
}

GameProcess::~GameProcess()
{
	delete m_pDXRenderer;
	delete m_pTimer;
	delete m_pDXCamera;

	delete mGridFactory;
	delete mWorldAxisFactory;
	delete mBoxFactory;
	delete mMeshObjectFactory;

	// ��� �ڿ��� ���� ������ �����Ǹ� resourceManager�� delete �Ѵ�.
	// �ֳ��ϸ� �ڵ��� ������ �ִ� ��ü���� �Ҹ��� ��, ���� �ڿ��� ���� �����͸� ������ ���� �Ŵϱ� �Ҹ� ���� ���������� �����ؼ� Release() ȣ���ؼ� refCnt ������ ����
	// �׷��� ������ �� resourceManager�� table���� ���ϴ� �ڿ��� �����͸� �����ٵ�,
	// resourceManager�� ���� �Ҹ��ؼ� table�� ��ȸ�ϸ鼭 IResource*�� delete �ϰԵǸ� �ڿ��� �����͸� ���� ���� ����.
	// �׷��� ���� �������� delete
	delete m_pResourceManager;

	// ���� �����ϴ� �̻����� ���ҽ� �Ŵ�����
	// � �ڿ��� resourceManager ���θ�(reference count==1) �����ϰ� ������ �ܺο����� �ƹ��� �������� �ʴ´ٴ� ������ �� �ڿ��� �޸𸮿��� �����ְ�
	// ���� � ��ü�� �ڿ��� �������� �ϴ¸� �޸𸮿� ������ ���� ���� �� �޸𸮿� �÷��� �����͸� ��ȯ���ִ�
	// �������� ����� ���ҽ� �Ŵ����� �ƴѰ� �ͱ��ϴ�.
	
	// ���� ���� �ۼ��� ���ҽ� �Ŵ����� ������ �ڿ��� �����ؼ� �ߺ��� �޸� �ε带 �����ϴ� ���Ҹ� �ϰ� �ִ�.
	// ������ �����ϰ� ���� ���� �� �޸𸮿��� �����ٰų�, �޸𸮿� ���� �ڿ��� ��û�ϸ� �ٽ� �޸𸮿� �� �ڿ��� �ø��ٴ°� �ϴ� ����� ����.
}

HRESULT GameProcess::Initialize(HINSTANCE hInstance, int nCmdShow)
{
	if (FAILED(InitWindow(hInstance, nCmdShow))) return E_FAIL;

	m_pDXRenderer = new DXRenderer();
	m_pDXRenderer->Initialize((int*)m_hWnd, m_clientWidth, m_clientHeight);

	m_pTimer = new Timer();
	m_pTimer->Initialize();

	m_pDXCamera = new DXCamera();
	m_pDXCamera->Initialize((int*)m_hWnd);
	m_pDXCamera->SetLens(DirectX::XMConvertToRadians(90.f), float(m_clientWidth) / m_clientHeight, 0.1f, 100.f);
	m_pDXCameraInstance = m_pDXCamera;

	m_pResourceManager = new ResourceManager();
	m_pResourceManager->Initialize(m_pDXRenderer->GetDevice(), m_pDXRenderer->GetDeviceContext());

	// builder and factory
	
	// Grid
	mGridFactory = new Factory<Grid>(m_pResourceManager);
	mGridFactory->Initialize();
	mGridFactory->GetBuilder()->SetHandleDevice(EResourceType::Device);
	mGridFactory->GetBuilder()->SetHandleDeviceContext(EResourceType::DeviceContext);
	mGridFactory->GetBuilder()->SetHandleVertexBuffer(EResourceType::VBGrid);
	mGridFactory->GetBuilder()->SetHandleIndexBuffer(EResourceType::IBGrid);
	// index buffer size�� index buffer�� ������ �� ������ �ֱ� ������
	// ���� enum class EIndexBufferSizeType�� ���� ������ �ʿ䰡 ���ٰ� ������
	// enum class�� �ΰ� �����ϸ� object type�� �߰��� ������ �ΰ��� enum�� �߰��ϴ� �͵� ���� ���ƺ����� �ʾ���
	// EResourceType�� �׳� �������
	mGridFactory->GetBuilder()->SetIndexBufferSize(EResourceType::IBGrid);
	mGridFactory->GetBuilder()->SetHandleConstantBuffer(EResourceType::CBWorldViewProj);
	mGridFactory->GetBuilder()->SetHandleVertexShader(EResourceType::VSColor);
	mGridFactory->GetBuilder()->SetHandleInputLayout(EResourceType::ILColor);
	mGridFactory->GetBuilder()->SetHandlePixelShader(EResourceType::PSColor);

	// grid�� non mesh�ӿ��� texture�� sampler�� ��ȿ�� ������ �����ؾ��ϴ� ���ո����� �κ��� �ذ���
	// grid�� ������ ���丮 ���ο��� �ڵ����� ������ builder class��
	// Ŭ���� ���ø� �κ� Ư��ȭ�� ���� ���ʿ� texture�� sampler�� �����ϴ� �Լ��� ������ ������ �ʾ���

	// mGridFactory->GetBuilder()->SetHandleTexture(EResourceType::None); // error // �Լ� ���� x
	// mGridFactory->GetBuilder()->SetHandleSampler(EResourceType::None); // error // �Լ� ���� x

	IObject* tmpObj = mGridFactory->CreateObject();
	mAllObjects.push_back(tmpObj);
	mGridObjects.push_back(tmpObj);

	// WorldAxis
	mWorldAxisFactory = new Factory<WorldAxis>(m_pResourceManager);
	mWorldAxisFactory->Initialize();
	
	// factory ����� ���忡���� ������ builder�� ��� �����Ǿ� �ִ����� �𸣳�
	// ���� �����ϰ��� �ϴ� object�� �´� builder�� �����ҰŶ� �ϰ�
	// builder���� �������ִ� �Լ��� ���� ���ϴ� �ڿ��� �����ؼ� CreateObject()�� ȣ���ϸ� �Ǵ� �� �󸶳� ����ϱ� ������
	mWorldAxisFactory->GetBuilder()->SetHandleDevice(EResourceType::Device);
	mWorldAxisFactory->GetBuilder()->SetHandleDeviceContext(EResourceType::DeviceContext);
	mWorldAxisFactory->GetBuilder()->SetHandleVertexBuffer(EResourceType::VBWorldAxis);
	mWorldAxisFactory->GetBuilder()->SetHandleIndexBuffer(EResourceType::IBWorldAxis);
	mWorldAxisFactory->GetBuilder()->SetIndexBufferSize(EResourceType::IBWorldAxis);
	mWorldAxisFactory->GetBuilder()->SetHandleConstantBuffer(EResourceType::CBWorldViewProj);
	mWorldAxisFactory->GetBuilder()->SetHandleVertexShader(EResourceType::VSColor);
	mWorldAxisFactory->GetBuilder()->SetHandleInputLayout(EResourceType::ILColor);
	mWorldAxisFactory->GetBuilder()->SetHandlePixelShader(EResourceType::PSColor);

	tmpObj = mWorldAxisFactory->CreateObject();
	mAllObjects.push_back(tmpObj);
	mWorldAxisObjects.push_back(tmpObj);

	// Box(Texture)
	mBoxFactory = new Factory<Box>(m_pResourceManager);
	mBoxFactory->Initialize();
	mBoxFactory->GetBuilder()->SetHandleDevice(EResourceType::Device);
	mBoxFactory->GetBuilder()->SetHandleDeviceContext(EResourceType::DeviceContext);
	mBoxFactory->GetBuilder()->SetHandleVertexBuffer(EResourceType::VBTextureBox);
	mBoxFactory->GetBuilder()->SetHandleIndexBuffer(EResourceType::IBTextureBox);
	mBoxFactory->GetBuilder()->SetIndexBufferSize(EResourceType::IBTextureBox);
	mBoxFactory->GetBuilder()->SetHandleConstantBuffer(EResourceType::CBWorldViewProj);
	mBoxFactory->GetBuilder()->SetHandleVertexShader(EResourceType::VSTexture);
	mBoxFactory->GetBuilder()->SetHandleInputLayout(EResourceType::ILTexture);
	mBoxFactory->GetBuilder()->SetHandlePixelShader(EResourceType::PSTexture);

	// mesh�� ������ box�� ������ factory�� builder�� Texture�� Sampler�� �����ϴ� �Լ��� ������
	// Ŭ���� ���ø� �κ� Ư��ȭ�� ���� mesh�� ���� object�� ���� builder�� Texture�� Sampler�� �����ϴ� �Լ��� ����� �־���
	mBoxFactory->GetBuilder()->SetHandleTexture(EResourceType::TexDarkBrick);
	mBoxFactory->GetBuilder()->SetHandleSampler(EResourceType::SamplerLinear);

	tmpObj = mBoxFactory->CreateObject();
	mAllObjects.push_back(tmpObj);
	mBoxObjects.push_back(tmpObj);

	mMeshObjectFactory = new Factory<MeshObject>(m_pResourceManager);
	mMeshObjectFactory->Initialize();
	mMeshObjectFactory->GetBuilder()->SetHandleDevice(EResourceType::Device);
	mMeshObjectFactory->GetBuilder()->SetHandleDeviceContext(EResourceType::DeviceContext);
	mMeshObjectFactory->GetBuilder()->SetHandleVertexBuffer(EResourceType::VBBoxMaterial);
	mMeshObjectFactory->GetBuilder()->SetHandleIndexBuffer(EResourceType::IBBoxMaterial);
	mMeshObjectFactory->GetBuilder()->SetIndexBufferSize(EResourceType::IBBoxMaterial);
	mMeshObjectFactory->GetBuilder()->SetHandleConstantBuffer(EResourceType::CBWorldViewProj);
	mMeshObjectFactory->GetBuilder()->SetHandleVertexShader(EResourceType::VSNormalTexture);
	mMeshObjectFactory->GetBuilder()->SetHandleInputLayout(EResourceType::ILNormalTexture);
	mMeshObjectFactory->GetBuilder()->SetHandlePixelShader(EResourceType::PSNormalTexture);
	// ������ ���õ� �ڵ� ���� �ٸ� �ڵ��� �������� �� �����ϰ� �ִ� �ڿ��� ���� Release()�� ����� �����ϴ��� Ȯ��
	mMeshObjectFactory->GetBuilder()->SetHandleTexture(EResourceType::TexDarkBrick);
	mMeshObjectFactory->GetBuilder()->SetHandleTexture(EResourceType::TexWoodCrate01);
	mMeshObjectFactory->GetBuilder()->SetHandleSampler(EResourceType::SamplerLinear);

	tmpObj = mMeshObjectFactory->CreateObject();
	mAllObjects.push_back(tmpObj);
	mMeshObjects.push_back(tmpObj);

	m_pTimer->Reset();

	return S_OK;
}

HRESULT GameProcess::Finalize()
{
	HRESULT hr = S_OK;

	m_pDXRenderer->Finalize();
	m_pTimer->Finalize();
	m_pDXCamera->Finalize();
	m_pResourceManager->Finalize();

	// ��� ���� object�� factory�� �ݳ��ϸ�
	// factory ���ο��� finalize(), �ּ� ȸ��, delete �� �Ҹ��� ȣ��
	for (auto* pGridObj : mGridObjects)
	{
		mGridFactory->ReturnObject(pGridObj);
	}

	for (auto* pWorldAxisObj : mWorldAxisObjects)
	{
		mWorldAxisFactory->ReturnObject(pWorldAxisObj);
	}

	for (auto* pBoxObj : mBoxObjects)
	{
		mBoxFactory->ReturnObject(pBoxObj);
	}

	for (auto* pMeshObj : mMeshObjects)
	{
		mMeshObjectFactory->ReturnObject(pMeshObj);
	}

	// AllObject ����Ʈ�� �ִ� ������Ʈ�� �̹� ������ �� �ݳ�
	mAllObjects.clear();

	mGridObjects.clear();
	mWorldAxisObjects.clear();
	mBoxObjects.clear();
	mMeshObjects.clear();

	mGridFactory->Finalize();
	mWorldAxisFactory->Finalize();
	mBoxFactory->Finalize();
	mMeshObjectFactory->Finalize(); // factory�� finalize�� �ݵ�� �ҷ������. ���⼭ �����Ҵ��� builder�� �Ҹ��Ű�µ� �� �� �ڵ��� ���̻� ������� �ʱ� ������ Release()�� ȣ��

	return hr;
}

void GameProcess::Loop()
{
	while (true)
	{
		if (PeekMessage(&m_msg, NULL, 0, 0, PM_REMOVE))
		{
			if (m_msg.message == WM_QUIT) break;

			TranslateMessage(&m_msg);
			DispatchMessage(&m_msg);
		}
		else
		{
			Update();
			Render();
		}
	}
}

LRESULT GameProcess::WinProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	case WM_RBUTTONDOWN:
		m_pDXCameraInstance->OnMouseDown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); // ����Ͱ� �������� ��쿡 ��ǥ�� ������ ���ü��� �ִٰ� �ؼ�
		return 0;

	case WM_MOUSEMOVE:
		m_pDXCameraInstance->OnMouseMove(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;

	case WM_RBUTTONUP:
		m_pDXCameraInstance->OnMouseUp();
		return 0;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

HRESULT GameProcess::InitWindow(HINSTANCE hInstance, int nCmdShow)
{
	const wchar_t CLASS_NAME[] = L"Ego Engine";

	WNDCLASS wc{};
	wc.style = WS_OVERLAPPED;
	wc.lpfnWndProc = WinProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = CLASS_NAME;

	RegisterClass(&wc);

	m_hWnd = CreateWindow(CLASS_NAME, CLASS_NAME, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL);

	if (m_hWnd == NULL)
	{
		return E_FAIL;
	}

	ShowWindow(m_hWnd, nCmdShow);

	RECT rc;
	GetClientRect(m_hWnd, &rc);
	m_clientWidth = rc.right - rc.left;
	m_clientHeight = rc.bottom - rc.top;

	return S_OK;
}

void GameProcess::Update()
{
	m_pTimer->Update();
	float deltaTime = m_pTimer->GetDeltaTime();

	m_pDXCamera->Update(deltaTime);

	for (auto* pObj : mAllObjects)
	{
		pObj->Update(deltaTime, m_pDXCamera->GetViewTM(), m_pDXCamera->GetProjTM());
	}
}

void GameProcess::Render()
{
	m_pDXRenderer->BeginRender();

	m_pDXRenderer->Render();

	for (auto* pObj : mAllObjects)
	{
		pObj->Render();
	}

	m_pDXRenderer->EndRender();
}
