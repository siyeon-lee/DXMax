#pragma once
#include "syStd.h"
class syArcBall
{
public:
	syArcBall();
	~syArcBall();
public:
	float		 m_fSpeed;
	bool		 m_bDrag;
	D3DXVECTOR3  m_vDownPt;
	D3DXVECTOR3  m_vCurrentPt;
	D3DXVECTOR2  m_vDragPt;
	D3DXVECTOR2  m_vDragAngle;
	D3DXVECTOR3  m_vEularAngle;
	D3DXMATRIX   m_matRotation;
public:
	void	OnBegin(int nX, int nY);
	void	OnMove(int nX, int nY);
	void	OnEnd(int nX, int nY);
	D3DXMATRIX GetRotationMatrix();
public:
	INT					m_nWidth;
	INT					m_nHeight;
	FLOAT				m_fRadius;
	D3DXVECTOR2			m_vCenter;
	D3DXQUATERNION		m_qDown;
	D3DXQUATERNION		m_qNow;
	D3DXMATRIX		    m_mRotation;
public:
	bool				Init();
	void				SetWindow(INT nWidth, INT nHeight, FLOAT fRadius = 0.9f);
	D3DXVECTOR3			ScreenToVector(float fScreenPtX, float fScreenPtY);
	static D3DXQUATERNION WINAPI QuatFromBallPoints(const D3DXVECTOR3 &vFrom, const D3DXVECTOR3 &vTo);
};

