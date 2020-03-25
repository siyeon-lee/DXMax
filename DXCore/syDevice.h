#pragma once
#include "syWindow.h"

class syDevice : public syWindow
{
public:
	syDevice();
	virtual ~syDevice();
public:

	IDXGIFactory*			m_pGIFactory;
	ID3D11Device*			m_pDevice;						//디바이스 객체
	ID3D11DeviceContext*	m_pImmediateContext;			//디바이스 컨텍스트 객체
	IDXGISwapChain*			m_pSwapChain;					//스왑체인 객체
	ID3D11RenderTargetView*	m_pRTView;						//메인 랜더타겟 뷰
	ID3D11DepthStencilView* m_pDepthStencilView;			//z버퍼 )깊이_
	D3D_FEATURE_LEVEL		m_FeatureLevel;					//DirectD 특성 레벨 속성값
	D3D_DRIVER_TYPE			m_DirverType;					//디바이스 타입 속성값
	DXGI_SWAP_CHAIN_DESC	m_SwapChainDesc;				//스왑체인 속성값
	D3D11_VIEWPORT			m_ViewPort;						//뷰포트 속성값

public:
	bool  InitDevice();
		HRESULT CreateDevice();
		HRESULT CreateGIFactory();
		HRESULT CreateSwapChain();
		HRESULT SetRenderTargetView();
		HRESULT SetDepthStencilView();
		HRESULT SetViewPort();

	bool ReleaseDevice();
	bool ResizeDevice(UINT width, UINT height) override;
	virtual HRESULT DeleteDXResource();
	virtual HRESULT CreateDXResource();
};

