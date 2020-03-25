#include "stdafx.h"
#include "syWriter.h"
struct AscendingSort
{
	bool operator()(TriList& rpStart, TriList& rpEnd)
	{
		return rpStart.iSubIndex < rpEnd.iSubIndex;
	}
};
static int g_iSearchIndex = 0;
struct IsSameInt // find_to�� ������ �Ǵ��� �ִ� �Լ���   
{
	bool operator()(TriList &value)
	{
		return value.iSubIndex == g_iSearchIndex;
	}
};
/*
max mŰ  : �ؽ��� ���� �� �־�
material bank  - diffuse color - bitmap (�ȼ����̴��� �����ͷ� �۾��ϴ� ��)

������ ��ܿ� render ��¼�� ������ �ϸ� �������� ��� ��� ���� FrameWindow

������ ��� ����
*/
syWriter::syWriter()
{
}

TCHAR* syWriter::SaveFileDlg(TCHAR* szExt, TCHAR* szTitle)
{
	TCHAR szFile[MAX_PATH] = { 0, };
	TCHAR szFileTitleFile[MAX_PATH] = { 0, };
	static TCHAR *szFilter;
	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	_tcscpy_s(szFile, _T("*."));
	_tcscat_s(szFile, szExt);
	_tcscat_s(szFile, _T("\0"));

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = GetActiveWindow();
	ofn.lpstrFilter = szFilter;
	ofn.lpstrCustomFilter = NULL;
	ofn.nMaxCustFilter = 0L;
	ofn.nFilterIndex = 1;
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFileTitle = szFileTitleFile;
	ofn.nMaxFileTitle = sizeof(szFileTitleFile);
	ofn.lpstrInitialDir = NULL;
	ofn.lpstrTitle = szTitle;
	ofn.Flags = 0L;
	ofn.nFileOffset = 0;
	ofn.nFileExtension = 0;
	ofn.lpstrDefExt = szExt;
	if (!GetSaveFileName(&ofn))
	{
		return NULL;
	}
	return szFile;
}

bool	syWriter::EqualPoint2(Point2 p1, Point2 p2)
{
	if (fabs(p1.x - p2.x) > ALMOST_ZERO)
		return false;
	if (fabs(p1.y - p2.y) > ALMOST_ZERO)
		return false;
	return true;
}
bool	syWriter::EqualPoint3(Point3 p1, Point3 p2)
{
	if (fabs(p1.x - p2.x) > ALMOST_ZERO)
		return false;
	if (fabs(p1.y - p2.y) > ALMOST_ZERO)
		return false;
	if (fabs(p1.z - p2.z) > ALMOST_ZERO)
		return false;

	return true;
}
bool	syWriter::EqualPoint4(Point4 p1, Point4 p2)
{
	if (fabs(p1.x - p2.x) > ALMOST_ZERO)
		return false;
	if (fabs(p1.y - p2.y) > ALMOST_ZERO)
		return false;
	if (fabs(p1.z - p2.z) > ALMOST_ZERO)
		return false;
	if (fabs(p1.w - p2.w) > ALMOST_ZERO)
		return false;
	return true;
}

TCHAR*  syWriter::FixupName(MSTR name)
{
	TCHAR m_tmpBuffer[MAX_PATH] = { 0, };
	memset(m_tmpBuffer, 0, sizeof(TCHAR)*MAX_PATH);

	TCHAR* cPtr;
	_tcscpy_s(m_tmpBuffer, name);
	cPtr = m_tmpBuffer;

	while (*cPtr)
	{
		if (*cPtr == '"')		*cPtr = SINGLE_QUOTE;
		else if (*cPtr == ' ' || *cPtr <= CTL_CHARS)
			*cPtr = _T('_');
		cPtr++;
	}
	return m_tmpBuffer;
}

