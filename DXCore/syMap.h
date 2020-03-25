#pragma once
#include "syObject.h"

struct syMapDesc
{
	int iNumCols;
	int iNumRows;
	int iCellNum;
	float fCellDistance;
	float fHeightScale;
	T_STR szmapTextureFile;
	T_STR szShaderFile;
	T_STR szCubeFile;
};
class syMap : public syModel
{
public:
	int m_iNumRows;
	int m_iNumCols;
	int m_iNumCellRows;
	int m_iNumCellCols;
	int m_iNumVertices;
	int m_iCellNum;
	int m_iNumFace;
	float m_fCellDistance;
	float m_fHeightScale;

public:
	int*								m_pNormalLookupTable;
	D3DXVECTOR3*						m_pFaceNormals;
	vector<PNCT_VERTEX>					m_VertexList;
	D3DXMATRIX							m_matNormal;
	syTexture*	m_pMapTex;
	//ÀÌ°Å ÅØ½ºÃÄ·Î ¹Ù²ã¾ß ÇÔ ±×·¡¾ß texindex¶û ¸ÊÇÎµÊ 
	syTexture*							m_backgroundTex[3];
	//ComPtr<ID3D11ShaderResourceView>	m_backgroundTex[3];

	vector<D3DXVECTOR3>					m_FaceNormals;
	vector<D3DXVECTOR3>					m_vTangentList;
	ComPtr<ID3D11Buffer>				m_pTangentVB;

public:
	void SetBGTex(int slot, int texID);

	void	GenVertexNormal();
	void    InitFaceNormals();
	void	CalcFaceNormals();


	D3DXVECTOR3	ComputeFaceNormal(DWORD i0, DWORD i1, DWORD i2);
	virtual float   GetHeightMap(UINT index)
	{
		return 0.0f;
	};
	virtual  D3DXVECTOR3  GetNormalMap(UINT index)
	{
		return D3DXVECTOR3(0, 1, 0);
	};
	virtual D3DXVECTOR4   GetColorMap(UINT index)
	{
		return D3DXVECTOR4(1, 1, 1, 1);
	};
public:
	void		GenNormalLookupTable();
	void		CalcPerVertexNormalsFastLookup();
	virtual bool    Load(ID3D11Device* pd3dDevice, syMapDesc& md);
	HRESULT CreateVertexData();
	HRESULT CreateVertexBuffer();
	HRESULT CreateIndexBuffer();
	HRESULT CreateIndexData();
	HRESULT SetInputLayout();
	bool UpdateBuffer();
public:
	void CreateTangentSpaceVectors(D3DXVECTOR3 *v0,
		D3DXVECTOR3 *v1, D3DXVECTOR3 *v2, D3DXVECTOR2 uv0,
		D3DXVECTOR2 uv1, D3DXVECTOR2 uv2,
		D3DXVECTOR3 *vTangent, D3DXVECTOR3 *vBiNormal, D3DXVECTOR3 *vNormal);
	void CreateTangentSpaceVectors(D3DXVECTOR3 *v1,
		D3DXVECTOR3 *v2,
		D3DXVECTOR3 *v3,
		float v1u, float v1v,
		float v2u, float v2v,
		float v3u, float v3v,
		D3DXVECTOR3 *vTangent,
		D3DXVECTOR3 *vBiNormal,
		D3DXVECTOR3 *vNormal);

public:

	virtual void PreRender(ID3D11DeviceContext* pContext, UINT iSize);
	bool	Render(ID3D11DeviceContext*	pContext);
	bool	Release();
public:
	syMap();
	virtual ~syMap();
};

class syHeightMap : public syMap
{
public:
	syHeightMap() { m_fHeightScale = 1.0f; };
	virtual ~syHeightMap() {}
public:
	vector<float >  m_fHeightList;
public:
	virtual float   GetHeightMap(UINT index)
	{
		return m_fHeightList[index] / m_fHeightScale;
	};
	virtual float GetHeightmap(int row, int col)
	{
		return m_fHeightList[row * m_iNumRows + col] / m_fHeightScale;
	}
	float Lerp(float fStart, float fEnd, float fTangent)
	{
		return fStart - (fStart*fTangent) + (fEnd*fTangent);
	}

	bool CreateHeightMap(ID3D11Device* pd3dDevice, ID3D11DeviceContext*	pContext, const TCHAR* pMapFile);

};