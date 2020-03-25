#include "syQuadTree.h"



syQuadTree::syQuadTree()
{
}


syQuadTree::~syQuadTree()
{
}
bool syQuadTree::Build(float fWidth, float fHeight, int iMaxDepth, float fMinSize)
{
	m_pRootNode = CreateNode(nullptr, -fWidth / 2, fWidth / 2, -fHeight / 2, fHeight / 2);
	m_iMaxDepthLimit = iMaxDepth;
	m_fMinDivideSize = fMinSize /2;
	return BuildTree(m_pRootNode);
}

syNode*  syQuadTree::CreateNode(syNode* pParentNode, float fMinX, float fMaxX, float fMinZ,	float fMaxZ)
{
	syNode* pNode = new syNode;
	pNode->m_ChildList.reserve(4);
	pNode->m_isLeaf = FALSE;

	pNode->m_tBox.vMax.x = fMaxX;
	pNode->m_tBox.vMax.y = 0.0f;
	pNode->m_tBox.vMax.z = fMaxZ;
	pNode->m_tBox.vMin.x = fMinX;
	pNode->m_tBox.vMin.y = 0.0f;
	pNode->m_tBox.vMin.z = fMinZ;
	pNode->m_tBox.vCenter = (pNode->m_tBox.vMax +
		pNode->m_tBox.vMin) / 2;
	pNode->m_tBox.vAxis[0] = D3DXVECTOR3(1, 0, 0);
	pNode->m_tBox.vAxis[1] = D3DXVECTOR3(0, 1, 0);
	pNode->m_tBox.vAxis[2] = D3DXVECTOR3(0, 0, 1);
	pNode->m_tBox.fExtent[0] =
		(pNode->m_tBox.vMax.x - pNode->m_tBox.vMin.x) / 2;
	pNode->m_tBox.fExtent[1] =
		(pNode->m_tBox.vMax.y - pNode->m_tBox.vMin.y) / 2;
	pNode->m_tBox.fExtent[2] =
		(pNode->m_tBox.vMax.z - pNode->m_tBox.vMin.z) / 2;

	pNode->m_iDepth = 0;
	if (pParentNode != nullptr)
	{
		pNode->m_iDepth = pParentNode->m_iDepth + 1;
	}
	return pNode;
}
bool syQuadTree::BuildTree(syNode* pNode)
{
	if (SubDivide(pNode))
	{
		for (int iNode = 0; iNode < pNode->m_ChildList.size(); iNode++)
		{
			BuildTree(pNode->m_ChildList[iNode]);
		}
	}
	return true;
}
bool  syQuadTree::SubDivide(syNode* pNode)
{
	if (m_iMaxDepthLimit <= pNode->m_iDepth)
	{
		pNode->m_isLeaf = TRUE;
		m_LeafNodeList.push_back(pNode);
		return false;
	}
	float fWidthSplit = (pNode->m_tBox.vMax.x - pNode->m_tBox.vMin.x) / 2;
	float fHeightSplit = (pNode->m_tBox.vMax.z - pNode->m_tBox.vMin.z) / 2;
	if (m_fMinDivideSize >= fWidthSplit || m_fMinDivideSize >= fHeightSplit)
	{
		pNode->m_isLeaf = TRUE;
		m_LeafNodeList.push_back(pNode);
		return false;
	}

	syNode* pChildNode = CreateNode(
		pNode,
		pNode->m_tBox.vMin.x,
		pNode->m_tBox.vMin.x + fWidthSplit,
		pNode->m_tBox.vMin.z,
		pNode->m_tBox.vMax.z - fHeightSplit);
	pNode->m_ChildList.push_back(pChildNode);
	pChildNode->m_ParentList.push_back(pNode);
	for (int iPar = 0; iPar < pNode->m_ParentList.size(); iPar++)
	{
		pChildNode->m_ParentList.push_back(pNode->m_ParentList[iPar]);
	}

	pChildNode = CreateNode(
		pNode,
		pNode->m_tBox.vMin.x + fWidthSplit,
		pNode->m_tBox.vMax.x,
		pNode->m_tBox.vMax.z - fHeightSplit,
		pNode->m_tBox.vMax.z);
	pNode->m_ChildList.push_back(pChildNode);
	pChildNode->m_ParentList.push_back(pNode);
	for (int iPar = 0; iPar < pNode->m_ParentList.size(); iPar++)
	{
		pChildNode->m_ParentList.push_back(pNode->m_ParentList[iPar]);
	}

	pChildNode = CreateNode(
		pNode,
		pNode->m_tBox.vMin.x + fWidthSplit,
		pNode->m_tBox.vMax.x,
		pNode->m_tBox.vMin.z,
		pNode->m_tBox.vMax.z - fHeightSplit);
	pNode->m_ChildList.push_back(pChildNode);
	pChildNode->m_ParentList.push_back(pNode);
	for (int iPar = 0; iPar < pNode->m_ParentList.size(); iPar++)
	{
		pChildNode->m_ParentList.push_back(pNode->m_ParentList[iPar]);
	}

	pChildNode = CreateNode(
		pNode,
		pNode->m_tBox.vMin.x,
		pNode->m_tBox.vMin.x + fWidthSplit,
		pNode->m_tBox.vMax.z - fHeightSplit,
		pNode->m_tBox.vMax.z);
	pNode->m_ChildList.push_back(pChildNode);
	pChildNode->m_ParentList.push_back(pNode);
	for (int iPar = 0; iPar < pNode->m_ParentList.size(); iPar++)
	{
		pChildNode->m_ParentList.push_back(pNode->m_ParentList[iPar]);
	}
	return true;
}
void syQuadTree::AddObject(SY_BaseObj* pObj)
{
	if (CheckRect(m_pRootNode, pObj))
	{
		syNode* pNode = FindNode(m_pRootNode, pObj);
		if (pNode)
		{
			pNode->m_ObjectList.push_back(pObj);
		}
	}
}
bool syQuadTree::CheckRect(syNode* pNode, SY_BaseObj* pObj)
{
	if (pNode->m_tBox.vMin.x <= pObj->box.vMin.x &&
		pNode->m_tBox.vMax.x >= pObj->box.vMax.x)
	{
		if (pNode->m_tBox.vMin.z <= pObj->box.vMin.z &&
			pNode->m_tBox.vMax.z >= pObj->box.vMax.z)
		{
			return true;
		}
	}
	return false;
}
bool syQuadTree::CheckNodeInSphere(syNode* pNode, SY_SPHERE* pObj)
{

	float delLen = (pObj->vCenter.x - pNode->m_tBox.vCenter.x)*(pObj->vCenter.x - pNode->m_tBox.vCenter.x)
		+ (pObj->vCenter.z - pNode->m_tBox.vCenter.z)*(pObj->vCenter.z - pNode->m_tBox.vCenter.z);
	delLen = sqrt(delLen);
	float delrad = pNode->m_tBox.fExtent[0] * pNode->m_tBox.fExtent[0] + pNode->m_tBox.fExtent[2] * pNode->m_tBox.fExtent[2];
	delrad = sqrt(delrad);
	if (delLen <= pObj->fRadius + delrad)
	{
		return true;
	}
	return false;
	
}
syNode* syQuadTree::FindNode(syNode* pNode, SY_BaseObj* pObj)
{
	do {
		for (int iNode = 0;
			iNode < pNode->m_ChildList.size(); iNode++)
		{
			if (CheckRect(pNode->m_ChildList[iNode], pObj))
			{
				m_QueueList.push(pNode->m_ChildList[iNode]);
			}
		}
		if (m_QueueList.empty()) break;

		pNode = m_QueueList.front();
		m_QueueList.pop();

	} while (pNode);
	return pNode;
}

