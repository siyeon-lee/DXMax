#include "stdafx.h"
#include "sySkinExp.h"
#include "syMatrixExp.h"
struct BAscendingSort
{
	bool operator()(BTriList& rpStart, BTriList& rpEnd)
	{
		return rpStart.iSubIndex < rpEnd.iSubIndex;
	}
};
static int g_iSearchIndex = 0;
struct BIsSameInt // find_to와 같은지 판단해 주는 함수자   
{
	bool operator()(BTriList &value)
	{
		return value.iSubIndex == g_iSearchIndex;
	}
};



sySkinExp::sySkinExp()
{
}


sySkinExp::~sySkinExp()
{
}


void sySkinExp::Set(const TCHAR* name, Interface* pMax)
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

	//m_ObjectList.clear();			// 근데 왜 여기만 고쳐 syWriter안고쳐?
	//m_bMeshList.clear();
	//m_MaterialList.clear();
	m_bMeshList.clear();

	/*PreProcess(m_pRootNode);*/
}
bool  sySkinExp::Export()
{
	Convert();

	m_pStream = nullptr;
	//header
	m_Scene.iNumMaterials = m_MtrlList.size();
	m_Scene.iNumObjects = m_bMeshList.size();
	_wfopen_s(&m_pStream, m_filename.c_str(), _T("wb"));
	_ftprintf(m_pStream, _T("%s"), _T("skinexporter100"));	//skinexporter100 : 파일 검증
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
		//서브 메터리얼 없을 때
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
	for (int iObj = 0; iObj < m_bMeshList.size(); iObj++)
	{
		_ftprintf(m_pStream, _T("\n%s %s %d %d %d"),
			m_bMeshList[iObj].name,
			m_bMeshList[iObj].ParentName,
			m_bMeshList[iObj].iMtrlID,
			m_bMeshList[iObj].bufferList.size(),
			m_bMeshList[iObj].triList.size());

		_ftprintf(m_pStream, _T("\n\t%10.4f %10.4f %10.4f %10.4f\n\t%10.4f %10.4f %10.4f %10.4f\n\t%10.4f %10.4f %10.4f %10.4f\n\t%10.4f %10.4f %10.4f %10.4f"),
			m_bMeshList[iObj].matWorld._11,
			m_bMeshList[iObj].matWorld._12,
			m_bMeshList[iObj].matWorld._13,
			m_bMeshList[iObj].matWorld._14,

			m_bMeshList[iObj].matWorld._21,
			m_bMeshList[iObj].matWorld._22,
			m_bMeshList[iObj].matWorld._23,
			m_bMeshList[iObj].matWorld._24,

			m_bMeshList[iObj].matWorld._31,
			m_bMeshList[iObj].matWorld._32,
			m_bMeshList[iObj].matWorld._33,
			m_bMeshList[iObj].matWorld._34,

			m_bMeshList[iObj].matWorld._41,
			m_bMeshList[iObj].matWorld._42,
			m_bMeshList[iObj].matWorld._43,
			m_bMeshList[iObj].matWorld._44);

		for (int iSubTri = 0; iSubTri < m_bMeshList[iObj].bufferList.size(); iSubTri++)
		{
			BvertexList& vList = m_bMeshList[iObj].vbList[iSubTri];
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
				_ftprintf(m_pStream, _T("%10.4f %10.4f %10.4f %10.4f"),
					vList[iVer].i1[0],
					vList[iVer].i1[1],
					vList[iVer].i1[2],
					vList[iVer].i1[3]);
				_ftprintf(m_pStream, _T("%10.4f %10.4f %10.4f %10.4f"),
					vList[iVer].w1[0],
					vList[iVer].w1[1],
					vList[iVer].w1[2],
					vList[iVer].w1[3]);
				_ftprintf(m_pStream, _T("%10.4f %10.4f %10.4f %10.4f"),
					vList[iVer].i2[0],
					vList[iVer].i2[1],
					vList[iVer].i2[2],
					vList[iVer].i2[3]);
				_ftprintf(m_pStream, _T("%10.4f %10.4f %10.4f %10.4f"),
					vList[iVer].w2[0],
					vList[iVer].w2[1],
					vList[iVer].w2[2],
					vList[iVer].w2[3]);
			}

			IndexList& iList = m_bMeshList[iObj].ibList[iSubTri];
			_ftprintf(m_pStream, _T("\nIndexList %d"), iList.size());
			for (int iIndex = 0; iIndex < iList.size(); iIndex += 3)
			{
				_ftprintf(m_pStream, _T("\n%d %d %d"),
					iList[iIndex + 0],
					iList[iIndex + 1],
					iList[iIndex + 2]);
			}
		}
	
	}
	//드레스 포즈의 각 바이패드 행렬 출력
	int iNumBone = syMatrixExp::Get().m_ObjectList.size();
	_ftprintf(m_pStream, _T("\n%s %d"), L"#INV_GETNODETM", iNumBone);
	for (int iObj = 0; iObj < iNumBone; iObj++)
	{
		INode* pNode =
			syMatrixExp::Get().m_ObjectList[iObj];
		Matrix3 wtm = pNode->GetNodeTM(m_Interval.Start());
		Matrix3 invWtm = Inverse(wtm);

		D3D_MATRIX tm;
		DumpMatrix3(invWtm, tm);
		
		_ftprintf(m_pStream,
			_T("\n\t%10.4f %10.4f %10.4f %10.4f\n\t%10.4f %10.4f %10.4f %10.4f\n\t%10.4f %10.4f %10.4f %10.4f\n\t%10.4f %10.4f %10.4f %10.4f"),
			tm._11,
			tm._12,
			tm._13,
			tm._14,

			tm._21,
			tm._22,
			tm._23,
			tm._24,

			tm._31,
			tm._32,
			tm._33,
			tm._34,

			tm._41,
			tm._42,
			tm._43,
			tm._44);
	}
	fclose(m_pStream);


	MessageBox(GetActiveWindow(), m_filename.c_str(),
		_T("Succeed!"), MB_OK);
	return true;
}

