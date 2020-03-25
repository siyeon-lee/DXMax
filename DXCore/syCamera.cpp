#include "syCamera.h"

bool   syCamera::Init()
{
	m_WorldArcBall.Init();
	m_ViewArcBall.Init();
	return true;
}

void  syCamera::SetMatrix(D3DXMATRIX* pWorld, D3DXMATRIX* pView, D3DXMATRIX* pProj)
{
	m_Frustum.m_FrustumBoxObj.SetMatrix(pWorld, pView, pProj);
}
bool syCamera::Render(ID3D11DeviceContext*	pImmediateContext)
{
	return true;
}
LRESULT	syCamera::MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (msg == WM_LBUTTONDOWN ||
		msg == WM_MBUTTONDOWN ||
		msg == WM_RBUTTONDOWN)
	{
		int iMouseX = LOWORD(lParam);
		int iMouseY = HIWORD(lParam);
		m_ViewArcBall.OnBegin(iMouseX, iMouseY);
		m_WorldArcBall.OnBegin(iMouseX, iMouseY);
	}
	if (msg == WM_MOUSEMOVE )
	{
		int iMouseX = LOWORD(lParam);
		int iMouseY = HIWORD(lParam);
		m_ViewArcBall.OnMove(iMouseX, iMouseY);
		m_WorldArcBall.OnMove(iMouseX, iMouseY);
	}
	if (msg == WM_LBUTTONUP ||
		msg == WM_MBUTTONUP||
		msg == WM_RBUTTONUP)
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
void   syCamera::CreateViewMatrix(
	D3DXVECTOR3 p,
	D3DXVECTOR3 t,
	D3DXVECTOR3 u)
{
	m_vPosition = p;
	m_vTarget = t;


	D3DXMatrixLookAtLH(&m_matView, &p, &t, &u);

	UpdateVector();

	m_ViewArcBall.m_vEularAngle.y = atan2f(m_vLook.x, m_vLook.z);	//초기 카메라 앵글 설정
	float fLen = sqrtf(m_vLook.z*m_vLook.z + m_vLook.x*m_vLook.x);
	m_ViewArcBall.m_vEularAngle.x = -atan2f(m_vLook.y, fLen);
	//m_vLook = D3DXVECTOR3(
	//	m_matView._13,
	//	m_matView._23,
	//	m_matView._33);
	//m_vUp = D3DXVECTOR3(
	//	m_matView._12,
	//	m_matView._22,
	//	m_matView._32);
	//m_vRight = D3DXVECTOR3(
	//	m_matView._11,
	//	m_matView._21,
	//	m_matView._31);

	//D3DXVec3Normalize(&m_vLook, &m_vLook);
	//D3DXVec3Normalize(&m_vUp, &m_vUp);
	//D3DXVec3Normalize(&m_vRight, &m_vRight);
}
void syCamera::CreateProjMatrix(float fFov,
	float fAspect,
	float fNear,
	float fFar)
{
	D3DXMatrixPerspectiveFovLH(
		&m_matProj, fFov,
		fAspect,
		fNear,
		fFar);
}
void syCamera::Forward()
{
	m_vPosition = m_vPosition + m_vLook * g_fSecondPerFrame * 20.0f;
}
void syCamera::Backward()
{
	m_vPosition = m_vPosition - m_vLook * g_fSecondPerFrame * 20.0f;
}
bool syCamera::Frame()
{
	m_vValue.x = m_ViewArcBall.m_vEularAngle.x;	//pitch
	m_vValue.y = m_ViewArcBall.m_vEularAngle.y;	//yaw
	m_vValue.z = m_ViewArcBall.m_vEularAngle.z;	//roll
	m_vValue.w = m_fRadius;

	D3DXMATRIX matRotation;
	D3DXQUATERNION m_qRotation;
	D3DXQuaternionRotationYawPitchRoll(
		&m_qRotation,
		m_vValue.y, m_vValue.x, m_vValue.z);

	D3DXMatrixRotationQuaternion(&matRotation, &m_qRotation);

	matRotation._41 = m_vPosition.x;
	matRotation._42 = m_vPosition.y;
	matRotation._43 = m_vPosition.z;

	D3DXMatrixInverse(&m_matView, NULL, &matRotation);



	//float rad = DegreeToRandian(g_fGameTimer * 100);
	//float dis =10;
	//m_vPosition = Vector3(
	//	m_vTarget.x + dis* cos(rad),
	//	m_vTarget.y + m_vPosition.y,
	//	m_vTarget.z + dis*sin(rad));
//	CreateViewMatrix(m_vPosition, m_vTarget);
	return UpdateVector();
}

bool syCamera::UpdateVector()
{
	m_vLook = D3DXVECTOR3(
		m_matView._13,
		m_matView._23,
		m_matView._33);
	m_vUp = D3DXVECTOR3(
		m_matView._12,
		m_matView._22,
		m_matView._32);
	m_vRight = D3DXVECTOR3(
		m_matView._11,
		m_matView._21,
		m_matView._31);

	D3DXVec3Normalize(&m_vLook, &m_vLook);
	D3DXVec3Normalize(&m_vUp, &m_vUp);
	D3DXVec3Normalize(&m_vRight, &m_vRight);

	return true;
}

syCamera::syCamera()
{
	m_fRadius = 5.0f;
	m_iMouseWhellDelta = 0;
	D3DXMatrixIdentity(&m_matWorld);
	D3DXMatrixIdentity(&m_matView);
	D3DXMatrixIdentity(&m_matProj);
	m_vPosition = D3DXVECTOR3(0.0f, 0.0f, -10.0f);
	m_vTarget = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_vUp = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
}

syCamera::~syCamera()
{
}
