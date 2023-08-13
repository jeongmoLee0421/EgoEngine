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

	SetCapture(m_hWnd); // ������ �ۿ��� ���콺�� �����̴��� ���� ĸ���� �����쿡 ���콺 �޽����� ��� ������
}

void DXCamera::OnMouseMove(unsigned long long mouseState, int x, int y)
{
	if (mouseState & MK_RBUTTON)
	{
		// ������ ��ǥ�� ���������� ������ Ŀ���� �Ʒ��� ������ Ŀ��
		int numOfPixelWidth = x - m_prevMouseX;
		int numOfPixelHeight = y - m_prevMouseY;

		// 1�ȼ��� 0.25�� ȸ������ 4�ȼ� ���̸� 1�� ȸ��
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
			// ���ȣ ������ �����ε�
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
			// �Ұ�ȣ ������ �����ε�
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
	// ī�޶��� y������ ���°� �ƴ϶� world�� y�� ���ƾ� �����Ҹ��� ī�޶��� �並 �� �� ����.

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
	// viewTM�� �� ī�޶��� ����ȯ�� ��� ������Ʈ�� ���� �ɾ��ִ� ��

	// ȸ���� ������ �ݺ��Ǹ� ��ġ ������ �����Ǿ ���� ���͵��� ���̻� �����������Ͱ� �ƴ� �� �ֱ� ������
	// right, up, look ���͸� ��������ȭ �ؾ���
	// ���������� ���� �ʰ� ������ ��� �����Ǵٺ��� ���������� �Ǿ� �� �ϳ��� �ҽǵɼ��� ������

	DirectX::XMVECTOR right = DirectX::XMLoadFloat3(&m_right);
	DirectX::XMVECTOR up = DirectX::XMLoadFloat3(&m_up);
	DirectX::XMVECTOR look = DirectX::XMLoadFloat3(&m_look);

	look = DirectX::XMVector3Normalize(look);
	up = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(look, right));
	right = DirectX::XMVector3Cross(up, look);

	// SRT����
	// ī�޶� �������� �ǹ̰� ����

	// (RT)-1 �� T-1R-1�ε�
	// T-1�� position���� x, y, z�� ��ȣ�� �ٲ� ���̰�

	// R-1�� R�� ��ġ��İ� ����(ȸ����� Ư��).
	// ���⼭ R�� ī�޶��� ���� right, up, look�� �຤�ͷ� �̷���� ���̸�
	// R-1�� ��ġ�̱� ������ right, up, look�� �����ͷ� ����
	// �׷��� 4�� ����� ����ó�� ����� ����

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
	// ���� ó���� �� ���� �´ٰ� ��������
	// fovx : fovy = width : height
	// fovx * height = fovy * width
	// fovx = fovy * width/height
	// fovx = fovy * aspectRatio

	// �ƹ��� ����ص� ���� ����� �̻��ϰ� ���Ǳ淡 ã�ƺ���
	// �þ߰��� ��� ��Ⱦ��� ��Ȯ�� ���� �ʴٰ� �Ѵ�.
	// ������ ���� ��ã��

	// �׷��� fovy�� ������ �����ϸ� ���� ���ϰ� �ű⿡ ��Ⱦ��� ������ �ùٸ��� �۵�����

	// �þ߰��� Ŀ���� �� ���� �͵��� ���� ��ԵǸ鼭 ��ü �ϳ��ϳ��� ũ��(������)�� �پ���.
	// cot �Լ��� ��� ���� Ŀ������ y���� �پ��� �Ǿ� �˸´�.

	// 0.5�� �����ִ� ������ cot ���� 90���� �Ѿ�� ������ �Ǵµ�
	// �����ϸ��� ������ �Ǹ� ������ �Ͼ�� ������ �þ߰��� �ִ� 179�� �����ϸ� 0.5�� ���ؼ� 90�� ���� �۰Ը���
	// �����ϸ� ���� ������ ���� �ʵ��� ����
	float h = 1 / tanf(m_fovAngleY * 0.5f);

	// h�� ���̿� ���� �����ϸ� ���̰�
	// ���� �ʺ� ���̺��� �þ߰��� ũ�� ������ ���̱� ������ w�� �����ϸ� ���� h���� �۾ƾ� �Ѵ�.
	// �� ��Ⱦ��� ���������ν� �����ϸ��� �۰���
	float w = h / m_aspectRatio;

	// �Է����� ������ z�� nearZ�� qzn - qzn���� 0�̵ǰ�
	// z�� farZ�� ���� farZ�� ���� �̸� w������ z�� ������ 1�̵ǰ�
	// z�� nearZ, farZ ���̰� �Ǹ� �ᱹ 0-1������ ������ ����ȭ�� �ȴ�.
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
	m_projTM(2, 3) = 1.f; // ���⼭ w���� �����ߴٰ� ���Ŀ� rasterization�� ���� �������ܰ迡�� ������ z�� 0.f - 1.f�� ����ȭ�� �Ǵ� ��
	m_projTM(3, 3) = 0.f;
}
