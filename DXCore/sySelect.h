#pragma once
#include "syStd.h"
struct SY_Ray
{
	D3DXVECTOR3 vOrigin;
	D3DXVECTOR3 vDirection;
	float		t;
};
class sySelect
{
public:
	sySelect();
	virtual ~sySelect();
public:
	D3DXMATRIX m_matWorld;
	D3DXMATRIX m_matView;
	D3DXMATRIX m_matProj;
public:
	SY_Ray      m_Ray;
	D3DXVECTOR3 m_vIntersection;
public:
	void  Update();
	void SetMatrix(D3DXMATRIX* matWorld,
		D3DXMATRIX* matView,
		D3DXMATRIX* matProj);
	bool  IntersectTriangle(
		D3DXVECTOR3& vOrig, D3DXVECTOR3& vDir,
		D3DXVECTOR3& v0, D3DXVECTOR3& v1, D3DXVECTOR3& v2,
		float* t, float* u, float* v);
	bool CheckPick(D3DXVECTOR3& v0, D3DXVECTOR3& v1, D3DXVECTOR3& v2);
	bool ChkOBBToRay(SY_BOX* pBox);
	bool ChkAABBToRay(SY_BOX* pBox);
};

