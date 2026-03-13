//********************************************************************
// Direct geometry Hardware independent library
//	By Julio Jerez
// VC: 6.0
//********************************************************************
#ifndef __ZeroGeoTypes__
#define __ZeroGeoTypes__

#include "ZeroVector3.h"
#include "ZeroQuaternion.h"
#include "ZeroUserData.h"

#pragma warning (push)
#pragma warning (disable : 4201)

//const unsigned HULL_OPTIMIZE			= 0x0001;
//const unsigned HULL_COLLAPSE_COPLANAR	= 0x0002;
//const unsigned HULL_PACK_VERTEX		= 0x0004;	


template <class T> class ZeroPoint
{
public:
	T x;
	union
	{
		T y;
		T z;
	};

public:
	ZeroPoint() { }
	ZeroPoint(T x, T y) : x(x), y(y) { }
	
	ZeroPoint &operator+=(const ZeroPoint &p)
	{
		x += p.x;
		y += p.y;
		return (*this);
	}
	ZeroPoint operator+(const ZeroPoint &p) const
	{
		return ZeroPoint(x + p.x, y + p.y);
	}
	
	const ZeroPoint &operator-=(const ZeroPoint &p)
	{
		x -= p.x;
		y -= p.y;
		return (*this);
	}
	ZeroPoint operator-(const ZeroPoint &p) const
	{
		return ZeroPoint(x - p.x, y - p.y);
	}
	
	const ZeroPoint &operator*=(const T &s)
	{
		x *= s;
		y *= s;
		return (*this);
	}
	ZeroPoint operator*(const T &s) const
	{
		return ZeroPoint(x * s, y * s);
	}
	
	const ZeroPoint &operator/=(const T &s)
	{
		x /= s;
		y /= s;
		return (*this);
	}
	ZeroPoint operator/(const T &s) const
	{
		return ZeroPoint(x / s, y / s);
	}
	
	friend bool operator==(const ZeroPoint<T> &p1, const ZeroPoint<T> &p2)
	{
		return (p1.x == p2.x && p1.y == p2.y);
	}
	friend bool operator!=(const ZeroPoint<T> &p1, const ZeroPoint<T> &p2)
	{
		return (p1.x != p2.x || p1.y != p2.y);
	}
	friend bool operator>(const ZeroPoint<T> &p1, const ZeroPoint<T> &p2)
	{
		return (p1.x > p2.x && p1.y > p2.y);
	}
	friend bool operator>=(const ZeroPoint<T> &p1, const ZeroPoint<T> &p2)
	{
		return (p1.x >= p2.x && p1.y >= p2.y);
	}
	friend bool operator<(const ZeroPoint<T> &p1, const ZeroPoint<T> &p2)
	{
		return (p1.x < p2.x && p1.y < p2.y);
	}
	friend bool operator<=(const ZeroPoint<T> &p1, const ZeroPoint<T> &p2)
	{
		return (p1.x <= p2.x && p1.y <= p2.y);
	}
	void Set(T aX, T aY)
	{
		x = aX;
		y = aY;
	}
};

template <class T> class ZeroArea
{
public:
	ZeroPoint<T> p0;
	ZeroPoint<T> p1;

public:
	ZeroArea() { }
	ZeroArea(ZeroPoint<T> p0, ZeroPoint<T> p1) : p0(p0), p1(p1) { }
	ZeroArea(T x0, T y0, T x1, T y1) : p0(x0, y0), p1(x1, y1) { }
	ZeroArea(ZeroPoint<T> p0, T x1, T y1) : p0(p0), p1(x1, y1) { }
	ZeroArea(T x0, T y0, ZeroPoint<T> p1) : p0(x0, y0), p1(p1) { }
	
	T Width() const
	{
		return (p1.x - p0.x);
	}
	T Height() const
	{
		return (p1.y - p0.y);
	}
	
	const ZeroArea &operator+=(const ZeroArea &a)
	{
		p0 += a.p0;
		p1 += a.p1;
		return (*this);
	}
	ZeroArea operator+(const ZeroArea &a) const
	{
		return ZeroArea(p0 + a.p0, p1 + a.p1);
	}
	
	const ZeroArea &operator-=(const ZeroArea &a)
	{
		p0 -= rhs.p0;
		p1 -= rhs.p1;
		return (*this);
	}
	ZeroArea operator-(const ZeroArea &a) const
	{
		return ZeroArea(p0 - a.p0, p1 + a.p1);
	}
	
	const ZeroArea &operator+=(const ZeroPoint<T> &p)
	{
		p0 += p;
		p1 += p;
		return (*this);
	}
	ZeroArea operator+(const ZeroPoint<T> &p) const
	{
		return ZeroArea(p0 + p, p1 + p);
	}
	
	const ZeroArea &operator-=(const ZeroPoint<T> &p)
	{
		p0 -= p;
		p1 -= p;
		return (*this);
	}
	ZeroArea operator-(const ZeroPoint<T> &p) const
	{
		return ZeroArea(p0 - p, p1 - p);
	}
	
	const ZeroArea &operator*=(const T &s)
	{
		p0 *= s;
		p1 *= s;
		return (*this);
	}
	ZeroArea operator*(const T &s) const
	{
		return ZeroArea(p0 * s, p1 * s);
	}
	
	const ZeroArea &operator/=(const T &s)
	{
		p0 /= s;
		p1 /= s;
		return (*this);
	}
	ZeroArea operator/(const T &s) const
	{
		return ZeroArea(p0 / s, p1 / s);
	}
	
	bool In(const ZeroPoint<T> &p) const
	{
		return (p >= p0 && p <= p1);
	}
	
	void Sort()
	{
		if (p0.x > p1.x) 
		{ 
			T t = p0.x;
			p0.x = p1.x;
			p1.x = t;
		}
		
		if (p0.y > p1.y) 
		{ 
			T t = p0.y;
			p0.y = p1.y;
			p1.y = t;
		}  
	}
};


