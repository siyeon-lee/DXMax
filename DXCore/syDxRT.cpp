#include "syDxRT.h"



syDxRT::syDxRT()
{
}


syDxRT::~syDxRT()
{
}
HRESULT syDxRT::Create(ID3D11Device* pd3dDevice, FLOAT Width, FLOAT Height)
{
	HRESULT hr = S_OK;
	D3D11_TEXTURE2D_DESC td;
	ZeroMemory(&td, sizeof(D3D11_TEXTURE2D_DESC));
	td.Width = Width;
	td.Height = Height;
	td.MipLevels = 1;
	td.SampleDesc.Count = 1;
	td.SampleDesc.Quality = 0;
	td.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	td.Usage = D3D11_USAGE_DEFAULT;
	td.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	td.ArraySize = 1;
	hr = pd3dDevice->CreateTexture2D(&td, NULL, m_pTexture.GetAddressOf());
	hr = pd3dDevice->CreateShaderResourceView(m_pTexture.Get(), NULL, m_pSRV.GetAddressOf() );
	hr = pd3dDevice->CreateRenderTargetView(m_pTexture.Get(), NULL, m_pRTV.GetAddressOf());
	
	td.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	td.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	hr = pd3dDevice->CreateTexture2D(&td, NULL, m_pDSTexture.GetAddressOf());

	D3D11_DEPTH_STENCIL_VIEW_DESC dsd;
	ZeroMemory(&dsd, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	dsd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	//dsd.Texture2d.MipSlice


	hr = pd3dDevice->CreateDepthStencilView(m_pDSTexture.Get(), &dsd, m_pDSDSV.GetAddressOf());
	
	m_vp.Height = Height;		//텍스쳐 데스크랑 스텐실 뷰 데스크 크기는 똑같아야함
	m_vp.Width = Width;
	m_vp.MaxDepth = 1;
	m_vp.MinDepth = 0;
	m_vp.TopLeftX = 0;
	m_vp.TopLeftY = 0;


	return hr;
}
bool    syDxRT::Begin(ID3D11DeviceContext*    pContext)
{



	m_iViewPorts = 1;
	pContext->RSGetViewports(&m_iViewPorts, &m_vpOLD);
	pContext->OMGetRenderTargets(1, m_prtvOLD.GetAddressOf(), m_pdsvOLD.GetAddressOf());

	float fColor[] = { 0,0,0,1 };
	pContext->ClearRenderTargetView(m_pRTV.Get(), fColor);
	pContext->ClearDepthStencilView(m_pDSDSV.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	ID3D11RenderTargetView* pRTVs = NULL;
	ID3D11DepthStencilView* pDSV = NULL;
	pContext->OMSetRenderTargets(1, &pRTVs, pDSV);
	pContext->OMSetRenderTargets(1, m_pRTV.GetAddressOf(), m_pDSDSV.Get());
	pContext->RSSetViewports(1, &m_vp);
	return true;
}
bool    syDxRT::End(ID3D11DeviceContext*    pContext)
{

	ID3D11RenderTargetView* pRTVs = NULL;
	ID3D11DepthStencilView* pDSV = NULL;
	pContext->OMSetRenderTargets(1, &pRTVs, pDSV);

	pContext->RSSetViewports(m_iViewPorts, &m_vpOLD);
	pContext->OMSetRenderTargets(1, m_prtvOLD.GetAddressOf(), m_pdsvOLD.Get());

	m_prtvOLD.Reset();
	m_pdsvOLD.Reset();
	return true;
}

bool	syDxRT::DrawAlphaTexture(ID3D11DeviceContext*    pContext)
{


	m_iViewPorts = 1;
	pContext->RSGetViewports(&m_iViewPorts, &m_vpOLD);
	pContext->OMGetRenderTargets(1, m_prtvOLD.GetAddressOf(), m_pdsvOLD.GetAddressOf());

	ID3D11RenderTargetView* pRTVs = NULL;


	pContext->OMSetRenderTargets(1, m_pRTV.GetAddressOf(), nullptr);
	pContext->RSSetViewports(1, &m_vp);
	return true;
}

bool	syDxRT::EndAlphaTexture(ID3D11DeviceContext*    pContext)
{

	ID3D11RenderTargetView* pRTVs = NULL;
	ID3D11DepthStencilView* pDSV = NULL;
	pContext->OMSetRenderTargets(1, &pRTVs, pDSV);

	pContext->RSSetViewports(m_iViewPorts, &m_vpOLD);
	pContext->OMSetRenderTargets(1, m_prtvOLD.GetAddressOf(), m_pdsvOLD.Get());

	//m_prtvOLD.Reset();
	//m_pdsvOLD.Reset();
	return true;
}

bool    syDxRT::Release()
{
	return true;
}