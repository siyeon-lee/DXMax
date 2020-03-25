#include "syDebugCamera.h"

bool syDebugCamera::Init()
{
	m_WorldArcBall.Init();
	m_ViewArcBall.Init();

	return true;
}
LRESULT	syDebugCamera::MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (msg == WM_LBUTTONDOWN )
	{
		int iMouseX = LOWORD(lParam);
		int iMouseY = HIWORD(lParam);
		m_ViewArcBall.OnBegin(iMouseX, iMouseY);
		m_WorldArcBall.OnBegin(iMouseX, iMouseY);
	}
	if (msg == WM_MOUSEMOVE)
	{
		int iMouseX = LOWORD(lParam);
		int iMouseY = HIWORD(lParam);
		m_ViewArcBall.OnMove(iMouseX, iMouseY);
		m_WorldArcBall.OnMove(iMouseX, iMouseY);
	}
	if (msg == WM_LBUTTONUP )
	{
		int iMouseX = LOWORD(lParam);
		int iMouseY = HIWORD(lParam);
		m_ViewArcBall.OnEnd(iMouseX, iMouseY);
		m_WorldArcBall.OnEnd(iMouseX, iMouseY);
	}
	if (msg == WM_MOUSEWHEEL)
	{
		m_iMouseWhellDelta = (short)HIWORD(wParam);
	}
	return 1;
}


void syDebugCamera::Forward()
{
	m_vPosition = m_vPosition + m_vLook * g_fSecondPerFrame * 20.0f;
}
void syDebugCamera::Backward()
{
	m_vPosition = m_vPosition - m_vLook * g_fSecondPerFrame * 20.0f;
}
D3DXMATRIX syDebugCamera::SetViewMatrix(D3DXVECTOR3 vPos, D3DXVECTOR3 vTarget, D3DXVECTOR3 vUp)
{
	m_vDefaultEye = m_vPosition = vPos;
	m_vDefaultLookAt = m_vTarget = vTarget;
	D3DXMatrixLookAtLH(&m_matView, &m_vPosition, &m_vTarget, &vUp);

	D3DXQUATERNION qRotation;
	D3DXVECTOR3 vTrans, vScale, vAxis;
	D3DXMATRIX mInvView;
	D3DXQUATERNION q;
	float fAngle;

	if (SUCCEEDED(D3DXMatrixDecompose(&vScale, &qRotation, &vTrans, &m_matView)))
	{
		D3DXQuaternionNormalize(&qRotation, &qRotation);
		D3DXQuaternionToAxisAngle(&qRotation, &vAxis, &fAngle);
		// 반드시 정규화 해야 한다.
		D3DXVec3Normalize(&vAxis, &vAxis);

		q.x = sin(fAngle / 2) * vAxis.x;
		q.y = sin(fAngle / 2) * vAxis.y;
		q.z = sin(fAngle / 2) * vAxis.z;
		q.w = cos(fAngle / 2);
	}
	D3DXMatrixInverse(&mInvView, NULL, &m_matView);
	D3DXVECTOR3* pZBasis = (D3DXVECTOR3*)&mInvView._31;

	m_fCameraYawAngle = atan2f(pZBasis->x, pZBasis->z);
	float fLen = sqrtf(pZBasis->z * pZBasis->z + pZBasis->x * pZBasis->x);
	m_fCameraPitchAngle = -atan2f(pZBasis->y, fLen);

	return 	UpdateVector();
}
D3DXMATRIX syDebugCamera::SetProjMatrix(FLOAT fFOV, FLOAT fAspect, FLOAT fNearPlane, FLOAT fFarPlane)
{
	m_fFOV = fFOV;
	m_fAspect = fAspect;
	m_fNearPlane = fNearPlane;
	m_fFarPlane = fFarPlane;

	D3DXMatrixPerspectiveFovLH(&m_matProj, fFOV, fAspect, fNearPlane, fFarPlane);
	return m_matProj;
}
D3DXMATRIX syDebugCamera::UpdateVector()
{
	m_vLook.x = m_matView._13;
	m_vLook.y = m_matView._23;
	m_vLook.z = m_matView._33;
	m_vRight.x = m_matView._11;
	m_vRight.y = m_matView._21;
	m_vRight.z = m_matView._31;
	m_vUp.x = m_matView._12;
	m_vUp.y = m_matView._22;
	m_vUp.z = m_matView._32;

	D3DXVec3Normalize(&m_vLook, &m_vLook);
	D3DXVec3Normalize(&m_vRight, &m_vRight);
	D3DXVec3Normalize(&m_vUp, &m_vUp);
	
	CreateFrustum(m_matView, m_matProj);

	//m_Frustum.CreateFrustum(m_matView, m_matProj);			syFrustum::CreateFrustum(syFrustum::m_matView, syFrustum::m_matProj);
	return m_matView;
}

