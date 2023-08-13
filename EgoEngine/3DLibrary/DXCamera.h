#pragma once

#include "../Inc/DX11Define.h"
#include "../Inc/ICamera.h"
#include "../Inc/EgoMatrix4x4.h"

// 2023 06 11 이정모 home

// 전후, 좌우, 상하 이동
// local x축 회전(pitch), world y축 회전

class DXCamera : public ICamera
{
public:
	DXCamera();
	~DXCamera();

public:
	void Initialize(int* hWnd);
	void Finalize();

public:
	void Update(float dt);

public:
	void OnMouseDown(int x, int y);
	void OnMouseMove(unsigned long long mouseState, int x, int y);
	void OnMouseUp();

public:
	void Walk(float distance);
	void Strafe(float distance);
	void UpDown(float distance);

	void SetLens(float fovAngleY, float aspectRatio, float nearZ, float farZ);

	void Pitch(float rotation);
	void RotateY(float rotation);

public:
	void MakeViewTM();
	void MakeProjTM();

	EgoMath::EgoMatrix4x4 GetViewTM();
	EgoMath::EgoMatrix4x4 GetProjTM();

private:
	// viewTM
	DirectX::XMFLOAT3 m_pos;
	DirectX::XMFLOAT3 m_look;
	DirectX::XMFLOAT3 m_right;
	DirectX::XMFLOAT3 m_up;
	DirectX::XMFLOAT3 m_worldUp;
	DirectX::XMFLOAT4X4 m_viewTM;

	// projTM
	float m_fovAngleY; // radian
	float m_aspectRatio;
	float m_nearZ;
	float m_farZ;
	DirectX::XMFLOAT4X4 m_projTM;

private:
	HWND m_hWnd;
	float m_moveSpeed;
	int m_prevMouseX;
	int m_prevMouseY;
};

