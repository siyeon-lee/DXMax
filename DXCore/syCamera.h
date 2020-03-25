#pragma once
#include "syStd.h"
#include "syMatrix.h"
#include "syArcBall.h"
#include "syFrustum.h"
class syCamera
{
public:
	syCamera();
	virtual ~syCamera();
public:
	syFrustum      m_Frustum;
	syArcBall      m_WorldArcBall;
	syArcBall      m_ViewArcBall;
	int			  m_iMouseWhellDelta;
	float		  m_fRadius;
	D3DXVECTOR4   m_vValue;	//yaw pitch roll값
public:
	//카메라 방향벡터 및 위치
	D3DXVECTOR3		m_vLook;
	D3DXVECTOR3		m_vUp;
	D3DXVECTOR3		m_vRight;
	D3DXVECTOR3		m_vPosition;
	D3DXVECTOR3		m_vTarget;
	D3DXQUATERNION	m_qRotation;//뷰회전에 대한 쿼터니언
	D3DXVECTOR3		m_vDefaultEye;
	D3DXVECTOR3		m_vDefaultLookAt;


	D3DXVECTOR3		m_vPosDelta;	//각 축에 대한 변위값 w = 반지름(원점부터의 거리)

	//float		  m_fFarPlane = 3000.0f;
	//float		  m_fNearPlane = 1.0f;
	//bool			isCameraMove = true;
public:
	D3DXMATRIX		m_matWorld;
	D3DXMATRIX		m_matView;
	D3DXMATRIX		m_matProj;
public:
	bool   Init();
	void   CreateViewMatrix(
		D3DXVECTOR3 p,
		D3DXVECTOR3 t,
		D3DXVECTOR3 u = D3DXVECTOR3(0.0f, 1.0f, 0.0f));
	LRESULT	MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	void CreateProjMatrix(float fFov,
		float fAspect,
		float fNear,
		float fFar);
	void Forward();
	void Backward();
	virtual bool Frame();
	virtual bool UpdateVector();
	virtual bool Render(ID3D11DeviceContext*	pImmediateContext);
	virtual void Update(D3DXVECTOR4 vValue)
	{
		m_vValue = vValue;
	};

	void  SetMatrix(D3DXMATRIX* pWorld,
		D3DXMATRIX* pView,
		D3DXMATRIX* pProj);
};

