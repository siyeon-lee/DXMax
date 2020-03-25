#include "syModel.h"
#include "syTextureMgr.h"


syModel::syModel()
{
	m_iTexIndex = -1;
	D3DXMatrixIdentity(&m_matWorld);
	D3DXMatrixIdentity(&m_matView);
	D3DXMatrixIdentity(&m_matProj);
	D3DXMatrixIdentity(&m_matLastRot);
}


syModel::~syModel()
{
}


bool  syModel::Load(T_STR filename, ID3D11Device* pd3dDevice, const TCHAR* pLoadShaderFile)
{
	_tfopen_s(&m_pStream, filename.c_str(), _T("rb"));
	if (m_pStream &&
		!Create(pd3dDevice, pLoadShaderFile, nullptr))
	{
		fclose(m_pStream);
		return false;
	}
	fclose(m_pStream);
	return true;
}


bool  syModel::Init()
{

	return true;
}
bool  syModel::Frame()
{
	return true;
}
bool  syModel::PreRender(ID3D11DeviceContext* pContext)
{
	m_dxObj.PreRender(pContext,	m_dxObj.m_iVertexSize);
	return true;
}
bool  syModel::Render(ID3D11DeviceContext* pContext)
{

	float fTime = g_fGameTimer;
	//D3D11_MAPPED_SUBRESOURCE msr;
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
	PreRender(pContext);
	PostRender(pContext);
	return true;
}
bool  syModel::PostRender(ID3D11DeviceContext* pContext)
{
	m_dxObj.PostRender(pContext, m_dxObj.m_iNumIndex);
	return true;
}
bool  syModel::Release()
{
	return true;
}
bool  syModel::Create(ID3D11Device* pd3dDevice, const TCHAR* pLoadShaderFile, const TCHAR* pLoadTextureFile)
{
	m_pDevice = pd3dDevice;
	if (FAILED(LoadShaderFile(pd3dDevice, pLoadShaderFile)))
	{
		return false;
	}
	if (FAILED(SetInputLayout()))
	{
		return false;
	}
	if (FAILED(CreateVertexData()))
	{
		return false;
	}
	if (FAILED(CreateIndexData()))
	{
		return false;
	}
	if (FAILED(CreateVertexBuffer()))
	{
		return false;
	}
	if (FAILED(CreateIndexBuffer()))
	{
		return false;
	}
	if (FAILED(CreateConstantBuffer()))
	{
		return false;
	}
	if (FAILED(LoadTextures(
		pd3dDevice,
		pLoadTextureFile)))
	{
		return false;
	}
	if (!UpdateBuffer())
	{
		return false;
	}
	if (FAILED(CreateResource()))
	{
		return false;
	}

	SetCollisionData();

	return Init();
}
HRESULT syModel::LoadShaderFile(ID3D11Device* pd3dDevice,const TCHAR* pLoadShaderFile)
{
	HRESULT hr = S_OK;
	// pixel shader
	m_dxObj.m_pVertexShader.Attach(
		DX::LoadVertexShaderFile(pd3dDevice,
			pLoadShaderFile,
			m_dxObj.m_pVSBlob.GetAddressOf()));
	m_dxObj.m_pPixelShader.Attach(
		DX::LoadPixelShaderFile(pd3dDevice,
			pLoadShaderFile));

	return hr;
}
HRESULT syModel::SetInputLayout()
{
	HRESULT hr = S_OK;
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0  },
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0  },
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0  },
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 40, D3D11_INPUT_PER_VERTEX_DATA, 0  },
	};
	int iNumElement = sizeof(layout) / sizeof(layout[0]);
	m_dxObj.m_pInputLayout.Attach(
		DX::CreateInputLayout(m_pDevice,
			m_dxObj.m_pVSBlob->GetBufferSize(),
			m_dxObj.m_pVSBlob->GetBufferPointer(),
			layout, iNumElement));
	return hr;
}
HRESULT syModel::CreateVertexData()
{
	HRESULT hr = S_OK;
	return hr;
}
HRESULT syModel::CreateIndexData()
{
	HRESULT hr = S_OK;
	return hr;
}
HRESULT syModel::CreateVertexBuffer()
{
	HRESULT hr = S_OK;
	m_dxObj.m_iVertexSize = sizeof(PNCT_VERTEX);
	m_dxObj.m_iNumVertex = m_VertexList.size();
	m_dxObj.m_pVertexBuffer.Attach(
		DX::CreateVertexBuffer(m_pDevice,
			&m_VertexList.at(0), m_dxObj.m_iNumVertex, m_dxObj.m_iVertexSize)
	);
	if (m_dxObj.m_pVertexBuffer.Get() == nullptr)
		return false;
	return hr;
}
HRESULT syModel::CreateIndexBuffer()
{
	HRESULT hr = S_OK;
	if (m_IndexList.size() <= 0) return S_OK;
	m_dxObj.m_iNumIndex = m_IndexList.size();
	m_dxObj.m_pIndexBuffer.Attach(
		DX::CreateIndexBuffer(m_pDevice,
			&m_IndexList.at(0), m_dxObj.m_iNumIndex, sizeof(DWORD))
	);
	if (m_dxObj.m_pIndexBuffer.Get() == nullptr)
		return false;
	return hr;
}
HRESULT syModel::CreateConstantBuffer()
{
	HRESULT hr = S_OK;

	syMatrix matWorld;
	VSCB vc;
	ZeroMemory(&vc, sizeof(VSCB));
	vc.matWorld = *((D3DXMATRIX*)&matWorld);
	vc.matView = m_matView;
	vc.matProj = m_matProj;
	vc.color[0] = 1.0f;
	vc.etc[0] = 0.0f;

	m_dxObj.m_pConstantBuffer.Attach(
		DX::CreateConstantBuffer(m_pDevice, &vc, 1, sizeof(VSCB), false)
	);

	if (m_dxObj.m_pConstantBuffer.Get() == nullptr)
		return false;

	return hr;
}
HRESULT syModel::LoadTextures(ID3D11Device* pd3dDevice, const TCHAR* pLoadTextureFile)
{
	HRESULT hr = S_OK;
	if (pLoadTextureFile == nullptr) return S_OK;
	m_iTexIndex = I_TextureMgr.Load(
		m_pDevice,
		pLoadTextureFile);

	if (m_iTexIndex >= 0)
	{
		m_dxObj.m_pSRV = I_TextureMgr.GetPtr(m_iTexIndex)->m_pSRV;
	}
	return hr;
}
bool   syModel::UpdateBuffer()
{
	return true;
}
HRESULT syModel::CreateResource()
{
	HRESULT hr = S_OK;
	return hr;
}
void syModel::SetCollisionData()
{
	m_syBox.vCenter.x = 0.0f;
	m_syBox.vCenter.y = 0.0f;
	m_syBox.vCenter.z = 0.0f;
	m_syBox.vMin = D3DXVECTOR3(-1, -1, -1);
	m_syBox.vMax = D3DXVECTOR3(1, 1, 1);
	  
	m_syBox.vAxis[0] = D3DXVECTOR3(1, 0, 0);
	m_syBox.vAxis[1] = D3DXVECTOR3(0, 1, 0);
	m_syBox.vAxis[2] = D3DXVECTOR3(0, 0, 1);
	m_syBox.fExtent[0] = 1.0f;
	m_syBox.fExtent[1] = 1.0f;
	m_syBox.fExtent[2] = 1.0f;
	m_sySphere.vCenter = m_syBox.vCenter;
	D3DXVECTOR3 v = m_syBox.vMax - m_syBox.vCenter;
	m_sySphere.fRadius = sqrtf(v.x*v.x + v.y*v.y + v.z*v.z);
}

void  syModel::SetMatrix(D3DXMATRIX* pWorld,	D3DXMATRIX* pView,	D3DXMATRIX* pProj)
{
	if (pWorld != nullptr)
	{
		m_matWorld = *pWorld;
	//	D3DXMatrixTranspose(&m_matWorld, &m_matWorld);
	}
	if (pView != nullptr)
	{
		m_matView = *pView;
	//	D3DXMatrixTranspose(&m_matView, &m_matView);
	}
	if (pProj != nullptr)
	{
		m_matProj = *pProj;
	//	D3DXMatrixTranspose(&m_matProj, &m_matProj);
	}
	D3DXMatrixTranspose(&m_cb.matWorld, &m_matWorld);
	D3DXMatrixTranspose(&m_cb.matView, &m_matView);
	D3DXMatrixTranspose(&m_cb.matProj, &m_matProj);
	
	m_syBox.vCenter.x = m_matWorld._41;
	m_syBox.vCenter.y = m_matWorld._42;
	m_syBox.vCenter.z = m_matWorld._43;
	m_sySphere.vCenter = m_syBox.vCenter;
}