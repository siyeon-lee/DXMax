#include "syDxState.h"

namespace DX
{
	ID3D11DepthStencilState*   syDxState::m_pDSSDepthEnable = 0;
	ID3D11DepthStencilState*   syDxState::m_pDSSDepthDisable = 0;
	ID3D11BlendState*	syDxState::m_pAlphaBlend = 0;
	ID3D11BlendState*	syDxState::m_pAlphaBlendDisable = 0;
	ID3D11BlendState*	syDxState::m_pAlphaBlend2 = 0;
	ID3D11BlendState*   syDxState::m_pAlphaBlendminus = 0;
	ID3D11RasterizerState*		syDxState::m_pRSWireFrame = 0;
	ID3D11SamplerState*      syDxState::m_pSSWrapPoint = 0;
	ID3D11SamplerState*      syDxState::m_pSSWrapAniso = 0;
	ID3D11RasterizerState*		syDxState::m_pRSSolidFrame = 0;
	ID3D11SamplerState*			syDxState::m_pSSWrapLinear = 0;
	syDxState::syDxState()
	{

	}


	syDxState::~syDxState()
	{
		Release();
	}

	void syDxState::SetState(ID3D11Device* pd3dDevice)
	{
		if (pd3dDevice == nullptr) return;

		HRESULT hr;
#pragma region ID3D11DepthStencilState
		D3D11_DEPTH_STENCIL_DESC dsd;
		ZeroMemory(&dsd, sizeof(D3D11_DEPTH_STENCIL_DESC));
		dsd.DepthEnable = TRUE;
		dsd.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
		dsd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		pd3dDevice->CreateDepthStencilState(&dsd, &m_pDSSDepthEnable);

		dsd.DepthEnable = FALSE;
		pd3dDevice->CreateDepthStencilState(&dsd, &m_pDSSDepthDisable);
#pragma endregion ID3D11DepthStencilState

#pragma region ID3D11SamplerState
		D3D11_SAMPLER_DESC sd;
		ZeroMemory(&sd, sizeof(D3D11_SAMPLER_DESC));
		sd.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		sd.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		sd.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		sd.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		sd.MaxLOD = FLT_MAX;
		sd.MinLOD = FLT_MIN;
		sd.MaxAnisotropy = 16;
		if (FAILED(hr = pd3dDevice->CreateSamplerState(
			&sd,
			&m_pSSWrapLinear)))
		{
			return;
		}
		sd.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		if (FAILED(hr = pd3dDevice->CreateSamplerState(
			&sd,
			&m_pSSWrapPoint)))
		{
			return;
		}

		sd.Filter = D3D11_FILTER_ANISOTROPIC;
		if (FAILED(hr = pd3dDevice->CreateSamplerState(
			&sd,
			&m_pSSWrapAniso)))
		{
			return;
		}
#pragma endregion ID3D11SamplerState

#pragma region BLEND
		//잘모르겠삼
		D3D11_BLEND_DESC ds;
		ZeroMemory(&ds, sizeof(D3D11_BLEND_DESC));
		ds.AlphaToCoverageEnable = FALSE;
		ds.IndependentBlendEnable = TRUE;

		ds.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		ds.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		ds.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;

		ds.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		ds.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		ds.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;

		ds.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		ds.RenderTarget[0].BlendEnable = FALSE;
		if (FAILED(hr = pd3dDevice->CreateBlendState(&ds, &m_pAlphaBlendDisable)))
		{
			return;
		}
		ds.RenderTarget[0].BlendEnable = TRUE;
		if (FAILED(hr = pd3dDevice->CreateBlendState(&ds, &m_pAlphaBlend)))
		{
			return;
		}
		ds.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
		if (FAILED(hr = pd3dDevice->CreateBlendState(&ds, &m_pAlphaBlend2)))
		{
			return;
		}
		////////지우개
		ds.RenderTarget[0].BlendOp = D3D11_BLEND_OP_REV_SUBTRACT;
		ds.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;

		if (FAILED(hr = pd3dDevice->CreateBlendState(&ds, &m_pAlphaBlendminus)))
		{
			return;
		}
		
#pragma endregion BLEND

#pragma region RASTERIZER
		//레스터라이즈 상태 객체 생성
		D3D11_RASTERIZER_DESC rsDesc;
		ZeroMemory(&rsDesc, sizeof(D3D11_RASTERIZER_DESC));
		rsDesc.DepthClipEnable = TRUE;
		rsDesc.FillMode = D3D11_FILL_WIREFRAME;
		rsDesc.CullMode = D3D11_CULL_NONE;
		//rsDesc.MultisampleEnable = TRUE;		//(추가)
	//	rsDesc.AntialiasedLineEnable = TRUE;	//(추가)
		if (FAILED(hr = pd3dDevice->CreateRasterizerState(&rsDesc, &m_pRSWireFrame)))
		{
			return;
		}

		rsDesc.FillMode = D3D11_FILL_SOLID;
		//rsDesc.CullMode = D3D11_CULL_BACK;		//(추가)
		if (FAILED(hr = pd3dDevice->CreateRasterizerState(&rsDesc, &m_pRSSolidFrame)))
		{
			return;
		}
#pragma endregion RASTERIZER
	}
	void syDxState::Release()
	{
		if (m_pSSWrapAniso)
			m_pSSWrapAniso->Release();
		if (m_pSSWrapLinear)
			m_pSSWrapLinear->Release();
		if (m_pSSWrapPoint)
			m_pSSWrapPoint->Release();
		if (m_pAlphaBlend)
			m_pAlphaBlend->Release();

		if (m_pAlphaBlendDisable)
			m_pAlphaBlendDisable->Release();

		if (m_pRSWireFrame)
			m_pRSWireFrame->Release();
		if (m_pRSSolidFrame)
			m_pRSSolidFrame->Release();

		if (m_pDSSDepthEnable)
			m_pDSSDepthEnable->Release();
		if (m_pDSSDepthDisable)
			m_pDSSDepthDisable->Release();

		m_pDSSDepthEnable = 0;
		m_pDSSDepthDisable = 0;
		m_pRSSolidFrame = 0;
		m_pRSWireFrame = 0;
		m_pSSWrapPoint = 0;
		m_pAlphaBlend = 0;
		m_pSSWrapLinear = 0;
		m_pSSWrapAniso = 0;
	}
	syDxState g_DXState;
}