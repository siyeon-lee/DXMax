#include "syMaxObject.h"


HRESULT syMaxObject::CreateVertexData()
{

	HRESULT hr = S_OK;
	char buffer[256];
	static TCHAR CheckFile[MAX_PATH];
	static TCHAR ExporterCheck[MAX_PATH];
	static TCHAR HeaderCheck[MAX_PATH];
	static TCHAR MaterialCheck[MAX_PATH];
	ZeroMemory(&buffer, sizeof(char) * 256);
	ZeroMemory(CheckFile, _countof(CheckFile));
	ZeroMemory(ExporterCheck, _countof(ExporterCheck));
	ZeroMemory(HeaderCheck, _countof(HeaderCheck));
	ZeroMemory(MaterialCheck, _countof(MaterialCheck));

	FILE* pStream = nullptr;
	_wfopen_s(&pStream, m_filename.c_str(), _T("rb"));
	if (pStream == nullptr) return false;

	_ftscanf_s(pStream, _T("%s"), CheckFile, _countof(CheckFile));
	//(필요) syexporter100인지 검증작업 필요 

	int objsize = 0;
	int texsize = 0;
	_ftscanf_s(pStream, _T("%s %d %d %d %d %d %d"), HeaderCheck, _countof(HeaderCheck),
		&m_Scene.iFirstFrame,
		&m_Scene.iLastFrame,
		&m_Scene.iFrameSpeed,
		&m_Scene.iTickPerFrame,
		&m_Scene.iNumObjects,
		&m_Scene.iNumMaterials);
	//HeaderCheck == #HEADER INFO" 검증
	m_tMeshList.resize(m_Scene.iNumObjects);
	m_MtrlList.resize(m_Scene.iNumMaterials);

	_ftscanf_s(pStream, _T("%s"), MaterialCheck, _countof(MaterialCheck));
	//MaterialCheck == #MATERIAL INFO" 검증


	for (int iMtl = 0; iMtl < m_MtrlList.size(); iMtl++)
	{
		static TCHAR texname[MAX_PATH];
		ZeroMemory(texname, _countof(texname));
		int subtexsize = 0;
		_ftscanf_s(pStream, _T("%s %d"),
			texname, _countof(texname),
			&subtexsize);
		m_MtrlList[iMtl].szName = texname;


		if (subtexsize > 0)//서브테스쳐 존재
		{
			m_MtrlList[iMtl].subMtrl.resize(subtexsize);
			for (int iSubMtrl = 0; iSubMtrl < m_MtrlList[iMtl].subMtrl.size(); iSubMtrl++)
			{
				static TCHAR subtexname[MAX_PATH];
				ZeroMemory(subtexname, _countof(subtexname));
				int subtexsize = 0;

				_ftscanf_s(pStream, _T("%s %d"),
					subtexname, _countof(subtexname),
					&subtexsize
				);
				m_MtrlList[iMtl].subMtrl[iSubMtrl].szName = subtexname;
				m_MtrlList[iMtl].subMtrl[iSubMtrl].texList.resize(subtexsize);

				for (int iTex = 0; iTex < m_MtrlList[iMtl].subMtrl[iSubMtrl].texList.size(); iTex++)
				{
					int mapid = 0;
					static TCHAR texfilename[MAX_PATH];
					ZeroMemory(texfilename, _countof(texfilename));
					_ftscanf_s(pStream, _T("%d %s"),
						&mapid,
						texfilename, _countof(texfilename));
					m_MtrlList[iMtl].subMtrl[iSubMtrl].texList[iTex].iMapID = mapid;
					m_MtrlList[iMtl].subMtrl[iSubMtrl].texList[iTex].szName = texfilename;

					T_STR fullpath = L"../../data/bitmap/";
					fullpath += m_MtrlList[iMtl].subMtrl[iSubMtrl].texList[iTex].szName;

					m_MtrlList[iMtl].subMtrl[iSubMtrl].iTexIndex =
						I_TextureMgr.Load(m_pDevice, fullpath);
				}
			}
		}
		else //서브텍스쳐 없음
		{
			static TCHAR subtexname[MAX_PATH];
			ZeroMemory(subtexname, _countof(subtexname));
			int subtexsize = 0;
			_ftscanf_s(pStream, _T("%s %d"),
				subtexname, _countof(subtexname),
				&subtexsize);
			//subtexsize == L"none" 체크 필요
			m_MtrlList[iMtl].szName = subtexname;
			m_MtrlList[iMtl].texList.resize(subtexsize);
			for (int iTex = 0; iTex < m_MtrlList[iMtl].texList.size(); iTex++)
			{
				static TCHAR texlistname[MAX_PATH];
				ZeroMemory(texlistname, _countof(texlistname));
				int MapID = 0;

				_ftscanf_s(pStream, _T("%d %s"),
					&MapID,
					texlistname, _countof(texlistname));

				m_MtrlList[iMtl].texList[iTex].iMapID = MapID;
				m_MtrlList[iMtl].texList[iTex].szName = texlistname;
				T_STR fullpath = L"../../data/bitmap/";
				fullpath += m_MtrlList[iMtl].texList[iTex].szName;

				m_MtrlList[iMtl].iTexIndex =
					I_TextureMgr.Load(m_pDevice, fullpath);
			}
		}
	}


	// mesh list
	static TCHAR meshname[MAX_PATH];
	ZeroMemory(meshname, _countof(meshname));
	_ftscanf_s(pStream, _T("%s"), meshname, _countof(meshname));
	//L"#OBJECT_INFO"일치하는지 검증 필요
	//검증

	for (int iObj = 0; iObj < m_tMeshList.size(); iObj++)
	{
		static TCHAR sname[MAX_PATH];
		ZeroMemory(sname, _countof(sname));
		static TCHAR sparnetname[MAX_PATH];
		ZeroMemory(sparnetname, _countof(sparnetname));
		int buffersize = 0;
		int vertexsize = 0;
		_ftscanf_s(pStream, _T("%s %s %d %d %d"),
			sname, _countof(sname),
			sparnetname, _countof(sparnetname),
			&m_tMeshList[iObj].iMtrlID,
			&m_tMeshList[iObj].ibufferNum,		//5    subobj 수
			&m_tMeshList[iObj].iNumFace);		//4158 전체 vertex 수
		m_tMeshList[iObj].m_szName = sname;
		m_tMeshList[iObj].m_ParentzName = sparnetname;
		m_tMeshList[iObj].vbList.resize(m_tMeshList[iObj].ibufferNum);
		m_tMeshList[iObj].ibList.resize(m_tMeshList[iObj].ibufferNum);
		_ftscanf_s(pStream, _T("%f %f %f %f\n\t%f %f %f %f\n\t%f %f %f %f\n\t%f %f %f %f"),
			&m_tMeshList[iObj].m_matWorld._11,
			&m_tMeshList[iObj].m_matWorld._12,
			&m_tMeshList[iObj].m_matWorld._13,
			&m_tMeshList[iObj].m_matWorld._14,

			&m_tMeshList[iObj].m_matWorld._21,
			&m_tMeshList[iObj].m_matWorld._22,
			&m_tMeshList[iObj].m_matWorld._23,
			&m_tMeshList[iObj].m_matWorld._24,

			&m_tMeshList[iObj].m_matWorld._31,
			&m_tMeshList[iObj].m_matWorld._32,
			&m_tMeshList[iObj].m_matWorld._33,
			&m_tMeshList[iObj].m_matWorld._34,

			&m_tMeshList[iObj].m_matWorld._41,
			&m_tMeshList[iObj].m_matWorld._42,
			&m_tMeshList[iObj].m_matWorld._43,
			&m_tMeshList[iObj].m_matWorld._44);

		//find parent 사실 여기있으면 안됨


		for (auto& othermesh : m_tMeshList)
		{
			if (m_tMeshList[iObj].m_ParentzName == L"none") continue;
			if (othermesh.m_szName == m_tMeshList[iObj].m_ParentzName)
			{
				m_tMeshList[iObj].m_pParent = &othermesh;
				break;
			}
		}



		//여기 어케바꾸지
		D3DXMATRIX matParent, matInvParent;

		D3DXMatrixIdentity(&matParent);
		D3DXMatrixInverse(&matInvParent, NULL, &matParent);
		if (m_tMeshList[iObj].m_pParent == nullptr)
		{
			m_tMeshList[iObj].m_matAnimSelf =
				m_tMeshList[iObj].m_matWorld *matInvParent;
		}
		else
		{
			//몰라1
			m_tMeshList[iObj].m_matAnimSelf =
				m_tMeshList[iObj].m_matWorld *m_tMeshList[iObj].m_pParent->m_matInvWorld;

		}

		D3DXMatrixInverse(
			&m_tMeshList[iObj].m_matInvWorld, NULL,
			&m_tMeshList[iObj].m_matWorld);

		D3DXMatrixDecompose(
			&m_tMeshList[iObj].m_vScaleTrack,
			&m_tMeshList[iObj].m_qRotTrack,
			&m_tMeshList[iObj].m_vPosTrack,
			&m_tMeshList[iObj].m_matAnimSelf);

		D3DXMatrixTranslation(&m_tMeshList[iObj].m_matPos,
			m_tMeshList[iObj].m_vPosTrack.x,
			m_tMeshList[iObj].m_vPosTrack.y,
			m_tMeshList[iObj].m_vPosTrack.z);
		D3DXMatrixScaling(&m_tMeshList[iObj].m_matScl,
			m_tMeshList[iObj].m_vScaleTrack.x,
			m_tMeshList[iObj].m_vScaleTrack.y,
			m_tMeshList[iObj].m_vScaleTrack.z);
		D3DXMatrixRotationQuaternion(
			&m_tMeshList[iObj].m_matRot,
			&m_tMeshList[iObj].m_qRotTrack);







		for (int iSubTri = 0; iSubTri < m_tMeshList[iObj].ibufferNum; iSubTri++)
		{
			vector<PNCT_VERTEX>& vList = m_tMeshList[iObj].vbList[iSubTri];

			static TCHAR verListCheck[MAX_PATH];
			ZeroMemory(verListCheck, _countof(verListCheck));
			int verlistSize = 0;
			_ftscanf_s(pStream, _T("%s %d"),
				verListCheck, _countof(verListCheck),
				&verlistSize);
			vList.resize(verlistSize);
			//verListCheck == nVertexList랑 같은지 검증 필요
			for (int iVer = 0; iVer < vList.size(); iVer++)
			{
				_ftscanf_s(pStream, _T("%f %f %f"),
					&vList[iVer].p.x,
					&vList[iVer].p.y,
					&vList[iVer].p.z);
				_ftscanf_s(pStream, _T("%f %f %f"),
					&vList[iVer].n.x,
					&vList[iVer].n.y,
					&vList[iVer].n.z);
				_ftscanf_s(pStream, _T("%f %f %f %f"),
					&vList[iVer].c.x,
					&vList[iVer].c.y,
					&vList[iVer].c.z,
					&vList[iVer].c.w);
				_ftscanf_s(pStream, _T("%f %f"),
					&vList[iVer].t.x,
					&vList[iVer].t.y);
			}

			vector<DWORD>& iList = m_tMeshList[iObj].ibList[iSubTri];
			static TCHAR indexListCheck[MAX_PATH];
			ZeroMemory(indexListCheck, _countof(indexListCheck));
			int indexlistSize = 0;
			_ftscanf_s(pStream, _T("%s %d"),
				indexListCheck, _countof(indexListCheck),
				&indexlistSize);
			iList.resize(indexlistSize);

			//nIndexList 체크
			for (int iIndex = 0; iIndex < iList.size(); iIndex += 3)
			{
				_ftscanf_s(pStream, _T("%d %d %d"),
					&iList[iIndex + 0],
					&iList[iIndex + 1],
					&iList[iIndex + 2]);
			}
		}

		//animation data

		int posSize = 0;
		int rotSize = 0;
		int scalSize = 0;
		static TCHAR AnimationtCheck[MAX_PATH];
		ZeroMemory(AnimationtCheck, _countof(AnimationtCheck));
		_ftscanf_s(pStream, _T("%s %d %d %d"),
			AnimationtCheck, _countof(AnimationtCheck),
			&posSize,
			&rotSize,
			&scalSize);
		m_tMeshList[iObj].animPos.resize(posSize);
		m_tMeshList[iObj].animRot.resize(rotSize);
		m_tMeshList[iObj].animScl.resize(scalSize);

		for (int aPos = 0; aPos < m_tMeshList[iObj].animPos.size(); aPos++)
		{
			int iTrack = 0;
			_ftscanf_s(pStream, _T("\n%d %d %f %f %f"),
				&iTrack,
				&m_tMeshList[iObj].animPos[aPos].i,
				&m_tMeshList[iObj].animPos[aPos].p.x,
				&m_tMeshList[iObj].animPos[aPos].p.y,
				&m_tMeshList[iObj].animPos[aPos].p.z);
			//iTrack==aPos
		}

		for (int aRot = 0; aRot < m_tMeshList[iObj].animRot.size(); aRot++)
		{
			int iTrack = 0;
			_ftscanf_s(pStream, _T("\n%d %d %f %f %f %f"),
				&iTrack,
				&m_tMeshList[iObj].animRot[aRot].i,
				&m_tMeshList[iObj].animRot[aRot].q.x,
				&m_tMeshList[iObj].animRot[aRot].q.y,
				&m_tMeshList[iObj].animRot[aRot].q.z,
				&m_tMeshList[iObj].animRot[aRot].q.w);
		}

		for (int aScal = 0; aScal < m_tMeshList[iObj].animScl.size(); aScal++)
		{
			int iTrack = 0;
			_ftscanf_s(pStream, _T("\n%d %d %f %f %f %f %f %f %f"),
				&iTrack,
				&m_tMeshList[iObj].animScl[aScal].i,
				&m_tMeshList[iObj].animScl[aScal].p.x,
				&m_tMeshList[iObj].animScl[aScal].p.y,
				&m_tMeshList[iObj].animScl[aScal].p.z,
				&m_tMeshList[iObj].animScl[aScal].q.x,
				&m_tMeshList[iObj].animScl[aScal].q.y,
				&m_tMeshList[iObj].animScl[aScal].q.z,
				&m_tMeshList[iObj].animScl[aScal].q.w);
		}
	}

	fclose(pStream);




	return hr;
}

