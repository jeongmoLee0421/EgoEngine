#pragma once

#include "../Inc/DX11Define.h"
#include <wrl/client.h>

// 2023 06 14 이정모 home

// 실제로 그림을 그릴 타겟으로
// 오브젝트 하나에 draw call(draw indexed)이 불릴 때마다 파이프라인 전체를 한번 타게 되고
// 화면의 픽셀을 결정함

class RenderTarget
{
public:
	RenderTarget();
	~RenderTarget();

	void Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, Microsoft::WRL::ComPtr<IDXGISwapChain> pSwapChain,
		long clientWidth, long clientHeight);
	void Finalize();

	// DXRenderer의 BeginRender()에서 사용하는 거라서 raw pointer 반환
	ID3D11RenderTargetView* GetRenderTargetView();
	ID3D11DepthStencilView* GetDepthStencilView();

private:
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_pRenderTargetView;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_pDepthStencil;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_pDepthStencilView;
	D3D11_VIEWPORT m_vp;

private:
	Microsoft::WRL::ComPtr<IDXGISwapChain> m_pSwapChain;
};

