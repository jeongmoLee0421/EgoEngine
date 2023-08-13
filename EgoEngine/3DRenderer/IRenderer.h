#pragma once

// 2023/06/09 ������ home

// ������ interface
// ���ΰ� dx, opengl �� � �׷��� api���� ������ �� �ֵ���

class IRenderer
{
public:
	IRenderer() {}
	virtual ~IRenderer() = 0 {}

public:
	// ������ �ڵ� ���� �޴µ� ������������ ��� �޾ƾ� �ұ�
	virtual void Initialize(int* hWnd, long clientWidth, long clientHeight) = 0;
	virtual void Finalize() = 0;

	virtual void BeginRender() = 0;
	virtual void Render() = 0;
	virtual void EndRender() = 0;

	virtual void* GetDevice() = 0;
	virtual void* GetDeviceContext() = 0;
};