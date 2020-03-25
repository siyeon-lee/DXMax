#include "syMap.h"
#include "syTextureMgr.h"


syMap::syMap()
{
	m_iNumFace = 0;
	D3DXMatrixIdentity(&m_matWorld);
	D3DXMatrixIdentity(&m_matView);
	D3DXMatrixIdentity(&m_matProj);
	m_iNumCols = 0;
	m_iNumRows = 0;

	m_pNormalLookupTable = nullptr;
	m_pFaceNormals = nullptr;

}

bool	syMap::Release()
{

	return syModel::Release();
}
syMap::~syMap()
{


}

void syMap::SetBGTex(int slot, int texID)
{
	if (texID < 0)  return;
	if (slot > 3) return;
	m_backgroundTex[slot] = I_TextureMgr.GetPtr(texID);
}
void syMap::GenVertexNormal()
{
	InitFaceNormals();
	GenNormalLookupTable();
	CalcPerVertexNormalsFastLookup();
}
void	syMap::GenNormalLookupTable()
{
	// We're going to create a table that lists, for each vertex, the
	// triangles which that vertex is a part of.

	if (m_pNormalLookupTable != NULL)
	{
		free(m_pNormalLookupTable);
		m_pNormalLookupTable = NULL;
	}

	// Each vertex may be a part of up to 6 triangles in the grid, so
	// create a buffer to hold a pointer to the normal of each neighbor.
	int buffersize = (m_iNumCellRows) * (m_iNumCellCols) * 6;
	

	m_pNormalLookupTable = (int *)malloc(sizeof(void *) * buffersize);
	for (int i = 0; i < buffersize; i++)
		m_pNormalLookupTable[i] = -1;

	// Now that the table is initialized, populate it with the triangle data.

	// For each triangle
	//   For each vertex in that triangle
	//     Append the triangle number to lookuptable[vertex]
	for (int i = 0; i < m_iNumFace; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			// Find the next empty slot in the vertex's lookup table "slot"
			for (int k = 0; k < 6; k++)
			{
				int vertex = m_IndexList[i * 3 + j];
				if (m_pNormalLookupTable[vertex * 6 + k] == -1)
				{
					m_pNormalLookupTable[vertex * 6 + k] = i;
					break;
				}
			}
		}  // For each vertex that is part of the current triangle
	} // For each triangle
}
void syMap::CreateTangentSpaceVectors(D3DXVECTOR3 *v0,
	D3DXVECTOR3 *v1, D3DXVECTOR3 *v2, D3DXVECTOR2 uv0,
	D3DXVECTOR2 uv1, D3DXVECTOR2 uv2,
	D3DXVECTOR3 *vTangent, D3DXVECTOR3 *vBiNormal, D3DXVECTOR3 *vNormal)
{
	D3DXVECTOR3 vEdge1 = *v1 - *v0;
	D3DXVECTOR3 vEdge2 = *v2 - *v0;
	D3DXVec3Normalize(&vEdge1, &vEdge1);
	D3DXVec3Normalize(&vEdge2, &vEdge2);
	// UV delta
	D3DXVECTOR2 deltaUV1 = uv1 - uv0;
	D3DXVECTOR2 deltaUV2 = uv2 - uv0;
	D3DXVec2Normalize(&deltaUV1, &deltaUV1);
	D3DXVec2Normalize(&deltaUV2, &deltaUV2);


	D3DXVECTOR3 biNormal;
	float fDet = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
	if (fabsf(fDet) < 1e-6f)
	{
		*vTangent = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
		biNormal = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
	}
	else
	{
		*vTangent = (vEdge1 * deltaUV2.y - vEdge2 * deltaUV1.y)*fDet;
		biNormal = (vEdge2 * deltaUV1.x - vEdge1 * deltaUV2.x)*fDet;
	}
	D3DXVec3Normalize(vTangent, vTangent);
	D3DXVec3Normalize(&biNormal, &biNormal);

	D3DXVec3Cross(vBiNormal, vNormal, vTangent);
	float crossinv = (D3DXVec3Dot(vBiNormal, &biNormal) < 0.0f) ? -1.0f : 1.0f;
	*vBiNormal *= crossinv;
}
void syMap::CreateTangentSpaceVectors(D3DXVECTOR3 *v1,
	D3DXVECTOR3 *v2,
	D3DXVECTOR3 *v3,
	float v1u, float v1v,
	float v2u, float v2v,
	float v3u, float v3v,
	D3DXVECTOR3 *vTangent,
	D3DXVECTOR3 *vBiNormal,
	D3DXVECTOR3 *vNormal)
{
	D3DXVECTOR3 vEdge1 = *v2 - *v1;
	D3DXVECTOR3 vEdge2 = *v3 - *v1;
	float fEdge1_U = v2u - v1u;
	float fEdge1_V = v2v - v1v;
	float fEdge2_U = v3u - v1u;
	float fEdge2_V = v3v - v1v;

	float fDenominator = fEdge1_U * fEdge2_V - fEdge2_U * fEdge1_V;

	if (fDenominator < 0.0001f && fDenominator > -0.0001f)
	{
		*vTangent = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
		*vBiNormal = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		*vNormal = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
	}
	else
	{
		float fScale1 = 1.0f / fDenominator;

		D3DXVECTOR3 T;
		D3DXVECTOR3 B;
		D3DXVECTOR3 N;

		T = D3DXVECTOR3((fEdge2_V * vEdge1.x - fEdge1_V * vEdge2.x) * fScale1,
			(fEdge2_V * vEdge1.y - fEdge1_V * vEdge2.y) * fScale1,
			(fEdge2_V * vEdge1.z - fEdge1_V * vEdge2.z) * fScale1);

		B = D3DXVECTOR3((-fEdge2_U * vEdge1.x + fEdge1_U * vEdge2.x) * fScale1,
			(-fEdge2_U * vEdge1.y + fEdge1_U * vEdge2.y) * fScale1,
			(-fEdge2_U * vEdge1.z + fEdge1_U * vEdge2.z) * fScale1);

		D3DXVec3Cross(&N, &T, &B);

		float fScale2 = 1.0f / ((T.x * B.y * N.z - T.z * B.y * N.x) +
			(B.x * N.y * T.z - B.z * N.y * T.x) +
			(N.x * T.y * B.z - N.z * T.y * B.x));

		D3DXVECTOR3 vTemp;

		(*vTangent).x = D3DXVec3Cross(&vTemp, &B, &N)->x * fScale2;
		(*vTangent).y = -(D3DXVec3Cross(&vTemp, &N, &T)->x * fScale2);
		(*vTangent).z = D3DXVec3Cross(&vTemp, &T, &B)->x * fScale2;
		D3DXVec3Normalize(&(*vTangent), &(*vTangent));

		(*vBiNormal).x = -(D3DXVec3Cross(&vTemp, &B, &N)->y * fScale2);
		(*vBiNormal).y = D3DXVec3Cross(&vTemp, &N, &T)->y * fScale2;
		(*vBiNormal).z = -(D3DXVec3Cross(&vTemp, &T, &B)->y * fScale2);
		D3DXVec3Normalize(&(*vBiNormal), &(*vBiNormal));

		(*vNormal).x = D3DXVec3Cross(&vTemp, &B, &N)->z * fScale2;
		(*vNormal).y = -(D3DXVec3Cross(&vTemp, &N, &T)->z * fScale2);
		(*vNormal).z = D3DXVec3Cross(&vTemp, &T, &B)->z * fScale2;
		D3DXVec3Normalize(&(*vNormal), &(*vNormal));
		//*vBiNormal = *vBiNormal * -1.0f;
	}
}
bool syMap::UpdateBuffer()
{

	////--------------------------------------------------------------------------------------
	//// 접선벡터 계산
	////--------------------------------------------------------------------------------------	
	//D3DXVECTOR3 vTangent, vBiNormal, vNormal;
	//int iIndex = 0;
	//int i0, i1, i2, i3, i4, i5;
	//m_vTangentList.resize(m_dxObj.m_iNumVertex, D3DXVECTOR3(0,0,0));
	//
	//for (int iIndex = 0; iIndex < m_dxObj.m_iNumIndex; iIndex += 3)
	//{
	//	i0 = m_IndexList[iIndex + 0];
	//	i1 = m_IndexList[iIndex + 1];
	//	i2 = m_IndexList[iIndex + 2];

	//	CreateTangentSpaceVectors(&m_VertexList[i0].p, &m_VertexList[i1].p, &m_VertexList[i2].p,
	//		m_VertexList[i0].t, m_VertexList[i1].t, m_VertexList[i2].t,
	//		&vTangent, &vBiNormal, &m_VertexList[i0].n);

	//	m_vTangentList[i0] += vTangent;

	//	i0 = m_IndexList[iIndex + 1];
	//	i1 = m_IndexList[iIndex + 2];
	//	i2 = m_IndexList[iIndex + 0];

	//	CreateTangentSpaceVectors(&m_VertexList[i0].p, &m_VertexList[i1].p, &m_VertexList[i2].p,
	//		m_VertexList[i0].t, m_VertexList[i1].t, m_VertexList[i2].t,
	//		&vTangent, &vBiNormal, &m_VertexList[i0].n);

	//	m_vTangentList[i0] += vTangent;

	//	i0 = m_IndexList[iIndex + 2];
	//	i1 = m_IndexList[iIndex + 0];
	//	i2 = m_IndexList[iIndex + 1];

	//	CreateTangentSpaceVectors(&m_VertexList[i0].p, &m_VertexList[i1].p, &m_VertexList[i2].p,
	//		m_VertexList[i0].t, m_VertexList[i1].t, m_VertexList[i2].t,
	//		&vTangent, &vBiNormal, &m_VertexList[i0].n);

	//	m_vTangentList[i0] += vTangent;

	//}
	//for (int i = 0; i < m_iNumRows*m_iNumCols; i++)
	//{
	//	D3DXVec3Normalize(&m_vTangentList[i], &m_vTangentList[i]);
	//}

	//m_dxObj.m_pVertexBuffer.Attach(DX::CreateVertexBuffer(m_pDevice, &m_VertexList.at(0),
	//	m_dxObj.m_iNumVertex, m_dxObj.m_iVertexSize));
	//UINT iNumVertex = m_vTangentList.size();
	//m_pTangentVB.Attach(DX::CreateVertexBuffer(
	//	m_pDevice,
	//	&m_vTangentList.at(0),
	//	iNumVertex,
	//	sizeof(D3DXVECTOR3)));

	return true;
}