void  syDebugCamera::SetMatrix(D3DXMATRIX* pWorld,
	D3DXMATRIX* pView,
	D3DXMATRIX* pProj)
{
	m_FrustumBoxObj.SetMatrix(pWorld, pView, pProj);
}

bool syDebugCamera::Render(ID3D11DeviceContext*	pContext)
{
	syFrustum::Render(pContext);
	return true;
}

D3DXMATRIX syDebugCamera::Update(D3DXVECTOR4 vDirValue)
{
	D3DXMATRIX matRotation;
	D3DXQuaternionRotationYawPitchRoll(&m_qRotation,
		vDirValue.y,
		vDirValue.x,
		vDirValue.z);

	m_vPosition += m_vLook * vDirValue.w;
	m_fRadius += vDirValue.w;

	D3DXMatrixAffineTransformation(&matRotation, 1.0f, NULL, &m_qRotation, &m_vPosition);
	D3DXMatrixInverse(&m_matView, NULL, &matRotation);

	return UpdateVector();
}
void syDebugCamera::MoveLook(float fValue)
{
	m_vPosition += m_vLook * fValue;
}
void syDebugCamera::MoveSide(float fValue)
{
	m_vPosition += m_vRight * fValue;
}
void syDebugCamera::MoveUp(float fValue)
{
	m_vPosition += m_vUp * fValue;
}
bool syDebugCamera::Frame()
{
	if (g_ActionInput.bSpace == KEY_PUSH)
	{
		m_fSpeed += 50;
	}
	if (g_ActionInput.bSpace == KEY_UP)
	{
		m_fSpeed -= 50;
	}
	if (g_ActionInput.bUPKEY == KEY_HOLD)
	{
		MoveLook(g_fSecondPerFrame * 5.0f * m_fSpeed);
	}
	if (g_ActionInput.bDOWNKEY == KEY_HOLD)
	{
		MoveLook(-g_fSecondPerFrame * 5.0f * m_fSpeed);
	}
	if (g_ActionInput.bLEFTKEY == KEY_HOLD)
	{
		MoveSide(-g_fSecondPerFrame * 5.0f * m_fSpeed);
	}
	if (g_ActionInput.bRIGHTKEY == KEY_HOLD)
	{
		MoveSide(g_fSecondPerFrame * 5.0f * m_fSpeed);
		/*m_matObjWorld._41 += -g_fSecondPerFrame * 8.282;*/
	}
	float fValue = 0;
	if (m_iMouseWhellDelta)
	{
		// 거리에 비례하여 크기를 변경한다.
		fValue += (m_iMouseWhellDelta / 120.0f) *m_fSpeed * g_fSecondPerFrame;
		m_iMouseWhellDelta = 0;
	}
	
	
	m_fCameraPitchAngle = m_ViewArcBall.m_vEularAngle.x;	//pitch
	m_fCameraYawAngle = m_ViewArcBall.m_vEularAngle.y;	//yaw
	m_fCameraRollAngle = m_ViewArcBall.m_vEularAngle.z;	//roll

	 
	D3DXMATRIX matRotation;
	D3DXQUATERNION m_qRotation;
	D3DXQuaternionRotationYawPitchRoll(
		&m_qRotation,
		m_fCameraYawAngle, m_fCameraPitchAngle, m_fCameraRollAngle);

	D3DXMatrixRotationQuaternion(&matRotation, &m_qRotation);

	matRotation._41 = m_vPosition.x;
	matRotation._42 = m_vPosition.y;
	matRotation._43 = m_vPosition.z;

	D3DXMatrixInverse(&m_matView, NULL, &matRotation);



	//Update(D3DXVECTOR4(m_fCameraPitchAngle, m_fCameraYawAngle, m_fCameraRollAngle, fValue));
	UpdateVector();
	return true;
}