HRESULT syMaxObject::CreateIndexData()
{
	HRESULT hr = S_OK;
	return hr;
}
HRESULT syMaxObject::CreateVertexBuffer()
{
	HRESULT hr = S_OK;

	for (int iObj = 0; iObj < m_Scene.iNumObjects; iObj++)
	{
		m_tMeshList[iObj].m_renderObjList.resize(m_tMeshList[iObj].ibufferNum);
		for (int iSubObj = 0; iSubObj < m_tMeshList[iObj].ibufferNum; iSubObj++)
		{
			m_tMeshList[iObj].m_renderObjList[iSubObj].m_iVertexSize = sizeof(PNCT_VERTEX);
			m_tMeshList[iObj].m_renderObjList[iSubObj].m_iNumVertex = m_tMeshList[iObj].vbList[iSubObj].size();
			if (m_tMeshList[iObj].m_renderObjList[iSubObj].m_iNumVertex == 0) continue;
			m_tMeshList[iObj].m_renderObjList[iSubObj].m_pVertexBuffer.Attach(
				DX::CreateVertexBuffer(m_pDevice,
					&m_tMeshList[iObj].vbList[iSubObj].at(0),
					m_tMeshList[iObj].m_renderObjList[iSubObj].m_iNumVertex,
					m_tMeshList[iObj].m_renderObjList[iSubObj].m_iVertexSize));

			if (m_tMeshList[iObj].m_renderObjList[iSubObj].m_pVertexBuffer.Get() == nullptr)
				return false;
		}
	}
	return hr;
}
HRESULT syMaxObject::CreateIndexBuffer()
{
	HRESULT hr = S_OK;
	for (int iObj = 0; iObj < m_Scene.iNumObjects; iObj++)
	{
		for (int iSubObj = 0;
			iSubObj < m_tMeshList[iObj].ibufferNum;
			iSubObj++)
		{
			m_tMeshList[iObj].m_renderObjList[iSubObj].m_iNumIndex =
				m_tMeshList[iObj].ibList[iSubObj].size();
			if (m_tMeshList[iObj].m_renderObjList[iSubObj].m_iNumIndex == 0) continue;
			m_tMeshList[iObj].m_renderObjList[iSubObj].m_pIndexBuffer.Attach(
				DX::CreateIndexBuffer(m_pDevice,
					&m_tMeshList[iObj].ibList[iSubObj].at(0),
					m_tMeshList[iObj].ibList[iSubObj].size(),
					sizeof(DWORD))
			);
			if (m_tMeshList[iObj].m_renderObjList[iSubObj].m_pIndexBuffer.Get() == nullptr)
				return false;
		}
	}

	return hr;
}
HRESULT syMaxObject::LoadTextures(ID3D11Device* pd3dDevice, const TCHAR* pLoadTextureFile)
{
	HRESULT hr = S_OK;

	m_iAttribute = I_TextureMgr.Load(m_pDevice, L"../../data/bitmap/att.dds");
	return syModel::LoadTextures(pd3dDevice, pLoadTextureFile);


}
bool   syMaxObject::Frame()
{
	m_fElapseTime += 1.0f* g_fSecondPerFrame *
		m_Scene.iFrameSpeed *
		m_Scene.iTickPerFrame;

	float fEndTime = m_Scene.iLastFrame*
		m_Scene.iTickPerFrame;
	if (m_fElapseTime >= fEndTime)
	{
		m_fElapseTime = 0.0f;
	}
	D3DXMATRIX matParent;
	D3DXMatrixIdentity(&matParent);
	for (int iObj = 0; iObj < m_tMeshList.size(); iObj++)
	{

		if (m_tMeshList[iObj].m_pParent != nullptr)
		{
			Interpolate(m_tMeshList[iObj],
				m_tMeshList[iObj].m_pParent->m_matCalculation,
				m_fElapseTime);
		}
		else
		{
			Interpolate(m_tMeshList[iObj],
				matParent,
				m_fElapseTime);
		}
	}

	return true;
}