bool  sySkinExp::Convert()
{
	for (int iObj = 0; iObj < m_ObjectList.size(); iObj++)
	{
		INode* pNode = m_ObjectList[iObj];
		syBMesh tMesh;
		tMesh.name = FixupName(pNode->GetName());
		INode* pParentNode = pNode->GetParentNode();
		if (pParentNode &&
			// 가상의 부모가 존재
			pParentNode->IsRootNode() == false)
		{
			tMesh.ParentName =
				FixupName(pParentNode->GetName());
		}
		SetBippedInfo(pNode, tMesh);

		// objTM = s*r*t*p * c;
		// GetNodeTM = srt * p;
		Matrix3 wtm = pNode->GetNodeTM(m_Interval.Start());
		DumpMatrix3(wtm, tMesh.matWorld);
		Matrix3 invWtm = Inverse(wtm);
		DumpMatrix3(invWtm, tMesh.matInvWorld);

		// 메터리얼 정보 얻고 / 저장
		Mtl* pMtl = pNode->GetMtl();
		if (pMtl)
		{
			tMesh.iMtrlID = FindMaterial(pNode);
		}
		if (tMesh.iMtrlID >= 0 && m_MtrlList[tMesh.iMtrlID].subMtrl.size() > 0)
		{
			tMesh.iSubMesh = m_MtrlList[tMesh.iMtrlID].subMtrl.size();
		}
		GetMesh(pNode, tMesh);
		m_bMeshList.push_back(tMesh);
	}
	return true;
}

