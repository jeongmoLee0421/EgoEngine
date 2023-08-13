#pragma once

#include "../Inc/IRenderer.h"
#include "../Inc/DX11Define.h"

// 2023/06/09 이정모 home

// dx11로 구성된 Renderer

class DXDevice;
class RenderTarget;

class DXRenderer: public IRenderer
{
public:
	DXRenderer();
	~DXRenderer();

public:
	void Initialize(int* hWnd, long clientWidth, long clientHeight);
	void Finalize();

	void BeginRender();
	void Render();
	void EndRender();

	void* GetDevice();
	void* GetDeviceContext();

private:
	DXDevice* m_pDXDevice;
	RenderTarget* m_pRenderTarget;

private:
	HWND m_hWnd;
	UINT m_clientWidth;
	UINT m_clientHeight;
};

