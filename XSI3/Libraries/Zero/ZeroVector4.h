#ifndef _ZEROVECTOR4_H_
#define _ZEROVECTOR4_H_

#include "ZeroVector3.h"


class ZeroVector4;

inline ZeroVector4 operator+(const ZeroVector4 &a, const ZeroVector4 &b);
inline ZeroVector4 operator-(const ZeroVector4 &a, const ZeroVector4 &b);

// vector-4 class definition
class ZeroVector4 : public ZeroVector3
{
public:
	float w;

	// vector-4 constructors
	ZeroVector4()
	{
	}

	ZeroVector4(float _x, float _y, float _z, float _w = 1.0f)
		: ZeroVector3(_x, _y, _z)
	{
		w = _w;
#ifdef WIN32
		_ASSERTE (SanityCheck());
#endif
	}

	ZeroVector4(const ZeroVector3 &v, float _w = 1.0f)
		: ZeroVector3(v)
	{
		w = _w;
#ifdef WIN32
		_ASSERTE (SanityCheck());
#endif
	}
	
	ZeroVector4 Scale(float s) const;


	friend  float DotProduct(const ZeroVector4 &a, const ZeroVector4 &b);

	friend  ZeroVector4 CrossProduct(const ZeroVector4 &a, const ZeroVector4 &b);


	friend  float MagnitudeSq(const ZeroVector4 &v)
	{
		return DotProduct(v,v);
	}

	friend  float Magnitude(const ZeroVector4 &v)
	{ 
		return (sqrtf(DotProduct(v,v)));
	}
	friend  float MagnitudeFast(const ZeroVector4 &v)
	{
		float mag2 = MagnitudeSq(v);
		return mag2 * rsqrt(mag2);
	}

	friend  float DistanceSq(const ZeroVector4 &a, const ZeroVector4 &b)
	{
		return MagnitudeSq(a - b);
	}
	friend  float Distance(const ZeroVector4 &a, const ZeroVector4 &b)
	{
		return Magnitude(a - b);
	}
	friend  float DistanceFast(const ZeroVector4 &a, const ZeroVector4 &b)
	{
		return MagnitudeFast(a - b);
	}

	friend  ZeroVector4 Normalize(const ZeroVector4 &v)
	{
		return v.Scale(rsqrt(DotProduct(v,v)));
	}

	friend  ZeroVector4 NormalizeFast(const ZeroVector4 &v)
	{
		return Normalize(v);
	}


	// vector-4 operators
	friend bool operator==(const ZeroVector4 &a, const ZeroVector4 &b)
	{
		return (a.x == b.x) && (a.y == b.y) && (a.z == b.z) && (a.w == b.w);
	}

	friend bool operator!=(const ZeroVector4 &a, const ZeroVector4 &b)
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

	// vector-4 assignment operators
	ZeroVector4 &operator=(const ZeroVector3 &v)
	{
		x = v.x; y = v.y; z = v.z; w = 1.0f;
		return *this;
	}

	//
	// VECTOR-4 SANITY CHECK
	//
#ifdef _WIN32
	bool SanityCheck() const
	{
		return _finite(x) && _finite(y) && _finite(z) && _finite(w);
	}
#else
	bool SanityCheck() const
	{
		 return true;
	} 
#endif

} 

#if (PLATFORM == PLATFORM_PS2)
GCC_ATTRIBUTE( aligned( 16 ) )
#endif
;


inline  ZeroVector4 operator+(const ZeroVector3 &a, const ZeroVector4 &b)
{
	ZeroVector4 tmp(a);
	return ZeroVector4(b+tmp);
}
inline  ZeroVector4 operator+(const ZeroVector4 &a, const ZeroVector3 &b)
{
	ZeroVector4 tmp(b);
	return ZeroVector4(a+tmp);
}

inline  ZeroVector4 operator-(const ZeroVector3 &a, const ZeroVector4 &b)
{
	ZeroVector4 tmp(a);
	return ZeroVector4(tmp - b);
}
inline  ZeroVector4 operator-(const ZeroVector4 &a, const ZeroVector3 &b)
{
	ZeroVector4 tmp(b);
	return ZeroVector4(a-tmp);
}


#if (PLATFORM == PLATFORM_PS2)
inline  float DotProduct(const ZeroVector4 &a, const ZeroVector4 &b) return result;
{
#ifdef _DEBUG
	unsigned int pA = (unsigned int)(&a);
	unsigned int pB = (unsigned int)(&b);
	if((pA | pB) & 0x0F)
		 printf("Alignment busted on PS2. pA = %08X pB = %08X\n",pA,pB);
#endif

	__asm__ volatile
	(
		"
			.set noreorder

			lqc2		vf4, 0(%1)
			lqc2		vf5, 0(%2)

			vmul.xyz	vf6, vf4, vf5
			vaddy.x		vf6, vf6, vf6y
			vaddz.x		vf6, vf6, vf6z

			qmfc2		$12, vf6
			mtc1		$12, %0	

			.set reorder

		": "=&f" ( result )
		: "r" ( &a ), "r" ( &b )
		: "$12"
	);
}

