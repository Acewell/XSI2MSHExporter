#ifndef _ZEROMATRIX_H_
#define _ZEROMATRIX_H_

#include "ZeroVector4.h"
#include "ZeroPlane.h"

class ZeroQuaternion;

class ZeroMatrix
{
	public:
	ZeroVector4 right;
	ZeroVector4 up;
	ZeroVector4 front;
	ZeroVector4 posit;

	ZeroMatrix()
		: right(1, 0, 0, 0), up(0, 1, 0, 0), front(0, 0, 1, 0), posit(0, 0, 0, 1)
	{
	}
	ZeroMatrix(const ZeroVector3 &r, const ZeroVector3 &u, const ZeroVector3 &f, const ZeroVector3 &p)
		: right(r, 0), up(u, 0), front(f, 0), posit(p, 1)
	{
#ifdef WIN32
		_ASSERTE (SanityCheck());
#endif
	}

	ZeroMatrix(const ZeroVector4 &r, const ZeroVector4 &u, const ZeroVector4 &f, const ZeroVector4 &p)
		: right(r), up(u), front(f), posit(p)
	{
#ifdef WIN32
		_ASSERTE (SanityCheck());
#endif
	}
	DLL_DECLSPEC ZeroMatrix(const ZeroQuaternion &q, const ZeroVector3 &p);
	DLL_DECLSPEC ZeroMatrix(const float neardist, const float fardist, const float fov, const float aspect = 1.0f);


	DLL_DECLSPEC ZeroVector4 CalcIKValues () const;
	DLL_DECLSPEC ZeroVector3 CalcPitchYawRollEulerAngles () const;


	DLL_DECLSPEC ZeroMatrix EigenVectors (ZeroVector3 &eigenvalues) const;


	ZeroMatrix Transpose ()	const
	{
		return ZeroMatrix(
			ZeroVector4(right.x, up.x, front.x, 0.0f),
			ZeroVector4(right.y, up.y, front.y, 0.0f),
			ZeroVector4(right.z, up.z, front.z, 0.0f),
			ZeroVector4(0.0f, 0.0f, 0.0f, 1.0f)
			);
	}
	
	ZeroMatrix FullTranspose () const
	{
		return ZeroMatrix(
			ZeroVector4(right.x, up.x, front.x, posit.x),
			ZeroVector4(right.y, up.y, front.y, posit.y),
			ZeroVector4(right.z, up.z, front.z, posit.z),
			ZeroVector4(right.w, up.w, front.w, posit.w)
			);
	}

	ZeroMatrix Inverse() const
	{
		return ZeroMatrix(
			ZeroVector4(right.x, up.x, front.x, 0.0f),
			ZeroVector4(right.y, up.y, front.y, 0.0f),
			ZeroVector4(right.z, up.z, front.z, 0.0f),
			ZeroVector4(-DotProduct(posit, right), -DotProduct(posit, up), -DotProduct(posit, front), 1.0f));
	}


	ZeroVector4 &operator[](const int i)
	{
		return (&right)[i];
	}
	ZeroVector4 operator[](const int i) const
	{
		return (&right)[i];
	}

	DLL_DECLSPEC friend ZeroMatrix __cdecl operator*(const ZeroMatrix &a, const ZeroMatrix &b);
	ZeroMatrix &operator=(const ZeroMatrix &M);

	DLL_DECLSPEC void TransformVectors (void const * dstPtr, int destStrideInBytes,
													void const * srcPtr, int srcStrideInBytes,
												  	int count,int precision = 0) const;

	DLL_DECLSPEC void IndexedTransformVectorsAndAccumulate (
								void const *dstPtr, int destStrideInBytes, int const *dstIndexArray,
								ZeroVector4 const *srcPtr, int count) const;


	ZeroVector4 RotateVector (const ZeroVector4 &v) const;

	ZeroVector4 UnrotateVector (const ZeroVector4 &v) const
	{
		return ZeroVector4 (v.x * right.x + v.y * right.y + v.z * right.z,
								  v.x * up.x + v.y * up.y + v.z * up.z,
								  v.x * front.x + v.y * front.y + v.z * front.z);
	}

	ZeroVector4 TransformVectorH (const ZeroVector4 &v) const;
	ZeroVector4 FullTransformVector( const ZeroVector4 &v ) const;
	ZeroVector4 TransformVector (const ZeroVector4 &v) const;

