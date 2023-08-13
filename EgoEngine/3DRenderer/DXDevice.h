#pragma once

#include "../Inc/DX11Define.h"
#include <wrl/client.h>

// 2023 06 14 ������ home

// DX �ʱ�ȭ �� �����Ǵ� device, context, swap chain�� �������� ���� ������ Ŭ������ ����
// �̸� �ʿ�� �ϴ� ���� �ѷ��� ����

class DXDevice
{
public:
	DXDevice();
	~DXDevice();

	void Initialize(int* hWnd, long clientWidth, long clientHeight);
	void Finalize();

	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetImmediateContext();

	// DXRederer�� EndRender()���� ���
	IDXGISwapChain* GetSwapChainRawPtr();

	// RenderTarget���� ����� ��� �ֱ⿡ ���
	Microsoft::WRL::ComPtr<IDXGISwapChain> GetSwapChainComPtr();

private:
	ID3D11Device* m_pd3dDevice;
	ID3D11DeviceContext* m_pImmediateContext;

	// swap chain�� ��� resourceManager�� ������ �ʿ䰡 ���
	// comPtr�� ���� �ʿ��� ���� comPtr�� �����ؼ� ������
	// �ڵ����� reference counting
	Microsoft::WRL::ComPtr<IDXGISwapChain> m_pSwapChain;

private:
	D3D_DRIVER_TYPE m_driverType;
	D3D_FEATURE_LEVEL m_featureLevel;
};

