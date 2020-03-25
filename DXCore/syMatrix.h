#pragma once
#include "syVector.h"
namespace DX
{

		struct float4x4
		{
			union {
				struct {
					float _11, _12, _13, _14;
					float _21, _22, _23, _24;
					float _31, _32, _33, _34;
					float _41, _42, _43, _44;
				};
				float m[4][4];
	
			};
		};
		class syMatrix : public float4x4
		{
		public:
			syMatrix ObjectLookAt(
				Vector3& Eye,
				Vector3& At,
				Vector3& Up);
			syMatrix ViewLookAt(
				Vector3& Eye,
				Vector3& At,
				Vector3& Up);
			syMatrix PerspectiveFovLH(
				float fNear,
				float fFar,
				float fFov,
				float fAspect);
			void Identity()
			{
				_11 = _12 = _13 = _14 = 0.0f;
				_21 = _22 = _23 = _24 = 0.0f;
				_31 = _32 = _33 = _34 = 0.0f;
				_41 = _42 = _43 = _44 = 0.0f;
				_11 = _22 = _33 = _44 = 1.0f;
			}
			void Translation(const Vector3& v);
			void Scale(const Vector3& v);
			void XRotate(float fRadian);
			void YRotate(float fRadian);
			void ZRotate(float fRadian);
			syMatrix Transpose();
		public:
			syMatrix()
			{
				Identity();
			};

			syMatrix(const syMatrix& mat)
			{
				_11 = mat._11; _12 = mat._12; _13 = mat._13; _14 = mat._14;
				_21 = mat._21; _22 = mat._22; _23 = mat._23; _24 = mat._24;
				_31 = mat._31; _32 = mat._32; _33 = mat._33; _34 = mat._34;
				_41 = mat._41; _42 = mat._42; _43 = mat._43; _44 = mat._44;
			};

			~syMatrix() {};
		};

}

