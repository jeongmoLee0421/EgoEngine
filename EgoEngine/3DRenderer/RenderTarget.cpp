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
	// swapchain�� ������ ������ �ڿ����� comPtr�� ���εξ��� ������ �˾Ƽ� ����
}

void RenderTarget::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, Microsoft::WRL::ComPtr<IDXGISwapChain> pSwapChain, long clientWidth, long clientHeight)
{
	// pDevice�� pContext�� ���⼭ ���� ���̻� �������� �ʱ� ������ AddRef/Release ���� ����

	m_pSwapChain = pSwapChain;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> pBackBuffer;
	HR(m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)pBackBuffer.GetAddressOf())); // (&������)&pBackBuffer�� �������̽��� release�ϰ� ���� ������ ��ȯ, .GetAddressOf()�� �������̽� �������� �ּҸ� ��ȯ
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
	// release�� �ϰ� ���� ������ �������� �ּҸ� ��� �ʹٸ� GetAddressOf()
	// release�� �ϸ鼭 �������� �ּ�(��Ȯ���� ���� ����)�� ��� �ʹٸ� operator& �Ǵ� ReleaseAndGetAddressOf()
	HR(pDevice->CreateTexture2D(&descDepth, NULL, m_pDepthStencil.ReleaseAndGetAddressOf()));

	D3D11_DEPTH_STENCIL_VIEW_DESC descDsv;
	ZeroMemory(&descDsv, sizeof(descDsv));
	descDsv.Format = descDepth.Format;
	descDsv.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDsv.Texture2D.MipSlice = 0;
	// DepthStencilView�� ���⼭ ó�� �ʱ�ȭ �ϴ°Ŷ� �ƹ� �͵� �������� �ʰ� ������
	// Ȥ�ó� �����ϴ� ���� ���� ��� �װͿ� ���� ���� ī��Ʈ�� �����ؾ��ϱ� ������
	// ReleaseAndGetAddressOf()�� ȣ���ؼ� ���� �����ϴ� �������̽��� �����͸� ����� ���ο� �������̽��� �����͸� �Ҵ��ϱ� ���� �������� �ּ�(**)�� ��ȯ
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
