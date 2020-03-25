#include "syFrustum.h"

bool SY_PLANE::CreatePlane(D3DXVECTOR3 v0, D3DXVECTOR3 v1, D3DXVECTOR3 v2)
{
	D3DXVECTOR3 vEdge0 = v1 - v0;
	D3DXVECTOR3 vEdge1 = v2 - v0;
	D3DXVECTOR3 vNormal;
	D3DXVec3Cross(&vNormal, &vEdge0, &vEdge1);
	D3DXVec3Normalize(&vNormal, &vNormal);
	fA = vNormal.x;
	fB = vNormal.y;
	fC = vNormal.z;
	fD = -(fA * v0.x + fB * v0.y + fC * v0.z);//평면의방정식
	return true;
}
bool SY_PLANE::CreatePlane(D3DXVECTOR3 vNormal,	D3DXVECTOR3 v0)
{
	D3DXVec3Normalize(&vNormal, &vNormal);
	fA = vNormal.x;
	fB = vNormal.y;
	fC = vNormal.z;
	fD = -(fA*v0.x + fB * v0.y + fC * v0.z);
	return true;
}
void SY_PLANE::Normalize()
{
	float fMag = sqrtf(fA * fA + fB * fB + fC * fC);
	fA = fA / fMag;
	fB = fB / fMag;
	fC = fC / fMag;
	fD = fD / fMag;
	
}

syFrustum::syFrustum()
{
	D3DXMatrixIdentity(&m_matWorld);
	D3DXMatrixIdentity(&m_matView);
	D3DXMatrixIdentity(&m_matProj);
	m_NearCenter = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
}


syFrustum::~syFrustum()
{
}