void syMap::CalcPerVertexNormalsFastLookup()
{
	// First, calculate the face normals for each triangle.
	CalcFaceNormals();

	// For each vertex, sum the normals indexed by the normal lookup table.
	int j = 0;
	for (int i = 0; i < m_iNumVertices; i++)
	{
		D3DXVECTOR3 avgNormal;
		avgNormal = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

		// Find all the triangles that this vertex is a part of.
		for (j = 0; j < 6; j++)
		{
			int triIndex;
			triIndex = m_pNormalLookupTable[i * 6 + j];

			// If the triangle index is valid, get the normal and average it in.
			if (triIndex != -1)
			{
				avgNormal += m_FaceNormals[triIndex];
			}
			else
				break;
		}

		// Complete the averaging step by dividing & normalizing.
		_ASSERT(j > 0);
		avgNormal.x /= (float)j;//.DivConst( (float)(j) );
		avgNormal.y /= (float)j;
		avgNormal.z /= (float)j;
		D3DXVec3Normalize(&avgNormal, &avgNormal);

		// Set the vertex normal to this new normal.
		m_VertexList[i].n.x = avgNormal.x;
		m_VertexList[i].n.y = avgNormal.y;
		m_VertexList[i].n.z = avgNormal.z;

	}  // For each vertex


}
void syMap::InitFaceNormals()
{
	m_FaceNormals.resize(m_iNumFace);
	for (int iFace = 0; iFace < m_iNumFace; iFace++)
	{
		m_FaceNormals[iFace] = D3DXVECTOR3(0, 0, 0);
	}
}
void syMap::CalcFaceNormals()
{
	//	m_FaceNormals.resize(m_iNumFace);
	int iFace = 0;
	for (int iIndex = 0; iIndex < m_iNumFace * 3; iIndex += 3)
	{
		DWORD i0 = m_IndexList[iIndex + 0];
		DWORD i1 = m_IndexList[iIndex + 1];
		DWORD i2 = m_IndexList[iIndex + 2];

		m_FaceNormals[iFace] = ComputeFaceNormal(i0, i1, i2);
		iFace++;
	}
}

