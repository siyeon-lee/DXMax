#pragma once
#include <windows.h>
#include <math.h>

namespace DX
{
	#define TBASIS_EPSILON ((float)0.001f)
	#define TBASIS_PI ((float)3.141592654f)
	#define DegreeToRandian(degree) (degree*(TBASIS_PI/180.0f ))
	#define RadianToDegree(radian) (radian*(180.0f/TBASIS_PI ))

	struct float2
	{
		union
		{
			struct { float x, y; };
			float v[2];
		};
	};
	struct float3
	{
		union
		{
			struct { float x, y, z; };
			float v[3];
		};
	};
	struct float4
	{
		union
		{
			struct { float x, y, z, w; };
			float v[4];
		};
	};
	class Vector2 : public float2
	{
	public:
		Vector2();
		Vector2(const Vector2& v);
		Vector2(float fx, float fy);
		~Vector2();
	};
	class Vector4 : public float4
	{
	public:
		Vector4();
		Vector4(const Vector4& v);
		Vector4(float fx, float fy, float fz, float fw);
		~Vector4();
	};
	class Vector3 : public float3
	{
	public:
		Vector3 operator + (Vector3 const & v);
		Vector3 operator - (Vector3 const & v);
		Vector3 operator * (float const & fScala);
		Vector3 operator / (float const & fScala);
		float   operator | (Vector3 const &v);
		Vector3 operator ^ (Vector3 const &v);
		bool    operator == (Vector3 const &v);
	public:
		float   Length();
		float   LengthSquared();
		Vector3 Normal();
		float   DAngle(Vector3& v);
	public:
		Vector3();
		Vector3(const Vector3& v);
		Vector3(float fx, float fy, float fz);
		~Vector3();
	};
}


