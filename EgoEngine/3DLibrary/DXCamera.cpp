#include "DXCamera.h"

DXCamera::DXCamera()
	: m_moveSpeed(0.f)
	, m_hWnd(nullptr)
	, m_prevMouseX(0)
	, m_prevMouseY(0)

	, m_pos()
	, m_look()
	, m_right()
	, m_up()
	, m_worldUp()
	, m_viewTM()

	, m_fovAngleY(0.f)
	, m_aspectRatio(0.f)
	, m_nearZ(0.f)
	, m_farZ(0.f)
	, m_projTM()
{
}

DXCamera::~DXCamera()
{
}

void DXCamera::Initialize(int* hWnd)
{
	m_hWnd = (HWND)hWnd;

	m_moveSpeed = 12.f;

	m_pos = DirectX::XMFLOAT3(2.f, 2.f, -5.f);
	m_look = DirectX::XMFLOAT3(0.f, 0.f, 1.f);
	m_right = DirectX::XMFLOAT3(1.f, 0.f, 0.f);
	m_up = DirectX::XMFLOAT3(0.f, 1.f, 0.f);
	m_worldUp = DirectX::XMFLOAT3(0.f, 1.f, 0.f);
}

void DXCamera::Finalize()
{
}

void DXCamera::Update(float dt)
{
	if (GetAsyncKeyState('W') & 0x8000) Walk(dt * m_moveSpeed);
	if (GetAsyncKeyState('S') & 0x8000) Walk(dt * -m_moveSpeed);

	if (GetAsyncKeyState('A') & 0x8000) Strafe(dt * -m_moveSpeed);
	if (GetAsyncKeyState('D') & 0x8000) Strafe(dt * m_moveSpeed);

	if (GetAsyncKeyState('Q') & 0x8000) UpDown(dt * -m_moveSpeed);
	if (GetAsyncKeyState('E') & 0x8000) UpDown(dt * m_moveSpeed);

	MakeViewTM();
}

void DXCamera::OnMouseDown(int x, int y)
{
	m_prevMouseX = x;
	m_prevMouseY = y;

	SetCapture(m_hWnd); // 윈도우 밖에서 마우스를 움직이더라도 내가 캡쳐한 윈도우에 마우스 메시지가 계속 가도록
}

void DXCamera::OnMouseMove(unsigned long long mouseState, int x, int y)
{
	if (mouseState & MK_RBUTTON)
	{
		// 윈도우 좌표는 오른쪽으로 갈수록 커지고 아래로 갈수록 커짐
		int numOfPixelWidth = x - m_prevMouseX;
		int numOfPixelHeight = y - m_prevMouseY;

		// 1픽셀당 0.25도 회전으로 4픽셀 차이면 1도 회전
		float rotationY = DirectX::XMConvertToRadians(0.25f * static_cast<float>(numOfPixelWidth));
		float rotationX = DirectX::XMConvertToRadians(0.25f * static_cast<float>(numOfPixelHeight));

		RotateY(rotationY);
		Pitch(rotationX);
	}

	m_prevMouseX = x;
	m_prevMouseY = y;
}

void DXCamera::OnMouseUp()
{
	ReleaseCapture();
}

EgoMath::EgoMatrix4x4 DXCamera::GetViewTM()
{
	EgoMath::EgoMatrix4x4 egoMat;
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			// 대괄호 연산자 오버로딩
			egoMat[i][j] = m_viewTM(i, j);
		}
	}

	return egoMat;
}

EgoMath::EgoMatrix4x4 DXCamera::GetProjTM()
{
	EgoMath::EgoMatrix4x4 egoMat;
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			// 소괄호 연산자 오버로딩
			egoMat(i, j) = m_projTM(i, j);
		}
	}

	return egoMat;
}

void DXCamera::Walk(float distance)
{
	DirectX::XMVECTOR look = DirectX::XMLoadFloat3(&m_look);
	DirectX::XMVECTOR dist = DirectX::XMVectorSet(distance, distance, distance, 0.f);
	DirectX::XMVECTOR pos = DirectX::XMLoadFloat3(&m_pos);

	pos = DirectX::XMVectorMultiplyAdd(look, dist, pos);

	DirectX::XMStoreFloat3(&m_pos, pos);
}