D3DXVECTOR3	syMap::ComputeFaceNormal(DWORD i0, DWORD i1, DWORD i2)
{
	D3DXVECTOR3 vNormal;
	D3DXVECTOR3 v0, v1, v2;
	v0 = m_VertexList[i0].p;

	v1 = m_VertexList[i1].p;

	v2 = m_VertexList[i2].p;


	D3DXVECTOR3 vEdge0 = v1 - v0;
	D3DXVECTOR3 vEdge1 = v2 - v0;

	D3DXVec3Cross(&vNormal, &vEdge0, &vEdge1);
	D3DXVec3Normalize(&vNormal, &vNormal);
	return vNormal;
}
HRESULT syMap::CreateVertexData()
{
	HRESULT hr = S_OK;
	using namespace DX;

	// 0(-1,1)   1(0,1)    2(1,1) 
	// 3(-1,0)   4(0,0)    5(1,0) 
	// 6(-1,-1)  7(0,-1)   8(1,-1) 

	/*syMapDesc m_MapDesc = md;
	m_iNumRows = md.iNumRows;
	m_iNumCols = md.iNumCols;
	m_iNumCellRows = m_iNumRows * md.iCellNum + 1;
	m_iNumCellCols = m_iNumCols * md.iCellNum + 1;
	m_iNumVertices = m_iNumCellRows * m_iNumCellCols;
	m_iNumFace = (m_iNumCellRows - 1) * (m_iNumCellCols - 1) * 2;
	m_fCellDistance = md.fCellDistance;
	m_fHeightScale = md.fHeightScale;*/
	float fHalfCols = (m_iNumCellCols - 1) / 2.0f;
	float fHalfRows = (m_iNumCellRows - 1) / 2.0f;
	float ftxOffsetU = 1.0f / (m_iCellNum);
	float ftxOffsetV = 1.0f / (m_iCellNum);

	m_VertexList.resize(m_iNumVertices);
	
	for (int iRow = 0; iRow < m_iNumCellRows; iRow++)
	{
	

		for (int iCol = 0; iCol < m_iNumCellCols; iCol++)
		{
		
			
			int iVertexIndex = iRow * (m_iNumCellCols) + iCol;
			m_VertexList[iVertexIndex].p.x = (iCol - fHalfCols)*m_fCellDistance;		//리소스의 픽셀을 맵 위에 올려줘
			m_VertexList[iVertexIndex].p.z = -((iRow - fHalfRows)*m_fCellDistance);
			m_VertexList[iVertexIndex].t.x = iCol * ftxOffsetU;
			m_VertexList[iVertexIndex].t.y = iRow * ftxOffsetV;
			m_VertexList[iVertexIndex].p.y = GetHeightMap(iVertexIndex);
			m_VertexList[iVertexIndex].n = GetNormalMap(iVertexIndex);
			m_VertexList[iVertexIndex].c = GetColorMap(iVertexIndex);

		}
	}
	return hr;
}
HRESULT syMap::SetInputLayout()
{
	HRESULT hr = S_OK;
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0  },
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0  },
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 28, D3D11_INPUT_PER_VERTEX_DATA, 0  },
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 40, D3D11_INPUT_PER_VERTEX_DATA, 0  },
		{"TANGENT", 0,DXGI_FORMAT_R32G32B32_FLOAT, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	int iNumElement = sizeof(layout) / sizeof(layout[0]);
	m_dxObj.m_pInputLayout.Attach(
		DX::CreateInputLayout(m_pDevice,
			m_dxObj.m_pVSBlob->GetBufferSize(),
			m_dxObj.m_pVSBlob->GetBufferPointer(),
			layout, iNumElement));
	return hr;
}

