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
struct IsSameInt // find_to와 같은지 판단해 주는 함수자   
{
	bool operator()(TriList &value)
	{
		return value.iSubIndex == g_iSearchIndex;
	}
};
/*
max m키  : 텍스쳐 입힐 수 있어
material bank  - diffuse color - bitmap (픽셀쉐이더를 에디터로 작업하는 셈)

오른쪽 상단에 render 어쩌구 주전자 하면 랜더링한 결과 출력 가능 FrameWindow

랜더링 결과 나옴
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
			// 가상의 부모가 존재
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
	_ftprintf(m_pStream, _T("%s"), _T("symaxexporter100"));	//syexporter100 : 파일 검증
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
	//max의 모든 장면은 그 단위로 iNode단위로 저장됨
	//루트부터 순환하면 모든 작업 다 할 수 있음

	//우리는 geometry object찾을거야

	//inode가 내가 찾는건지 검색해서 objectlist에 추가하는게 다임
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
//INode->TriObject 변환한다
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
	//디자이너가 어떤 랜더링 방식으로 어떤 텍스트를 사용했는지 정보가 정말 많아...
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

	//max : 디폴트 12개 확장 24개쯤
	//알파맵. 노말맵 높이맵 컬러맵(디퓨즈) 엠비언트 스펙츌러 리플렉션
	/*텍스쳐 종류가 잇고 슬롯이란 뱅크가 있다. 뱅크에 텍스쳐 한장/여러장 들어갈 수 있음
	
	하나의 메터리얼이 하나의 슬롯이라고 보면 된다. 텍스쳐 몇장인지 모르니 포문 돌려
	
	*/
	
	//텍스쳐의 종류가 몇 개냐
	//1번이 디퓨즈 맵
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
	int numnormals = rVertex->rFlags & NORCT_MASK;//마스크 처리
	
	Point3 vertexNormal;
	//명시적 노말
	if (rVertex->rFlags & SPECIFIED_NORMAL)
	{
		vertexNormal = rVertex->rn.getNormal();
	}
	else if(numnormals && smGroup)
	{
		//하나의 정점에 영향을 끼치는 모든 노말이 있구 ㄱ그 노말의 개수만큼 돌려주어야 함
		//그룹의 인덱스는 같아야 함
		//스무싱그룹 체크
		if (numnormals == 1)
		{
			vertexNormal = rVertex->rn.getNormal();
		}
		else
		{
			//같은 그룹 안에 들어왔을 때 노말을 
			//그룹 스무싱 노말 공유될 수 있으니까....................
			//스무싱그룹 같으면 해당하는 스무싱 그룹 노말 리턴
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
		//페이스 노말이 정점말 따라다녀
		//작업을 스무싱작업했는지 여부에 따라 또 다름
		vertexNormal = mesh->getFaceNormal(iFace);
		//페이스 노말을 정점 노말에 넣음
	}
	return vertexNormal;
}
bool syWriter::TMNegParity(Matrix3 tm)
{
	//x축 y축 외적했을 때 z인지 -z인지 여부로 뒤집힘 판단한다
	Point3 v0 = tm.GetRow(0);
	Point3 v1 = tm.GetRow(1);
	Point3 v2 = tm.GetRow(2);

	Point3 vCross = CrossProd(v0, v1);
	return (DotProd(vCross, v2) < 0.0f) ? true : false;
}
void syWriter::GetMesh(INode* pNode, syMesh& tMesh)
{//트라이앵글리스트 구축 + 앞에서의 Mesh까지의 변환 담음


	//멕스에서는 행렬 이렇게 씀. 4*4 안쓰고 4*3까지만 슴
	Matrix3 tm = pNode->GetObjTMAfterWSM(m_Interval.Start());//월드행렬		//(질문) 여기는 왜 m_Interval.Start() 아니야?
	
		//1. 트라이앵글 오브젝트 만들기
	bool deleteit = false;
	TriObject* tri = GetTriObjectFromNode(pNode, m_Interval.Start(), deleteit);
	if (tri == nullptr) return;

		//2. 메쉬 오브젝트 가져오기 -> 정점갯수 포지션 텍스쳐 컬러 모두 있어
	Mesh* mesh = &tri->GetMesh();
	//바운딩박스 정보
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
		//멕스는 원본출력하고 편집은 게임툴에서 하는게 나음/
		//여기서 최적화하면 원본 날라감
		for (int iFace = 0; iFace < iNumFace; iFace++)
		{
			//max는 pnct가 모두 별개
		//p list, face position list
			int iNumPos = mesh->getNumVerts();
			if (iNumPos > 0) 
			{
				//Point3* posList = mesh->tVerts;
				//Face* posFaceList = mesh->faces;
				Point3 v = mesh->verts[mesh->faces[iFace].v[v0]] * tm;		//(질문) 왜 tm inverse가 아니야?
				//cf z y 순서 바꾸고 ,index순서 바꾸기
				DumpPoint3(triList[iFace].v[0].p, v);			//좌표계가 달라서 박스가 뜨진 않음
																//모델링 제작하면 local, 맥스에서 통맵 만들었으면 월드
																//기준은 월드좌표로!!! -> tm 필요해요 objectafterwsm (작업 완료)
				v = mesh->verts[mesh->faces[iFace].v[v2]] * tm;
				DumpPoint3(triList[iFace].v[1].p, v);

				v = mesh->verts[mesh->faces[iFace].v[v1]] * tm;
				DumpPoint3(triList[iFace].v[2].p, v);
			}

			
			/*
			rendervertex : 랜더링되는 좌표계의 버텍스 -> 월드		rVertex 좌표계는 월드야~
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

			//texcoord list, face texture list - 중복되지 않은 정점만 들어있음 
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
			//정점노말의 좌표계는???
		//n list, face normal list	-> 스무싱 처리를 따로 해야하므로 별개로 할께
	//=> pnct 리스트 만들어줘서 인덱스버퍼 없이 작업 할 수도 있지만
	//최종 목표는 vb,(->중복리스트제거하면) ib 만들기
			mesh->buildNormals();
			int vert = mesh->faces[iFace].getVert(v0); //int vert = mesh->faces[iFace].v[v0];		//v[v0]은 내장함수 getVert(0)으로 구할 수 있어
			RVertex* rVertex = mesh->getRVertPtr(vert);	//normal값만 계산해주는 정점 리스트
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
				//예외처리
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
	//작업중 및 작업완료된 파이프라인의 결과에 엑세스하여 현재 노드의 결과를 돌려준다.
	ObjectState os = pNode->EvalWorldState(m_Interval.Start());	//현재 오브젝트 상태 get
	if (os.obj)
	{
		if (os.obj->ClassID() == Class_ID(TARGET_CLASS_ID, 0))
		{
			return;
		}

		switch (os.obj->SuperClassID())
		{
		case GEOMOBJECT_CLASS_ID :	//이게 아니면 vbid 없어 카메라 빼고
		case HELPER_CLASS_ID:
			m_ObjectList.push_back(pNode);
			break;
		default:
			break;
		}
	}
	//GEOMOBJECT_CLASS_ID	//정적일 때는 얘만
	//	//pNode가 이거GEOMOBJECT_CLASS_ID랑 같은지 따져


	//	HELPER_CLASS_ID//애니메이션에서는 얘도

}

void syWriter::AddMaterial(INode* pNode)
{
	Mtl* pMtl = pNode->GetMtl();
	if (pMtl)
	{
		//중복제거
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
	//-------------------> 중요
	TimeValue startFrame = m_Interval.Start();
	// tm = selfTm * parentTm * Inverse(parentTm);
	Matrix3 tm = pNode->GetNodeTM(startFrame) * Inverse(pNode->GetParentTM(startFrame));
	// 행렬 분해
	AffineParts StartAP;
	decomp_affine(tm, &StartAP);
	// quaternion -> axis, angle 변환
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
	// 시작+1프레임
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
				if (Start_RotValue != Frame_RotValue)	//(수정)손목
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
	//-------------------> 중요
	TimeValue startFrame = m_Interval.Start();
	// tm = selfTm * parentTm * Inverse(parentTm);
	Matrix3 tm = pNode->GetNodeTM(startFrame) * Inverse(pNode->GetParentTM(startFrame));
	// 행렬 분해
	AffineParts StartAP;
	decomp_affine(tm, &StartAP);
	// quaternion -> axis, angle 변환
	Point3  Start_RotAxis;
	float   Start_RotValue;
	AngAxisFromQ(StartAP.q, &Start_RotValue, Start_RotAxis);
	//<----------------------------

	TimeValue start = m_Interval.Start() + GetTicksPerFrame();
	TimeValue end = m_Interval.End();
	// 시작+1프레임
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