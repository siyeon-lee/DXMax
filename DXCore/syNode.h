#pragma once
#include "syModel.h"
#include <time.h>
struct SY_BaseObj
{
	SY_BOX box;
	SY_BaseObj()
	{

	}
};
class syNode
{
public:
	syNode();
	virtual ~syNode();
public:
	vector<syNode*>			m_ChildList;
	vector<syNode*>			m_ParentList;
	vector<syNode*>			m_NeighborList;
	vector<DWORD>			m_CornerIndex;
	SY_BOX					m_tBox;
	BOOL					m_isLeaf;
	UINT					m_iDepth;
	vector<SY_BaseObj*>		m_ObjectList;
	vector<DWORD>			m_IndexData;
	vector<PNCT_VERTEX>		m_VertexData;
	int						m_VertexSize;
	ComPtr<ID3D11Buffer>	m_IndexBuffer;
	ComPtr<ID3D11Buffer>	m_VertexBuffer;
	ComPtr<ID3D11Buffer>    m_pTangentVB;
	syTexture*	m_DrawTex;

	/*ComPtr<ID3D11ShaderResourceView>	m_backgroundTex;*/
	void SetMapTex(int texID);
	//void SetbackgroundTex(int texID);
};

