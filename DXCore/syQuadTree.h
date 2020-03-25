#pragma once
#include "syNode.h"
#include <queue>
#include <time.h>
#include "syDebugCamera.h"
#include "syMap.h"
class syQuadTree
{
public:
	syQuadTree();
	virtual ~syQuadTree();
public:
	syNode*   m_pRootNode;
	int		 m_iMaxDepthLimit;
	float    m_fMinDivideSize;
	std::queue<syNode*>  m_QueueList;
	syDebugCamera*	m_pCamera;
	vector<syNode*>		m_LeafNodeList;
	vector<syNode*>		m_DrawNodeList;
	vector<SY_BaseObj*>  m_DrawObjList;
public:
	bool Build(float fWidth, float fHeight,
		int   iMaxDepth = 5,
		float fMinSize = 10.0f);
	virtual syNode*  CreateNode(syNode* pParentNode,
		float fMinX,
		float fMaxX,
		float fMinZ,
		float fMaxZ);
	bool  BuildTree(syNode* pNode);
	virtual bool  SubDivide(syNode* pNode);
	void  AddObject(SY_BaseObj* pModel);
	bool   CheckRect(syNode* pNode, SY_BaseObj* pObj);	//xz평면에서 pObj가 노드에 들어있는지
	syNode* FindNode(syNode* pNode, SY_BaseObj* pObj);
	bool   CheckNodeInSphere(syNode* pNode, SY_SPHERE* pObj);
	void  Update(syDebugCamera* pCamera);
	void  DrawFindNode(syNode* pNode);
	void  VisibleObject(syNode* pNode);
	void  VisibleNode(syNode* pNode);
public:
	virtual bool  Release();
	virtual bool  Render(ID3D11DeviceContext*	pContext);
	bool  Frame();
};

class syQuadTreeIndex : public syQuadTree
{
public:
	syQuadTreeIndex();
	~syQuadTreeIndex() {};
public:
	DWORD	m_dwWidth;
	DWORD   m_dwHeight;
	syMap*	m_pMap;
	int		m_iCellCnt;
	std::vector<DWORD>		m_Sharedindex;
	ComPtr<ID3D11Buffer>	m_SharedIndexBuffer;
public:
	bool Build(syMap* pMap,
		int   iMaxDepth = 5,
		float fMinSize = 10.0f);
	void CreateSharedIndex();
	syNode* CreateNode(syNode* pParentNode,
		float fMinX,
		float fMaxX,
		float fMinZ,
		float fMaxZ) override;
	bool   SubDivide(syNode* pNode)override;
	void   ComputeBoundingBox(syNode* pNode);
	void	UpdateBoundingBox(syNode* pNode);
	void   FindNeighborNode(syNode* pNode);
	void	QuadTreeToMap();
	D3DXVECTOR2 GetHeightFromNode(
		DWORD fMinX,
		DWORD fMaxX,
		DWORD fMinZ,
		DWORD fMaxZ);
	void CreateIndexBuffer(syNode* pNode);
	void CreateVertexBuffer(syNode* pNode);

	//void SetMapHeight(syNode* pSelNode);
public:
	bool  Render(ID3D11DeviceContext*	pContext)override;
	bool Release()override;
};