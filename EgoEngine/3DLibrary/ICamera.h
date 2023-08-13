#pragma once

#include "../Inc/EgoMatrix4x4.h"

// 2023 06 12 이정모 home

// 카메라의 interface로 내부 구현이 dx 일수도, opengl 일수도 있음
// 하지만 interface는 그래픽 api에 종속적이면 안되기 떄문에 직접 만든 행렬 사용

class ICamera
{
public:
	ICamera() {}
	virtual ~ICamera() = 0 {}

public:
	virtual void Initialize(int* hWnd) = 0;
	virtual void Finalize() = 0;

public:
	virtual void Update(float dt) = 0;

public:
	virtual void OnMouseDown(int x, int y) = 0;
	virtual void OnMouseMove(unsigned long long mouseState, int x, int y) = 0;
	virtual void OnMouseUp() = 0;

public:
	virtual void Walk(float distance) = 0;
	virtual void Strafe(float distance) = 0;
	virtual void UpDown(float distance) = 0;

	virtual void SetLens(float fovAngleY, float aspectRatio, float nearZ, float farZ) = 0;

	virtual void Pitch(float rotation) = 0;
	virtual void RotateY(float rotation) = 0;

public:
	virtual void MakeViewTM() = 0;
	virtual void MakeProjTM() = 0;
	virtual EgoMath::EgoMatrix4x4 GetViewTM() = 0;
	virtual EgoMath::EgoMatrix4x4 GetProjTM() = 0;
};