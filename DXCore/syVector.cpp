#include "syVector.h"
namespace DX
{
	Vector2::Vector2()
	{
		x = y = 0.0f;
	}
	Vector2::Vector2(const Vector2& v)
	{
		x = v.x;
		y = v.y;
	}
	Vector2::Vector2(float fx, float fy)
	{
		x = fx;
		y = fy;
	}
	Vector2::~Vector2()
	{
	}
	Vector4::Vector4()
	{
		x = y = z = w = 0.0f;
	}
	Vector4::Vector4(const Vector4& v)
	{
		x = v.x;
		y = v.y;
		z = v.z;
		w = v.w;
	}
	Vector4::Vector4(float fx, float fy,
		float fz, float fw)
	{
		x = fx;
		y = fy;
		z = fz;
		w = fw;
	}
	Vector4::~Vector4()
	{
	}


	Vector3 Vector3::operator + (Vector3 const & v)
	{
		return Vector3(v.x + x, v.y + y, v.z + z);
	}
	Vector3 Vector3::operator - (Vector3 const & v)
	{
		return Vector3(x - v.x, y - v.y, z - v.z);
	}
	Vector3 Vector3:: operator * (float const & fScala)
	{
		return Vector3(x*fScala, y*fScala, z*fScala);
	}
	Vector3 Vector3::operator / (float const & fScala)
	{
		float fInvert = 1.0f / fScala;
		return Vector3(x*fInvert, y*fInvert, z*fInvert);
	}
	float   Vector3::operator | (Vector3 const &v)
	{
		return x * v.x + y * v.y + z * v.z;
	}
	Vector3 Vector3::operator ^ (Vector3 const &v)
	{
		return Vector3(y*v.z - z * v.y,
			z*v.x - x * v.z,
			x*v.y - y * v.x);
	}
	bool    Vector3::operator == (Vector3 const &v)
	{
		if (fabs(x - v.x) < TBASIS_EPSILON)
		{
			if (fabs(y - v.y) < TBASIS_EPSILON)
			{
				if (fabs(z - v.z) < TBASIS_EPSILON)
				{
					return true;
				}
			}
		}
		return false;
	}
	float   Vector3::Length()
	{
		return (float)sqrt(LengthSquared());
	}
	float   Vector3::LengthSquared()
	{
		return (x*x + y * y + z * z);
	}
	Vector3 Vector3::Normal()
	{
		float InvertLength = 1.0f / Length();
		return Vector3(x * InvertLength,
			y * InvertLength,
			z * InvertLength);
	}
	float   Vector3::DAngle(Vector3& v)
	{
		Vector3 v0 = Normal();
		Vector3 v1 = v.Normal();
		float fDot = v0 | v1;
		return RadianToDegree((acos(fDot)));
	}
	Vector3::Vector3(const Vector3& v)
	{
		x = v.x;
		y = v.y;
		z = v.z;
	}
	Vector3::Vector3(float fx, float fy, float fz)
	{
		x = fx;
		y = fy;
		z = fz;
	}

	Vector3::Vector3()
	{
		x = y = z = 0.0f;
	}


	Vector3::~Vector3()
	{
	}
}