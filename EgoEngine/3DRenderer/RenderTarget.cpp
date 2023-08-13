#include "RenderTarget.h"

RenderTarget::RenderTarget()
	: m_pRenderTargetView()
	, m_pDepthStencil()
	, m_pDepthStencilView()

	, m_pSwapChain()
	, m_vp()
{
}

RenderTarget::~RenderTarget()
{
	// swapchain을 포함한 나머지 자원들을 comPtr로 감싸두었기 때문에 알아서 해제
}

void RenderTarget::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, Microsoft::WRL::ComPtr<IDXGISwapChain> pSwapChain, long clientWidth, long clientHeight)
{
	// pDevice와 pContext를 여기서 쓰고 더이상 참조하지 않기 때문에 AddRef/Release 하지 않음

	m_pSwapChain = pSwapChain;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> pBackBuffer;
	HR(m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)pBackBuffer.GetAddressOf())); // (&연산자)&pBackBuffer는 인터페이스를 release하고 약한 참조를 반환, .GetAddressOf()는 인터페이스 포인터의 주소를 반환
	HR(pDevice->CreateRenderTargetView(pBackBuffer.Get(), NULL, m_pRenderTargetView.ReleaseAndGetAddressOf()));

	D3D11_TEXTURE2D_DESC descDepth;
	ZeroMemory(&descDepth, sizeof(descDepth));
	descDepth.Width = clientWidth;
	descDepth.Height = clientHeight;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;
	// release를 하고 싶지 않지만 포인터의 주소를 얻고 싶다면 GetAddressOf()
	// release를 하면서 포인터의 주소(정확히는 약한 참조)를 얻고 싶다면 operator& 또는 ReleaseAndGetAddressOf()
	HR(pDevice->CreateTexture2D(&descDepth, NULL, m_pDepthStencil.ReleaseAndGetAddressOf()));

	D3D11_DEPTH_STENCIL_VIEW_DESC descDsv;
	ZeroMemory(&descDsv, sizeof(descDsv));
	descDsv.Format = descDepth.Format;
	descDsv.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDsv.Texture2D.MipSlice = 0;
	// DepthStencilView는 여기서 처음 초기화 하는거라 아무 것도 관리하지 않고 있지만
	// 혹시나 관리하는 것이 있을 경우 그것에 대한 참조 카운트를 감소해야하기 때문에
	// ReleaseAndGetAddressOf()를 호출해서 원래 관리하던 인터페이스의 포인터를 지우고 새로운 인터페이스의 포인터를 할당하기 위해 포인터의 주소(**)를 반환
	HR(pDevice->CreateDepthStencilView(m_pDepthStencil.Get(), &descDsv, m_pDepthStencilView.ReleaseAndGetAddressOf()));

	pContext->OMSetRenderTargets(1, m_pRenderTargetView.GetAddressOf(), m_pDepthStencilView.Get());

	m_vp.Width = (FLOAT)clientWidth;
	m_vp.Height = (FLOAT)clientHeight;
	m_vp.MinDepth = 0.f;
	m_vp.MaxDepth = 1.f;
	m_vp.TopLeftX = 0;
	m_vp.TopLeftY = 0;

	pContext->RSSetViewports(1, &m_vp);
}

void RenderTarget::Finalize()
{

}

ID3D11RenderTargetView* RenderTarget::GetRenderTargetView()
{
	return m_pRenderTargetView.Get();
}

ID3D11DepthStencilView* RenderTarget::GetDepthStencilView()
{
	return m_pDepthStencilView.Get();
}
