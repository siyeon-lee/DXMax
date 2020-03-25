#pragma once
#include <d3dx11.h>
#include <memory>

namespace DX
{
	class syDxState
	{
	public:
		syDxState();
		~syDxState();
	public:
		//ID3D11BlendState
		static ID3D11BlendState*	m_pAlphaBlend;
		static ID3D11BlendState*	m_pAlphaBlend2;
		static ID3D11BlendState*	m_pAlphaBlendminus;
		static ID3D11BlendState*	m_pAlphaBlendDisable;

		//ID3D11RasterizerState
		static ID3D11RasterizerState*		m_pRSWireFrame;
		static ID3D11RasterizerState*		m_pRSSolidFrame;

		//ID3D11SamplerState*  
		static ID3D11SamplerState*   m_pSSWrapLinear;
		static ID3D11SamplerState*   m_pSSWrapPoint;
		static ID3D11SamplerState*   m_pSSWrapAniso;

		//ID3D11DepthStencilState* 
		static ID3D11DepthStencilState*   m_pDSSDepthEnable;
		static ID3D11DepthStencilState*   m_pDSSDepthDisable;
	public:
		static void SetState(ID3D11Device* pd3dDevice);
		static void Release();
	};
	static void ApplyDSS(ID3D11DeviceContext* pContext,
		ID3D11DepthStencilState* pDSS,
		UINT iRef = 1)
	{
		pContext->OMSetDepthStencilState(pDSS, iRef);
	}
	static void ApplySS(ID3D11DeviceContext* pContext,
		ID3D11SamplerState* pState, UINT iSlot = 0, UINT iArray = 1)
	{
		pContext->PSSetSamplers(iSlot, iArray, &pState);
	};

	static void ApplyRS(ID3D11DeviceContext* pContext, ID3D11RasterizerState* pState)
	{
		pContext->RSSetState(pState);
	};
	static void ApplyBS(ID3D11DeviceContext* pContext, ID3D11BlendState* pState,
		const FLOAT fBlendFactor[] = 0, UINT iMask = 0xff)
	{
		pContext->OMSetBlendState(pState, fBlendFactor,iMask);
	};

}


