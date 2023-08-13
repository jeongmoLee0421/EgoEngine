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

	// 모든 자원에 대한 참조가 해제되면 resourceManager를 delete 한다.
	// 왜냐하면 핸들을 가지고 있던 객체들이 소멸할 때, 이제 자원에 대한 포인터를 얻어오지 않을 거니까 소멸 전에 마지막으로 참조해서 Release() 호출해서 refCnt 감소할 것임
	// 그런데 참조할 때 resourceManager의 table에서 원하는 자원의 포인터를 얻어올텐데,
	// resourceManager가 먼저 소멸해서 table을 순회하면서 IResource*를 delete 하게되면 자원의 포인터를 얻어올 수가 없다.
	// 그래서 가장 마지막에 delete
	delete m_pResourceManager;

	// 내가 생각하는 이상적인 리소스 매니저는
	// 어떤 자원을 resourceManager 본인만(reference count==1) 참조하고 있으면 외부에서는 아무도 접근하지 않는다는 것으로 그 자원을 메모리에서 내려주고
	// 만약 어떤 객체가 자원을 얻으려고 하는메 메모리에 없으면 새로 생성 후 메모리에 올려서 포인터를 반환해주는
	// 이정도는 해줘야 리소스 매니저가 아닌가 싶긴하다.
	
	// 지금 내가 작성한 리소스 매니저는 동일한 자원을 공유해서 중복된 메모리 로드를 방지하는 역할만 하고 있다.
	// 누구도 참조하고 있지 않을 때 메모리에서 내린다거나, 메모리에 없는 자원을 요청하면 다시 메모리에 그 자원을 올린다는가 하는 기능은 없다.
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
	// index buffer size는 index buffer를 생성할 때 정해져 있기 때문에
	// 굳이 enum class EIndexBufferSizeType을 따로 관리할 필요가 없다고 생각함
	// enum class를 두개 관리하면 object type이 추가될 때마다 두개의 enum을 추가하는 것도 별로 좋아보이지 않았음
	// EResourceType을 그냥 사용하자
	mGridFactory->GetBuilder()->SetIndexBufferSize(EResourceType::IBGrid);
	mGridFactory->GetBuilder()->SetHandleConstantBuffer(EResourceType::CBWorldViewProj);
	mGridFactory->GetBuilder()->SetHandleVertexShader(EResourceType::VSColor);
	mGridFactory->GetBuilder()->SetHandleInputLayout(EResourceType::ILColor);
	mGridFactory->GetBuilder()->SetHandlePixelShader(EResourceType::PSColor);

	// grid는 non mesh임에도 texture와 sampler를 무효한 값으로 세팅해야하는 비합리적인 부분을 해결함
	// grid로 생성한 팩토리 내부에서 자동으로 생성된 builder class는
	// 클래스 템플릿 부분 특수화를 통해 애초에 texture나 sampler를 세팅하는 함수와 변수를 만들지 않았음

	// mGridFactory->GetBuilder()->SetHandleTexture(EResourceType::None); // error // 함수 존재 x
	// mGridFactory->GetBuilder()->SetHandleSampler(EResourceType::None); // error // 함수 존재 x

	IObject* tmpObj = mGridFactory->CreateObject();
	mAllObjects.push_back(tmpObj);
	mGridObjects.push_back(tmpObj);

	// WorldAxis
	mWorldAxisFactory = new Factory<WorldAxis>(m_pResourceManager);
	mWorldAxisFactory->Initialize();
	
	// factory 사용자 입장에서는 내부의 builder가 어떻게 구성되어 있는지는 모르나
	// 내가 생성하고자 하는 object에 맞는 builder가 존재할거라 믿고
	// builder에서 제공해주는 함수로 내가 원하는 자원만 세팅해서 CreateObject()만 호출하면 되니 이 얼마나 사용하기 좋은가
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

	// mesh를 가지는 box로 생성된 factory의 builder는 Texture와 Sampler를 세팅하는 함수가 존재함
	// 클래스 템플릿 부분 특수화를 통해 mesh를 가진 object에 대한 builder는 Texture와 Sampler를 세팅하는 함수를 만들어 주었음
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
	// 기존에 세팅된 핸들 위에 다른 핸들을 세팅했을 때 참조하고 있던 자원에 대해 Release()가 제대로 동작하는지 확인
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

	// 사용 다한 object를 factory에 반납하면
	// factory 내부에서 finalize(), 주소 회수, delete 및 소멸자 호출
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

	// AllObject 리스트에 있는 오브젝트는 이미 위에서 다 반납
	mAllObjects.clear();

	mGridObjects.clear();
	mWorldAxisObjects.clear();
	mBoxObjects.clear();
	mMeshObjects.clear();

	mGridFactory->Finalize();
	mWorldAxisFactory->Finalize();
	mBoxFactory->Finalize();
	mMeshObjectFactory->Finalize(); // factory의 finalize를 반드시 불러줘야함. 여기서 동적할당한 builder를 소멸시키는데 이 때 핸들을 더이상 사용하지 않기 때문에 Release()를 호출

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
		m_pDXCameraInstance->OnMouseDown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); // 모니터가 여러대인 경우에 좌표가 음수가 나올수도 있다고 해서
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
