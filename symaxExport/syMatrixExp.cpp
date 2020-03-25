#include "stdafx.h"
#include "syMatrixExp.h"


syMatrixExp::syMatrixExp()
{
}


syMatrixExp::~syMatrixExp()
{
}

void  syMatrixExp::PreProcess(INode* pNode)
{
	if (pNode == NULL) return;
	AddObject(pNode);
	int iNumChildren = pNode->NumberOfChildren();
	for (int iChild = 0; iChild < iNumChildren; iChild++)
	{
		INode* pChild = pNode->GetChildNode(iChild);
		PreProcess(pChild);
	}
}
bool  syMatrixExp::Export()
{
	Convert();

	m_pStream = nullptr;
	// header	
	m_Scene.iNumObjects = m_tMeshList.size();
	_wfopen_s(&m_pStream, m_filename.c_str(), _T("wb"));
	_ftprintf(m_pStream, _T("%s"), _T("MatrixExp100"));
	_ftprintf(m_pStream, _T("\n%s %d %d %d %d %d %d"),
		L"#HEADERINFO",
		m_Scene.iFirstFrame,
		m_Scene.iLastFrame,
		m_Scene.iFrameSpeed,
		m_Scene.iTickPerFrame,
		m_Scene.iNumObjects,
		m_Scene.iNumMaterials);

	// mesh list
	_ftprintf(m_pStream, _T("\n%s"), L"#OBJECTINFO");
	for (int iObj = 0; iObj < m_tMeshList.size(); iObj++)
	{
		_ftprintf(m_pStream, _T("\n%s %s %d %10.4f %10.4f %10.4f %10.4f %10.4f %10.4f"),
			m_tMeshList[iObj].name,
			m_tMeshList[iObj].ParentName,
			m_tMeshList[iObj].triList.size(),
			m_tMeshList[iObj].box.pmax.x,
			m_tMeshList[iObj].box.pmax.z,
			m_tMeshList[iObj].box.pmax.y,
			m_tMeshList[iObj].box.pmin.x,
			m_tMeshList[iObj].box.pmin.z,
			m_tMeshList[iObj].box.pmin.y);

		_ftprintf(m_pStream, _T("\n\t%10.4f %10.4f %10.4f %10.4f\n\t%10.4f %10.4f %10.4f %10.4f\n\t%10.4f %10.4f %10.4f %10.4f\n\t%10.4f %10.4f %10.4f %10.4f"),
			m_tMeshList[iObj].matWorld._11,
			m_tMeshList[iObj].matWorld._12,
			m_tMeshList[iObj].matWorld._13,
			m_tMeshList[iObj].matWorld._14,

			m_tMeshList[iObj].matWorld._21,
			m_tMeshList[iObj].matWorld._22,
			m_tMeshList[iObj].matWorld._23,
			m_tMeshList[iObj].matWorld._24,

			m_tMeshList[iObj].matWorld._31,
			m_tMeshList[iObj].matWorld._32,
			m_tMeshList[iObj].matWorld._33,
			m_tMeshList[iObj].matWorld._34,

			m_tMeshList[iObj].matWorld._41,
			m_tMeshList[iObj].matWorld._42,
			m_tMeshList[iObj].matWorld._43,
			m_tMeshList[iObj].matWorld._44);

		if (m_tMeshList[iObj].triList.size() > 0)
		{
			vertexList& vList = m_tMeshList[iObj].vbList[0];	//왜 0번만? unique buffer때문인가?
			_ftprintf(m_pStream, _T("\nVertexList %d"), vList.size());
			for (int iVer = 0; iVer < vList.size(); iVer++)
			{
				_ftprintf(m_pStream, _T("\n%10.4f %10.4f %10.4f"),
					vList[iVer].p.x,
					vList[iVer].p.y,
					vList[iVer].p.z);
				_ftprintf(m_pStream, _T("%10.4f %10.4f %10.4f"),
					vList[iVer].n.x,
					vList[iVer].n.y,
					vList[iVer].n.z);
				_ftprintf(m_pStream, _T("%10.4f %10.4f %10.4f %10.4f"),
					vList[iVer].c.x,
					vList[iVer].c.y,
					vList[iVer].c.z,
					vList[iVer].c.w);
				_ftprintf(m_pStream, _T("%10.4f %10.4f"),
					vList[iVer].t.x,
					vList[iVer].t.y);
			}

			IndexList& iList =
				m_tMeshList[iObj].ibList[0];
			_ftprintf(m_pStream, _T("\nIndexList %d"), iList.size());
			for (int iIndex = 0; iIndex < iList.size(); iIndex += 3)
			{
				_ftprintf(m_pStream, _T("\n%d %d %d"),
					iList[iIndex + 0],
					iList[iIndex + 1],
					iList[iIndex + 2]);
			}
		}
		ExportAnimation(m_tMeshList[iObj]);
	}

	fclose(m_pStream);

	MessageBox(GetActiveWindow(), m_filename.c_str(),
		_T("Succeed!"), MB_OK);
	return true;
}
bool  syMatrixExp::Convert()
{
	for (int iObj = 0; iObj < m_ObjectList.size(); iObj++)
	{
		INode* pNode = m_ObjectList[iObj];
		syMesh tMesh;
		tMesh.name = FixupName(pNode->GetName());
		INode* pParentNode = pNode->GetParentNode();
		if (pParentNode &&
			// 가상의 부모가 존재
			pParentNode->IsRootNode() == false)
		{
			tMesh.ParentName =
				FixupName(pParentNode->GetName());
		}
		Matrix3 wtm = pNode->GetNodeTM(m_Interval.Start());
		DumpMatrix3(wtm, tMesh.matWorld);

		// helper || biped
		Object* pObj = pNode->GetObjectRef();
		Control* pControl = pNode->GetTMController();

		pObj->GetDeformBBox(0, tMesh.box, &pNode->GetObjectTM(0));

		tMesh.iType = CLASS_GEOM;
		if (pObj && pObj->ClassID()
			== Class_ID(BONE_CLASS_ID, 0))
		{
			tMesh.iType = CLASS_BONE;
		}
		if (pObj && pObj->ClassID()
			== Class_ID(DUMMY_CLASS_ID, 0))
		{
			tMesh.iType = CLASS_DUMMY;
		}
		if (pControl && pControl->ClassID()
			== BIPBODY_CONTROL_CLASS_ID)
		{
			tMesh.iType = CLASS_BIPED;
		}
		if (pControl && pControl->ClassID()
			== BIPSLAVE_CONTROL_CLASS_ID)
		{
			tMesh.iType = CLASS_BIPED;
		}

		if (tMesh.iType > 0)
		{
			GetMesh(pNode, tMesh);
		}
		GetAnimation(pNode, tMesh);
		m_tMeshList.push_back(tMesh);
	}
	return true;
}
void  syMatrixExp::SetUniqueBuffer(syMesh& tMesh)
{
	tMesh.vbList.resize(1);
	tMesh.ibList.resize(1);
	for (int iFace = 0; iFace <	tMesh.triList.size(); iFace++)
	{
		vectorTriList& triArray = tMesh.triList;
		TriList& tri = triArray[iFace];
		vertexList& vList = tMesh.vbList[0];
		IndexList& iList = tMesh.ibList[0];
		for (int iVer = 0; iVer < 3; iVer++)
		{
			int iPos = IsEqulVerteList(tri.v[iVer], vList);
			if (iPos < 0)
			{
				vList.push_back(tri.v[iVer]);
				iPos = vList.size() - 1;
			}
			iList.push_back(iPos);
		}
	}
}