#include "syNode.h"
#include "syTextureMgr.h"


syNode::syNode()
{
}


syNode::~syNode()
{
	for (int iNode = 0; iNode < m_ChildList.size(); iNode++)
	{
		delete m_ChildList[iNode];
	}
}
void syNode::SetMapTex(int texID)
{
	if (texID < 0)  return;
	
	m_DrawTex = I_TextureMgr.GetPtr(texID);
}

//void syNode::SetbackgroundTex(int texID)
//{
//	if (texID < 0)  return;
//
//	m_backgroundTex = I_TextureMgr.GetPtr(texID)->m_pSRV;
//}