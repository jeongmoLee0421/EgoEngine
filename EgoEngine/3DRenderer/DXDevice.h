#pragma once

#include "../Inc/DX11Define.h"
#include <wrl/client.h>

// 2023 06 14 이정모 home

// DX 초기화 후 생성되는 device, context, swap chain은 응집도가 높기 때문에 클래스로 묶고
// 이를 필요로 하는 곳에 뿌려줄 것임

class DXDevice
{
public:
	DXDevice();
	~DXDevice();

	void Initialize(int* hWnd, long clientWidth, long clientHeight);
	void Finalize();

	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetImmediateContext();

	// DXRederer의 EndRender()에서 사용
	IDXGISwapChain* GetSwapChainRawPtr();

	// RenderTarget에서 멤버로 들고 있기에 사용
	Microsoft::WRL::ComPtr<IDXGISwapChain> GetSwapChainComPtr();

private:
	ID3D11Device* m_pd3dDevice;
	ID3D11DeviceContext* m_pImmediateContext;

	// swap chain의 경우 resourceManager가 관리할 필요가 없어서
	// comPtr로 만들어서 필요한 곳에 comPtr로 복사해서 나눠줌
	// 자동으로 reference counting
	Microsoft::WRL::ComPtr<IDXGISwapChain> m_pSwapChain;

private:
	D3D_DRIVER_TYPE m_driverType;
	D3D_FEATURE_LEVEL m_featureLevel;
};