	ZeroVector4 UntransformVector (const ZeroVector4 &v) const 
	{
		return UnrotateVector(v - posit);
	}

	ZeroPlane TransformPlane (const ZeroPlane &p) const
	{
		ZeroVector4 n(RotateVector(p));
		return ZeroPlane(n, p.w - DotProduct(posit, n));
	}

	ZeroPlane UntransformPlane (const ZeroPlane &p) const
	{
		return ZeroPlane(UnrotateVector(p), p.w + DotProduct(posit, p));
	}

#if (PLATFORM != PLATFORM_PS2)
	bool SanityCheck(void) const
	{
		return right.SanityCheck() && up.SanityCheck() && 
			   front.SanityCheck() && posit.SanityCheck();  
			   //(right.w == 0) && (up.w == 0) && (front.w == 0) && (posit.w == 1);
	}
#else
	bool SanityCheck(void) const
	{
		return true;
	}

#endif

	DLL_DECLSPEC void RecordValues(int mode);
};


inline ZeroMatrix PitchMatrix(
	float pitch, 
	const ZeroVector3 &pos = ZeroVector4 (0.0f, 0.0f, 0.0f, 1.0f))
{
	float Sp = Zero_sin(pitch);
	float Cp = Zero_cos(pitch);

	return ZeroMatrix(ZeroVector4(1.0f, 0.0f, 0.0f, 0.0f),
							ZeroVector4(0.0f, Cp, Sp, 0.0f),
							ZeroVector4(0.0f, -Sp, Cp, 0.0f),
							ZeroVector4(pos.x, pos.y, pos.z, 1.0f));
}

inline ZeroMatrix YawMatrix(
	float yaw, 
	const ZeroVector3 &pos = ZeroVector4 (0.0f, 0.0f, 0.0f, 1.0f))
{
	float Sy = Zero_sin(yaw);
	float Cy = Zero_cos(yaw);

	return ZeroMatrix(ZeroVector4(Cy, 0.0f, -Sy, 0.0f),
							ZeroVector4(0.0f, 1.0f, 0.0f, 0.0f),
							ZeroVector4(Sy, 0.0f, Cy, 0.0f),
							ZeroVector4(pos.x, pos.y, pos.z, 1.0f));
}

inline ZeroMatrix RollMatrix(
	float roll, 
	const ZeroVector3 &pos = ZeroVector4 (0.0f, 0.0f, 0.0f, 1.0f))
{
	float Sr = Zero_sin(roll);
	float Cr = Zero_cos(roll);

	return ZeroMatrix(ZeroVector4(Cr, Sr, 0.0f, 0.0f),
							ZeroVector4(-Sr, Cr, 0.0f, 0.0f),
							ZeroVector4(0.0f, 0.0f, 1.0f, 0.0f),
							ZeroVector4(pos.x, pos.y, pos.z, 1.0f));
}

inline ZeroMatrix PitchYawMatrix(
	float pitch, 
	float yaw, 
	const ZeroVector3 &pos = ZeroVector4 (0.0f, 0.0f, 0.0f, 1.0f))
{
	float Sp = Zero_sin(pitch);
	float Cp = Zero_cos(pitch);
	float Sy = Zero_sin(yaw);
	float Cy = Zero_cos(yaw);

	return ZeroMatrix(ZeroVector4(Cy, 0.0f, -Sy, 0.0f),
							ZeroVector4(Sy * Sp, Cp, Cy * Sp, 0.0f),
							ZeroVector4(Sy * Cp, -Sp, Cy * Cp, 0.0f),
							ZeroVector4(pos.x, pos.y, pos.z, 1.0f));
}

inline ZeroMatrix RollPitchYawMatrix(
	float roll, 
	float pitch, 
	float yaw, 
	const ZeroVector3 &pos = ZeroVector4 (0.0f, 0.0f, 0.0f, 1.0f))
{
	float Sr = Zero_sin(roll);
	float Cr = Zero_cos(roll);
	float Sp = Zero_sin(pitch);
	float Cp = Zero_cos(pitch);
	float Sy = Zero_sin(yaw);
	float Cy = Zero_cos(yaw);

	return ZeroMatrix(ZeroVector4(Sr * Sy * Sp + Cr * Cy, Sr * Cp, Sr * Cy * Sp - Cr * Sy, 0.0f),
							ZeroVector4(Cr * Sy * Sp - Sr * Cy, Cr * Cp, Cr * Cy * Sp + Sr * Sy, 0.0f),
							ZeroVector4(Sy * Cp, -Sp, Cy * Cp, 0.0f),
							ZeroVector4 (pos.x, pos.y, pos.z, 1.0f));
}

