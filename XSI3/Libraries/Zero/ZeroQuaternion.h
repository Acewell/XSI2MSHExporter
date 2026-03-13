#ifndef _ZEROQUATERNION_H_
#define _ZEROQUATERNION_H_

#include "ZeroTypes.h"
#include "ZeroMatrix.h"
#include "ZeroVector3.h"
//#include "ZeroVector4.h"

// quaternion class definition
class ZeroQuaternion
{
	public:
	float x;
	float y;
	float z;
	float s;

	ZeroQuaternion()
	{
		s = 1.0f;
		x = 0.0f;
		y = 0.0f;
		z = 0.0f;
	}

	ZeroQuaternion(float _s, float _x, float _y, float _z) 
	{
		s = _s;
		x = _x;
		y = _y;
		z = _z;
	}

//	ZeroQuaternion(const ZeroVector4& p) 
//	{
//		x = p.x;
//		y = p.y;
//		z = p.z;
//		s = p.w;
//	}


	ZeroQuaternion(const ZeroVector3 &axis, float angle) 
	{
		float a;;
		float ac;
		float as;

		//_ASSERTE(fabsf(MagnitudeSq(axis) - 1.0f) < EPSILON);
		a = angle * 0.5f;
		ac = Zero_cos(a);
		as = Zero_sin(a);
		s = ac;
		x = axis.x * as;
		y = axis.y * as;
		z = axis.z * as;
	}

	DLL_DECLSPEC ZeroQuaternion(const ZeroMatrix &matrix);
	
	friend  ZeroQuaternion operator+(const ZeroQuaternion &a, const ZeroQuaternion &b)
	{
		return ZeroQuaternion(a.s + b.s, a.x + b.x, a.y + b.y, a.z + b.z);
	}
	friend ZeroQuaternion operator-(const ZeroQuaternion &a, const ZeroQuaternion &b)
	{
		return ZeroQuaternion(a.s - b.s, a.x - b.x, a.y - b.y, a.z - b.z);
	}

	ZeroQuaternion	Scale (float scale) const
	{
		return ZeroQuaternion (s * scale, x * scale, y * scale, z * scale);
	}

	float DotProduct(const ZeroQuaternion &b) const
	{
		return s * b.s + x * b.x + y * b.y + z * b.z;
	}

	ZeroQuaternion Inverse() const 
	{
		return ZeroQuaternion(s, -x, -y, -z);
	}


	ZeroQuaternion &operator+=(const ZeroQuaternion& q)
	{
		x += q.x; y += q.y; z += q.z; s += q.s;
		return *this;
	}


	DLL_DECLSPEC ZeroVector3 RotateVector (const ZeroVector3 &v) const;
	DLL_DECLSPEC ZeroVector3 UnrotateVector (const ZeroVector3 &v) const;

	// calculate a constant angular velocity vector such that applied to this over dt
	// will rotate it into q.
	// note: this function work only for relativaly small rotations
	DLL_DECLSPEC ZeroVector3 GetOmega (const ZeroQuaternion &q, float dt) const;


	DLL_DECLSPEC friend ZeroQuaternion __cdecl operator*(const ZeroQuaternion &a, const ZeroQuaternion &b);
	DLL_DECLSPEC friend ZeroQuaternion __cdecl Interpolate(const ZeroQuaternion &a, const ZeroQuaternion &b, float t);

	bool SanityCheck() const
	{
#if ((PLATFORM == PLATFORM_GAMECUBE) || (PLATFORM == PLATFORM_PS2) )
		return true;
#else
		return _finite(s) && _finite(x) && 
			   _finite(y) && _finite(z) &&  (fabs(DotProduct(*this) - 1.0f) < 0.01f);
#endif
	}

};

inline ZeroQuaternion PitchQuaternion(float pitch)
{
	float Sp = Zero_sin(pitch*0.5f);
	float Cp = Zero_cos(pitch*0.5f);
	return ZeroQuaternion(Cp, Sp, 0.0f, 0.0f);
}

inline ZeroQuaternion YawQuaternion(float yaw)
{
	float Sy = Zero_sin(yaw*0.5f);
	float Cy = Zero_cos(yaw*0.5f);
	return ZeroQuaternion(Cy, 0.0f, Sy, 0.0f);
}

inline ZeroQuaternion RollQuaternion(float roll)
{
	float Sr = Zero_sin(roll*0.5f);
	float Cr = Zero_cos(roll*0.5f);
	return ZeroQuaternion(Cr, 0.0f, 0.0f, Sr);
}

inline ZeroQuaternion PitchYawQuaternion(float pitch, float yaw)
{
	float Sp = Zero_sin(pitch*0.5f);
	float Cp = Zero_cos(pitch*0.5f);
	float Sy = Zero_sin(yaw*0.5f);
	float Cy = Zero_cos(yaw*0.5f);
	return ZeroQuaternion(Cp * Cy, Sp * Cy, Cp * Sy, Sp * Sy);
}

inline ZeroQuaternion YawRollQuaternion(float yaw, float roll)
{
	float Sy = Zero_sin(yaw*0.5f);
	float Cy = Zero_cos(yaw*0.5f);
	float Sr = Zero_sin(roll*0.5f);
	float Cr = Zero_cos(roll*0.5f);
	
	return ZeroQuaternion(Cy * Cr, -Sy * Sr, Sy * Cr, Cy * Sr); 
}

extern const ZeroQuaternion identityQuaternion;

#endif
      