bool  syWriter::Convert()
{
	for (int iObj = 0; iObj < m_ObjectList.size(); iObj++)
	{
		INode* pNode = m_ObjectList[iObj];
		syMesh tMesh;
		tMesh.name = FixupName(pNode->GetName());
		INode* pParentNode = pNode->GetParentNode();
		if (pParentNode &&
			// ������ �θ� ����
			pParentNode->IsRootNode() == false)
		{
			tMesh.ParentName =
				FixupName(pParentNode->GetName());
		}
		// objTM = s*r*t*p * c;
		// GetNodeTM = srt * p;
		Matrix3 wtm = pNode->GetNodeTM(m_Interval.Start());	
		DumpMatrix3(wtm, tMesh.matWorld);
		Matrix3 invWtm = Inverse(wtm);
		DumpMatrix3(invWtm, tMesh.matInvWorld);

		tMesh.iMtrlID = FindMaterial(pNode);
		if (tMesh.iMtrlID >= 0 && m_MtrlList[tMesh.iMtrlID].subMtrl.size() > 0)
		{
			tMesh.iSubMesh = m_MtrlList[tMesh.iMtrlID].subMtrl.size();
		}
		GetMesh(pNode, tMesh);

		GetAnimation(pNode, tMesh);
		m_tMeshList.push_back(tMesh);
	}
	return true;
}
bool  syWriter::Export()
{
	Convert();
	m_pStream = nullptr;
	//header
	m_Scene.iNumMaterials = m_MtrlList.size();
	m_Scene.iNumObjects = m_tMeshList.size();
	_wfopen_s(&m_pStream, m_filename.c_str(), _T("wb"));
	_ftprintf(m_pStream, _T("%s"), _T("symaxexporter100"));	//syexporter100 : ���� ����
	_ftprintf(m_pStream, _T("\n%s "), L"#HEADER_INFO");
	_ftprintf(m_pStream, _T("%d %d %d %d %d %d"),
		m_Scene.iFirstFrame,
		m_Scene.iLastFrame,
		m_Scene.iFrameSpeed,
		m_Scene.iTickPerFrame,
		m_Scene.iNumObjects,
		m_Scene.iNumMaterials);

	// root material
	_ftprintf(m_pStream, _T("\n%s"), L"#MATERIAL_INFO");

	for (int iMtl = 0; iMtl < m_MtrlList.size(); iMtl++)
	{
		_ftprintf(m_pStream, _T("\n%s %d"),
			m_MtrlList[iMtl].szName,
			m_MtrlList[iMtl].subMtrl.size());

		if (m_MtrlList[iMtl].subMtrl.size() > 0)
		{
			for (int iSubMtrl = 0; iSubMtrl < m_MtrlList[iMtl].subMtrl.size(); iSubMtrl++)
			{
				_ftprintf(m_pStream, _T("\n%s %d"),
					m_MtrlList[iMtl].subMtrl[iSubMtrl].szName,
					m_MtrlList[iMtl].subMtrl[iSubMtrl].texList.size()
				);

				for (int iTex = 0; iTex < m_MtrlList[iMtl].subMtrl[iSubMtrl].texList.size(); iTex++)
				{
					_ftprintf(m_pStream, _T("\n%d %s"),
						m_MtrlList[iMtl].subMtrl[iSubMtrl].texList[iTex].iMapID,
						m_MtrlList[iMtl].subMtrl[iSubMtrl].texList[iTex].szName);
				}
			}
		}
		else
		{
			_ftprintf(m_pStream, _T("\n%s %d"),
				L"none",
				m_MtrlList[iMtl].texList.size()
			);

			for (int iTex = 0; iTex < m_MtrlList[iMtl].texList.size(); iTex++)
			{
				_ftprintf(m_pStream, _T("\n%d %s"),
					m_MtrlList[iMtl].texList[iTex].iMapID,
					m_MtrlList[iMtl].texList[iTex].szName);
			}
		}
	}


	// mesh list
	_ftprintf(m_pStream, _T("\n%s"), L"#OBJECT_INFO");
	for (int iObj = 0; iObj < m_tMeshList.size(); iObj++)
	{
		_ftprintf(m_pStream, _T("\n%s %s %d %d %d"),
			m_tMeshList[iObj].name,
			m_tMeshList[iObj].ParentName,
			m_tMeshList[iObj].iMtrlID,
			m_tMeshList[iObj].bufferList.size(),
			m_tMeshList[iObj].triList.size());

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

		for (int iSubTri = 0; iSubTri < m_tMeshList[iObj].bufferList.size(); iSubTri++)
		{
			vertexList& vList = m_tMeshList[iObj].vbList[iSubTri];
			_ftprintf(m_pStream, _T("\nVertexList %d"),	vList.size());
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

			IndexList& iList =	m_tMeshList[iObj].ibList[iSubTri];
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
syWriter::~syWriter()
{
}
void syWriter::Set(const TCHAR* name, Interface* pMax)
{
	m_pMax = pMax;
	if (name != nullptr)
	{
		m_filename = name;
	}

	m_pRootNode = m_pMax->GetRootNode();
	m_Interval = m_pMax->GetAnimRange();

	m_Scene.iFirstFrame = m_Interval.Start() / GetTicksPerFrame();
	m_Scene.iLastFrame = m_Interval.End() / GetTicksPerFrame();
	m_Scene.iFrameSpeed = GetFrameRate();
	m_Scene.iTickPerFrame = GetTicksPerFrame();

	m_ObjectList.clear();
	m_tMeshList.clear();
	m_MaterialList.clear();
	m_MtrlList.clear();

	PreProcess(m_pRootNode);
}

void syWriter::PreProcess(INode* pNode)
{
	//max�� ��� ����� �� ������ iNode������ �����
	//��Ʈ���� ��ȯ�ϸ� ��� �۾� �� �� �� ����

	//�츮�� geometry objectã���ž�

	//inode�� ���� ã�°��� �˻��ؼ� objectlist�� �߰��ϴ°� ����
	if (pNode == NULL) return;
	AddObject(pNode);
	AddMaterial(pNode);
	int iNumChildren = pNode->NumberOfChildren();
	for (int iChild = 0; iChild < iNumChildren; iChild++)
	{
		INode* pChild = pNode->GetChildNode(iChild);
		PreProcess(pChild);
	}
}
void syWriter::DumpPoint3(Point3& v0, Point3& v1)
{
	v0.x = v1.x;
	v0.y = v1.z;
	v0.z = v1.y;
}
void   syWriter::DumpMatrix3(Matrix3& matWorld, D3D_MATRIX& world)
{
	Point3 row;
	row = matWorld.GetRow(0);
	world._11 = row.x;
	world._13 = row.y;
	world._12 = row.z;
	world._14 = 0.0f;
	row = matWorld.GetRow(2);
	world._21 = row.x;
	world._23 = row.y;
	world._22 = row.z;
	world._24 = 0.0f;
	row = matWorld.GetRow(1);
	world._31 = row.x;
	world._33 = row.y;
	world._32 = row.z;
	world._34 = 0.0f;
	row = matWorld.GetRow(3);
	world._41 = row.x;
	world._43 = row.y;
	world._42 = row.z;
	world._44 = 1.0f;
}
//INode->TriObject ��ȯ�Ѵ�
TriObject* syWriter::GetTriObjectFromNode(INode* pNode, TimeValue time, bool& deleteit)
{
	Object* obj = pNode->EvalWorldState(time).obj;
	if (obj->CanConvertToType(Class_ID(TRIOBJ_CLASS_ID,0)))
	{
		TriObject* tri = (TriObject*)obj->ConvertToType(time, Class_ID(TRIOBJ_CLASS_ID, 0));
		if (obj != tri) deleteit = true;
		return tri;
	}
	return nullptr;
}
int  syWriter::FindMaterial(INode* pNode)
{
	Mtl* pMtl = pNode->GetMtl();
	if (pMtl)
	{
		for (int iMtl = 0; iMtl < m_MaterialList.size(); iMtl++)
		{
			if (m_MaterialList[iMtl] == pMtl)
			{
				return iMtl;
			}
		}
	}
	return -1;

}
void syWriter::GetMaterial(INode* pNode)
{
	Mtl* pSrcMtl = pNode->GetMtl();
	//�����̳ʰ� � ������ ������� � �ؽ�Ʈ�� ����ߴ��� ������ ���� ����...
	syMtrl tMtrl;
	tMtrl.szName = FixupName(pSrcMtl->GetName());
	// sub-material
	int iNumSub = pSrcMtl->NumSubMtls();
	if (iNumSub > 0)
	{
		for (int iSub = 0; iSub < iNumSub; iSub++)
		{
			Mtl* pSubMtl = pSrcMtl->GetSubMtl(iSub);
			syMtrl  tSubMtrl;
			tSubMtrl.szName = FixupName(pSubMtl->GetName());
			GetTexture(pSubMtl, tSubMtrl);
			tMtrl.subMtrl.push_back(tSubMtrl);
		}
	}
	else
	{
		GetTexture(pSrcMtl, tMtrl);
	}
	m_MtrlList.push_back(tMtrl);
}
void syWriter::GetTexture(Mtl* pMtl, syMtrl& tMtrl)
{

	//max : ����Ʈ 12�� Ȯ�� 24����
	//���ĸ�. �븻�� ���̸� �÷���(��ǻ��) �����Ʈ �������� ���÷���
	/*�ؽ��� ������ �հ� �����̶� ��ũ�� �ִ�. ��ũ�� �ؽ��� ����/������ �� �� ����
	
	�ϳ��� ���͸����� �ϳ��� �����̶�� ���� �ȴ�. �ؽ��� �������� �𸣴� ���� ����
	
	*/
	
	//�ؽ����� ������ �� ����
	//1���� ��ǻ�� ��
	int iNumMap = pMtl->NumSubTexmaps();

	for (int iSubMap = 0; iSubMap < iNumMap; iSubMap++)
	{
		Texmap* tex = pMtl->GetSubTexmap(iSubMap);
		if (tex)
		{
			if (tex->ClassID() == Class_ID(BMTEX_CLASS_ID, 0X00))
			{
				syTexMap tTex;
				tTex.iMapID = iSubMap;

				TSTR fullName;
				TSTR mapName = ((BitmapTex*)(tex))->GetMapName();
				SplitPathFile(mapName, &fullName, &tTex.szName);
				tMtrl.texList.push_back(tTex);
			}
		}
		
	}
}
Point3 syWriter::GetVertexNormal
(Mesh* mesh, int iFace, RVertex* rVertex)
{
	Face* f = &mesh->faces[iFace];
	DWORD smGroup = f->smGroup;
	int numnormals = rVertex->rFlags & NORCT_MASK;//����ũ ó��
	
	Point3 vertexNormal;
	//����� �븻
	if (rVertex->rFlags & SPECIFIED_NORMAL)
	{
		vertexNormal = rVertex->rn.getNormal();
	}
	else if(numnormals && smGroup)
	{
		//�ϳ��� ������ ������ ��ġ�� ��� �븻�� �ֱ� ���� �븻�� ������ŭ �����־�� ��
		//�׷��� �ε����� ���ƾ� ��
		//�����̱׷� üũ
		if (numnormals == 1)
		{
			vertexNormal = rVertex->rn.getNormal();
		}
		else
		{
			//���� �׷� �ȿ� ������ �� �븻�� 
			//�׷� ������ �븻 ������ �� �����ϱ�....................
			//�����̱׷� ������ �ش��ϴ� ������ �׷� �븻 ����
			for (int i = 0; i < numnormals; i++)
			{
				if (rVertex->ern[i].getSmGroup() & smGroup)
				{
					vertexNormal = rVertex->ern[i].getNormal();
				}
			}
		}
	}
	else
	{
		//���̽� �븻�� ������ ����ٳ�
		//�۾��� �������۾��ߴ��� ���ο� ���� �� �ٸ�
		vertexNormal = mesh->getFaceNormal(iFace);
		//���̽� �븻�� ���� �븻�� ����
	}
	return vertexNormal;
}
bool syWriter::TMNegParity(Matrix3 tm)
{
	//x�� y�� �������� �� z���� -z���� ���η� ������ �Ǵ��Ѵ�
	Point3 v0 = tm.GetRow(0);
	Point3 v1 = tm.GetRow(1);
	Point3 v2 = tm.GetRow(2);

	Point3 vCross = CrossProd(v0, v1);
	return (DotProd(vCross, v2) < 0.0f) ? true : false;
}
void syWriter::GetMesh(INode* pNode, syMesh& tMesh)
{//Ʈ���̾ޱ۸���Ʈ ���� + �տ����� Mesh������ ��ȯ ����


	//�߽������� ��� �̷��� ��. 4*4 �Ⱦ��� 4*3������ ��
	Matrix3 tm = pNode->GetObjTMAfterWSM(m_Interval.Start());//�������		//(����) ����� �� m_Interval.Start() �ƴϾ�?
	
		//1. Ʈ���̾ޱ� ������Ʈ �����
	bool deleteit = false;
	TriObject* tri = GetTriObjectFromNode(pNode, m_Interval.Start(), deleteit);
	if (tri == nullptr) return;

		//2. �޽� ������Ʈ �������� -> �������� ������ �ؽ��� �÷� ��� �־�
	Mesh* mesh = &tri->GetMesh();
	//�ٿ���ڽ� ����
	mesh->buildBoundingBox();
	tMesh.box = mesh->getBoundingBox(&tm);

	bool negScale = TMNegParity(tm);
	int v0, v1, v2;
	if (negScale) { v0 = 2; v1 = 1; v2 = 0; }
	else { v0 = 0; v1 = 1; v2 = 2; }
	if (mesh)
	{
		int iNumFace = mesh->getNumFaces();

		vectorTriList& triList = tMesh.triList;
		triList.resize(iNumFace);
		tMesh.bufferList.resize(tMesh.iSubMesh);
		//�߽��� ��������ϰ� ������ ���������� �ϴ°� ����/
		//���⼭ ����ȭ�ϸ� ���� ����
		for (int iFace = 0; iFace < iNumFace; iFace++)
		{
			//max�� pnct�� ��� ����
		//p list, face position list
			int iNumPos = mesh->getNumVerts();
			if (iNumPos > 0) 
			{
				//Point3* posList = mesh->tVerts;
				//Face* posFaceList = mesh->faces;
				Point3 v = mesh->verts[mesh->faces[iFace].v[v0]] * tm;		//(����) �� tm inverse�� �ƴϾ�?
				//cf z y ���� �ٲٰ� ,index���� �ٲٱ�
				DumpPoint3(triList[iFace].v[0].p, v);			//��ǥ�谡 �޶� �ڽ��� ���� ����
																//�𵨸� �����ϸ� local, �ƽ����� ��� ��������� ����
																//������ ������ǥ��!!! -> tm �ʿ��ؿ� objectafterwsm (�۾� �Ϸ�)
				v = mesh->verts[mesh->faces[iFace].v[v2]] * tm;
				DumpPoint3(triList[iFace].v[1].p, v);

				v = mesh->verts[mesh->faces[iFace].v[v1]] * tm;
				DumpPoint3(triList[iFace].v[2].p, v);
			}

			
			/*
			rendervertex : �������Ǵ� ��ǥ���� ���ؽ� -> ����		rVertex ��ǥ��� �����~
			*/
			//c list, face color list
			int iNumColor = mesh->getNumVertCol();
			triList[iFace].v[0].c = Point4(1, 1, 1, 1);
			triList[iFace].v[1].c = Point4(1, 1, 1, 1);
			triList[iFace].v[2].c = Point4(1, 1, 1, 1);
			if (iNumColor > 0)
			{
				triList[iFace].v[0].c =
					mesh->vertCol[mesh->vcFace[iFace].t[v0]];
				triList[iFace].v[1].c =
					mesh->vertCol[mesh->vcFace[iFace].t[v2]];
				triList[iFace].v[2].c =
					mesh->vertCol[mesh->vcFace[iFace].t[v1]];
			}

			//texcoord list, face texture list - �ߺ����� ���� ������ ������� 
				/*		UVVert* texList = mesh->tVerts;
			TVFace* colorFaceList = mesh->tvFace;*/
			int iNumTex = mesh->getNumTVerts();
			if (iNumTex > 0)
			{
				Point2 v = (Point2)mesh->tVerts[mesh->tvFace[iFace].t[v0]];
				triList[iFace].v[0].t.x = v.x;
				triList[iFace].v[0].t.y = 1.0f - v.y;

				v = (Point2)mesh->tVerts[mesh->tvFace[iFace].t[v2]];
				triList[iFace].v[1].t.x = v.x;
				triList[iFace].v[1].t.y = 1.0f - v.y;

				v = (Point2)mesh->tVerts[mesh->tvFace[iFace].t[v1]];
				triList[iFace].v[2].t.x = v.x;
				triList[iFace].v[2].t.y = 1.0f - v.y;
			}
			//�����븻�� ��ǥ���???
		//n list, face normal list	-> ������ ó���� ���� �ؾ��ϹǷ� ������ �Ҳ�
	//=> pnct ����Ʈ ������༭ �ε������� ���� �۾� �� ���� ������
	//���� ��ǥ�� vb,(->�ߺ�����Ʈ�����ϸ�) ib �����
			mesh->buildNormals();
			int vert = mesh->faces[iFace].getVert(v0); //int vert = mesh->faces[iFace].v[v0];		//v[v0]�� �����Լ� getVert(0)���� ���� �� �־�
			RVertex* rVertex = mesh->getRVertPtr(vert);	//normal���� ������ִ� ���� ����Ʈ
			Point3 vn = GetVertexNormal(mesh, iFace, rVertex);
			DumpPoint3(triList[iFace].v[v0].n, vn);

			vert = mesh->faces[iFace].getVert(v2);
			rVertex = mesh->getRVertPtr(vert);
			vn = GetVertexNormal(mesh, iFace, rVertex);
			DumpPoint3(triList[iFace].v[v1].n, vn);

			vert = mesh->faces[iFace].getVert(v1);
			rVertex = mesh->getRVertPtr(vert);
			vn = GetVertexNormal(mesh, iFace, rVertex);
			DumpPoint3(triList[iFace].v[v2].n, vn);



			if(tMesh.iType == CLASS_GEOM)
			{
				// sub material index
				triList[iFace].iSubIndex = mesh->faces[iFace].getMatID();

				if (tMesh.iMtrlID < 0 || 
					m_MtrlList[tMesh.iMtrlID].subMtrl.size() <= 0)
				{
					triList[iFace].iSubIndex = 0;
				}
				//����ó��
				if (tMesh.iMtrlID >0 && triList[iFace].iSubIndex < m_MtrlList[tMesh.iMtrlID].subMtrl.size())
				{
					tMesh.bufferList[triList[iFace].iSubIndex].push_back(triList[iFace]);
					
				}
			}
		}
		if (tMesh.iType == CLASS_GEOM)
		{
			std::sort(triList.begin(), triList.end(), AscendingSort());
			int iFace;
			for (int iMtrl = 0; iMtrl < tMesh.iSubMesh; iMtrl++)
			{
				g_iSearchIndex = iMtrl;
				iFace = count_if(triList.begin(), triList.end(), IsSameInt());
			}
		}
		
		// vb, ib		
		SetUniqueBuffer(tMesh);
	}
	if (deleteit) delete tri;
}
void syWriter::AddObject(INode* pNode)
{
	//�۾��� �� �۾��Ϸ�� ������������ ����� �������Ͽ� ���� ����� ����� �����ش�.
	ObjectState os = pNode->EvalWorldState(m_Interval.Start());	//���� ������Ʈ ���� get
	if (os.obj)
	{
		if (os.obj->ClassID() == Class_ID(TARGET_CLASS_ID, 0))
		{
			return;
		}

		switch (os.obj->SuperClassID())
		{
		case GEOMOBJECT_CLASS_ID :	//�̰� �ƴϸ� vbid ���� ī�޶� ����
		case HELPER_CLASS_ID:
			m_ObjectList.push_back(pNode);
			break;
		default:
			break;
		}
	}
	//GEOMOBJECT_CLASS_ID	//������ ���� �길
	//	//pNode�� �̰�GEOMOBJECT_CLASS_ID�� ������ ����


	//	HELPER_CLASS_ID//�ִϸ��̼ǿ����� �굵

}

void syWriter::AddMaterial(INode* pNode)
{
	Mtl* pMtl = pNode->GetMtl();
	if (pMtl)
	{
		//�ߺ�����
		for (int iMtl = 0; iMtl < m_MaterialList.size(); iMtl++)
		{
			if (m_MaterialList[iMtl] == pMtl)
			{
				return;
			}
		}
		m_MaterialList.push_back(pMtl);
		GetMaterial(pNode);
	}
}

void  syWriter::SetUniqueBuffer(syMesh& tMesh)
{
	tMesh.vbList.resize(tMesh.bufferList.size());
	tMesh.ibList.resize(tMesh.bufferList.size());
	for (int iSub = 0; iSub < tMesh.bufferList.size(); iSub++)
	{
		for (int iFace = 0; iFace < tMesh.bufferList[iSub].size(); iFace++)
		{
			vectorTriList& triArray = tMesh.bufferList[iSub];
			TriList& tri = triArray[iFace];
			vertexList& vList = tMesh.vbList[iSub];
			IndexList& iList = tMesh.ibList[iSub];
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
}

int   syWriter::IsEqulVerteList(PNCT& vertex, vertexList& vList)
{
	for (int iVer = 0; iVer < vList.size(); iVer++)
	{
		if (EqualPoint3(vertex.p, vList[iVer].p) &&
			EqualPoint3(vertex.n, vList[iVer].n) &&
			EqualPoint4(vertex.c, vList[iVer].c) &&
			EqualPoint2(vertex.t, vList[iVer].t))
		{
			return iVer;
		}
	}
	return -1;
}

void syWriter::GetAnimation(INode* pNode, syMesh& tMesh)
{
	tMesh.bAnimatin[0] = false;
	tMesh.bAnimatin[1] = false;
	tMesh.bAnimatin[2] = false;
	//-------------------> �߿�
	TimeValue startFrame = m_Interval.Start();
	// tm = selfTm * parentTm * Inverse(parentTm);
	Matrix3 tm = pNode->GetNodeTM(startFrame) * Inverse(pNode->GetParentTM(startFrame));
	// ��� ����
	AffineParts StartAP;
	decomp_affine(tm, &StartAP);
	// quaternion -> axis, angle ��ȯ
	Point3  Start_RotAxis;
	float   Start_RotValue;
	AngAxisFromQ(StartAP.q, &Start_RotValue, Start_RotAxis);
	//<----------------------------
	syAnimTrack StartAnim;
	ZeroMemory(&StartAnim, sizeof(syAnimTrack));
	StartAnim.i = startFrame;
	StartAnim.p = StartAP.t;
	StartAnim.q = StartAP.q;
	tMesh.animPos.push_back(StartAnim);
	tMesh.animRot.push_back(StartAnim);
	StartAnim.p = StartAP.k;
	StartAnim.q = StartAP.u;
	tMesh.animScl.push_back(StartAnim);

	TimeValue start =
		m_Interval.Start() + GetTicksPerFrame();
	TimeValue end = m_Interval.End();
	// ����+1������
	for (TimeValue t = start; t <= end; t += GetTicksPerFrame())
	{
		Matrix3 tm = pNode->GetNodeTM(t)
			* Inverse(pNode->GetParentTM(t));		
													
		AffineParts FrameAP;
		decomp_affine(tm, &FrameAP);

		syAnimTrack anim;
		ZeroMemory(&anim, sizeof(syAnimTrack));
		anim.i = t;
		anim.p = FrameAP.t;
		anim.q = FrameAP.q;
		tMesh.animPos.push_back(anim);
		tMesh.animRot.push_back(anim);
		anim.p = FrameAP.k;
		anim.q = FrameAP.u;
		tMesh.animScl.push_back(anim);			

		Point3  Frame_RotAxis;
		float   Frame_RotValue;
		AngAxisFromQ(FrameAP.q, &Frame_RotValue, Frame_RotAxis);


		if (!tMesh.bAnimatin[0]) {
			if (!EqualPoint3(StartAP.t, FrameAP.t))
			{
				tMesh.bAnimatin[0] = true;
			}
		}

		if (!tMesh.bAnimatin[1]) {
			if (!EqualPoint3(Start_RotAxis, Frame_RotAxis))
			{
				tMesh.bAnimatin[1] = true;
			}
			else
			{
				if (Start_RotValue != Frame_RotValue)	//(����)�ո�
				{
					tMesh.bAnimatin[1] = true;
				}
			}
		}

		if (!tMesh.bAnimatin[2]) {
			if (!EqualPoint3(StartAP.k, FrameAP.k))
			{
				tMesh.bAnimatin[2] = true;
			}
		}
	}

}
void syWriter::ExportAnimation(syMesh& tMesh)
{
	_ftprintf(m_pStream, _T("\n%s %d %d %d"),
		L"#AnimationData",
		(tMesh.bAnimatin[0]) ? tMesh.animPos.size() : 0,
		(tMesh.bAnimatin[1]) ? tMesh.animRot.size() : 0,
		(tMesh.bAnimatin[2]) ? tMesh.animScl.size() : 0);
	if (tMesh.bAnimatin[0])
	{
		for (int iTrack = 0; iTrack < tMesh.animPos.size(); iTrack++)
		{
			_ftprintf(m_pStream, _T("\n%d %d %10.4f %10.4f %10.4f"),
				iTrack,
				tMesh.animPos[iTrack].i,
				tMesh.animPos[iTrack].p.x,
				tMesh.animPos[iTrack].p.z,
				tMesh.animPos[iTrack].p.y);
		}
	}
	if (tMesh.bAnimatin[1])
	{
		for (int iTrack = 0; iTrack < tMesh.animRot.size(); iTrack++)
		{
			_ftprintf(m_pStream, _T("\n%d %d %10.4f %10.4f %10.4f %10.4f"),
				iTrack,
				tMesh.animRot[iTrack].i,
				tMesh.animRot[iTrack].q.x,
				tMesh.animRot[iTrack].q.z,
				tMesh.animRot[iTrack].q.y,
				tMesh.animRot[iTrack].q.w);
		}
	}
	if (tMesh.bAnimatin[2])
	{
		for (int iTrack = 0; iTrack < tMesh.animScl.size(); iTrack++)
		{
			_ftprintf(m_pStream, _T("\n%d %d %10.4f %10.4f %10.4f %10.4f %10.4f %10.4f %10.4f"),
				iTrack,
				tMesh.animScl[iTrack].i,
				tMesh.animScl[iTrack].p.x,
				tMesh.animScl[iTrack].p.z,
				tMesh.animScl[iTrack].p.y,
				tMesh.animScl[iTrack].q.x,
				tMesh.animScl[iTrack].q.z,
				tMesh.animScl[iTrack].q.y,
				tMesh.animScl[iTrack].q.w);
		}
	}
}


bool syWriter::CheckForAnimation(INode* pNode, bool&  bPos, bool& bRot, bool& bScl)
{
	//-------------------> �߿�
	TimeValue startFrame = m_Interval.Start();
	// tm = selfTm * parentTm * Inverse(parentTm);
	Matrix3 tm = pNode->GetNodeTM(startFrame) * Inverse(pNode->GetParentTM(startFrame));
	// ��� ����
	AffineParts StartAP;
	decomp_affine(tm, &StartAP);
	// quaternion -> axis, angle ��ȯ
	Point3  Start_RotAxis;
	float   Start_RotValue;
	AngAxisFromQ(StartAP.q, &Start_RotValue, Start_RotAxis);
	//<----------------------------

	TimeValue start = m_Interval.Start() + GetTicksPerFrame();
	TimeValue end = m_Interval.End();
	// ����+1������
	for (TimeValue t = start; t <= end;	t += GetTicksPerFrame())
	{
		Matrix3 tm = pNode->GetNodeTM(t) * Inverse(pNode->GetParentTM(t));

		AffineParts FrameAP;
		decomp_affine(tm, &FrameAP);
		Point3  Frame_RotAxis;
		float   Frame_RotValue;
		AngAxisFromQ(FrameAP.q, &Frame_RotValue, Frame_RotAxis);


		if (!bPos) {
			if (!EqualPoint3(StartAP.t, FrameAP.t))
			{
				bPos = true;
			}
		}

		if (!bRot) {
			if (!EqualPoint3(Start_RotAxis, Frame_RotAxis))
			{
				bRot = true;
			}
			else
			{
				if (Frame_RotValue != Frame_RotValue)
				{
					bRot = true;
				}
			}
		}

		if (!bScl) {
			if (!EqualPoint3(StartAP.k, FrameAP.k))
			{
				bScl = true;
			}
		}
		if (bPos &&  bRot && bScl) break;
	}

	return bPos || bRot || bScl;
}
void syWriter::DumpPosSample(INode* pNode, syMesh& pMesh)
{
	TimeValue start = m_Interval.Start();
	TimeValue end = m_Interval.End();

	for (TimeValue t = start; t <= end; t += GetTicksPerFrame())
	{
		Matrix3 tm = pNode->GetNodeTM(t) * Inverse(pNode->GetParentTM(t));
		AffineParts FrameAP;
		decomp_affine(tm, &FrameAP);
		syAnimTrack anim;
		ZeroMemory(&anim, sizeof(syAnimTrack));
		anim.i = t;
		anim.p = FrameAP.t;
		pMesh.animPos.push_back(anim);
	}
}
void syWriter::DumpRotSample(INode* pNode, syMesh& pMesh)
{
	TimeValue start = m_Interval.Start();
	TimeValue end = m_Interval.End();

	for (TimeValue t = start; t <= end; t += GetTicksPerFrame())
	{
		Matrix3 tm = pNode->GetNodeTM(t)* Inverse(pNode->GetParentTM(t));
		AffineParts FrameAP;
		decomp_affine(tm, &FrameAP);
		syAnimTrack anim;
		ZeroMemory(&anim, sizeof(syAnimTrack));
		anim.i = t;
		anim.q = FrameAP.q;
		pMesh.animRot.push_back(anim);
	}
}
void syWriter::DumpSclSample(INode* pNode, syMesh& pMesh)
{
	TimeValue start = m_Interval.Start();
	TimeValue end = m_Interval.End();

	for (TimeValue t = start; t <= end; t += GetTicksPerFrame())
	{
		Matrix3 tm = pNode->GetNodeTM(t) * Inverse(pNode->GetParentTM(t));
		AffineParts FrameAP;
		decomp_affine(tm, &FrameAP);
		syAnimTrack anim;
		ZeroMemory(&anim, sizeof(syAnimTrack));
		anim.i = t;
		anim.q = FrameAP.u;
		anim.p = FrameAP.k;
		pMesh.animScl.push_back(anim);
	}
}