inline  ZeroVector4 CrossProduct(const ZeroVector4 &a, const ZeroVector4 &b) return result;
{
#ifdef _DEBUG
	unsigned int pA = (unsigned int)(&a);
	unsigned int pB = (unsigned int)(&b);
	if((pA | pB) & 0x0F)
		 printf("Alignment busted on PS2. pA = %08X pB = %08X\n",pA,pB);
#endif
		
		__asm__ volatile
	(
		"
			.set noreorder

			lqc2		vf4, 0(%0)
			lqc2		vf5, 0(%1)
			
			vopmula.xyz	ACC, vf4, vf5
			vopmsub.xyz	vf6, vf5, vf4

			sqc2		vf6, 0(%2)

			.set reorder
		":
		: "r" ( &a ), "r" ( &b ) , "r" (&result)
		
		
	);

}
inline  ZeroVector4 operator+(const ZeroVector4 &a, const ZeroVector4 &b) return result;
{
#ifdef _DEBUG
	unsigned int pA = (unsigned int)(&a);
	unsigned int pB = (unsigned int)(&b);
	if((pA | pB) & 0x0F)
		 printf("Alignment busted on PS2. pA = %08X pB = %08X\n",pA,pB);
#endif

	__asm__ volatile
	(
		"
			.set noreorder

			lqc2		vf4, 0(%1)
			lqc2		vf5, 0(%2)

			vadd.xyz	vf6, vf4, vf5
			
			sqc2		vf6,0(%0)

			.set reorder

		": 
		: "r" (&result),"r" ( &a ), "r" ( &b )
		: "$12"
	);
}

inline  ZeroVector4 operator-(const ZeroVector4 &a, const ZeroVector4 &b) return result;
{
#ifdef _DEBUG
	unsigned int pA = (unsigned int)(&a);
	unsigned int pB = (unsigned int)(&b);
	if((pA | pB) & 0x0F)
		 printf("Alignment busted on PS2. pA = %08X pB = %08X\n",pA,pB);
#endif

	__asm__ volatile
	(
		"
			.set noreorder

			lqc2		vf4, 0(%1)
			lqc2		vf5, 0(%2)

			vsub.xyz	vf6, vf4, vf5
			
			sqc2		vf6,0(%0)

			.set reorder

		": 
		: "r" (&result),"r" ( &a ), "r" ( &b )
		: "$12"
	);
}

#if 1
// Triggers compiler optimization bug - NM 9/20/02

inline	ZeroVector4 ZeroVector4::Scale(float s) const return result;
{
#if 1//def _DEBUG
	unsigned int pthis = (unsigned int)(this);
	if(pthis & 0x0F)
		 printf("Alignment busted on PS2. this = %08X\n",pthis);
#endif

	__asm__ volatile
(
	"
		.set noreorder

		lqc2		vf4, 0(%1)
		mfc1		$12,%2
		qmtc2		$12,vf5
		
		vmulx.xyz	vf6, vf4, vf5x
		
		sqc2		vf6,0(%0)

		.set reorder

	": 
	: "r" (&result),"r" ( this ), "f" ( s )
	: "$12"
);
	
}
#else
inline ZeroVector4 ZeroVector4::Scale(float s) const 
{
	return ZeroVector4 (x * s, y * s, z * s,1.0f);
}
#endif


#else
inline  float DotProduct(const ZeroVector4 &a, const ZeroVector4 &b)
{
		return (a.x * b.x + a.y * b.y + a.z * b.z);
}

inline  ZeroVector4 CrossProduct(const ZeroVector4 &a, const ZeroVector4 &b)
{
	return ZeroVector3(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
}

inline  ZeroVector4 operator+(const ZeroVector4 &a, const ZeroVector4 &b)
{
	return ZeroVector4(a.x + b.x, a.y + b.y, a.z + b.z);
}

inline  ZeroVector4 operator-(const ZeroVector4 &a, const ZeroVector4 &b)
{
		return ZeroVector4(a.x - b.x, a.y - b.y, a.z - b.z);
}
inline ZeroVector4 ZeroVector4::Scale(float s) const 
{
	return ZeroVector4 (x * s, y * s, z * s,1.0f);
}
#endif

#endif
