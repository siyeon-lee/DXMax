#include "syArcBall.h"


bool syArcBall::Init()
{
	SetWindow(g_rtClient.right, g_rtClient.bottom);
	return true;
}
void syArcBall::SetWindow(INT nWidth, INT nHeight, FLOAT fRadius)
{
	m_nWidth = nWidth;
	m_nHeight = nHeight;
	m_fRadius = fRadius;
	m_vCenter = D3DXVECTOR2(m_nWidth / 2.0f, m_nHeight / 2.0f);
}

// 모델의 중점을 구방정식의 중점으로 보고 반구를 기준으로 화면의 넓이와 높이로 일치시켜 
// 화면의 넓이와 높이에 대한 구 성분값에 따라 최종 구방정식에 따라서 성분값을 구한다.
D3DXVECTOR3 syArcBall::ScreenToVector(float fScreenPtX, float fScreenPtY)
{
	// <<구 반지름 : 화면넓이 = 구 성분값 : 화면성분값>> 비례식 이용
	FLOAT x = -(fScreenPtX - m_nWidth / 2) / (m_fRadius*m_nWidth / 2);
	FLOAT y = -(fScreenPtY - m_nHeight / 2) / (m_fRadius*m_nHeight / 2);
	FLOAT z = 0.0f;

	FLOAT mag = x * x + y * y;

	//화면 넓이를 구 반지름 1로 두었기 때문에 1보다 크면 화면 밖. 따라서 가까운 값으로 대체한다.

	if (mag > 1.0f)
	{
		FLOAT scale = 1.0f / sqrtf(mag);
		x *= scale;
		y *= scale;
	}
	else  //화면 안
	{
		z = sqrtf(1.0f - mag);	//구의 반지름 1
	}

	return D3DXVECTOR3(x, y, z);
}
D3DXQUATERNION syArcBall::QuatFromBallPoints(const D3DXVECTOR3 &vFrom, const D3DXVECTOR3 &vTo)
{
	// 임의의 축과 사원수의 관계는 (회전각 : 회전각/2 )이기 때문에
	// 아래와 같이 사원수를 쓰면 두배의 각으로 회전된다. 
	// 이는 화면의 가로 및 세로 한계를 1 ~ -1로 두었기 때문에 코사인 1( 90도)에서 2배인 180로 회전된다.

	D3DXVECTOR3 vPart;

	float fDot = D3DXVec3Dot(&vFrom, &vTo);	//회전각
	D3DXVec3Cross(&vPart, &vFrom, &vTo);	//회전축

	return D3DXQUATERNION(vPart.x, vPart.y, vPart.z, fDot);
}
syArcBall::syArcBall()
{
	m_fSpeed = 0.2f;
	m_bDrag = false;
	m_vDragAngle.x = 0;
	m_vDragAngle.y = 0;
	m_vEularAngle.x = 0;
	m_vEularAngle.y = 0;
	m_vEularAngle.z = 0;
	D3DXQuaternionIdentity(&m_qDown);
	D3DXQuaternionIdentity(&m_qNow);
}


syArcBall::~syArcBall()
{
}
D3DXMATRIX syArcBall::GetRotationMatrix()
{
	//D3DXMatrixRotationYawPitchRoll(
	//	&m_matRotation,
	//	m_vEularAngle.y,
	//	m_vEularAngle.x,
	//	0.0f);
	//return m_matRotation;
	return *D3DXMatrixRotationQuaternion(&m_mRotation, &m_qNow);
}
void	syArcBall::OnBegin(int nX, int nY)
{
	m_bDrag = true;
	m_vDragPt.x = nX;
	m_vDragPt.y = nY;
	m_qDown = m_qNow;
	m_vDownPt = ScreenToVector((float)nX, (float)nY);
}
void	syArcBall::OnMove(int nX, int nY)
{
	if (m_bDrag)
	{
		m_vDragAngle.x = (nX - m_vDragPt.x);
		m_vDragAngle.y = (nY - m_vDragPt.y);
		m_vDragPt.x = nX;
		m_vDragPt.y = nY;
		m_vEularAngle.x += D3DXToRadian(m_vDragAngle.y /*/ 400.0f*/) * m_fSpeed;
		m_vEularAngle.y += D3DXToRadian(m_vDragAngle.x /*/ 300.0f*/) * m_fSpeed;

		// 추가
		m_vCurrentPt = ScreenToVector((float)nX, (float)nY);
		// 현재 회전된 사원수와 현재 설정된 사원수를 곱하여 누적시킨다.
		m_qNow = m_qDown * QuatFromBallPoints(m_vDownPt, m_vCurrentPt);
	}
}
void	syArcBall::OnEnd(int nX, int nY)
{
	m_bDrag = false;
}