HRESULT syMap::CreateIndexData()
{
	HRESULT hr = S_OK;
	m_IndexList.resize(m_iNumFace * 3);
	// 0(0)   1(1)    2(1,1) 
	// 3(2)   4(3)    5(1,0) 
	// 6()  7()   8(1,-1) 

	


	int iIndex = 0;
	for (int iRow = 0; iRow < m_iNumCellRows -1; iRow++)
	{
		for (int iCol = 0; iCol < m_iNumCellCols -1; iCol++)
		{
			int iNextRow = iRow + 1;
			int iNextCol = iCol + 1;
			m_IndexList[iIndex + 0] = iRow * m_iNumCellCols + iCol;
			m_IndexList[iIndex + 1] = iRow * m_iNumCellCols + iNextCol;
			m_IndexList[iIndex + 2] = iNextRow * m_iNumCellCols + iCol;

			m_IndexList[iIndex + 3] = m_IndexList[iIndex + 2];
			m_IndexList[iIndex + 4] = m_IndexList[iIndex + 1];
			m_IndexList[iIndex + 5] = iNextRow * m_iNumCellCols + iNextCol;

			iIndex += 6;
		}
	}
	GenVertexNormal();
	return hr;
}
HRESULT syMap::CreateIndexBuffer()
{
	HRESULT hr = S_OK;
	m_dxObj.m_iNumIndex = m_iNumFace * 3;//(m_iNumRows - 1)*(m_iNumCols - 1) * 2 * 3
	m_dxObj.m_pIndexBuffer.Attach(DX::CreateIndexBuffer(m_pDevice, &m_IndexList.at(0), m_dxObj.m_iNumIndex, sizeof(DWORD)));
	return hr;
}
bool    syMap::Load(ID3D11Device* pd3dDevice, syMapDesc& md)
{
	syMapDesc m_MapDesc = md;
	m_iNumRows = md.iNumRows;
	m_iNumCols = md.iNumCols;
	m_iCellNum = md.iCellNum;
	m_iNumCellRows = m_iNumRows * md.iCellNum + 1;
	m_iNumCellCols = m_iNumCols * md.iCellNum + 1;
	m_iNumVertices = m_iNumCellRows * m_iNumCellCols;
	m_iNumFace = (m_iNumCellRows-1) * (m_iNumCellCols-1) * 2;
	m_fCellDistance = md.fCellDistance;
	m_fHeightScale = md.fHeightScale;
	//m_iNumRows = md.iNumRows;
	//m_iNumCols = md.iNumCols;
	//m_iNumCellRows = m_iNumRows - 1;
	//m_iNumCellCols = m_iNumCols - 1;
	//m_iNumVertices = m_iNumRows * m_iNumCols;
	//m_iNumFace = m_iNumCellRows * m_iNumCellCols * 2;
	//m_fCellDistance = md.fCellDistance;

	m_iTexIndex = I_TextureMgr.Load(
		pd3dDevice,
		md.szmapTextureFile);
	
	m_pMapTex = I_TextureMgr.GetPtr(m_iTexIndex);

	if (!Create(pd3dDevice,
		m_MapDesc.szShaderFile.c_str(),
		m_MapDesc.szCubeFile.c_str()))
	{
		return false;
	}
	return true;
}

