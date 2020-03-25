#include "syObject.h"
#include "syTextureMgr.h"
#include "syInput.h"
#include "syTexture.h"

syObject::syObject()
{
	m_fAlpha = 255.0f;
}
syObject::~syObject()
{
}
void syObject::Set(ID3D11Device* pDevice, ID3D11DeviceContext* pImmediateContext, syObjectInfo info)
{
	m_Pos = info.InitPos;
	m_fSpeed = info.fSpeed;
	m_rtObj = info.rtClient;
	SetDrawRect(m_Pos, m_rtObj);

	m_pDevice = pDevice;
	m_pImmediateContext = pImmediateContext;
}




bool syObject::Init()
{

	return true;
}

bool syObject::Frame()
{

	return true;
}

bool syObject::Render(ID3D11DeviceContext*	pContext)
{

	//if (m_pTextureMask != nullptr)
	//{
	//	m_pTextureMask->Draw(m_rtDraw.left, m_rtDraw.top, m_rtObj, SRCAND);
	//	m_pTexture->Draw(m_rtDraw.left, m_rtDraw.top, m_rtObj, SRCINVERT);
	//	m_pTextureMask->Draw(m_rtDraw.left, m_rtDraw.top, m_rtObj, SRCINVERT);
	//}
	//else
	//{
	//	m_pTexture->Draw(m_rtDraw.left, m_rtDraw.top, m_rtObj, SRCCOPY);
	//}
	return syModel::Render(pContext);
}
bool syObject::Release()
{

	return true;
}
void syObject::Move(syPoint pos)
{
	float dx = pos.x - m_Pos.x;
	float dy = pos.y - m_Pos.y;
	float dsize = sqrt(dx*dx + dy * dy);
	if (dsize != 0)
	{
		dx /= dsize;
		dy /= dsize;
	}
	m_Pos.x += dx * m_fSpeed * g_fSecondPerFrame;
	m_Pos.y += dy * m_fSpeed * g_fSecondPerFrame;
	SetPos(m_Pos);
}

//-----------------
bool syObject::Load(ID3D11Device* pDevice, std::wstring filename)
{
	int iKey = I_TextureMgr.Load(pDevice, filename);
	m_pTexture = I_TextureMgr.GetPtr(iKey);
	if (m_pTexture == nullptr) return false;

	T_STR name = m_pTexture->m_szPath;
	name += L"Mask_";
	name += m_pTexture->m_szName;

	iKey = I_TextureMgr.Load(pDevice, name);
	m_pTextureMask = I_TextureMgr.GetPtr(iKey);

	return true;
}
void syObject::SetColliRect(syPoint p, RECT rt)
{
	m_rtColl.left = p.x - (rt.right / 2);
	m_rtColl.top = p.y - (rt.bottom / 2);
	m_rtColl.right = rt.right + m_rtColl.left;
	m_rtColl.bottom = rt.bottom + m_rtColl.top;

	float fDistance = sqrt(
		(m_rtColl.right - m_rtColl.left) * (m_rtColl.right - m_rtColl.left)
		+
		(m_rtColl.bottom - m_rtColl.top) * (m_rtColl.bottom - m_rtColl.top));

	m_Sphere.vCenter = m_Pos;
	m_Sphere.fRadius = fDistance / 2;
}

void syObject::SetDrawRect(syPoint p, RECT rt)
{
	m_rtDraw.left = p.x - (rt.right / 2);
	m_rtDraw.top = p.y - (rt.bottom / 2);
	m_rtDraw.right = rt.right;
	m_rtDraw.bottom = rt.bottom;
	SetColliRect(p, rt);
}



void syObject::Set(syPoint pos, RECT rt, float fSpeed)
{
	m_rtObj = rt;
	m_fSpeed = fSpeed;
	SetPos(pos);

}
void syObject::SetPos(POINT pos)
{
	m_Pos.x = pos.x;
	m_Pos.y = pos.y;
	SetDrawRect(m_Pos, m_rtObj);
}
void syObject::SetPos(syPoint pos)
{
	m_Pos = pos;
	SetDrawRect(m_Pos, m_rtObj);
}
void syObject::SetPos(float x, float y)
{
	m_Pos.x = x;
	m_Pos.y = y;
	SetDrawRect(m_Pos, m_rtObj);
}