bool  syFrustum::PreRender(ID3D11DeviceContext* pContext, UINT iSize)
{
	m_FrustumBoxObj.m_VertexList[0] =
		PNCT_VERTEX(
			D3DXVECTOR3(m_vFrustum[1].x, m_vFrustum[1].y, m_vFrustum[1].z),
			D3DXVECTOR3(0.0f, 0.0f, -1.0f), D3DXVECTOR4(1.0f, 0.0f, 0.0f, 0.2f), D3DXVECTOR2(0.0f, 0.0f));
	m_FrustumBoxObj.m_VertexList[1] =
		PNCT_VERTEX(
			D3DXVECTOR3(m_vFrustum[2].x, m_vFrustum[2].y, m_vFrustum[2].z),
			D3DXVECTOR3(0.0f, 0.0f, -1.0f), D3DXVECTOR4(1.0f, 0.0f, 0.0f, 0.2f), D3DXVECTOR2(1.0f, 0.0f));
	m_FrustumBoxObj.m_VertexList[2] =
		PNCT_VERTEX(
			D3DXVECTOR3(m_vFrustum[3].x, m_vFrustum[3].y, m_vFrustum[3].z),
			D3DXVECTOR3(0.0f, 0.0f, -1.0f), D3DXVECTOR4(1.0f, 0.0f, 0.0f, 0.2f), D3DXVECTOR2(1.0f, 1.0f));
	m_FrustumBoxObj.m_VertexList[3] =
		PNCT_VERTEX(
			D3DXVECTOR3(m_vFrustum[0].x, m_vFrustum[0].y, m_vFrustum[0].z),
			D3DXVECTOR3(0.0f, 0.0f, -1.0f), D3DXVECTOR4(1.0f, 0.0f, 0.0f, 0.2f), D3DXVECTOR2(0.0f, 1.0f));
	// 뒷면
	m_FrustumBoxObj.m_VertexList[4] =
		PNCT_VERTEX(
			D3DXVECTOR3(m_vFrustum[6].x, m_vFrustum[6].y, m_vFrustum[6].z),
			D3DXVECTOR3(0.0f, 0.0f, 1.0f), D3DXVECTOR4(0.0f, 0.0f, 0.0f, 0.2f), D3DXVECTOR2(0.0f, 0.0f));
	m_FrustumBoxObj.m_VertexList[5] =
		PNCT_VERTEX(
			D3DXVECTOR3(m_vFrustum[5].x, m_vFrustum[5].y, m_vFrustum[5].z),
			D3DXVECTOR3(0.0f, 0.0f, 1.0f), D3DXVECTOR4(0.0f, 1.0f, 0.0f, 0.2f), D3DXVECTOR2(1.0f, 0.0f));
	m_FrustumBoxObj.m_VertexList[6] =
		PNCT_VERTEX(
			D3DXVECTOR3(m_vFrustum[4].x, m_vFrustum[4].y, m_vFrustum[4].z),
			D3DXVECTOR3(0.0f, 0.0f, 1.0f), D3DXVECTOR4(0.0f, 1.0f, 0.0f, 0.2f), D3DXVECTOR2(1.0f, 1.0f));
	m_FrustumBoxObj.m_VertexList[7] =
		PNCT_VERTEX(
			D3DXVECTOR3(m_vFrustum[7].x, m_vFrustum[7].y, m_vFrustum[7].z),
			D3DXVECTOR3(0.0f, 0.0f, 1.0f), D3DXVECTOR4(0.0f, 1.0f, 0.0f, 0.2f), D3DXVECTOR2(0.0f, 1.0f));

	// 오른쪽
	m_FrustumBoxObj.m_VertexList[8] =
		PNCT_VERTEX(
			D3DXVECTOR3(m_vFrustum[5].x, m_vFrustum[5].y, m_vFrustum[5].z), D3DXVECTOR3(1.0f, 0.0f, 0.0f), D3DXVECTOR4(0.0f, 0.0f, 1.0f, 0.7f), D3DXVECTOR2(0.0f, 0.0f));
	m_FrustumBoxObj.m_VertexList[9] =
		PNCT_VERTEX(
			D3DXVECTOR3(m_vFrustum[1].x, m_vFrustum[1].y, m_vFrustum[1].z), D3DXVECTOR3(1.0f, 0.0f, 0.0f), D3DXVECTOR4(0.0f, 0.0f, 1.0f, 0.7f), D3DXVECTOR2(1.0f, 0.0f));
	m_FrustumBoxObj.m_VertexList[10] =
		PNCT_VERTEX(
			D3DXVECTOR3(m_vFrustum[0].x, m_vFrustum[0].y, m_vFrustum[0].z), D3DXVECTOR3(1.0f, 0.0f, 0.0f), D3DXVECTOR4(0.0f, 0.0f, 1.0f, 0.7f), D3DXVECTOR2(1.0f, 1.0f));
	m_FrustumBoxObj.m_VertexList[11] =
		PNCT_VERTEX(
			D3DXVECTOR3(m_vFrustum[4].x, m_vFrustum[4].y, m_vFrustum[4].z), D3DXVECTOR3(1.0f, 0.0f, 0.0f), D3DXVECTOR4(0.0f, 0.0f, 1.0f, 0.7f), D3DXVECTOR2(0.0f, 1.0f));

	// 왼쪽
	m_FrustumBoxObj.m_VertexList[12] =
		PNCT_VERTEX(
			D3DXVECTOR3(m_vFrustum[2].x, m_vFrustum[2].y, m_vFrustum[2].z), D3DXVECTOR3(-1.0f, 0.0f, 0.0f), D3DXVECTOR4(1.0f, 1.0f, 0.0f, 0.7f), D3DXVECTOR2(0.0f, 0.0f));
	m_FrustumBoxObj.m_VertexList[13] =
		PNCT_VERTEX(
			D3DXVECTOR3(m_vFrustum[6].x, m_vFrustum[6].y, m_vFrustum[6].z), D3DXVECTOR3(-1.0f, 0.0f, 0.0f), D3DXVECTOR4(1.0f, 1.0f, 0.0f, 0.7f), D3DXVECTOR2(1.0f, 0.0f));
	m_FrustumBoxObj.m_VertexList[14] =
		PNCT_VERTEX(
			D3DXVECTOR3(m_vFrustum[7].x, m_vFrustum[7].y, m_vFrustum[7].z), D3DXVECTOR3(-1.0f, 0.0f, 0.0f), D3DXVECTOR4(1.0f, 1.0f, 0.0f, 0.7f), D3DXVECTOR2(1.0f, 1.0f));
	m_FrustumBoxObj.m_VertexList[15] =
		PNCT_VERTEX(
			D3DXVECTOR3(m_vFrustum[3].x, m_vFrustum[3].y, m_vFrustum[3].z), D3DXVECTOR3(-1.0f, 0.0f, 0.0f), D3DXVECTOR4(1.0f, 1.0f, 0.0f, 0.7f), D3DXVECTOR2(0.0f, 1.0f));

	// 윗면
	m_FrustumBoxObj.m_VertexList[16] =
		PNCT_VERTEX(
			D3DXVECTOR3(m_vFrustum[5].x, m_vFrustum[5].y, m_vFrustum[5].z), D3DXVECTOR3(0.0f, 1.0f, 0.0f), D3DXVECTOR4(1.0f, 0.5f, 1.0f, 0.7f), D3DXVECTOR2(0.0f, 0.0f));
	m_FrustumBoxObj.m_VertexList[17] =
		PNCT_VERTEX(
			D3DXVECTOR3(m_vFrustum[6].x, m_vFrustum[6].y, m_vFrustum[6].z), D3DXVECTOR3(0.0f, 1.0f, 0.0f), D3DXVECTOR4(1.0f, 0.5f, 1.0f, 0.7f), D3DXVECTOR2(1.0f, 0.0f));
	m_FrustumBoxObj.m_VertexList[18] =
		PNCT_VERTEX(
			D3DXVECTOR3(m_vFrustum[2].x, m_vFrustum[2].y, m_vFrustum[2].z), D3DXVECTOR3(0.0f, 1.0f, 0.0f), D3DXVECTOR4(1.0f, 0.5f, 1.0f, 0.7f), D3DXVECTOR2(1.0f, 1.0f));
	m_FrustumBoxObj.m_VertexList[19] =
		PNCT_VERTEX(
			D3DXVECTOR3(m_vFrustum[1].x, m_vFrustum[1].y, m_vFrustum[1].z), D3DXVECTOR3(0.0f, 1.0f, 0.0f), D3DXVECTOR4(1.0f, 0.5f, 1.0f, 0.7f), D3DXVECTOR2(0.0f, 1.0f));

	// 아랫면
	m_FrustumBoxObj.m_VertexList[20] =
		PNCT_VERTEX(
			D3DXVECTOR3(m_vFrustum[0].x, m_vFrustum[0].y, m_vFrustum[0].z), D3DXVECTOR3(0.0f, -1.0f, 0.0f), D3DXVECTOR4(0.0f, 1.0f, 1.0f, 0.7f), D3DXVECTOR2(0.0f, 0.0f));
	m_FrustumBoxObj.m_VertexList[21] =
		PNCT_VERTEX(
			D3DXVECTOR3(m_vFrustum[3].x, m_vFrustum[3].y, m_vFrustum[3].z), D3DXVECTOR3(0.0f, -1.0f, 0.0f), D3DXVECTOR4(0.0f, 1.0f, 1.0f, 0.2f), D3DXVECTOR2(1.0f, 0.0f));
	m_FrustumBoxObj.m_VertexList[22] =
		PNCT_VERTEX(
			D3DXVECTOR3(m_vFrustum[7].x, m_vFrustum[7].y, m_vFrustum[7].z), D3DXVECTOR3(0.0f, -1.0f, 0.0f), D3DXVECTOR4(0.0f, 1.0f, 1.0f, 0.2f), D3DXVECTOR2(1.0f, 1.0f));
	m_FrustumBoxObj.m_VertexList[23] =
		PNCT_VERTEX(
			D3DXVECTOR3(m_vFrustum[4].x, m_vFrustum[4].y, m_vFrustum[4].z), D3DXVECTOR3(0.0f, -1.0f, 0.0f), D3DXVECTOR4(0.0f, 1.0f, 1.0f, 0.2f), D3DXVECTOR2(0.0f, 1.0f));

	pContext->UpdateSubresource(m_FrustumBoxObj.m_dxObj.m_pVertexBuffer.Get(), 0, NULL, &m_FrustumBoxObj.m_VertexList.at(0), 0, 0);
	m_FrustumBoxObj.m_dxObj.PreRender(pContext, iSize);
	return true;
}
bool  syFrustum::Render(ID3D11DeviceContext*    pContext)
{
	/*DX::ApplyBS(pContext, DX::syDxState::m_pAlphaBlend);
	DX::ApplyDSS(pContext, DX::syDxState::m_pDSSDepthEnable);
*/

	pContext->UpdateSubresource(m_FrustumBoxObj.m_dxObj.m_pConstantBuffer.Get(), 0, NULL, &m_FrustumBoxObj.m_cb, 0, 0);
	PreRender(pContext, m_FrustumBoxObj.m_dxObj.m_iVertexSize);
	pContext->PSSetShader(m_pPixelShader.Get(), NULL, 0);
	PostRender(pContext);
	//DX::ApplyBS(pContext, DX::syDxState::m_pAlphaBlendDisable);
	//DX::ApplyDSS(pContext, DX::syDxState::m_pDSSDepthDisable);

	return true;
}
bool  syFrustum::PostRender(ID3D11DeviceContext*    pContext)
{
	m_FrustumBoxObj.m_dxObj.PostRender(pContext, m_FrustumBoxObj.m_dxObj.m_iNumIndex);
	return true;
}
bool  syFrustum::CreateFrustumObject(ID3D11Device* pd3dDevice)
{
	if (!m_FrustumBoxObj.Create(pd3dDevice, L"../../data/shader/shader.hlsl", L"../../data/bitmap/mario.bmp"))
	{
		return false;
	}
	m_pPixelShader.Attach(DX::LoadPixelShaderFile(pd3dDevice, L"../../data/shader/shader.hlsl", NULL, "PS_Color"));
	return true;
}
void  syFrustum::CreateFrustum(D3DXMATRIX& matView, D3DXMATRIX& matProj)
{
	m_matView = matView;
	m_matProj = matProj;
	D3DXMATRIX matInvVP = matView * matProj;
	D3DXMatrixInverse(&matInvVP, NULL, &matInvVP);

	// 5   6
	// 4   7

	// 1   2
	// 0   3
	m_vFrustum[0] = D3DXVECTOR3(-1.0f, -1.0f, 0.0f);
	m_vFrustum[1] = D3DXVECTOR3(-1.0f, 1.0f, 0.0f);
	m_vFrustum[2] = D3DXVECTOR3(1.0f, 1.0f, 0.0f);
	m_vFrustum[3] = D3DXVECTOR3(1.0f, -1.0f, 0.0f);

	m_vFrustum[4] = D3DXVECTOR3(-1.0f, -1.0f, 1.0f);
	m_vFrustum[5] = D3DXVECTOR3(-1.0f, 1.0f, 1.0f);
	m_vFrustum[6] = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
	m_vFrustum[7] = D3DXVECTOR3(1.0f, -1.0f, 1.0f);

	for (int iVer = 0; iVer < 8; iVer++)
	{
		D3DXVec3TransformCoord(
			&m_vFrustum[iVer],
			&m_vFrustum[iVer],
			&matInvVP);
	}
	m_Plane[0].CreatePlane(m_vFrustum[5], m_vFrustum[0], m_vFrustum[1]);	// 좌 평면(left)
	m_Plane[1].CreatePlane(m_vFrustum[3], m_vFrustum[6], m_vFrustum[2]);	// 우 평면(right)
	m_Plane[2].CreatePlane(m_vFrustum[5], m_vFrustum[2], m_vFrustum[6]);	// 상 평면(top)
	m_Plane[3].CreatePlane(m_vFrustum[0], m_vFrustum[7], m_vFrustum[3]);	// 하 평면(bottom)
	m_Plane[4].CreatePlane(m_vFrustum[0], m_vFrustum[2], m_vFrustum[1]);	// 근 평면(near)
	m_Plane[5].CreatePlane(m_vFrustum[6], m_vFrustum[4], m_vFrustum[5]);	// 원 평면(far)

}

