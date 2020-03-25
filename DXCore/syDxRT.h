#pragma once
#include "syObject.h"
class syDxRT
{
public:
	syDxRT();
	virtual ~syDxRT();
public:
	Microsoft::WRL::ComPtr<ID3D11Texture2D>				m_pTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	m_pSRV;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>		m_pRTV;





	Microsoft::WRL::ComPtr<ID3D11Texture2D>				m_pDSTexture;
	//Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	m_pDSSRV;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>		m_pDSDSV;
	D3D11_VIEWPORT										m_vp;

	UINT												m_iViewPorts;
	D3D11_VIEWPORT										m_vpOLD;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>		m_prtvOLD;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>		m_pdsvOLD;

public:
	HRESULT Create(ID3D11Device* pd3dDevice, FLOAT Width, FLOAT Height);
	bool    Begin(ID3D11DeviceContext*    pContext);
	bool    End(ID3D11DeviceContext*    pContext);
	bool	DrawAlphaTexture(ID3D11DeviceContext*    pContext);
	bool	EndAlphaTexture(ID3D11DeviceContext*    pContext);
	bool    Release();

};

