#pragma once

#include "../Inc/DX11Define.h"
#include <wrl/client.h>

// 2023 06 14 ������ home

// ������ �׸��� �׸� Ÿ������
// ������Ʈ �ϳ��� draw call(draw indexed)�� �Ҹ� ������ ���������� ��ü�� �ѹ� Ÿ�� �ǰ�
// ȭ���� �ȼ��� ������

class RenderTarget
{
public:
	RenderTarget();
	~RenderTarget();

	void Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, Microsoft::WRL::ComPtr<IDXGISwapChain> pSwapChain,
		long clientWidth, long clientHeight);
	void Finalize();

	// DXRenderer�� BeginRender()���� ����ϴ� �Ŷ� raw pointer ��ȯ
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