inline ZeroMatrix PitchYawRollMatrix(
	float pitch, 
	float yaw, 
	float roll, 
	const ZeroVector3 &pos = ZeroVector4 (0.0f, 0.0f, 0.0f, 1.0f))
{
	float Sp = Zero_sin(pitch);
	float Cp = Zero_cos(pitch);
	float Sy = Zero_sin(yaw);
	float Cy = Zero_cos(yaw);
	float Sr = Zero_sin(roll);
	float Cr = Zero_cos(roll);

	return ZeroMatrix(ZeroVector4(Cy * Cr, Cy * Sr, -Sy, 0.0f),
							ZeroVector4(Sp * Sy * Cr - Cp * Sr, Sp * Sy * Sr + Cp * Cr, Sp * Cy, 0.0f),
							ZeroVector4(Cp * Sy * Cr + Sp * Sr, Cp * Sy * Sr - Sp * Cr, Cp * Cy, 0.0f),
							ZeroVector4(pos.x, pos.y, pos.z, 1.0f));
}


inline ZeroMatrix DirectionMatrix(
	const ZeroVector3 &front, 
	const ZeroVector3 &up, 
	const ZeroVector3 &pos = ZeroVector4 (0.0f, 0.0f, 0.0f, 1.0f))
{
	ZeroVector3 f (Normalize(front));
	ZeroVector3 r (Normalize(CrossProduct(up, front)));
	ZeroVector3 u (CrossProduct(f, r));

	return ZeroMatrix(ZeroVector4(r.x, r.y, r.z, 0.0f),
							ZeroVector4(u.x, u.y, u.z, 0.0f),
							ZeroVector4(f.x, f.y, f.z, 0.0f),
							ZeroVector4(pos.x, pos.y, pos.z, 1.0f));
}

inline ZeroMatrix NormalMatrix(
	const ZeroVector3 &front, 
	const ZeroVector3 &up, 
	const ZeroVector3 &pos = ZeroVector4 (0.0f, 0.0f, 0.0f, 1.0f))
{
	ZeroVector3 u (Normalize(up));
	ZeroVector3 r (Normalize(CrossProduct(up, front)));
	ZeroVector3 f (CrossProduct(r, u));

	return ZeroMatrix(ZeroVector4(r.x, r.y, r.z, 0.0f),
							ZeroVector4(u.x, u.y, u.z, 0.0f),
							ZeroVector4(f.x, f.y, f.z, 0.0f),
							ZeroVector4(pos.x, pos.y, pos.z, 1.0f));
}

#if (PLATFORM == PLATFORM_PS2)
inline ZeroMatrix perspectiveMatrix(float fovy, float aspect, float znear, float zfar,float zoom)
{
	
	float xmin, xmax, ymin, ymax;
	float x, y, c, d; // r, b, a  // unused varbs removed NM 8/5/02

	ymax = (znear * sinf(fovy * 0.5f)) / cosf (fovy * 0.5f);;

	ymin = -ymax;

	xmin = ymin /** aspect*/;
	xmax = ymax /** aspect*/;

	x = ( 2.0f * znear * zoom ) / ( xmax - xmin );
	y = ( 2.0f * znear * zoom ) / ( ymax - ymin );
	c = ( zfar + znear ) / ( zfar - znear );
	d = -( 2.0f * zfar * znear ) / ( zfar - znear );

	return ZeroMatrix(ZeroVector4(x, 0.0f, 0.0f, 0.0f),
							ZeroVector4(0.0f, 0.0f, c, 1.0f),
							ZeroVector4(0.0f, -y , 0.0f, 0.0f),
							ZeroVector4(0.0f, 0.0f, d, 0.0));

}
#endif

void ZeroMatrixMultiply(ZeroMatrix &R, const ZeroMatrix &A, const ZeroMatrix &B);

extern const ZeroMatrix identityMatrix;
extern const ZeroMatrix emptyMatrix;

#endif
