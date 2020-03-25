#include "syBoneObj.h"


HRESULT syBoneObj::CreateVertexData()
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
	//(필요) MatrixExp100인지 검증작업 필요 

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

		_ftscanf_s(pStream, _T("%s %s %d %f %f %f %f %f %f"),
			sname, _countof(sname),
			sparnetname,_countof(sparnetname),
			&m_tMeshList[iObj].iNumFace,
			&m_tMeshList[iObj].m_box.vMax.x,
			&m_tMeshList[iObj].m_box.vMax.y,
			&m_tMeshList[iObj].m_box.vMax.z,
			&m_tMeshList[iObj].m_box.vMin.x,
			&m_tMeshList[iObj].m_box.vMin.y,
			&m_tMeshList[iObj].m_box.vMin.z);
		m_tMeshList[iObj].m_szName = sname;
		m_tMeshList[iObj].m_ParentzName = sparnetname;



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


		for (auto& othermesh : m_tMeshList)
		{
			if (m_tMeshList[iObj].m_ParentzName == L"none") continue;
			if (othermesh.m_szName == m_tMeshList[iObj].m_ParentzName)
			{
				m_tMeshList[iObj].m_pParent = &othermesh;
				break;
			}
		}


		//find parent 
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

			
		if (m_tMeshList[iObj].iNumFace > 0)
		{
			m_tMeshList[iObj].vbList.resize(1);
			vector<PNCT_VERTEX>& vList = m_tMeshList[iObj].vbList[0];

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
			m_tMeshList[iObj].ibList.resize(1);
			vector<DWORD>& iList = m_tMeshList[iObj].ibList[0];
			static TCHAR indexListCheck[MAX_PATH];
			ZeroMemory(indexListCheck, _countof(indexListCheck));
			int indexlistSize = 0;
			_ftscanf_s(pStream, _T("%s %d"),
				indexListCheck, _countof(indexListCheck),
				&indexlistSize);
			iList.resize(indexlistSize);

			//indexListCheck == nIndexList 검증 체크
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
		//AnimationtCheck == #AnimationData 검증 체크
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

HRESULT syBoneObj::CreateIndexData()
{
	HRESULT hr = S_OK;
	return hr;
}
HRESULT syBoneObj::CreateVertexBuffer()
{
	HRESULT hr = S_OK;

	for (int iObj = 0; iObj < m_Scene.iNumObjects; iObj++)
	{
		if (m_tMeshList[iObj].iNumFace <= 0) continue;

		m_tMeshList[iObj].m_renderObjList.resize(1);
		for (int iSubObj = 0; iSubObj < 1; iSubObj++)
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
HRESULT syBoneObj::CreateIndexBuffer()
{
	HRESULT hr = S_OK;
	for (int iObj = 0; iObj < m_Scene.iNumObjects; iObj++)
	{
		if (m_tMeshList[iObj].iNumFace <= 0) continue;
		for (int iSubObj = 0; iSubObj < 1; iSubObj++)
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
HRESULT syBoneObj::LoadTextures(ID3D11Device* pd3dDevice, const TCHAR* pLoadTextureFile)
{
	HRESULT hr = S_OK;

	m_iAttribute = I_TextureMgr.Load(m_pDevice, L"../../data/bitmap/att.dds");
	return syModel::LoadTextures(pd3dDevice, pLoadTextureFile);


}
bool   syBoneObj::Frame()
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
	m_calList.clear();
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
		if (m_tMeshList[iObj].m_szName == L"Bip01_R_Hand")
		{
			m_knifecalList = m_tMeshList[iObj].m_matCalculation;
		}
		m_calList.push_back(m_tMeshList[iObj].m_matCalculation);
	}

	return true;
}

void syBoneObj::Interpolate(syMesh& tMesh, D3DXMATRIX &matParent, float fElapseTime)
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

bool syBoneObj::GetAnimationTrack(float fElapseTime,
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
//bool  syBoneObj::Render(ID3D11DeviceContext* pContext)
//{
//	//if (m_dxObj.m_pSRV == nullptr) return true;
//	return syModel::Render(pContext);
//}
bool syBoneObj::PreRender(ID3D11DeviceContext* pContext)
{
	m_dxObj.PreRender(pContext, m_dxObj.m_iVertexSize);

	return true;
}

bool  syBoneObj::PostRender(ID3D11DeviceContext* pContext)
{
	for (int iObj = 0; iObj < m_Scene.iNumObjects; iObj++)
	{
		/*int dwRootIndex = m_tMeshList[iObj].iMtrlID;
		if (dwRootIndex < 0) continue;*/

		//몰라3
		m_cb.matWorld = m_tMeshList[iObj].m_matInvWorld * m_tMeshList[iObj].m_matCalculation;
		D3DXMatrixTranspose(&m_cb.matWorld,	&m_cb.matWorld);

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
		if (m_tMeshList[iObj].iNumFace > 0)
		{
			pContext->PSSetShaderResources(0, 1,
				I_TextureMgr.GetPtr(m_iAttribute)->m_pSRV.GetAddressOf());
			pContext->PSSetShaderResources(1, 1,
				I_TextureMgr.GetPtr(m_iAttribute)->m_pSRV.GetAddressOf());

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