void  syQuadTree::Update(syDebugCamera* pCamera)
{
	m_pCamera = pCamera;
}
void  syQuadTree::DrawFindNode(syNode* pNode)
{
	SY_POSITION t_Pos =
		m_pCamera->CheckPositionOBB(pNode->m_tBox);

	if (t_Pos == P_BACK) return;

	if (pNode->m_isLeaf && t_Pos != P_BACK)
	{
		m_DrawNodeList.push_back(pNode);
		return;
	}
	if (t_Pos == P_FRONT)
	{
		VisibleNode(pNode);
		//m_DrawNodeList.push_back(pNode);
		return;
	}
	if (t_Pos == P_SPANNING)
	{
		VisibleObject(pNode);
	}
	for (int iNode = 0; iNode < pNode->m_ChildList.size(); iNode++)
	{
		DrawFindNode(pNode->m_ChildList[iNode]);
	}
}
void  syQuadTree::VisibleObject(syNode* pNode)
{
	for (int iObj = 0; iObj < pNode->m_ObjectList.size(); iObj++)
	{
		if (m_pCamera->CheckOBB(pNode->m_tBox))
		{
			m_DrawObjList.push_back(pNode->m_ObjectList[iObj]);
		}
	}
}
void  syQuadTree::VisibleNode(syNode* pNode)
{
	if (pNode == nullptr) return;
	if (pNode->m_isLeaf)
	{
		m_DrawNodeList.push_back(pNode);
	}
	for (int iNode = 0; iNode < pNode->m_ChildList.size();
		iNode++)
	{
		VisibleNode(pNode->m_ChildList[iNode]);

	}
}
bool  syQuadTree::Release()
{
	delete m_pRootNode;
	for (int i = 0; i<m_QueueList.size(); i++)
	{
		delete m_QueueList.front();
		m_QueueList.pop();
	}


	////if (m_pCamera) m_pCamera->Release();

	//for (auto &a : m_LeafNodeList)
	//{
	//	if(a) delete a;
	//}
	m_LeafNodeList.clear();
	//
	//for (auto &a : m_DrawNodeList)
	//{
	//	delete a;
	//}
	m_DrawNodeList.clear();

	//for (auto &a : m_DrawObjList)
	//{
	//	delete a;
	//}
	m_DrawObjList.clear();
	return true;
}
bool syQuadTree::Render(ID3D11DeviceContext*	pContext)
{
	return true;
}
bool  syQuadTree::Frame()
{
	m_DrawObjList.clear();
	m_DrawNodeList.clear();

	DrawFindNode(m_pRootNode);
	return true;
}