class ZeroColor
{
	public:
#if (PLATFORM == PLATFORM_GAMECUBE)
	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char a;
#else
	unsigned char b;
	unsigned char g;
	unsigned char r;
	unsigned char a;
#endif
	ZeroColor ()
	{
	}
	
	ZeroColor (unsigned val)
	{
#if (PLATFORM == PLATFORM_GAMECUBE)
		*(unsigned int *)this = (val >> 24) | (val << 8);
#else
		*(unsigned int *)this = val;
#endif
	}
	
	ZeroColor (unsigned char r, unsigned char g, unsigned char b, unsigned char a = 0xFF)
		: b(b), g(g), r(r), a(a)
	{
	}

	operator unsigned int ()
	{
		return *(unsigned int *)this;
	}

	ZeroColor &operator= (unsigned int val)
	{
#if (PLATFORM == PLATFORM_GAMECUBE)
		*(unsigned int *)this = (val >> 24) | (val << 8);
#else
		*(unsigned int *)this = val;
#endif
		return *this;
	}

	bool operator== (const ZeroColor &c)
	{
		return *(unsigned int *)this == *(unsigned int *)&c;
	}

	bool operator!= (const ZeroColor &c)
	{
		return *(unsigned int *)this != *(unsigned int *)&c;
	}
	
	void Set(unsigned char aR, unsigned char aG, unsigned char aB, unsigned char aA = 0xFF)
	{
		r = aR;
		g = aG;
		b = aB;
		a = aA;
	}
};



class ZeroColorValue
{
	public:
	float r;
	float g;
	float b;
	float a;

	ZeroColorValue ()
	{
	}

	ZeroColorValue (float r, float g, float b, float a = 1.0f)
		: r(r), g(g), b(b), a(a)
	{
	}

	ZeroColorValue(const ZeroColorValue& c) 
		: r(c.r), g(c.g), b(c.b), a(c.a) 
	{
	}

	ZeroColorValue operator=(const ZeroColorValue& c) 	
	{ 
		r = c.r;
		g = c.g;
		b = c.b;
		a = c.a;
		return *this;
	}

	ZeroColorValue operator-(const ZeroColorValue &c) const
	{
		return ZeroColorValue(r - c.r, g - c.g, b - c.b, a - c.a);
	}

	ZeroColorValue operator+(const ZeroColorValue &c) const
	{
		return ZeroColorValue(r + c.r, g + c.g, b + c.b, a + c.a);
	}


	bool operator==(const ZeroColorValue &c) const
	{
		return r == c.r && g == c.g && b == c.b && a == c.a;
	}


	ZeroColorValue operator*(float s) const
	{
		return ZeroColorValue(r * s, g * s, b * s, a * s);
	}

	ZeroColorValue& operator+=(const ZeroColorValue& c)
	{
		r += c.r;
		g += c.g;
		b += c.b;
		a += c.a;
		return *this;
	}

	friend ZeroColorValue Interpolate(const ZeroColorValue &a, const ZeroColorValue &b, const float t)
	{
		return ZeroColorValue (
			a.r * (1.0f - t) + b.r * t,
			a.g * (1.0f - t) + b.g * t,
			a.b * (1.0f - t) + b.b * t,
			a.a * (1.0f - t) + b.a * t
			);
	}
	
	void Set(float aR, float aG, float aB, float aA = 1.0f)
	{
		r = aR;
		g = aG;
		b = aB;
		a = aA;
	}
};


const unsigned int ZERO_COLOR_VALUE_USER_DATA_ID = 0xE40278AB; // "ZERO_COLOR_VALUE_USER_DATA_ID"

class ZeroColorValueUserData : public ZeroUserData
{
public:

	ZeroColorValueUserData()
		: ZeroUserData(ZERO_COLOR_VALUE_USER_DATA_ID, NULL)
	{
	}