void sySkinExp::GetMesh(INode* pNode, syBMesh& tMesh)
{
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
	bool negScale = TMNegParity(tm);
	int v0, v1, v2;
	if (negScale) { v0 = 2; v1 = 1; v2 = 0; }
	else { v0 = 0; v1 = 1; v2 = 2; }
	if (mesh)
	{
		int iNumFace = mesh->getNumFaces();

		BvectorTriList& triList = tMesh.triList;
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



			//biped index & weight save

			for (int iWeight = 0;
				iWeight <
				tMesh.bipedList[mesh->faces[iFace].v[v0]].m_iNumWeight;
				iWeight++)
			{
				if (iWeight < 4)
				{
					triList[iFace].v[v0].i1[iWeight] =
						tMesh.bipedList[mesh->faces[iFace].v[v0]].m_IDList[iWeight];
					triList[iFace].v[v0].w1[iWeight] =
						tMesh.bipedList[mesh->faces[iFace].v[v0]].m_WeightList[iWeight];
				}
				else
				{
					triList[iFace].v[v0].i2[iWeight - 4] =
						tMesh.bipedList[mesh->faces[iFace].v[v0]].m_IDList[iWeight];
					triList[iFace].v[v0].w2[iWeight - 4] =
						tMesh.bipedList[mesh->faces[iFace].v[v0]].m_WeightList[iWeight];
				}
			}
			for (int iWeight = 0;
				iWeight <
				tMesh.bipedList[mesh->faces[iFace].v[v2]].m_iNumWeight;
				iWeight++)
			{
				if (iWeight < 4)
				{
					triList[iFace].v[v1].i1[iWeight] =
						tMesh.bipedList[mesh->faces[iFace].v[v2]].m_IDList[iWeight];
					triList[iFace].v[v1].w1[iWeight] =
						tMesh.bipedList[mesh->faces[iFace].v[v2]].m_WeightList[iWeight];
				}
				else
				{
					triList[iFace].v[v1].i2[iWeight - 4] =
						tMesh.bipedList[mesh->faces[iFace].v[v2]].m_IDList[iWeight];
					triList[iFace].v[v1].w2[iWeight - 4] =
						tMesh.bipedList[mesh->faces[iFace].v[v2]].m_WeightList[iWeight];
				}
			}
			for (int iWeight = 0;
				iWeight <
				tMesh.bipedList[mesh->faces[iFace].v[v1]].m_iNumWeight;
				iWeight++)
			{
				if (iWeight < 4)
				{
					triList[iFace].v[v2].i1[iWeight] =
						tMesh.bipedList[mesh->faces[iFace].v[v1]].m_IDList[iWeight];
					triList[iFace].v[v2].w1[iWeight] =
						tMesh.bipedList[mesh->faces[iFace].v[v1]].m_WeightList[iWeight];
				}
				else
				{
					triList[iFace].v[v2].i2[iWeight - 4] =
						tMesh.bipedList[mesh->faces[iFace].v[v1]].m_IDList[iWeight];
					triList[iFace].v[v2].w2[iWeight - 4] =
						tMesh.bipedList[mesh->faces[iFace].v[v1]].m_WeightList[iWeight];
				}
			}

			// sub material index
			triList[iFace].iSubIndex =
				mesh->faces[iFace].getMatID();
			if (tMesh.iMtrlID < 0 ||
				m_MtrlList[tMesh.iMtrlID].subMtrl.size() <= 0)
			{
				triList[iFace].iSubIndex = 0;
			}

			tMesh.bufferList[
				triList[iFace].iSubIndex].push_back(
					triList[iFace]);
		}

		std::sort(triList.begin(), triList.end(), BAscendingSort());
		int iFace;
		for (int iMtrl = 0; iMtrl < tMesh.iSubMesh; iMtrl++)
		{
			g_iSearchIndex = iMtrl;
			iFace = count_if(triList.begin(),
				triList.end(),
				BIsSameInt());
		}

		// vb, ib		
		SetUniqueBuffer(tMesh);
	}
	if (deleteit) delete tri;
}

