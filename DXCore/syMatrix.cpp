#include "syMatrix.h"
namespace DX
{
	syMatrix syMatrix::PerspectiveFovLH(
		float fNear,
		float fFar,
		float fFov,
		float fAspect)
	{
		float w, h, q;
		h = 1 / tan(fFov*0.5f);
		w = h / fAspect;
		q = fFar / (fFar - fNear);

		syMatrix mat;
		ZeroMemory(&mat, sizeof(syMatrix));
		mat._11 = w;
		mat._22 = h;
		mat._33 = q;
		mat._43 = -q * fNear;
		mat._34 = 1;
		*this = mat;
		return mat;
	}
	syMatrix syMatrix::ObjectLookAt(
		Vector3& Eye,
		Vector3& At,
		Vector3& Up)
	{
		syMatrix mat;
		Vector3 vLook = (At - Eye).Normal();
		Vector3 vRight = (vLook ^ Up).Normal();
		Vector3 vUp = (vRight ^ vLook).Normal();
		mat._11 = vRight.x;
		mat._12 = vRight.y;
		mat._13 = vRight.z;
		mat._21 = vUp.x;
		mat._22 = vUp.y;
		mat._23 = vUp.z;
		mat._31 = vLook.x;
		mat._32 = vLook.y;
		mat._33 = vLook.z;
		return mat;
	}

	syMatrix syMatrix::ViewLookAt(		//ObjectLookAt랑 서로 역행렬관계
		Vector3& Eye,
		Vector3& At,
		Vector3& Up)
	{
		syMatrix mat;
		Vector3 vLook = (At - Eye).Normal();
		/*Vector3 vRight = (Up ^ vLook).Normal();
		Vector3 vUp = (vLook ^ vRight).Normal();*/

		float fDot = vLook | Up;
		Vector3 vUp = (Up - (vLook * fDot)).Normal();		//그람슈미트 직교화 과정
		Vector3 vRight = (vUp ^ vLook).Normal();

		_11 = vRight.x; _12 = vUp.x; _13 = vLook.x;
		_21 = vRight.y; _22 = vUp.y; _23 = vLook.y;
		_31 = vRight.z; _32 = vUp.z; _33 = vLook.z;
		_41 = -(Eye | vRight);
		_42 = -(Eye | vUp);
		_43 = -(Eye | vLook);
		return mat;
	}

	void syMatrix::Translation(const Vector3& v)
	{
		_41 = v.x;
		_42 = v.y;
		_43 = v.z;
	}
	void syMatrix::Scale(const Vector3& v)
	{
		_11 = v.x;
		_22 = v.y;
		_33 = v.z;
	}
	void syMatrix::XRotate(float fRadian)
	{
		float fCos = (float)cos(fRadian);
		float fSin = (float)sin(fRadian);
		_22 = fCos;		_23 = fSin;
		_32 = -fSin;	_33 = fCos;
	};
	void syMatrix::YRotate(float fRadian)
	{
		float fCos = (float)cos(fRadian);
		float fSin = (float)sin(fRadian);
		_11 = fCos;		_13 = -fSin;
		_31 = fSin;		_33 = fCos;
	} 
	void syMatrix::ZRotate(float fRadian)
	{
		float fCos = (float)cos(fRadian);
		float fSin = (float)sin(fRadian);
		_11 = fCos;		_12 = fSin;
		_21 = -fSin;	_22 = fCos;
	}
	syMatrix syMatrix::Transpose()
	{
		syMatrix mat;
		mat._11 = _11; mat._12 = _21; mat._13 = _31; mat._14 = _41;
		mat._21 = _12; mat._22 = _22; mat._23 = _32; mat._24 = _42;
		mat._31 = _13; mat._32 = _23; mat._33 = _33; mat._34 = _43;
		mat._41 = _14; mat._42 = _24; mat._43 = _34; mat._44 = _44;
		return mat;
	}

}