	// copy constructor
	ZeroColorValueUserData(const ZeroColorValueUserData& aOriginal, ZeroObject* aOwner)
		: ZeroUserData( ZERO_COLOR_VALUE_USER_DATA_ID, aOwner ),
		mColor(aOriginal.mColor)
	{
	}

	// clone the object
	virtual ZeroUserData* CreateClone(ZeroObject *aOwner,  bool isSim) 
	{
		return new (true) ZeroColorValueUserData(*this, aOwner);
	}


	ZeroColorValue mColor;
};


class ZeroColorFormat
{
	public:
	unsigned int mask;
	unsigned int shift;
	unsigned int scale;
};

class ZeroPixelFormat
{
	public:
	int bitCount;
	ZeroColorFormat red;
	ZeroColorFormat green;
	ZeroColorFormat blue;
	ZeroColorFormat alpha;
};

class ZeroFace
{
	public:
	unsigned index[3];
};
#ifndef WIN32
class ZeroUVAligned128
{
	public:
	float u;
	float v;
	float padding1;
	float padding2;

	ZeroUVAligned128()
	{
	}

	ZeroUVAligned128(const float u, const float v)
		: u(u), v(v)
	{
		padding1 = 0.0f;
		padding2 = 0.0f;
	}

	friend ZeroUVAligned128 Interpolate(const ZeroUVAligned128 &a, const ZeroUVAligned128 &b, const float t)
	{
		return ZeroUVAligned128 (
			a.u * (1.0f - t) + b.u * t,
			a.v * (1.0f - t) + b.v * t
			);
	}
};
#endif
class ZeroUV
{
	public:
	float u;
	float v;
	

	ZeroUV()
	{
	}

	ZeroUV(const float u, const float v)
		: u(u), v(v)
	{
	
	}

	friend ZeroUV Interpolate(const ZeroUV &a, const ZeroUV &b, const float t)
	{
		return ZeroUV (
			a.u * (1.0f - t) + b.u * t,
			a.v * (1.0f - t) + b.v * t
			);
	}
};
class ZeroWeight
{
    public:
	float w1;
	float w2;
	float w3;
    union {
        unsigned bonesValue;
    	unsigned char boneIndex[4];
    };

    public:
	ZeroWeight()
	{
		w1 = 1.0f;
		w2 = 0.0f;
		w3 = 0.0f;
        bonesValue = 0;
	}
};


class ZeroRect
{
	public:
	int x0;
	int y0;
	int x1;
	int y1;
	
	ZeroRect ()
	{
	}
	
	ZeroRect (int x0, int y0, int x1, int y1)
		: x0(x0), y0(y0), x1(x1), y1(y1)
	{
	}
	
	int Clip (ZeroRect &Source)
	{
		unsigned int  code0;
		unsigned int  code1;
		
		code0 = ((unsigned int)(x0 - Source.x0) & 0x80000000);
		code1 = ((unsigned int)(x1 - Source.x0) & 0x80000000);
		
		code0 |= (((unsigned int)(x0 - Source.x1 - 1) & 0x80000000) >> 1);
		code1 |= (((unsigned int)(x1 - Source.x1 - 1) & 0x80000000) >> 1);
		
		code0 |= (((unsigned int)(y0 - Source.y0) & 0x80000000) >> 2);
		code1 |= (((unsigned int)(y1 - Source.y0) & 0x80000000) >> 2);
		
		code0 |= (((unsigned int)(y0 - Source.y1 - 1) & 0x80000000) >> 3);
		code1 |= (((unsigned int)(y1 - Source.y1 - 1) & 0x80000000) >> 3);
		
		code0 ^= (5 << 28);
		code1 ^= (5 << 28);
		
		if (code0 & code1) {
			return -1;
		}
		
		if (!(code0 | code1)) {
			return 0;
		}
		
		if (code0 & ((unsigned)0x8<<28)) {
			x0 = Source.x0;
		}
		
		if (code0 & ((unsigned)0x2<<28)) {
			y0 = Source.y0;
		}
		
		if (code1 & ((unsigned)0x4<<28)) {
			x1 = Source.x1;
		}
		
		if (code1 & ((unsigned)0x1<<28)) {
			y1 = Source.y1;
		}
		
		return 1;
	}
};

class ZeroViewport
{
	public:
	ZeroRect pane;
	float width2;
	float height2;
	float origx;
	float origy;
	
	float fov;
	float homScalex;
	float homScaley;
	float homScalez0;
	float homScalez1;
	
	float nearPlane;
	float farPlane;
	float zoomFactor;                   
};

class ZeroPositionRotation
{
	public:
	ZeroVector3 position;
	ZeroQuaternion rotation;
	
	ZeroPositionRotation ()
		:position(0, 0, 0), rotation()
	{
	}
};

typedef enum ZeroRenderBlendMode
{
	ZRBM_NORMAL,
	ZRBM_ADDITIVE,
	ZRBM_DISTORTION
};

#pragma warning (pop)

#endif
