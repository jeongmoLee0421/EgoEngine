#include "DXRenderer.h"
#include "../Inc/DXDevice.h"
#include "../Inc/RenderTarget.h"

DXRenderer::DXRenderer()
	: m_pDXDevice(nullptr)
	, m_pRenderTarget(nullptr)

	, m_hWnd(nullptr)
	, m_clientWidth(0)
	, m_clientHeight(0)
{
}

DXRenderer::~DXRenderer()
{
	delete m_pDXDevice;
	delete m_pRenderTarget;
}

void DXRenderer::Initialize(int* hWnd, long clientWidth, long clientHeight)
{
	HRESULT hr = S_OK;

	m_hWnd = (HWND)hWnd;

	m_clientWidth = clientWidth;
	m_clientHeight = clientHeight;

	m_pDXDevice = new DXDevice();
	m_pDXDevice->Initialize((int*)m_hWnd, m_clientWidth, m_clientHeight);

	m_pRenderTarget = new RenderTarget();
	m_pRenderTarget->Initialize(m_pDXDevice->GetDevice(), m_pDXDevice->GetImmediateContext(), m_pDXDevice->GetSwapChainComPtr(),
		m_clientWidth, m_clientHeight);
}

void DXRenderer::Finalize()
{
	m_pDXDevice->Finalize();
	m_pRenderTarget->Finalize();
}

void DXRenderer::BeginRender()
{
	float clearColor[4] = { 1.f, 1.f, 1.f, 1.f };
	m_pDXDevice->GetImmediateContext()->ClearRenderTargetView(m_pRenderTarget->GetRenderTargetView(), clearColor);

	m_pDXDevice->GetImmediateContext()->ClearDepthStencilView(m_pRenderTarget->GetDepthStencilView(), D3D11_CLEAR_DEPTH, 1.f, NULL);
}

void DXRenderer::Render()
{

}

void DXRenderer::EndRender()
{
	m_pDXDevice->GetSwapChainRawPtr()->Present(0, 0);
}

void* DXRenderer::GetDevice()
{
	return reinterpret_cast<void*>(m_pDXDevice->GetDevice());
}

void* DXRenderer::GetDeviceContext()
{
	return reinterpret_cast<void*>(m_pDXDevice->GetImmediateContext());
}