bool  syFrustum::CheckPoint(D3DXVECTOR3& v)
{
	for (int iPlane = 0; iPlane < 6; iPlane++)
	{
		float fDistance = m_Plane[iPlane].fA* v.x +
			m_Plane[iPlane].fB* v.y +
			m_Plane[iPlane].fC* v.z +
			m_Plane[iPlane].fD;
		if (fDistance < 0.0f)
		{
			return false;
		}

	}
	return true;
}
bool  syFrustum::CheckOBB(SY_BOX& vBox)
{
	float fPlaneToCenter = 0;
	float fDistance = 0;
	D3DXVECTOR3 vDir;
	D3DXVECTOR3 vNormal;
	for (int i = 0; i < 6; i++)
	{
		vDir = vBox.vAxis[0] * vBox.fExtent[0];
		vNormal = D3DXVECTOR3(m_Plane[i].fA,
			m_Plane[i].fB,
			m_Plane[i].fC);

		fDistance = fabs(D3DXVec3Dot(&vNormal, &vDir));
		vDir = vBox.vAxis[1] * vBox.fExtent[1];
		fDistance += fabs(D3DXVec3Dot(&vNormal, &vDir));


		vDir = vBox.vAxis[2] * vBox.fExtent[2];
		fDistance += fabs(D3DXVec3Dot(&vNormal, &vDir));

		fPlaneToCenter =
			m_Plane[i].fA * vBox.vCenter.x +
			m_Plane[i].fB * vBox.vCenter.y +
			m_Plane[i].fC * vBox.vCenter.z +
			m_Plane[i].fD;

		if (fPlaneToCenter <= -fDistance)
		{
			return false;
		}
	}
	return true;
}
SY_POSITION  syFrustum::CheckPositionOBB(SY_BOX& vBox)
{
	float fPlaneToCenter = 0;
	float fDistance = 0;
	D3DXVECTOR3 vDir;
	D3DXVECTOR3 vNormal;

	SY_POSITION t_pos = P_FRONT;
	for (int i = 0; i < 6; i++)
	{
		vDir = vBox.vAxis[0] * vBox.fExtent[0];
		vNormal = D3DXVECTOR3(m_Plane[i].fA,
			m_Plane[i].fB,
			m_Plane[i].fC);

		fDistance = fabs(D3DXVec3Dot(&vNormal, &vDir));
		vDir = vBox.vAxis[1] * vBox.fExtent[1];
		fDistance += fabs(D3DXVec3Dot(&vNormal, &vDir));


		vDir = vBox.vAxis[2] * vBox.fExtent[2];
		fDistance += fabs(D3DXVec3Dot(&vNormal, &vDir));

		fPlaneToCenter =
			m_Plane[i].fA * vBox.vCenter.x +
			m_Plane[i].fB * vBox.vCenter.y +
			m_Plane[i].fC * vBox.vCenter.z +
			m_Plane[i].fD;

		if (fPlaneToCenter <= fDistance)
		{
			t_pos = P_SPANNING;
		}
		if (fPlaneToCenter < -fDistance)
		{
			return P_BACK;
		}
	}
	return t_pos;
}
bool  syFrustum::CheckSphere(SY_SPHERE& vSphere)
{
	return true;
}