/****************************************************************************
*
*  Visual C++ 6.0 created by: Julio Jerez
*
****************************************************************************/
#ifndef __ZeroSphere__
#define __ZeroSphere__

#include "ZeroTypes.h"
#include "ZeroPlane.h"
#include "ZeroMatrix.h"

class ZeroColor;
class ZeroFace;
class ZeroCamera;
//class ZeroRenderer;

// sphere class declaration
class ZeroSphere: public ZeroMatrix
{
	public:
	ZeroVector4 size;

	// sphere constructors
	ZeroSphere(void)
		: ZeroMatrix(), size(0, 0, 0, 0)
	{
	}
	ZeroSphere(const ZeroMatrix &matrix, const ZeroVector4 &sizeArg = ZeroVector4(0, 0, 0, 0))
		: ZeroMatrix(matrix) 
	{
		SetDimensions (sizeArg.x, sizeArg.y, sizeArg.z);
	}
	ZeroSphere(const ZeroQuaternion &rotation, const ZeroVector3 &position, const ZeroVector4 &sizeArg = ZeroVector4(0, 0, 0, 0))
		: ZeroMatrix(rotation, position)
	{
		SetDimensions (sizeArg.x, sizeArg.y, sizeArg.z);
	}
	ZeroSphere (const ZeroSphere &sphere)
		: ZeroMatrix(sphere), size(sphere.size)
	{
	}

	// sphere assignment
	ZeroSphere &operator=(const ZeroMatrix &m)
	{
		right = m.right;
		up	   = m.up;
		front = m.front;
		posit = m.posit;
		return *this;
	}

	// sphere transformation
	friend ZeroSphere __cdecl operator*(const ZeroSphere &a, const ZeroMatrix &b)
	{
		return ZeroSphere(((const ZeroMatrix &)a) * b, a.size);
	}

	// world-aligned bounds of the sphere
	void GetExtents (ZeroVector3 &pmin, ZeroVector3 &pmax) const
	{
		float w = size.x * fabsf(right.x) + size.y * fabsf(up.x) + size.z * fabsf(front.x);
		float h = size.x * fabsf(right.y) + size.y * fabsf(up.y) + size.z * fabsf(front.y);
		float b = size.x * fabsf(right.z) + size.y * fabsf(up.z) + size.z * fabsf(front.z);

		pmin.x = posit.x - w;
		pmin.y = posit.y - h;
		pmin.z = posit.z - b;

		pmax.x = posit.x + w;
		pmax.y = posit.y + h;
		pmax.z = posit.z + b;
	}

	// set dimensions
	void SetDimensions (float W, float H, float D)
	{
		size.x = W;
		size.y = H;
		size.z = D;
		size.w = sqrtf (DotProduct (size, size));
	}
	DLL_DECLSPEC void SetDimensions (const float *vertexArray, int strideInBytes, ZeroFace* const faceArray, int faceCount, const ZeroMatrix *basis = NULL);
	DLL_DECLSPEC void SetDimensions (const float *vertexArray, int strideInBytes, int vertexCount, const ZeroMatrix *basis = NULL);
	DLL_DECLSPEC void SetDimensions (const float *vertexArray, int strideInBytes, const int *indexArray, int indexCount, const ZeroMatrix *basis = NULL);
	DLL_DECLSPEC void SetDimensionsFromTriStrips (const float *vertexArray, int strideInBytes, const unsigned short *indexArray, int indexCount, const ZeroMatrix *basis = NULL);

	DLL_DECLSPEC void DebugRender(const ZeroCamera* camera, const ZeroMatrix& worldMatrix, ZeroColor color) const;

	// scale
	void Scale (float Ws, float Hs, float Ds) 
	{
		SetDimensions (size.x * Ws, size.y * Hs, size.z * Ds);
	}

	DLL_DECLSPEC float RayBoxHit (const ZeroVector3& p0, const ZeroVector3& p1, ZeroVector3& unnormalizeNormal) const;
	DLL_DECLSPEC float RaySphereHit (const ZeroVector3& p0, const ZeroVector3& p1, ZeroVector3& unnormalizeNormal) const;

	DLL_DECLSPEC bool RayBoxTest (const ZeroVector3& p0, const ZeroVector3& p1) const;
	DLL_DECLSPEC bool RaySphereTest (const ZeroVector3& p0, const ZeroVector3& p1) const;
	
	int SpherePlaneTest(const ZeroPlane &plane) const
	{
		// get the radius along the normal
		float dR = size.w;

		// get the position along the normal
		float R = plane.Evalue(posit);
		
		// get the sign of the R + dR plane test
		FLOATSIGN flag0;
		flag0.f = R + dR;
		flag0.i = (flag0.i >> 31) & 1;

		// get the sign of the R - dR plane test
		FLOATSIGN flag1;
		flag1.f = R - dR;
		flag1.i = (flag1.i >> 31) & 1;

		// -1: completely on the negative side of the plane
		// 0: completely on the positive side of the plane
		// 1: intersecting the plane
		return -(flag0.i & flag1.i) | flag0.i | flag1.i;
	}

	int SphereVolumeTest(const ZeroPlane planes[], int count) const
	{
		// for each plane...
		int code = 0;
		for (int i = 0; i < count; i++)
		{
			// perform the plane test
			code |= SpherePlaneTest(planes[i]);
			if (code < 0)
				break;
		}

		// return the code
		return code;
	}

	int BoxPlaneTest(const ZeroPlane &plane) const
	{
		// get the radius along the normal
		float dR = 
			size.x * fabsf (DotProduct(right, plane)) + 
			size.y * fabsf (DotProduct(up, plane)) +
			size.z * fabsf (DotProduct(front, plane));

		// get the position along the normal
		float R = plane.Evalue(posit);
		
		// get the sign of the R + dR plane test
		FLOATSIGN flag0;
		flag0.f = R + dR;
		flag0.i = (flag0.i >> 31) & 1;

		// get the sign of the R - dR plane test
		FLOATSIGN flag1;
		flag1.f = R - dR;
		flag1.i = (flag1.i >> 31) & 1;

		// -1: completely on the negative side of the plane
		// 0: completely on the positive side of the plane
		// 1: intersecting the plane
		return -(flag0.i & flag1.i) | flag0.i | flag1.i;
	}

	int BoxVolumeTest(const ZeroPlane planes[], int count) const
	{
		// for each plane...
		int code = 0;
		for (int i = 0; i < count; i++)
		{
			// perform the plane test
			code |= BoxPlaneTest(planes[i]);
			if (code < 0)
				break;
		}

		// return the code
		return code;
	}
}; 

extern const ZeroSphere identitySphere;

#endif
