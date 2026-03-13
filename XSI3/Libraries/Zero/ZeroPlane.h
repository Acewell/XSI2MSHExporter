#ifndef _ZEROPLANE_H_
#define _ZEROPLANE_H_

#include "ZeroVector4.h"

// plane class definition
class ZeroPlane : public ZeroVector4
{
	public:
	//float w;

	// plane constructor
	 ZeroPlane()
		: ZeroVector4()
	{
		 w = 0;
	}
	 ZeroPlane(float x, float y, float z, float _d)
		: ZeroVector4(x, y, z)
	{
		 w = _d;
	}
	 ZeroPlane(const ZeroVector4 &n, float _d)
		: ZeroVector4(n)
	{
		 w = _d;
	}

	ZeroPlane(const ZeroVector4 &a, const ZeroVector4 &b, const ZeroVector4 &c)
  		: ZeroVector4(CrossProduct(b - a, c - a))
	{
  		w = -DotProduct(*this, a);
	}

	float Evalue(const float *v) const
	{
		return x * v[0] + y * v[1] + z * v[2] + w;
	}
	
	float Evalue(const ZeroVector4& v) const
	{
		return Evalue (&v.x);
	}


	// plane distance evaluation
	friend float PlaneDist(const ZeroPlane &p, const float *v)
	{
		return p.Evalue (v);
	}
	friend float PlaneDist(const ZeroPlane &p, const ZeroVector4 &v)
	{
		return p.Evalue (v);
	}
	
	// plane normalization
	friend ZeroPlane Normalize(const ZeroPlane &p)
	{
		float  f = 1.0f / sqrtf(DotProduct(p,p));

		return ZeroPlane( p.x*f , p.y*f , p.z*f , p.w*f);

		//return p / Magnitude(p);
	}

	friend ZeroPlane NormalizeFast(const ZeroPlane &v)
	{
		return v * rsqrt(MagnitudeSq(v));
	}


	// plane operators
	friend  bool operator==(const ZeroPlane &a, const ZeroPlane &b)
	{
		return (a.x == b.x) && (a.y == b.y) && (a.z == b.z) && (a.w == b.w);
	}
	friend  bool operator!=(const ZeroPlane &a, const ZeroPlane &b)
	{
		return (a.x != b.x) || (a.y != b.y) || (a.z == b.z) || (a.w == b.w);
	}
	 float &operator[](const int i)
	{
		return (&x)[i];
	}

	float operator[](const int i) const
	{
		return (&x)[i];
	}
	friend  ZeroPlane operator*(const ZeroPlane &p, float s)
	{
		return ZeroPlane(p.x * s, p.y * s, p.z * s, p.w * s);
	}
	friend  ZeroPlane operator/(const ZeroPlane &p, float s)
	{
		float is = 1.0f / s;
		return ZeroPlane(p.x * is, p.y * is, p.z * is, p.w * is);
	}

	// plane assignment operators
	 ZeroPlane &operator*=(float s)
	{
		x *= s; y *= s; z *= s; w *= s;
		return *this;
	}

	ZeroPlane &operator/=(float s)
	{
		float is = 1.0f / s;
		x *= is; y *= is; z *= is; w *= is;
		return *this;
	}

	ZeroPlane &operator=(const ZeroVector4 &v)
	{
		x = v.x; y = v.y; z = v.z;
		return *this;
	}
};

#endif