void  sySkinExp::SetUniqueBuffer(syBMesh& tMesh)
{
	tMesh.vbList.resize(tMesh.bufferList.size());
	tMesh.ibList.resize(tMesh.bufferList.size());
	for (int iSub = 0; iSub < tMesh.bufferList.size(); iSub++)
	{
		for (int iFace = 0; iFace < tMesh.bufferList[iSub].size(); iFace++)
		{
			BvectorTriList& triArray = tMesh.bufferList[iSub];
			BTriList& tri = triArray[iFace];
			BvertexList& vList = tMesh.vbList[iSub];
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

int   sySkinExp::IsEqulVerteList(PNCTWI_VERTEX& vertex, BvertexList& vList)
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
void  sySkinExp::SetBippedInfo(INode* pNode, syBMesh& tMesh)
{
	//애니메이션 제작 도구
	Modifier* phyMod = FindModifier(pNode, Class_ID(PHYSIQUE_CLASS_ID_A, PHYSIQUE_CLASS_ID_B));
	Modifier* skinMod = FindModifier(pNode, SKIN_CLASSID);
	if (phyMod)
	{
		ExportPhysiqueData(pNode, phyMod, tMesh);
	}
	else if (skinMod)
	{
		ExportSkinData(pNode, skinMod, tMesh);
	}
}
Modifier*   sySkinExp::FindModifier(
	INode* pNode, Class_ID classID)
{
	Object *ObjectPtr = pNode->GetObjectRef();
	if (!ObjectPtr)
	{
		return NULL;
	}
	while (ObjectPtr->SuperClassID() == GEN_DERIVOB_CLASS_ID && ObjectPtr)
	{
		IDerivedObject *DerivedObjectPtr = (IDerivedObject *)(ObjectPtr);
		int ModStackIndex = 0;
		while (ModStackIndex < DerivedObjectPtr->NumModifiers())
		{
			Modifier* ModifierPtr = DerivedObjectPtr->GetModifier(ModStackIndex);
			if (ModifierPtr->ClassID() == classID)
			{
				return ModifierPtr;
			}
			ModStackIndex++;
		}
		ObjectPtr = DerivedObjectPtr->GetObjRef();
	}
	return NULL;
}

void  sySkinExp::ExportPhysiqueData(INode* pNode, Modifier* phyMod, syBMesh& tMesh)
{
	//Physique Modifier로 정점당 정보 얻기
	IPhysiqueExport *phyExport = (IPhysiqueExport*)phyMod->GetInterface(I_PHYINTERFACE);
	IPhyContextExport *contextExport = (IPhyContextExport *)phyExport->GetContextInterface(pNode);

	//모든 정점을 IPhyRigidVertex or IPhyBlendedRigidVertex로 변환한다
	contextExport->ConvertToRigid(true);
	contextExport->AllowBlending(true);

	//mesh 정점개수와 동일해야 함
	int iNumVert = contextExport->GetNumberVertices();
	tMesh.bipedList.resize(iNumVert);

	//이제 iVertex마다 정점 타입 확인하여 해당 인터페이스로 변환하여 링크된 노드, 가중치 얻으면 된다.
	for (int iVertex = 0; iVertex < iNumVert; iVertex++)
	{
		IPhyVertexExport *vtxExport =
			(IPhyVertexExport *)contextExport->GetVertexInterface(iVertex);
		if (vtxExport)
		{
			int iVertexType = vtxExport->GetVertexType();
			switch (iVertexType)
			{
			case RIGID_NON_BLENDED_TYPE:
			{
				IPhyRigidVertex* pVertex =
					(IPhyRigidVertex*)vtxExport;
				INode* node = pVertex->GetNode();
				int  iIndex = GetFindIndex(node);
				tMesh.bipedList[iVertex].m_IDList.push_back(iIndex);
				tMesh.bipedList[iVertex].m_WeightList.push_back(1.0f);
				tMesh.bipedList[iVertex].m_iNumWeight = 1;
			}break;
			case RIGID_BLENDED_TYPE:
			{
				IPhyBlendedRigidVertex* pVertex =
					(IPhyBlendedRigidVertex*)vtxExport;
				for (int iWeight = 0;
					iWeight < pVertex->GetNumberNodes();
					iWeight++)
				{
					INode* node = pVertex->GetNode(iWeight);
					int  iIndex = GetFindIndex(node);
					tMesh.bipedList[iVertex].m_IDList.push_back(iIndex);
					tMesh.bipedList[iVertex].m_WeightList.push_back(pVertex->GetWeight(iWeight));

				}
				tMesh.bipedList[iVertex].m_iNumWeight =
					tMesh.bipedList[iVertex].m_IDList.size();

			}break;
			default:break;
			}
		}
		contextExport->ReleaseVertexInterface(vtxExport);
	}
	phyExport->ReleaseContextInterface(contextExport);
	phyMod->ReleaseInterface(I_PHYINTERFACE, phyExport);
}
void  sySkinExp::ExportSkinData(INode* pNode, Modifier* skinMod, syBMesh& tMesh)
{
	//주의) 블랜딩과 논블랜딩 구별하지 않음
	ISkin* skin = (ISkin*)skinMod->GetInterface(I_SKIN);
	ISkinContextData* skinData = skin->GetContextInterface(pNode);

	if (!skin || !skinData) return;

	int iNumVertex = skinData->GetNumPoints();
	tMesh.bipedList.resize(iNumVertex);

	for (int iVertex = 0; iVertex < iNumVertex; iVertex++)
	{
		tMesh.bipedList[iVertex].m_iNumWeight = skinData->GetNumAssignedBones(iVertex);
		for (int iWeight = 0; iWeight < tMesh.bipedList[iVertex].m_iNumWeight; iWeight++)
		{
			int iBoneID = skinData->GetAssignedBone(iVertex, iWeight);
			INode* node = skin->GetBone(iBoneID);
			tMesh.bipedList[iVertex].m_IDList.push_back(GetFindIndex(node));
			tMesh.bipedList[iVertex].m_WeightList.push_back(skinData->GetBoneWeight(iVertex, iWeight));
		}
	}

}

int  sySkinExp::GetFindIndex(INode* node)
{
	for (int iObj = 0; iObj < syMatrixExp::Get().m_ObjectList.size(); iObj++)
	{
		if (syMatrixExp::Get().m_ObjectList[iObj] == node)
		{
			return iObj;
		}
	}
	return -1;
}