#pragma once

// 2023/06/09 이정모 home

// 렌더러 interface
// 내부가 dx, opengl 등 어떤 그래픽 api에도 대응될 수 있도록

class IRenderer
{
public:
	IRenderer() {}
	virtual ~IRenderer() = 0 {}

public:
	// 윈도우 핸들 값을 받는데 리눅스에서는 어떻게 받아야 할까
	virtual void Initialize(int* hWnd, long clientWidth, long clientHeight) = 0;
	virtual void Finalize() = 0;

	virtual void BeginRender() = 0;
	virtual void Render() = 0;
	virtual void EndRender() = 0;

	virtual void* GetDevice() = 0;
	virtual void* GetDeviceContext() = 0;
};