HRESULT syMap::CreateVertexBuffer()
{
	HRESULT hr = S_OK;
	m_dxObj.m_iVertexSize = sizeof(PNCT_VERTEX);
	m_dxObj.m_iNumVertex = m_iNumVertices;
	m_dxObj.m_pVertexBuffer.Attach(DX::CreateVertexBuffer(m_pDevice, &m_VertexList.at(0), m_dxObj.m_iNumVertex, m_dxObj.m_iVertexSize));
	return hr;
}


bool	syMap::Render(ID3D11DeviceContext*	pContext)
{
	float fTime = g_fGameTimer;
//	D3D11_MAPPED_SUBRESOURCE msr;
	//D3DXMatrixIdentity(&m_cb.matWorld);
//	m_cb.matWorld = m_matWorld;
//	m_cb.matView = m_matView;
//	m_cb.matProj = m_matProj;
	m_cb.color[0] = cosf(fTime);
	m_cb.color[1] = sinf(fTime);
	m_cb.color[2] = 1 - cosf(fTime);
	m_cb.color[3] = 1.0f;
	m_cb.etc[0] = fTime;
	pContext->UpdateSubresource(
		m_dxObj.m_pConstantBuffer.Get(),
		0, NULL, &m_cb, 0, 0);

	//if (SUCCEEDED(pContext->Map(
	//	m_dxObj.m_pConstantBuffer.Get(), 0,
	//	D3D11_MAP_WRITE_DISCARD, 0, &msr)))
	//{
	//	VSCB* data = (VSCB*)msr.pData;
	//	data->matWorld = m_matWorld;
	//	data->matView = m_matView;
	//	data->matProj = m_matProj;
	//	data->color[0] = cosf(fTime);
	//	data->color[1] = sinf(fTime);
	//	data->color[2] = 1 - cosf(fTime);
	//	data->color[3] = 1.0f;
	//	data->etc[0] = fTime;
	//	pContext->Unmap(m_dxObj.m_pConstantBuffer.Get(), 0);
	//}

	PreRender(pContext, m_dxObj.m_iVertexSize);
	ID3D11Buffer* Buffer[2] = { m_dxObj.m_pVertexBuffer.Get(), m_pTangentVB.Get() };
	UINT stride[2] = { sizeof(PNCT_VERTEX), sizeof(D3DXVECTOR3) };
	UINT offset[2] = { 0, 0 };
	pContext->IASetVertexBuffers(0, 2, Buffer, stride, offset);


	PostRender(pContext);
	return true;
}
void syMap::PreRender(ID3D11DeviceContext* pContext, UINT iSize)
{
	//obj ; cube : 1번  / m_pMapTex :0번
	pContext->VSSetConstantBuffers(0, 1, m_dxObj.m_pConstantBuffer.GetAddressOf());
	pContext->PSSetShaderResources(0, 1, m_pMapTex->m_pSRV.GetAddressOf());
//	pContext->PSSetShaderResources(1, 1, m_dxObj.m_pSRV.GetAddressOf());

	pContext->IASetInputLayout(m_dxObj.m_pInputLayout.Get());
	pContext->VSSetShader(m_dxObj.m_pVertexShader.Get(), NULL, 0);
	pContext->PSSetShader(m_dxObj.m_pPixelShader.Get(), NULL, 0);

	UINT stride = iSize;
	UINT offset = 0;
	pContext->IASetVertexBuffers(0, 1, m_dxObj.m_pVertexBuffer.GetAddressOf(),
		&stride, &offset);
	pContext->IASetIndexBuffer(m_dxObj.m_pIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);


	pContext->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


}