bool syQuadTreeIndex::Build(syMap* pMap, int   iMaxDepth,float fMinSize)
{                                     
	m_iCellCnt = pMap->m_iCellNum;
	m_dwWidth = (pMap->m_iNumCellCols)/*/pMap->m_fCellDistance*/;
	m_dwHeight = (pMap->m_iNumCellRows) /*/ pMap->m_fCellDistance*/;
	
	m_pMap = pMap;
	CreateSharedIndex();
	m_pRootNode = CreateNode(nullptr,
		0, m_dwWidth - 1,
		m_dwWidth * (m_dwHeight - 1),
		m_dwWidth * m_dwHeight - 1);

	m_iMaxDepthLimit = iMaxDepth;
	m_fMinDivideSize = fMinSize/2;
	BuildTree(m_pRootNode);
	for (auto &a : m_LeafNodeList)
	{
		FindNeighborNode(a);
	}

	return true;
}

syNode*  syQuadTreeIndex::CreateNode(syNode* pParentNode, float fMinX, float fMaxX, float fMinZ, float fMaxZ)
{
	syNode* pNode = new syNode;
	pNode->m_ChildList.reserve(4);
	pNode->m_isLeaf = FALSE;
	pNode->m_CornerIndex.push_back((DWORD)fMinX);
	pNode->m_CornerIndex.push_back((DWORD)fMaxX);
	pNode->m_CornerIndex.push_back((DWORD)fMinZ);
	pNode->m_CornerIndex.push_back((DWORD)fMaxZ);


	ComputeBoundingBox(pNode);
	pNode->m_iDepth = 0;
	if (pParentNode != nullptr)
	{
		pNode->m_iDepth = pParentNode->m_iDepth + 1;
	}
	return pNode;
}
bool  syQuadTreeIndex::SubDivide(syNode* pNode)
{
	if (m_iMaxDepthLimit <= pNode->m_iDepth)
	{
		pNode->m_isLeaf = TRUE;
		m_LeafNodeList.push_back(pNode);
		CreateVertexBuffer(pNode);
		CreateIndexBuffer(pNode);
		return false;
	}
	float fWidthSplit = (pNode->m_tBox.vMax.x - pNode->m_tBox.vMin.x) / 2;
	float fHeightSplit = (pNode->m_tBox.vMax.z - pNode->m_tBox.vMin.z) / 2;
	if (m_fMinDivideSize >= fWidthSplit || m_fMinDivideSize >= fHeightSplit)
	{
		pNode->m_isLeaf = TRUE;
		m_LeafNodeList.push_back(pNode);
		CreateVertexBuffer(pNode);
		CreateIndexBuffer(pNode);
		return false;
	}
	DWORD dwEdgeCenter[4];
	DWORD dwCenter =
		(pNode->m_CornerIndex[0] + pNode->m_CornerIndex[3]) / 2;


	dwEdgeCenter[0] =
		(pNode->m_CornerIndex[0] + pNode->m_CornerIndex[1]) / 2;
	dwEdgeCenter[1] =
		(pNode->m_CornerIndex[1] + pNode->m_CornerIndex[3]) / 2;
	dwEdgeCenter[2] =
		(pNode->m_CornerIndex[2] + pNode->m_CornerIndex[3]) / 2;
	dwEdgeCenter[3] =
		(pNode->m_CornerIndex[0] + pNode->m_CornerIndex[2]) / 2;

	syNode* pChildNode = CreateNode(
		pNode,
		pNode->m_CornerIndex[0],
		dwEdgeCenter[0],
		dwEdgeCenter[3],
		dwCenter);
	pNode->m_ChildList.push_back(pChildNode);
	pChildNode->m_ParentList.push_back(pNode);
	for (int iPar = 0; iPar < pNode->m_ParentList.size(); iPar++)
	{
		pChildNode->m_ParentList.push_back(pNode->m_ParentList[iPar]);
	}

	pChildNode = CreateNode(
		pNode,
		dwEdgeCenter[0],
		pNode->m_CornerIndex[1],
		dwCenter,
		dwEdgeCenter[1]);
	pNode->m_ChildList.push_back(pChildNode);
	pChildNode->m_ParentList.push_back(pNode);
	for (int iPar = 0; iPar < pNode->m_ParentList.size(); iPar++)
	{
		pChildNode->m_ParentList.push_back(pNode->m_ParentList[iPar]);
	}

	pChildNode = CreateNode(
		pNode,
		dwCenter,
		dwEdgeCenter[1],
		dwEdgeCenter[2],
		pNode->m_CornerIndex[3]);
	pNode->m_ChildList.push_back(pChildNode);
	pChildNode->m_ParentList.push_back(pNode);
	for (int iPar = 0; iPar < pNode->m_ParentList.size(); iPar++)
	{
		pChildNode->m_ParentList.push_back(pNode->m_ParentList[iPar]);
	}

	pChildNode = CreateNode(
		pNode,
		dwEdgeCenter[3],
		dwCenter,
		pNode->m_CornerIndex[2],
		dwEdgeCenter[2]);
	pNode->m_ChildList.push_back(pChildNode);
	pChildNode->m_ParentList.push_back(pNode);
	for (int iPar = 0; iPar < pNode->m_ParentList.size(); iPar++)
	{
		pChildNode->m_ParentList.push_back(pNode->m_ParentList[iPar]);
	}
	return true;
}
void	syQuadTreeIndex::UpdateBoundingBox(syNode* pNode)
{
	
	float fMin = 99999.0f;
	float fMax = -99999.0f;

	
	for (int ver = 0; ver < pNode->m_VertexSize; ver++)
	{

		if (pNode->m_VertexData[ver].p.y < fMin)
		{
			fMin = pNode->m_VertexData[ver].p.y;
		}
		if (pNode->m_VertexData[ver].p.y > fMax)
		{
			fMax = pNode->m_VertexData[ver].p.y;
		}
	}
	D3DXVECTOR2 vHeight(fMin, fMax);

	D3DXVECTOR3 v0 = pNode->m_VertexData[0].p;
	D3DXVECTOR3 v1 = pNode->m_VertexData[pNode->m_VertexSize -1].p;

	pNode->m_tBox.vMax.x = v1.x;
	pNode->m_tBox.vMax.y = vHeight.y;
	pNode->m_tBox.vMax.z = v0.z;
	pNode->m_tBox.vMin.x = v0.x;
	pNode->m_tBox.vMin.y = vHeight.x;
	pNode->m_tBox.vMin.z = v1.z;
	pNode->m_tBox.vCenter = (pNode->m_tBox.vMax +
		pNode->m_tBox.vMin) / 2;
	pNode->m_tBox.vAxis[0] = D3DXVECTOR3(1, 0, 0);
	pNode->m_tBox.vAxis[1] = D3DXVECTOR3(0, 1, 0);
	pNode->m_tBox.vAxis[2] = D3DXVECTOR3(0, 0, 1);
	pNode->m_tBox.fExtent[0] =
		(pNode->m_tBox.vMax.x - pNode->m_tBox.vMin.x) / 2;
	pNode->m_tBox.fExtent[1] =
		(pNode->m_tBox.vMax.y - pNode->m_tBox.vMin.y) / 2;
	pNode->m_tBox.fExtent[2] =
		(pNode->m_tBox.vMax.z - pNode->m_tBox.vMin.z) / 2;

	for (int iPar = 0; iPar < pNode->m_ParentList.size(); iPar++)
	{
		int chek = 0;
		if (pNode->m_ParentList[iPar]->m_tBox.vMax.y < pNode->m_tBox.vMax.y)
		{
			pNode->m_ParentList[iPar]->m_tBox.vMax.y = pNode->m_tBox.vMax.y;
			chek++;
		}
		if (pNode->m_ParentList[iPar]->m_tBox.vMin.y > pNode->m_tBox.vMin.y)
		{
			pNode->m_ParentList[iPar]->m_tBox.vMin.y = pNode->m_tBox.vMin.y;
			chek++;
		}
		if (chek == 0) return;
		pNode->m_ParentList[iPar]->m_tBox.vCenter = (pNode->m_ParentList[iPar]->m_tBox.vMax +pNode->m_ParentList[iPar]->m_tBox.vMin) / 2;
		pNode->m_ParentList[iPar]->m_tBox.fExtent[1] =
			(pNode->m_ParentList[iPar]->m_tBox.vMax.y - pNode->m_ParentList[iPar]->m_tBox.vMin.y) / 2;
	}
}
void  syQuadTreeIndex::ComputeBoundingBox(syNode* pNode)
{
	D3DXVECTOR2 vHeight =
		GetHeightFromNode(
			pNode->m_CornerIndex[0],
			pNode->m_CornerIndex[1],
			pNode->m_CornerIndex[2],
			pNode->m_CornerIndex[3]);

	D3DXVECTOR3 v0 = m_pMap->m_VertexList[pNode->m_CornerIndex[0]].p;
	D3DXVECTOR3 v1 = m_pMap->m_VertexList[pNode->m_CornerIndex[3]].p;

	pNode->m_tBox.vMax.x = v1.x;
	pNode->m_tBox.vMax.y = vHeight.y;
	pNode->m_tBox.vMax.z = v0.z;
	pNode->m_tBox.vMin.x = v0.x;
	pNode->m_tBox.vMin.y = vHeight.x;
	pNode->m_tBox.vMin.z = v1.z;
	pNode->m_tBox.vCenter = (pNode->m_tBox.vMax +
		pNode->m_tBox.vMin) / 2;
	pNode->m_tBox.vAxis[0] = D3DXVECTOR3(1, 0, 0);
	pNode->m_tBox.vAxis[1] = D3DXVECTOR3(0, 1, 0);
	pNode->m_tBox.vAxis[2] = D3DXVECTOR3(0, 0, 1);
	pNode->m_tBox.fExtent[0] =
		(pNode->m_tBox.vMax.x - pNode->m_tBox.vMin.x) / 2;
	pNode->m_tBox.fExtent[1] =
		(pNode->m_tBox.vMax.y - pNode->m_tBox.vMin.y) / 2;
	pNode->m_tBox.fExtent[2] =
		(pNode->m_tBox.vMax.z - pNode->m_tBox.vMin.z) / 2;
}
bool syQuadTreeIndex::Release()
{
	if (m_pMap) m_pMap->Release();
	return syQuadTree::Release();
}
D3DXVECTOR2 syQuadTreeIndex::GetHeightFromNode(
	DWORD fMinX,
	DWORD fMaxX,
	DWORD fMinZ,
	DWORD fMaxZ)
{
	float fMin = 99999.0f;
	float fMax = -99999.0f;

	DWORD dwStartRow = fMinX / m_dwWidth;
	DWORD dwEndRow = fMinZ / m_dwWidth;
	DWORD dwStartCol = fMinX % m_dwWidth;
	DWORD dwEndCol = fMaxX % m_dwWidth;
	for (DWORD dwRow = dwStartRow;
		dwRow <= dwEndRow;
		dwRow++)
	{
		for (DWORD dwCol = dwStartCol;
			dwCol <= dwEndCol;
			dwCol++)
		{
			DWORD dwIndex = dwRow * m_dwWidth + dwCol;
			if (m_pMap->m_VertexList[dwIndex].p.y < fMin)
			{
				fMin = m_pMap->m_VertexList[dwIndex].p.y;
			}
			if (m_pMap->m_VertexList[dwIndex].p.y > fMax)
			{
				fMax = m_pMap->m_VertexList[dwIndex].p.y;
			}
		}
	}
	return D3DXVECTOR2(fMin, fMax);
}
syQuadTreeIndex::syQuadTreeIndex() 
{
	//m_Sharedindex.resize(30); 
}
void syQuadTreeIndex::CreateSharedIndex()
{
	//°øÀ¯ÀÎµ¦½º

	DWORD dwStartRow = 0;
	DWORD dwEndRow = m_iCellCnt;
	DWORD dwStartCol = 0;
	DWORD dwEndCol = m_iCellCnt;
	DWORD delRow = dwEndRow - dwStartRow + 1;
	DWORD delCol = dwEndCol - dwStartCol + 1;
	int num = (m_iCellCnt)*(m_iCellCnt) * 2 * 3;
	
//	m_Sharedindex.push_back(0);

	m_Sharedindex.resize(num);

	int iIndex = 0;
	for (DWORD iRow = dwStartRow;	iRow < dwEndRow;
		iRow++)
	{
		for (DWORD iCol = dwStartCol;	iCol < dwEndCol;
			iCol++)
		{
			DWORD dwIndex = iRow * delCol + iCol;
			int iNextRow = iRow + 1;
			int iNextCol = iCol + 1;
			m_Sharedindex[iIndex + 0] = iRow * delCol + iCol;
			m_Sharedindex[iIndex + 1] = iRow * delCol + iNextCol;
			m_Sharedindex[iIndex + 2] = iNextRow * delCol + iCol;

			m_Sharedindex[iIndex + 3] = m_Sharedindex[iIndex + 2];
			m_Sharedindex[iIndex + 4] = m_Sharedindex[iIndex + 1];
			m_Sharedindex[iIndex + 5] = iNextRow * delCol + iNextCol;
			iIndex += 6;
		}
	}

	m_SharedIndexBuffer.Attach(
		DX::CreateIndexBuffer(g_pDevice,	//(µð¹ÙÀÌ½º ¸ô¶ó)
			&m_Sharedindex.at(0),
			m_Sharedindex.size(), sizeof(DWORD)));
}
void syQuadTreeIndex::CreateIndexBuffer(syNode* pNode)
{
	DWORD dwStartRow = pNode->m_CornerIndex[0] / m_dwWidth;
	DWORD dwEndRow = pNode->m_CornerIndex[2] / m_dwWidth;
	DWORD dwStartCol = pNode->m_CornerIndex[0] % m_dwWidth;
	DWORD dwEndCol = pNode->m_CornerIndex[1] % m_dwWidth;

	pNode->m_IndexData.resize(
		(dwEndRow - dwStartRow)*(dwEndCol - dwStartCol) * 2 * 3);

	int iIndex = 0;
	for (DWORD iRow = dwStartRow;	iRow < dwEndRow;
		iRow++)
	{
		for (DWORD iCol = dwStartCol;	iCol < dwEndCol;
			iCol++)
		{
			DWORD dwIndex = iRow * m_dwWidth + iCol;
			int iNextRow = iRow + 1;
			int iNextCol = iCol + 1;
			pNode->m_IndexData[iIndex + 0] = iRow * m_dwWidth + iCol;
			pNode->m_IndexData[iIndex + 1] = iRow * m_dwWidth + iNextCol;
			pNode->m_IndexData[iIndex + 2] = iNextRow * m_dwWidth + iCol;

			pNode->m_IndexData[iIndex + 3] = pNode->m_IndexData[iIndex + 2];
			pNode->m_IndexData[iIndex + 4] = pNode->m_IndexData[iIndex + 1];
			pNode->m_IndexData[iIndex + 5] = iNextRow * m_dwWidth + iNextCol;
			iIndex += 6;
		}
	}

	pNode->m_IndexBuffer.Attach(
		DX::CreateIndexBuffer(m_pMap->m_pDevice,
			&pNode->m_IndexData.at(0),
			pNode->m_IndexData.size(), sizeof(DWORD)));
}