void syMaxObject::Interpolate(syMesh& tMesh, D3DXMATRIX &matParent, float fElapseTime)
{
	D3DXMATRIX matAnimScale, matAnimRot, matAnimPos;
	matAnimScale = tMesh.m_matScl;
	matAnimRot = tMesh.m_matRot;
	matAnimPos = tMesh.m_matPos;

	D3DXMatrixIdentity(&tMesh.m_matCalculation);

	syAnimTrack StartTrack, EndTrack;
	D3DXQUATERNION qRotatin;
	D3DXVECTOR3 vTrans;

	if (tMesh.animRot.size() > 0)
	{
		if (GetAnimationTrack(
			fElapseTime,
			tMesh.animRot,
			StartTrack,
			EndTrack))
		{
			float t = (fElapseTime - StartTrack.i) / (EndTrack.i - StartTrack.i);
			D3DXQuaternionSlerp(&qRotatin,
				&StartTrack.q,
				&EndTrack.q, t);
		}
		D3DXMatrixRotationQuaternion(&matAnimRot, &qRotatin);
	}
	if (tMesh.animScl.size() > 0)
	{
		if (GetAnimationTrack(
			fElapseTime,
			tMesh.animScl,
			StartTrack,
			EndTrack))
		{
			float t = (fElapseTime - StartTrack.i) / (EndTrack.i - StartTrack.i);
			D3DXQuaternionSlerp(&qRotatin,
				&StartTrack.q,
				&EndTrack.q, t);
			D3DXVec3Lerp(&vTrans,
				&StartTrack.p,
				&EndTrack.p, t);
		}
		D3DXMATRIX matScaleVector;
		D3DXMATRIX matScaleRotation, matScaleRotInverse;
		D3DXMatrixScaling(&matScaleVector, vTrans.x, vTrans.y, vTrans.z);
		D3DXMatrixRotationQuaternion(&matScaleRotation, &qRotatin);
		D3DXMatrixInverse(&matScaleRotInverse, NULL, &matScaleRotation);
		matAnimScale = matScaleRotInverse * matScaleVector * matScaleRotation;
	}


	if (tMesh.animPos.size() > 0)
	{
		if (GetAnimationTrack(
			fElapseTime, tMesh.animPos, StartTrack, EndTrack))
		{
			float t = (fElapseTime - StartTrack.i) / (EndTrack.i - StartTrack.i);
			D3DXVec3Lerp(&vTrans,
				&StartTrack.p,
				&EndTrack.p, t);
		}
		D3DXMatrixTranslation(&matAnimPos, vTrans.x,
			vTrans.y, vTrans.z);
	}

	D3DXMATRIX matAnim;
	matAnim = matAnimScale * matAnimRot;
	matAnim._41 = matAnimPos._41;
	matAnim._42 = matAnimPos._42;
	matAnim._43 = matAnimPos._43;
	//몰라2

	tMesh.m_matCalculation = matAnim * matParent;
	int i = 0;
}

