#pragma once
#include "syDxHelperEx.h"
#include "syTexture.h"
#include "syCollision.h"
#include "syModel.h"

struct syObjectInfo
{
	syPoint		InitPos;
	int			iNumFrame;
	float		fTime;
	float		fLifeTime;
	int			iTextureID;
	int			iMaskID;
	bool		bLoop;
	RECT		rtClient;
	float		fSpeed;

	std::vector<RECT> rtList;
	syObjectInfo() {
		iTextureID = -1;
		iMaskID = -1;
		bLoop = false;
	}
	syObjectInfo(syPoint p, RECT r, float f)
	{
		InitPos = p;
		rtClient = r;
		fSpeed = f;
		iTextureID = -1;
		iMaskID = -1;
		bLoop = false;
	}
};


class syObject : public syModel
{
public:
	syObject();
	virtual ~syObject();
public:
	DX::syDxHelperEx		m_dxObj;
	syObjectInfo			m_ObjectInfo;
	vector<UINT>			m_iTextureList;
	//ID3D11Device*			m_pDevice;
	//ID3D11DeviceContext*	m_pImmediateContext;
protected:
	syTexture*   m_pTexture;
	syTexture*   m_pTextureMask;
	syPoint		m_Pos;
	float		m_fSpeed;
	RECT        m_rtDraw;
	float		m_fAlpha;
public:
	RECT		m_rtObj;
	RECT        m_rtColl;
	sySphere	m_Sphere;
public:
	syTexture*  GetTexture() { return m_pTexture; }
	syTexture*  GetTextureMask() { return m_pTextureMask; }
	RECT		GetDrawRect() { return m_rtDraw; }
	RECT		GetCollRect() { return m_rtColl; }
	syPoint		GetPos() { return m_Pos; }
	float       GetSpeed() { return m_fSpeed; }
	void		SetSpeed(float sp) { m_fSpeed = sp; }
	void		SetPos(POINT pos);
	void		SetPos(syPoint pos);
	void		SetPos(float x, float y);
	void		Move(syPoint pos); 
public:
	/*virtual */void		SetDrawRect(syPoint p, RECT rt);
	/*virtual */void		SetColliRect(syPoint p, RECT rt);
	virtual void		Set(syPoint pos, RECT rt, float fSpeed = 100.0f);

public:
	virtual void Set(ID3D11Device* pDevice, ID3D11DeviceContext*	pImmediateContext, syObjectInfo info);
	virtual bool Load(ID3D11Device* pDevice, std::wstring filename);
	virtual bool Init();
	virtual bool Frame();
	virtual bool Render(ID3D11DeviceContext* pContext);
	virtual bool Release();

};

