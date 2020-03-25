#include "syDevice.h"
ID3D11Device*			g_pDevice;
syDevice::syDevice()
{
	
	m_pRTView = 0;
	m_pDepthStencilView = 0;
	m_pSwapChain = 0;
	m_pImmediateContext = 0;
	m_pDevice = 0;
}

syDevice::~syDevice()
{
}

bool syDevice::InitDevice()
{
	CreateGIFactory();
	CreateDevice();
	CreateSwapChain();
	SetRenderTargetView();

	SetViewPort();
	return true;
}
HRESULT syDevice::CreateDevice()
{
	HRESULT hr;
	UINT  CreateDeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#ifdef _DEBUG
	CreateDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	//특성레벨 속성값, 디바이스 타입 속성값 설정
	D3D_DRIVER_TYPE driverTypes[] =
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE
	};
	D3D_FEATURE_LEVEL pfeatureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_0
	};
	UINT iNumDriverType = ARRAYSIZE(driverTypes);
	UINT iNumFeatureLevels = ARRAYSIZE(pfeatureLevels);

	for (UINT i = 0; i < iNumDriverType; i++)
	{
		m_DirverType = driverTypes[i];
		if (SUCCEEDED(hr = D3D11CreateDevice(
			NULL, m_DirverType, NULL,
			CreateDeviceFlags, 
			pfeatureLevels, 
			iNumFeatureLevels,
			D3D11_SDK_VERSION, 
			&m_pDevice, 
			&m_FeatureLevel, 
			&m_pImmediateContext)))
		{
			if (m_FeatureLevel < D3D_FEATURE_LEVEL_11_0)
			{

				m_pImmediateContext->Release();
				m_pDevice->Release();

				continue;
			}
			break;
		}
	}
	g_pDevice = m_pDevice;
	if (FAILED(hr))
	{
		return false;
	}
	
	return S_OK;
}
HRESULT syDevice::CreateGIFactory()
{
	HRESULT hr;
	hr = CreateDXGIFactory(__uuidof(IDXGIFactory),
		(void**)&m_pGIFactory);
	return S_OK;
}
HRESULT syDevice::CreateSwapChain()
{
	HRESULT hr;
	//디바이스 객체(CreateDeviceAndSwapChain)와 스왑체인(DXGI_SWAP_CHAIN_DESC) 및 디바이스컨텍스트 생성
	//스왑 체인 속성 지정
	ZeroMemory(&m_SwapChainDesc, sizeof(m_SwapChainDesc));
	m_SwapChainDesc.BufferCount = 1;
	///백 버퍼 세팅
	m_SwapChainDesc.BufferDesc.Width = g_rtClient.right;
	m_SwapChainDesc.BufferDesc.Height = g_rtClient.bottom;
	m_SwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	m_SwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	m_SwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	m_SwapChainDesc.SampleDesc.Count = 1;
	m_SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	m_SwapChainDesc.OutputWindow = m_hWnd;
	m_SwapChainDesc.Windowed = true;

	hr = m_pGIFactory->CreateSwapChain(
		m_pDevice,
		&m_SwapChainDesc,
		&m_pSwapChain);

	return hr;
}
HRESULT syDevice::SetRenderTargetView()
{
	HRESULT hr;
	//랜더 타겟 뷰 생성
	ID3D11Texture2D* pBackBuffer;
	///백 버퍼->타겟뷰
	hr = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
	if (FAILED(hr))
	{
		return false;
	}
	///랜더 타겟 뷰 객체 생성
	hr = m_pDevice->CreateRenderTargetView(pBackBuffer, NULL, &m_pRTView);
	pBackBuffer->Release();	//***반드시 바로 해제
	
	if (FAILED(hr = SetDepthStencilView()))
	{
		return false;
	}
	
	return S_OK;
}
HRESULT syDevice::SetDepthStencilView()
{
	HRESULT hr = S_OK;
	// 깊이스텐실 버퍼 생성
	Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
	D3D11_TEXTURE2D_DESC td;
	ZeroMemory(&td, sizeof(D3D11_TEXTURE2D_DESC));
	td.Width = m_SwapChainDesc.BufferDesc.Width;
	td.Height = m_SwapChainDesc.BufferDesc.Height;
	td.ArraySize = 1;
	td.MipLevels = 1;
	td.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	td.SampleDesc.Count = 1;
	td.SampleDesc.Quality = 0;
	td.Usage = D3D11_USAGE_DEFAULT;
	td.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	hr = m_pDevice->CreateTexture2D(&td, NULL, texture.GetAddressOf());

	D3D11_DEPTH_STENCIL_VIEW_DESC dsd;
	ZeroMemory(&dsd, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	dsd.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

	hr = m_pDevice->CreateDepthStencilView(
		texture.Get(), &dsd, &m_pDepthStencilView);
	// 랜더타켓 바인딩
	///랜더 타겟 뷰를 디바이스가 사용 할 수 있게끔 세팅
	m_pImmediateContext->OMSetRenderTargets(1, &m_pRTView, m_pDepthStencilView);

	return hr;
}
HRESULT syDevice::SetViewPort()
{
	//뷰포트 생성
	m_ViewPort.Width = m_SwapChainDesc.BufferDesc.Width;
	m_ViewPort.Height = m_SwapChainDesc.BufferDesc.Height;
	m_ViewPort.MinDepth = 0;
	m_ViewPort.MaxDepth = 1;
	m_ViewPort.TopLeftX = 0;
	m_ViewPort.TopLeftY = 0;
	m_pImmediateContext->RSSetViewports(1, &m_ViewPort);
	g_rtClient.right = m_SwapChainDesc.BufferDesc.Width;
	g_rtClient.bottom = m_SwapChainDesc.BufferDesc.Height;
	return S_OK;
}

bool syDevice::ResizeDevice(UINT width, UINT height)
{
	if (m_pDevice == nullptr) return true;
	HRESULT hr;
	// 기존에 생성된 객체 소멸
	DeleteDXResource();
	// 랜더타켓과  깊이스텐실 버퍼 해제
	m_pImmediateContext->OMSetRenderTargets(0, NULL, NULL);
	m_pRTView->Release(); //ReleaseANdGetAddressOf();

	// 백버퍼의 크기 조절
	hr = m_pSwapChain->ResizeBuffers(
		m_SwapChainDesc.BufferCount,
		width,
		height,
		m_SwapChainDesc.BufferDesc.Format,
		m_SwapChainDesc.Flags);
	//변경된 백버퍼의 크기를 얻는다
	m_pSwapChain->GetDesc(&m_SwapChainDesc);


	// 랜더타겟, 깊이스텐실 버퍼 생성
	SetRenderTargetView();
	SetViewPort();
	// 기존에 생성된 객체 생성
	CreateDXResource();
	
	return syWindow::ResizeDevice(width, height);
}
HRESULT syDevice::DeleteDXResource()
{
	return S_OK;
}
HRESULT syDevice::CreateDXResource()
{
	return S_OK;
}
bool syDevice::ReleaseDevice()
{
	if (m_pDepthStencilView) m_pDepthStencilView->Release();
	if (m_pRTView)m_pRTView->Release();
	if (m_pSwapChain)m_pSwapChain->Release();
	if (m_pImmediateContext)m_pImmediateContext->Release();
	if (m_pDevice)m_pDevice->Release();

	m_pDepthStencilView = 0;
	m_pRTView = 0;
	m_pSwapChain = 0;
	m_pImmediateContext = 0;
	m_pDevice = 0;
	return true;
}
