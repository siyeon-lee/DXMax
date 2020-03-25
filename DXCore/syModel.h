#pragma once
#include "syDxHelperEx.h"
#include "syTexture.h"
#include "syMatrix.h"

struct PT_VERTEX
{
	D3DXVECTOR3 p;
	D3DXVECTOR2 t;
	/*bool operator == (const PT_VERTEX & Vertex)
	{
		if (p == Vertex.p  && t == Vertex.t)
		{
			return true;
		}
		return  false;
	}*/
	PT_VERTEX() {}
	PT_VERTEX(D3DXVECTOR3		vp,
		D3DXVECTOR2		vt)
	{
		p = vp, t = vt;
	}
};
struct PC_VERTEX
{
	D3DXVECTOR3 p;
	D3DXVECTOR4 c;
	bool operator == (const PC_VERTEX & Vertex)
	{
		if (p == Vertex.p  && c == Vertex.c)
		{
			return true;
		}
		return  false;
	}
	PC_VERTEX() {}
	PC_VERTEX(D3DXVECTOR3		vp,
		D3DXVECTOR4		vc)
	{
		p = vp, c = vc;
	}
};
struct PNCT_VERTEX
{
	D3DXVECTOR3 p;
	D3DXVECTOR3 n;
	D3DXVECTOR4 c;
	D3DXVECTOR2 t;
	//bool operator == (const PNCT_VERTEX & Vertex)
	//{
	//	if (p == Vertex.p  && n == Vertex.n && 	c == Vertex.c  &&	t == Vertex.t)
	//	{
	//		return true;
	//	}
	//	return  false;
	//}
	PNCT_VERTEX() {}
	PNCT_VERTEX(
		D3DXVECTOR3		vp,
		D3DXVECTOR3		vn,
		D3DXVECTOR4		vc,
		D3DXVECTOR2     vt)
	{
		p = vp, n = vn, c = vc, t = vt;
	}
};

struct PNCT2T3_VERTEX
{
	D3DXVECTOR3 p;
	D3DXVECTOR3 n;
	D3DXVECTOR4 c;
	D3DXVECTOR2 t;
	D3DXVECTOR3 tan;

	PNCT2T3_VERTEX() {}
	PNCT2T3_VERTEX(
		D3DXVECTOR3		vp,
		D3DXVECTOR3		vn,
		D3DXVECTOR4		vc,
		D3DXVECTOR2     vt,
		D3DXVECTOR3		vtan)
	{
		p = vp, n = vn, c = vc, t = vt, tan = vtan;
	}
};
struct PNCTI8W8_VERTEX
{
	D3DXVECTOR3		p;
	D3DXVECTOR3		n;
	D3DXVECTOR4		c;
	D3DXVECTOR2     t;
	D3DXVECTOR4		i0;
	D3DXVECTOR4		w0;
	D3DXVECTOR4		i1;
	D3DXVECTOR4		w1;
};
struct VSCB
{
	D3DXMATRIX matWorld;
	D3DXMATRIX matView;
	D3DXMATRIX matProj;
	float color[4];
	float etc[4];
};

class syModel
{
public:
	syModel();
	virtual ~syModel();
public:
	int        m_iTexIndex;
	D3DXMATRIX	m_matWorld;
	D3DXMATRIX	m_matView;
	D3DXMATRIX	m_matProj;
	D3DXMATRIX m_matLastRot;
	VSCB		m_cb;
	SY_BOX      m_syBox;
	SY_SPHERE   m_sySphere;
	vector<PNCT_VERTEX> m_VertexList;
	vector<DWORD>		m_IndexList;
	DX::syDxHelperEx		m_dxObj;
	ID3D11Device*			m_pDevice;
	ID3D11DeviceContext*	m_pImmediateContext;
protected:
	syTexture*   m_pTexture;
	syTexture*   m_pTextureMask;
	FILE *		m_pStream;
public:
	virtual D3DXMATRIX*   FrameMatrix(int iStart, int iEnd, float fTimer) {
		return nullptr;
	};
	virtual bool  Load(
		T_STR filename,
		ID3D11Device* pd3dDevice,
		const TCHAR* pLoadShaderFile);
	virtual bool  Init();
	virtual bool  Frame();
	virtual bool  PreRender(ID3D11DeviceContext* pContext);
	virtual bool  Render(ID3D11DeviceContext* pContext);
	virtual bool  PostRender(ID3D11DeviceContext* pContext);
	virtual bool  Release();
	bool  Create(ID3D11Device* pd3dDevice,
		const TCHAR* pLoadShaderFile,
		const TCHAR* pLoadTextureFile);

	virtual HRESULT LoadShaderFile(
		ID3D11Device* pd3dDevice,
		const TCHAR* pLoadShaderFile);
	virtual HRESULT SetInputLayout();
	virtual HRESULT CreateVertexData();
	virtual HRESULT CreateIndexData();
	virtual HRESULT CreateVertexBuffer();
	virtual HRESULT CreateIndexBuffer();
	virtual HRESULT CreateConstantBuffer();
	virtual HRESULT LoadTextures(
		ID3D11Device* pd3dDevice,
		const TCHAR* pLoadTextureFile);
	virtual bool   UpdateBuffer();
	virtual HRESULT CreateResource();
	void SetCollisionData();

	virtual void  SetMatrix(D3DXMATRIX* pWorld,
		D3DXMATRIX* pView,
		D3DXMATRIX* pProj);
};