void DXCamera::Strafe(float distance)
{
	DirectX::XMVECTOR right = DirectX::XMLoadFloat3(&m_right);
	DirectX::XMVECTOR dist = DirectX::XMVectorSet(distance, distance, distance, 0.f);
	DirectX::XMVECTOR pos = DirectX::XMLoadFloat3(&m_pos);

	pos = DirectX::XMVectorMultiplyAdd(right, dist, pos);

	DirectX::XMStoreFloat3(&m_pos, pos);
}

void DXCamera::UpDown(float distance)
{
	DirectX::XMVECTOR up = DirectX::XMLoadFloat3(&m_up);
	DirectX::XMVECTOR dist = DirectX::XMVectorSet(distance, distance, distance, 0.f);
	DirectX::XMVECTOR pos = DirectX::XMLoadFloat3(&m_pos);

	pos = DirectX::XMVectorMultiplyAdd(up, dist, pos);

	DirectX::XMStoreFloat3(&m_pos, pos);
}

void DXCamera::SetLens(float fovAngleY, float aspectRatio, float nearZ, float farZ)
{
	m_fovAngleY = fovAngleY;
	m_aspectRatio = aspectRatio;
	m_nearZ = nearZ;
	m_farZ = farZ;

	MakeProjTM();
}

void DXCamera::Pitch(float rotation)
{
	DirectX::XMVECTOR rotationAxis = DirectX::XMLoadFloat3(&m_right);
	DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationAxis(rotationAxis, rotation);

	DirectX::XMVECTOR up = DirectX::XMLoadFloat3(&m_up);
	DirectX::XMVECTOR look = DirectX::XMLoadFloat3(&m_look);

	up = DirectX::XMVector3TransformNormal(up, rotationMatrix);
	look = DirectX::XMVector3TransformNormal(look, rotationMatrix);

	DirectX::XMStoreFloat3(&m_up, up);
	DirectX::XMStoreFloat3(&m_look, look);
}

void DXCamera::RotateY(float rotation)
{
	// 카메라의 y기저로 도는게 아니라 world의 y로 돌아야 납득할만한 카메라의 뷰를 볼 수 있음.

	DirectX::XMVECTOR rotationAxis = DirectX::XMLoadFloat3(&m_worldUp);
	DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationAxis(rotationAxis, rotation);

	DirectX::XMVECTOR right = DirectX::XMLoadFloat3(&m_right);
	DirectX::XMVECTOR up = DirectX::XMLoadFloat3(&m_up);
	DirectX::XMVECTOR look = DirectX::XMLoadFloat3(&m_look);

	right = DirectX::XMVector3TransformNormal(right, rotationMatrix);
	up = DirectX::XMVector3TransformNormal(up, rotationMatrix);
	look = DirectX::XMVector3TransformNormal(look, rotationMatrix);

	DirectX::XMStoreFloat3(&m_right, right);
	DirectX::XMStoreFloat3(&m_up, up);
	DirectX::XMStoreFloat3(&m_look, look);
}