void syDebugCamera::SetRadius(float fDefaultRadius, float fMinRadius, float fMaxRadius)
{
	m_fDefaultRadius = m_fRadius = fDefaultRadius;
	m_fMinRadius = fMinRadius;
	m_fMaxRadius = fMaxRadius;
}

void syDebugCamera::SetModelCenter(D3DXVECTOR3 vModelCenter)
{
	m_ModelCenter = vModelCenter;
}
syDebugCamera::syDebugCamera()
{
	// 추가
	m_vPosDelta = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	m_fDefaultRadius = 0.0f;
	m_fMinRadius = 1.0f;
	m_fMaxRadius = FLT_MAX;
	m_fRadius = 0.0f;

	m_fCameraYawAngle = 0.0f;
	m_fCameraPitchAngle = 0.0f;
	m_fCameraRollAngle = 0.0f;

	m_fSpeed = 10.0f;

	// 추가
	m_iMouseWhellDelta = 0;

	m_ModelCenter = D3DXVECTOR3(0, 0, 0);

	D3DXMatrixIdentity(&m_matWorld);
	D3DXMatrixIdentity(&m_mModelRot);
	D3DXMatrixIdentity(&m_mModelLastRot);
	D3DXMatrixIdentity(&m_mCameraRotLast);


	SetProjMatrix(3.141592f / 4.0f, 1.0f, 0.1f, 1000.0f);
	SetViewMatrix(D3DXVECTOR3(0.0f, 0.0f, 10.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 1.0f, 0.0f));
}

syDebugCamera::~syDebugCamera()
{
}


D3DXMATRIX syGameCamera::Update(D3DXVECTOR4 vPos)
{
	syDebugCamera::Update(vPos);
	m_vTarget.x = vPos.x;
	m_vTarget.y = vPos.y;
	m_vTarget.z = vPos.z;

	m_fOffsetPos += vPos.w * g_fSecondPerFrame *m_fSpeed;
	m_vPosition = m_vTarget + D3DXVECTOR3(0, 0, -1.0f) * m_fOffsetPos;

	return m_matView;
}
bool    syGameCamera::Frame()
{
	D3DXMatrixLookAtLH(
		&m_matView,
		&m_vPosition,
		&m_vTarget,
		&m_vUp);

	m_vLook.x = m_matView._13;
	m_vLook.y = m_matView._23;
	m_vLook.z = m_matView._33;
	m_vRight.x = m_matView._11;
	m_vRight.y = m_matView._21;
	m_vRight.z = m_matView._31;
	m_vUp.x = m_matView._12;
	m_vUp.y = m_matView._22;
	m_vUp.z = m_matView._32;

	D3DXVec3Normalize(&m_vLook, &m_vLook);
	D3DXVec3Normalize(&m_vRight, &m_vRight);
	D3DXVec3Normalize(&m_vUp, &m_vUp);
	//UpdateVector();
	syDebugCamera::Frame();
	return true;
};
syGameCamera::syGameCamera()
{
	m_fSpeed = 10.0f;
	m_fOffsetPos = 30.0f;
}
syGameCamera::~syGameCamera()
{
}