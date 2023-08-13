#include "DXDevice.h"

DXDevice::DXDevice()
	: m_driverType(D3D_DRIVER_TYPE_NULL)
	, m_featureLevel(D3D_FEATURE_LEVEL_11_0)

	, m_pd3dDevice(nullptr)
	, m_pImmediateContext(nullptr)
	, m_pSwapChain()
{
}

DXDevice::~DXDevice()
{
}

void DXDevice::Initialize(int* hWnd, long clientWidth, long clientHeight)
{
	HRESULT hr = S_OK;

	UINT createDeviceFlags = 0;
#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_DRIVER_TYPE driverTypes[] =
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE,
	};
	UINT numDriverTypes = ARRAYSIZE(driverTypes);

	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
	};
	UINT numFeatureLevels = ARRAYSIZE(featureLevels);

	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 1;
	sd.BufferDesc.Width = clientWidth;
	sd.BufferDesc.Height = clientHeight;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = (HWND)hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;

	for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; ++driverTypeIndex)
	{
		m_driverType = driverTypes[driverTypeIndex];
		hr = D3D11CreateDeviceAndSwapChain
		(
			NULL, m_driverType, NULL, createDeviceFlags, featureLevels, numFeatureLevels,
			D3D11_SDK_VERSION, &sd, m_pSwapChain.ReleaseAndGetAddressOf(), &m_pd3dDevice, &m_featureLevel, &m_pImmediateContext // 처음 자원 생성할 때는 참조 개수 1
		);

		if (SUCCEEDED(hr))
		{
			break;
		}
	}
	if (FAILED(hr)) return;
}

void DXDevice::Finalize()
{
	if (m_pd3dDevice)
	{
		m_pd3dDevice->Release();
		m_pd3dDevice = nullptr;
	}

	if (m_pImmediateContext)
	{
		m_pImmediateContext->Release();
		m_pImmediateContext = nullptr;
	}
}

ID3D11Device* DXDevice::GetDevice()
{
	return m_pd3dDevice;
}

ID3D11DeviceContext* DXDevice::GetImmediateContext()
{
	return m_pImmediateContext;
}

IDXGISwapChain* DXDevice::GetSwapChainRawPtr()
{
	return m_pSwapChain.Get();
}

Microsoft::WRL::ComPtr<IDXGISwapChain> DXDevice::GetSwapChainComPtr()
{
	return m_pSwapChain;
}