void DXCamera::MakeViewTM()
{
	// viewTM은 곧 카메라의 역변환을 모든 오브젝트의 점에 걸어주는 것

	// 회전이 여러번 반복되면 수치 오차가 누적되어서 기저 벡터들이 더이상 정규직교벡터가 아닐 수 있기 때문에
	// right, up, look 벡터를 정규직교화 해야함
	// 정규직교가 되지 않고 오차가 계속 누적되다보면 선형종속이 되어 축 하나가 소실될수도 있을듯

	DirectX::XMVECTOR right = DirectX::XMLoadFloat3(&m_right);
	DirectX::XMVECTOR up = DirectX::XMLoadFloat3(&m_up);
	DirectX::XMVECTOR look = DirectX::XMLoadFloat3(&m_look);

	look = DirectX::XMVector3Normalize(look);
	up = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(look, right));
	right = DirectX::XMVector3Cross(up, look);

	// SRT에서
	// 카메라에 스케일은 의미가 없고

	// (RT)-1 은 T-1R-1인데
	// T-1은 position에서 x, y, z의 부호를 바꾼 것이고

	// R-1은 R의 전치행렬과 같음(회전행렬 특성).
	// 여기서 R은 카메라의 기저 right, up, look이 행벡터로 이루어진 것이며
	// R-1은 전치이기 때문에 right, up, look이 열벡터로 변함
	// 그래서 4행 계산은 내적처럼 계산이 가능

	DirectX::XMVECTOR pos = DirectX::XMLoadFloat3(&m_pos);
	pos = DirectX::XMVectorNegate(pos);

	m_viewTM(0, 0) = m_right.x;
	m_viewTM(1, 0) = m_right.y;
	m_viewTM(2, 0) = m_right.z;
	m_viewTM(3, 0) = DirectX::XMVectorGetX(DirectX::XMVector3Dot(pos, right));

	m_viewTM(0, 1) = m_up.x;
	m_viewTM(1, 1) = m_up.y;
	m_viewTM(2, 1) = m_up.z;
	m_viewTM(3, 1) = DirectX::XMVectorGetX(DirectX::XMVector3Dot(pos, up));

	m_viewTM(0, 2) = m_look.x;
	m_viewTM(1, 2) = m_look.y;
	m_viewTM(2, 2) = m_look.z;
	m_viewTM(3, 2) = DirectX::XMVectorGetX(DirectX::XMVector3Dot(pos, look));

	m_viewTM(0, 3) = 0.f;
	m_viewTM(1, 3) = 0.f;
	m_viewTM(2, 3) = 0.f;
	m_viewTM(3, 3) = 1.f;
}

void DXCamera::MakeProjTM()
{
	// 나는 처음에 이 식이 맞다고 생각했음
	// fovx : fovy = width : height
	// fovx * height = fovy * width
	// fovx = fovy * width/height
	// fovx = fovy * aspectRatio

	// 아무리 계산해도 투영 행렬이 이상하게 계산되길래 찾아보니
	// 시야각의 비는 종횡비와 정확히 맞지 않다고 한다.
	// 이유는 아직 못찾음

	// 그래서 fovy를 가지고 스케일링 값을 구하고 거기에 종횡비로 나누니 올바르게 작동했음

	// 시야각이 커지면 더 많은 것들을 눈에 담게되면서 물체 하나하나의 크기(스케일)은 줄어든다.
	// cot 함수의 경우 각이 커질수록 y값이 줄어들게 되어 알맞다.

	// 0.5를 곱해주는 이유는 cot 값이 90도가 넘어가면 음수가 되는데
	// 스케일링이 음수가 되면 반전이 일어나기 때문에 시야각이 최대 179라 가정하면 0.5를 곱해서 90도 보다 작게만들어서
	// 스케일링 값을 음수가 되지 않도록 방지
	float h = 1 / tanf(m_fovAngleY * 0.5f);

	// h는 높이에 대한 스케일링 값이고
	// 나는 너비가 높이보다 시야각을 크게 설정할 것이기 때문에 w는 스케일링 값이 h보다 작아야 한다.
	// 즉 종횡비로 나눠줌으로써 스케일링을 작게함
	float w = h / m_aspectRatio;

	// 입력으로 들어오는 z가 nearZ면 qzn - qzn으로 0이되고
	// z가 farZ면 분자 farZ만 남고 이를 w보존값 z로 나누면 1이되고
	// z가 nearZ, farZ 사이가 되면 결국 0-1사이의 값으로 정규화가 된다.
	float q = m_farZ / (m_farZ - m_nearZ);

	m_projTM(0, 0) = w;
	m_projTM(1, 0) = 0.f;
	m_projTM(2, 0) = 0.f;
	m_projTM(3, 0) = 0.f;

	m_projTM(0, 1) = 0.f;
	m_projTM(1, 1) = h;
	m_projTM(2, 1) = 0.f;
	m_projTM(3, 1) = 0.f;

	m_projTM(0, 2) = 0.f;
	m_projTM(1, 2) = 0.f;
	m_projTM(2, 2) = q;
	m_projTM(3, 2) = -q * m_nearZ;

	m_projTM(0, 3) = 0.f;
	m_projTM(1, 3) = 0.f;
	m_projTM(2, 3) = 1.f; // 여기서 w값을 보존했다가 이후에 rasterization의 원근 나눗셈단계에서 나누면 z가 0.f - 1.f로 정규화가 되는 것
	m_projTM(3, 3) = 0.f;
}
