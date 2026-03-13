#ifndef _ZEROVECTOR3_H_
#define _ZEROVECTOR3_H_

#include <math.h>
#include <float.h>
#include "ZeroTypes.h"
#include "ZeroMathTypes.h"

class ZeroVector3;

inline ZeroVector3 operator+(const ZeroVector3 &a, const ZeroVector3 &b);
inline  ZeroVector3 operator-(const ZeroVector3 &a, const ZeroVector3 &b);

class ZeroVector3
{
	public:
	float x;
	float y; 
	float z;

	ZeroVector3()
	{
	}
	ZeroVector3(float xarg, float yarg, float zarg) 
	{
		 x = xarg;
		 y = yarg;
		 z = zarg;
#ifdef WIN32
		_ASSERTE (SanityCheck());
#endif
	}

	ZeroVector3(const float *v)
	{
		x = v[0];
		y = v[1];
		z = v[2];
#ifdef WIN32
		_ASSERTE (SanityCheck());
#endif
	}

	friend  bool operator==(const ZeroVector3 &a, const ZeroVector3 &b)
	{
		return (a.x == b.x) && (a.y == b.y) && (a.z == b.z);
	}

	friend  bool operator!=(const ZeroVector3 &a, const ZeroVector3 &b)
	{
		return (a.x != b.x) || (a.y != b.y) || (a.z != b.z);
	}
	
		
	friend  ZeroVector3 operator-(const ZeroVector3 &v)
	{
		return ZeroVector3(-v.x, -v.y, -v.z);
	}

	friend  ZeroVector3 operator*(const ZeroVector3 &v, const float s)
	{
		return ZeroVector3(v.x * s, v.y * s, v.z * s);
	}

	friend  ZeroVector3 operator*(const float s, const ZeroVector3 &v)
	{
		return ZeroVector3(v.x * s, v.y * s, v.z * s);
	}
	
	friend  ZeroVector3 operator/(const ZeroVector3 &a, const ZeroVector3 &b)
	{
		return ZeroVector3(a.x / b.x, a.y / b.y, a.z / b.z);
	}

	friend  ZeroVector3 operator/(const ZeroVector3 &v, const float s)
	{
		float is = 1.0f / s;
		return ZeroVector3(v.x * is, v.y * is, v.z * is);
	}

	 float &operator[](const int i)
	{
		return (&x)[i];
	}

	 float operator[](const int i) const
	{
		return (&x)[i];
	}

	// vector assignment operators
	 ZeroVector3 &operator+=(const ZeroVector3 &v)
	{
		x += v.x; y += v.y; z += v.z; 
		return *this;
	}

	 ZeroVector3 &operator-=(const ZeroVector3 &v)
	{
		x -= v.x; y -= v.y; z -= v.z;
		return *this;
	}

	 ZeroVector3 &operator*=(const ZeroVector3 &v)
	{
		x *= v.x; y *= v.y; z *= v.z;
		return *this;
	}

	ZeroVector3 &operator*=(const float s)
	{
		x *= s; y *= s; z *= s;
		return *this;
	}

	ZeroVector3 &operator/=(const float s)
	{
		float is = 1.0f / s;
		x *= is; y *= is; z *= is;
		return *this;
	}

	ZeroVector3 Scale(float s) const
	{
		return ZeroVector3 (x * s, y * s, z * s);
	}


	ZeroVector3 ComponentsProduct(const ZeroVector3 &a) const
	{
		return ZeroVector3(a.x * x, a.y * y, a.z * z);
	}

	// vector dot product
//	friend float operator% (const ZeroVector3 &a, const ZeroVector3 &b)
//	{
//		return a.x * b.x + a.y * b.y + a.z * b.z;
//	}

	

	// vector cross product
//	friend  ZeroVector3 operator*(const ZeroVector3 &a, const ZeroVector3 &b)
//	{
//		return ZeroVector3(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
//	}

	


	// vector magnitude
	friend  float MagnitudeSq(const ZeroVector3 &v)
	{
		return v.x * v.x + v.y * v.y + v.z * v.z;
	}
	friend  float Magnitude(const ZeroVector3 &v)
	{ 
		return sqrtf(MagnitudeSq(v)); 
	}
	friend  float MagnitudeFast(const ZeroVector3 &v)
	{
		float mag2 = MagnitudeSq(v);
		return mag2 * rsqrt(mag2);
	}

	// vector x-z magnitude
	friend  float MagnitudeXZSq(const ZeroVector3 &v)
	{
		return v.x * v.x + v.z * v.z;
	}
	friend  float MagnitudeXZ(const ZeroVector3 &v)
	{ 
		return sqrtf(MagnitudeXZSq(v)); 
	}
	friend  float MagnitudeXZFast(const ZeroVector3 &v)
	{
		float mag2 = MagnitudeXZSq(v);
		return mag2 * rsqrt(mag2);
	}

	// vector distance
	friend  float DistanceSq(const ZeroVector3 &a, const ZeroVector3 &b)
	{
		return MagnitudeSq(a - b);
	}
	friend  float Distance(const ZeroVector3 &a, const ZeroVector3 &b)
	{
		return Magnitude(a - b);
	}
	friend  float DistanceFast(const ZeroVector3 &a, const ZeroVector3 &b)
	{
		return MagnitudeFast(a - b);
	}

	// vector distance
	friend  float DistanceXZSq(const ZeroVector3 &a, const ZeroVector3 &b)
	{
		return MagnitudeXZSq(a - b);
	}
	friend  float DistanceXZ(const ZeroVector3 &a, const ZeroVector3 &b)
	{
		return MagnitudeXZ(a - b);
	}
	friend  float DistanceXZFast(const ZeroVector3 &a, const ZeroVector3 &b)
	{
		return MagnitudeXZFast(a - b);
	}

	// vector normalization
	friend  ZeroVector3 Normalize(const ZeroVector3 &v)
	{
		return v / Magnitude(v);
	}
	
	friend  ZeroVector3 NormalizeFast(const ZeroVector3 &v)
	{
		return v * rsqrt(MagnitudeSq(v));
	}

	friend  ZeroVector3 NormalizeXZ(const ZeroVector3 &v)
	{
		return v / MagnitudeXZ(v);
	}
	friend  ZeroVector3 NormalizeXZFast(const ZeroVector3 &v)
	{
		return v * rsqrt(MagnitudeXZSq(v));
	}

	// vector interpolation
	friend ZeroVector3 Interpolate(const ZeroVector3 &a, const ZeroVector3 &b, const float t)
	{
		return a * (1.0f - t) + b * t;
	}
	
	ZeroVector3& Set(float aX, float aY, float aZ)
	{
		x = aX;
		y = aY;
		z = aZ;
		return *this;
	}

	// vector sanity check
#ifdef _WIN32
	bool SanityCheck() const
	{
		return _finite(x) && _finite(y) && _finite(z);
	}
#else

	bool SanityCheck() const
	{
		return true;
	}
#endif
}
/*
#ifndef WIN32
GCC_ATTRIBUTE( aligned( 16 ) )
#endif
*/
;

inline  ZeroVector3 operator+(const ZeroVector3 &a, const ZeroVector3 &b)
{
		return ZeroVector3(a.x + b.x, a.y + b.y, a.z + b.z);
}

inline  ZeroVector3 operator-(const ZeroVector3 &a, const ZeroVector3 &b)
{
		return ZeroVector3(a.x - b.x, a.y - b.y, a.z - b.z);
}
#endif