bool syQuadTreeIndex::Render(ID3D11DeviceContext*	pContext)
{
	float fTime = g_fGameTimer;

	m_pMap->m_cb.color[0] = m_pMap->m_iNumCols;
	pContext->UpdateSubresource(
		m_pMap->m_dxObj.m_pConstantBuffer.Get(),
		0, NULL, &m_pMap->m_cb, 0, 0);
	m_pMap->PreRender(pContext, m_pMap->m_dxObj.m_iVertexSize);
	for (int texslot = 0; texslot < 3; texslot++)
	{
		if (m_pMap->m_backgroundTex[texslot])
		{
			pContext->PSSetShaderResources(texslot+2, 1, m_pMap->m_backgroundTex[texslot]->m_pSRV.GetAddressOf());
		}
	}

	UINT stride =  sizeof(PNCT_VERTEX);
	UINT offset = 0;
	pContext->IASetIndexBuffer(m_SharedIndexBuffer.Get(),
		DXGI_FORMAT_R32_UINT, 0);
	for (int iNode = 0; iNode < m_DrawNodeList.size(); iNode++)
	{
		

		syNode* pNode = m_DrawNodeList[iNode];
		if (pNode->m_DrawTex)
		{
			pContext->PSSetShaderResources(0, 1, pNode->m_DrawTex->m_pSRV.GetAddressOf());
		}
		else
		{
			pContext->PSSetShaderResources(0, 1, m_pMap->m_pMapTex->m_pSRV.GetAddressOf());
		}
		
	
		pContext->IASetVertexBuffers(0, 1, pNode->m_VertexBuffer.GetAddressOf(), &stride, &offset);		
		pContext->DrawIndexed(m_Sharedindex.size(), 0, 0);

	}
	return true;
}
void	syQuadTreeIndex::QuadTreeToMap()
{
	//QuadTree To Map
	for (syNode* pNode : m_LeafNodeList)
	{
		DWORD dwTL = pNode->m_CornerIndex[0];
		DWORD dwTR = pNode->m_CornerIndex[1];
		DWORD dwBL = pNode->m_CornerIndex[2];
		DWORD dwBR = pNode->m_CornerIndex[3];


		DWORD dwSize = ((dwTR - dwTL) + 1)*((dwTR - dwTL) + 1);


		DWORD dwStartRow = dwTL / m_dwWidth;
		DWORD dwEndRow = dwBL / m_dwWidth;

		DWORD dwStartCol = dwTL % m_dwWidth;
		DWORD dwEndCol = dwTR % m_dwWidth;

		pNode->m_VertexData.resize(dwSize);
		pNode->m_VertexSize = dwSize;
		DWORD iVertexIndex = 0;
		for (DWORD dwRow = dwStartRow; dwRow <= dwEndRow; dwRow++)
		{
			for (DWORD dwCol = dwStartCol; dwCol <= dwEndCol; dwCol++)
			{
				m_pMap->m_VertexList[dwRow*m_dwWidth + dwCol] = pNode->m_VertexData[iVertexIndex++];
				//pNode->m_pTangentVB = m_pMap->m_pTangentVB;

			}
		}
	}
	m_pMap->m_dxObj.m_pVertexBuffer.Attach(
		DX::CreateVertexBuffer(m_pMap->m_pDevice, &m_pMap->m_VertexList.at(0),
			m_pMap->m_dxObj.m_iNumVertex, m_pMap->m_dxObj.m_iVertexSize));
	
}
void syQuadTreeIndex::CreateVertexBuffer(syNode* pNode)
{
	DWORD dwTL = pNode->m_CornerIndex[0];
	DWORD dwTR = pNode->m_CornerIndex[1];
	DWORD dwBL = pNode->m_CornerIndex[2];
	DWORD dwBR = pNode->m_CornerIndex[3];


	DWORD dwSize = ((dwTR - dwTL) + 1)*((dwTR - dwTL) + 1);


	DWORD dwStartRow = dwTL / m_dwWidth;
	DWORD dwEndRow = dwBL / m_dwWidth;

	DWORD dwStartCol = dwTL % m_dwWidth;
	DWORD dwEndCol = dwTR % m_dwWidth;

	pNode->m_VertexData.resize(dwSize);
	pNode->m_VertexSize = dwSize;
	DWORD iVertexIndex = 0;
	for (DWORD dwRow = dwStartRow; dwRow <= dwEndRow; dwRow++)
	{
		for (DWORD dwCol = dwStartCol; dwCol <= dwEndCol; dwCol++)
		{
			pNode->m_VertexData[iVertexIndex++] = m_pMap->m_VertexList[dwRow*m_dwWidth + dwCol];
			//pNode->m_pTangentVB = m_pMap->m_pTangentVB;

		}
	}


	pNode->m_VertexBuffer.Attach(
		DX::CreateVertexBuffer(m_pMap->m_pDevice,
			&pNode->m_VertexData.at(0), pNode->m_VertexSize, sizeof(PNCT_VERTEX))
	);

	//pNode->SetMapTex(m_pMap->m_iTexIndex);
}
void   syQuadTreeIndex::FindNeighborNode(syNode* pNode)
{

	//pNode->m_NeighborList.push_back(pNode);

	for (auto &a : m_LeafNodeList)
	{
		//¿ÞÂÊ
		if (pNode->m_CornerIndex[0] == a->m_CornerIndex[1])
		{
			pNode->m_NeighborList.push_back(a);
			continue;
		}
		//¿À¸¥ÂÊ
		if (pNode->m_CornerIndex[1] == a->m_CornerIndex[0])
		{
			pNode->m_NeighborList.push_back(a);
			continue;
		}
		//»ó
		if (pNode->m_CornerIndex[0] == a->m_CornerIndex[2])
		{
			pNode->m_NeighborList.push_back(a);
			continue;
		}
		//ÇÏ
		if (pNode->m_CornerIndex[2] == a->m_CornerIndex[0])
		{
			pNode->m_NeighborList.push_back(a);
			continue;
		}
		//ºÏ¼­
		if (pNode->m_CornerIndex[0] == a->m_CornerIndex[3])
		{
			pNode->m_NeighborList.push_back(a);
			continue;
		}
		//ºÏµ¿
		if (pNode->m_CornerIndex[1] == a->m_CornerIndex[2])
		{
			pNode->m_NeighborList.push_back(a);
			continue;
		}
		//³²¼­
		if (pNode->m_CornerIndex[2] == a->m_CornerIndex[1])
		{
			pNode->m_NeighborList.push_back(a);
			continue;
		}
		//³²µ¿
		if (pNode->m_CornerIndex[3] == a->m_CornerIndex[0])
		{
			pNode->m_NeighborList.push_back(a);
			continue;
		}
	}
	//pNode.
}
//void syQuadTreeIndex::SetMapHeight(syNode* pSelNode)
//{
//	if (!pSelNode) return;
//	for (int iNode = 0; iNode <
//		pSelNode->m_NeighborList.size(); iNode++)
//	{
//		syNode* pNode = pSelNode->m_NeighborList[iNode];
//		for (int iTri = 0;
//			iTri < pNode->m_VertexSize;
//			iTri++)
//		{
//
//		//	float delLen = D3DXVec3Length(&(m_Select.m_vIntersection - pNode->m_VertexData[iTri].p));
//			//pNode->m_VertexData[iTri].p.y += max(0, rad - delLen);
//		}
//		UpdateBoundingBox(pNode);
//		pNode->m_VertexBuffer.Attach(
//			DX::CreateVertexBuffer(g_pDevice,
//				&pNode->m_VertexData.at(0), pNode->m_VertexSize, sizeof(PNCT_VERTEX)));
//
//	}
//}