bool syHeightMap::CreateHeightMap(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pContext, const TCHAR* pMapFile)
{
	D3DX11_IMAGE_LOAD_INFO imageload;
	ZeroMemory(&imageload, sizeof(D3DX10_IMAGE_LOAD_INFO));
	imageload.MipLevels = 1;
	imageload.Usage = D3D11_USAGE_STAGING;	//필수
	imageload.CpuAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;	//이거 필수
	imageload.Format = DXGI_FORMAT_B8G8R8A8_UNORM;


	ID3D11Resource* pMapHeight = NULL;
	D3DX11CreateTextureFromFile(pd3dDevice, pMapFile, &imageload, NULL, &pMapHeight, NULL);

	ID3D11Texture2D* pTex2D = (ID3D11Texture2D*)pMapHeight;	//원래는 쿼리인터페이스 통해서 변환해야함
	D3D11_TEXTURE2D_DESC td;
	pTex2D->GetDesc(&td);

	m_fHeightList.resize(td.Width*td.Height);
	//가로 세로 픽셀을 정점으로 대체 -> 텍스처의 크기와 같은 정점이 만들어 짐
	D3D11_MAPPED_SUBRESOURCE mapsub;
	if (SUCCEEDED(pContext->Map(pTex2D, D3D11CalcSubresource(0, 0, 1), D3D11_MAP_READ, 0, &mapsub)))
	{									//D3D11CalcSubresource : 원하는 밉맵 레벨 조정
		UCHAR* pTexels = (UCHAR*)mapsub.pData;	//메모리 시작 주소값
		for (UINT row = 0; row < td.Height; row++)
		{
			UINT rowStart = row * mapsub.RowPitch;		//RowPitch : 텍스쳐 가로 크기와 지정된 가로 크기 다를 수 있어(압축)
			for (UINT col = 0; col < td.Width; col++)
			{
				UINT uRed = pTexels[rowStart + col * 4];	//4바이트씩 이동하여 1개 픽셀의 RGBA바이트를 얻는다
				m_fHeightList[row*td.Width + col] = uRed;
			}
		}
		pContext->Unmap(pTex2D, D3D11CalcSubresource(0, 0, 1));
	}

	m_iNumRows = td.Height;
	m_iNumCols = td.Width;
	pMapHeight->Release();
	pMapHeight = nullptr;
	return true;
}