bool syMaxObject::GetAnimationTrack(float fElapseTime,
	std::vector<syAnimTrack>& trackList,
	syAnimTrack& StartTrack,
	syAnimTrack& EndTrack)
{
	for (auto& track : trackList)
	{
		if (track.i > fElapseTime)
		{
			EndTrack = track;
			break;
		}
		StartTrack = track;
	}
	return true;
}
bool  syMaxObject::Render(ID3D11DeviceContext* pContext)
{
	//if (m_dxObj.m_pSRV == nullptr) return true;
	return syModel::Render(pContext);
}
bool syMaxObject::PreRender(ID3D11DeviceContext* pContext)
{
	m_dxObj.PreRender(pContext, m_dxObj.m_iVertexSize);

	return true;
}

bool  syMaxObject::PostRender(ID3D11DeviceContext* pContext)
{
	for (int iObj = 0; iObj < m_Scene.iNumObjects; iObj++)
	{



		//몰라3
		m_cb.matWorld = m_tMeshList[iObj].m_matInvWorld * m_tMeshList[iObj].m_matCalculation;
		D3DXMatrixTranspose(&m_cb.matWorld,
			&m_cb.matWorld);

		float fTime = g_fGameTimer;
		//	m_cb.matWorld = m_tMeshList[iObj].m_matCalculation;
		m_cb.color[0] = cosf(fTime);
		m_cb.color[1] = sinf(fTime);
		m_cb.color[2] = 1 - cosf(fTime);
		m_cb.color[3] = 1.0f;
		m_cb.etc[0] = fTime;
		pContext->UpdateSubresource(
			m_dxObj.m_pConstantBuffer.Get(),
			0, NULL, &m_cb, 0, 0);
		int dwRootIndex = m_tMeshList[iObj].iMtrlID;
		if (dwRootIndex >= 0 && m_MtrlList[dwRootIndex].subMtrl.size() > 0)
		{

			for (int iSubObj = 0; iSubObj <
				m_tMeshList[iObj].ibufferNum;
				iSubObj++)
			{

				if (dwRootIndex >= 0)
				{
					DWORD dwSubIndex = m_MtrlList[dwRootIndex].subMtrl[iSubObj].iTexIndex;
					if (dwSubIndex >= 0)
					{
						pContext->PSSetShaderResources(0, 1,
							I_TextureMgr.GetPtr(dwSubIndex)->m_pSRV.GetAddressOf());
						pContext->PSSetShaderResources(1, 1,
							I_TextureMgr.GetPtr(m_iAttribute)->m_pSRV.GetAddressOf());

					}
				}
				else
				{
					pContext->PSSetShaderResources(0, 1,
						m_dxObj.m_pSRV.GetAddressOf());
					pContext->PSSetShaderResources(1, 1,
						I_TextureMgr.GetPtr(m_iAttribute)->m_pSRV.GetAddressOf());
				}
				UINT stride =
					m_tMeshList[iObj].m_renderObjList[iSubObj].m_iVertexSize;// m_dxObj.m_iVertexSize;
				UINT offset = 0;
				pContext->IASetVertexBuffers(0, 1,
					m_tMeshList[iObj].m_renderObjList[iSubObj].m_pVertexBuffer.GetAddressOf(),
					&stride, &offset);

				pContext->IASetIndexBuffer(
					m_tMeshList[iObj].m_renderObjList[iSubObj].m_pIndexBuffer.Get(),
					DXGI_FORMAT_R32_UINT, 0);

				pContext->DrawIndexed(
					m_tMeshList[iObj].ibList[iSubObj].size(), 0, 0);

			}
		}
		else if (dwRootIndex >= 0)
		{
			DWORD dwSubIndex = m_MtrlList[dwRootIndex].iTexIndex;
			if (dwSubIndex >= 0)
			{
				pContext->PSSetShaderResources(0, 1,
					I_TextureMgr.GetPtr(dwSubIndex)->m_pSRV.GetAddressOf());
				pContext->PSSetShaderResources(1, 1,
					I_TextureMgr.GetPtr(m_iAttribute)->m_pSRV.GetAddressOf());
			}
			UINT stride =
				m_tMeshList[iObj].m_renderObjList[0].m_iVertexSize;// m_dxObj.m_iVertexSize;
			UINT offset = 0;
			pContext->IASetVertexBuffers(0, 1,
				m_tMeshList[iObj].m_renderObjList[0].m_pVertexBuffer.GetAddressOf(),
				&stride, &offset);

			pContext->IASetIndexBuffer(
				m_tMeshList[iObj].m_renderObjList[0].m_pIndexBuffer.Get(),
				DXGI_FORMAT_R32_UINT, 0);

			pContext->DrawIndexed(
				m_tMeshList[iObj].ibList[0].size(), 0, 0);

		}
	}
	return true;
}

