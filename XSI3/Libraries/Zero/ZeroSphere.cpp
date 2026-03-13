#include "ZeroTypes.h"
#include "ZeroSphere.h"
#include "ZeroCamera.h"
#include "ZeroVector3.h"
#include "ZeroGeoTypes.h"
#include "ZeroMathTypes.h"
#include "ZeroRenderDescriptor.h"
#include "ZeroRenderState.h"

const ZeroSphere identitySphere;

//
// SPHERE-PRIVATE NAMESPACE
//
namespace InternalSphere
{
	const float ASPECT = 0.5f;
	const float STAT_EPSILON = 1.0e-6f;
	const float SPHERE_TOL = 0.002f;	

	static float AspectRatio (float x, float y)
	{
		float tmp;

		x = fabsf (x);
		y = fabsf (y);
		if (x > y) {
			tmp = x;
			x = y;
			y = tmp;
		}
		if (y < 1.0e-12f) {
			x = 0;
		}
		return x / y;
	}
	
	static void BoundingBox (
		const ZeroMatrix &Mat,
		const float *vertexArray, 
		int vertexStride, 
		const int *indexArray,
		int indexCount, 
		ZeroVector3 &Min, 
		ZeroVector3 &Max)
	{
		int i;
		int j;
		float xmin;
		float xmax;
		float ymin;
		float ymax;
		float zmin;
		float zmax;
		
		xmin = 1e10;
		ymin = 1e10;
		zmin = 1e10;
		
		xmax = -1e10;
		ymax = -1e10;
		zmax = -1e10;
		
		for (j = 0 ; j < indexCount; j ++ ) {
			i = indexArray[j] * vertexStride;
			ZeroVector3 Tmp (vertexArray[i + 0], vertexArray[i + 1], vertexArray[i + 2]);
			Tmp = Mat.UnrotateVector (Tmp);
			if (Tmp.x < xmin) xmin = Tmp.x;
			if (Tmp.y < ymin) ymin = Tmp.y;
			if (Tmp.z < zmin) zmin = Tmp.z;
			if (Tmp.x > xmax) xmax = Tmp.x;
			if (Tmp.y > ymax) ymax = Tmp.y;
			if (Tmp.z > zmax) zmax = Tmp.z;
		}
		
		Min = ZeroVector3 (xmin, ymin, zmin);
		Max = ZeroVector3 (xmax, ymax, zmax);
	}
	
	static void BoundingBoxFromTriStrips (
		const ZeroMatrix &Mat,
		const float *vertexArray, 
		int vertexStride, 
		const unsigned short *indexArray,
		int indexCount, 
		ZeroVector3 &Min, 
		ZeroVector3 &Max)
	{
		int i;
		int j;
		float xmin;
		float xmax;
		float ymin;
		float ymax;
		float zmin;
		float zmax;
		
		xmin = 1e10;
		ymin = 1e10;
		zmin = 1e10;
		
		xmax = -1e10;
		ymax = -1e10;
		zmax = -1e10;
		
		for (j = 0 ; j < indexCount; j ++ ) {
			i = indexArray[j] * vertexStride;
			ZeroVector3 Tmp (vertexArray[i + 0], vertexArray[i + 1], vertexArray[i + 2]);
			Tmp = Mat.UnrotateVector (Tmp);
			if (Tmp.x < xmin) xmin = Tmp.x;
			if (Tmp.y < ymin) ymin = Tmp.y;
			if (Tmp.z < zmin) zmin = Tmp.z;
			if (Tmp.x > xmax) xmax = Tmp.x;
			if (Tmp.y > ymax) ymax = Tmp.y;
			if (Tmp.z > zmax) zmax = Tmp.z;
		}
		
		Min = ZeroVector3 (xmin, ymin, zmin);
		Max = ZeroVector3 (xmax, ymax, zmax);
	}
	
	
	// Compute axis aligned box
	static void BoundingBox (
		const ZeroMatrix &Mat,
		const float *vertexArray, 
		int vertexStride,
		int vertexCount, 
		ZeroVector3 &Min, 
		ZeroVector3 &Max)
	{
		int i;
		float *Ptr;
		float xmin;
		float xmax;
		float ymin;
		float ymax;
		float zmin;
		float zmax;
		
		xmin = 1e10;
		ymin = 1e10;
		zmin = 1e10;
		
		xmax = -1e10;
		ymax = -1e10;
		zmax = -1e10;
		
		Ptr = (float*) vertexArray;
		for (i = 0 ; i < vertexCount; i ++ ) {
			ZeroVector3 Tmp (Ptr[0], Ptr[1], Ptr[2]);
			Ptr += vertexStride;
			Tmp = Mat.UnrotateVector (Tmp);
			if (Tmp.x < xmin) xmin = Tmp.x;
			if (Tmp.y < ymin) ymin = Tmp.y;
			if (Tmp.z < zmin) zmin = Tmp.z;
			if (Tmp.x > xmax) xmax = Tmp.x;
			if (Tmp.y > ymax) ymax = Tmp.y;
			if (Tmp.z > zmax) zmax = Tmp.z;
		}
		
		Min = ZeroVector3 (xmin, ymin, zmin);
		Max = ZeroVector3 (xmax, ymax, zmax);
	}
	
	
	static void Statistics (
		ZeroSphere &sphere,
		ZeroVector3 &eigenValues,
		ZeroVector3 &scaleVector,
		const float *vertexArray, 
		int vertexStride,
		const int *indexArray,
		int indexCount)
	{
#if (PLATFORM == PLATFORM_PS2)
	_ASSERTE(0); // We shouldn't be in here!
#else
		int i;
		int j;
		float x;
		float z;
		float y;
		double k;
		double Ixx;
		double Iyy;
		double Izz;
		double Ixy;
		double Ixz;
		double Iyz;

		ZeroVector3 massCenter (0, 0, 0);
		ZeroVector3 Var (0, 0, 0);
		ZeroVector3 Cov (0, 0, 0);
		
		for (i = 0; i < indexCount; i ++) {
			j = indexArray[i] * vertexStride;
			x = vertexArray[j + 0] * scaleVector.x;
			y = vertexArray[j + 1] * scaleVector.y;
			z = vertexArray[j + 2] * scaleVector.z;
			massCenter += ZeroVector3 (x, y, z);
			Var += ZeroVector3 (x * x, y * y, z * z);
			Cov += ZeroVector3 (x * y, x * z, y * z);
		}
		
		k = 1.0 / indexCount;
		Var *= (float)k;
		Cov *= (float)k;
		massCenter *= (float)k;
		
		Ixx = Var.x - massCenter.x * massCenter.x;
		Iyy = Var.y - massCenter.y * massCenter.y;
		Izz = Var.z - massCenter.z * massCenter.z;
		
		Ixy = Cov.x - massCenter.x * massCenter.y;
		Ixz = Cov.y - massCenter.x * massCenter.z;
		Iyz = Cov.z - massCenter.y * massCenter.z;
		
		sphere.right = ZeroVector3 ((float)Ixx, (float)Ixy, (float)Ixz);
		sphere.up	 = ZeroVector3 ((float)Ixy, (float)Iyy, (float)Iyz);
		sphere.front = ZeroVector3 ((float)Ixz, (float)Iyz, (float)Izz);
		sphere = sphere.EigenVectors (eigenValues);
#endif
	}
	
	
	static void Statistics (
		ZeroSphere &sphere,
		ZeroVector3 &eigenValues,
		ZeroVector3 &scaleVector,
		const float *vertexArray, 
		int vertexStride,
		int vertexCount)
	{
#if (PLATFORM == PLATFORM_PS2)
	_ASSERTE(0); // We shouldn't be in here!
#else
		int i;
		float *Ptr;
		float x;
		float z;
		float y;
		double k;
		double Ixx;
		double Iyy;
		double Izz;
		double Ixy;
		double Ixz;
		double Iyz;

		ZeroVector3 massCenter (0, 0, 0);
		ZeroVector3 Var (0, 0, 0);
		ZeroVector3 Cov (0, 0, 0);
		
		Ptr = (float*)vertexArray;
		for (i = 0; i < vertexCount; i ++) {
			x = Ptr[0] * scaleVector.x;
			y = Ptr[1] * scaleVector.y;
			z = Ptr[2] * scaleVector.z;
			Ptr += vertexStride;
			massCenter += ZeroVector3 (x, y, z);
			Var += ZeroVector3 (x * x, y * y, z * z);
			Cov += ZeroVector3 (x * y, x * z, y * z);
		}
		
		k = 1.0 / vertexCount;
		Var *= (float)k;
		Cov *= (float)k;
		massCenter *= (float)k;
		
		Ixx = Var.x - massCenter.x * massCenter.x;
		Iyy = Var.y - massCenter.y * massCenter.y;
		Izz = Var.z - massCenter.z * massCenter.z;
		
		Ixy = Cov.x - massCenter.x * massCenter.y;
		Ixz = Cov.y - massCenter.x * massCenter.z;
		Iyz = Cov.z - massCenter.y * massCenter.z;
		
		sphere.right = ZeroVector3 ((float)Ixx, (float)Ixy, (float)Ixz);
		sphere.up    = ZeroVector3 ((float)Ixy, (float)Iyy, (float)Iyz);
		sphere.front = ZeroVector3 ((float)Ixz, (float)Iyz, (float)Izz);
		sphere = sphere.EigenVectors (eigenValues);
#endif
	}
	
	static void Statistics (
		ZeroSphere &sphere,
		ZeroVector3 &eigenValues,
		const ZeroVector3 &scaleVector,
		const float *vertexArray, 
		int vertexStride,
		ZeroFace* const faceArray,
		int faceCount)
	{
#if (PLATFORM == PLATFORM_PS2)
	_ASSERTE(0); // We shouldn't be in here!
#else
		int i;
		float *Ptr;
		double K;
		double Ixx;
		double Iyy;
		double Izz;
		double Ixy;
		double Ixz;
		double Iyz;
		double Area;
		double Total_Area;
		
		ZeroVector3 Center (0, 0, 0);
		ZeroVector3 massCenter (0, 0, 0);
		ZeroVector3 Var (0, 0, 0);
		ZeroVector3 Cov (0, 0, 0);
		
		Total_Area = 0.0;
		Ptr = (float*)vertexArray;
		for (i = 0; i < faceCount; i ++)	{
			ZeroVector3 P0 (scaleVector.ComponentsProduct (ZeroVector3(&Ptr[faceArray[i].index[0] * vertexStride])));
			ZeroVector3 P1 (scaleVector.ComponentsProduct (ZeroVector3(&Ptr[faceArray[i].index[1] * vertexStride])));
			ZeroVector3 P2 (scaleVector.ComponentsProduct (ZeroVector3(&Ptr[faceArray[i].index[2] * vertexStride])));
			
			ZeroVector3 Normal (CrossProduct(P1 - P0, P2 - P0));
			
			Area = 0.5f * Magnitude (Normal);
			
			Center = P0 + P1 + P2;
			Center *= (1.0f /3.0f);
			
			Ixx = P0.x * P0.x + P1.x * P1.x + P2.x * P2.x;	
			Iyy = P0.y * P0.y + P1.y * P1.y + P2.y * P2.y;	
			Izz = P0.z * P0.z + P1.z * P1.z + P2.z * P2.z;	
			
			Ixy = P0.x * P0.y + P1.x * P1.y + P2.x * P2.y;	
			Iyz = P0.y * P0.z + P1.y * P1.z + P2.y * P2.z;	
			Ixz = P0.x * P0.z + P1.x * P1.z + P2.x * P2.z;	
			
			if (Area > STAT_EPSILON * 10.0)	{
				K = Area / 12.0;
				
				Ixx = K * (Ixx + 9.0 * Center.x * Center.x);
				Iyy = K * (Iyy + 9.0 * Center.y * Center.y);
				Izz = K * (Izz + 9.0 * Center.z * Center.z);
				
				Ixy = K * (Ixy + 9.0 * Center.x * Center.y);
				Ixz = K * (Ixz + 9.0 * Center.x * Center.z);
				Iyz = K * (Iyz + 9.0 * Center.y * Center.z);
				Center *= (float)Area;
			} 
			
			Total_Area += Area;
			massCenter += Center;
			Var += ZeroVector3 ((float)Ixx, (float)Iyy, (float)Izz);
			Cov += ZeroVector3 ((float)Ixy, (float)Ixz, (float)Iyz);
		}
		
		if (Total_Area > STAT_EPSILON * 10.0)	{
			K = 1.0 / Total_Area; 
			Var *= (float)K;
			Cov *= (float)K;
			massCenter *= (float)K;
		}
		
		Ixx = Var.x - massCenter.x * massCenter.x;
		Iyy = Var.y - massCenter.y * massCenter.y;
		Izz = Var.z - massCenter.z * massCenter.z;
		
		Ixy = Cov.x - massCenter.x * massCenter.y;
		Ixz = Cov.y - massCenter.x * massCenter.z;
		Iyz = Cov.z - massCenter.y * massCenter.z;
		
		sphere.right = ZeroVector3 ((float)Ixx, (float)Ixy, (float)Ixz);
		sphere.up    = ZeroVector3 ((float)Ixy, (float)Iyy, (float)Iyz);
		sphere.front = ZeroVector3 ((float)Ixz, (float)Iyz, (float)Izz);
		sphere = sphere.EigenVectors(eigenValues);
#endif
	}

	static void StatisticsFromTriStrips (
		ZeroSphere &sphere,
		ZeroVector3 &eigenValues,
		const ZeroVector3 &scaleVector,
		const float *vertexArray, 
		int vertexStride,
		const unsigned short*faceArray,
		int faceCount)
	{
#if (PLATFORM == PLATFORM_PS2)
	_ASSERTE(0); // We shouldn't be in here!
#else
		int i;
		float *Ptr;
		double K;
		double Ixx;
		double Iyy;
		double Izz;
		double Ixy;
		double Ixz;
		double Iyz;
		double Area;
		double Total_Area;

		ZeroVector3 Center (0, 0, 0);
		ZeroVector3 massCenter (0, 0, 0);
		ZeroVector3 Var (0, 0, 0);
		ZeroVector3 Cov (0, 0, 0);
		
		Total_Area = 0.0;
		Ptr = (float*)vertexArray;
		for (i = 0; i < faceCount; i ++)	{
			ZeroVector3 P0 (scaleVector.ComponentsProduct (ZeroVector3(&Ptr[faceArray[i] * vertexStride])));
			ZeroVector3 P1 (scaleVector.ComponentsProduct (ZeroVector3(&Ptr[faceArray[i+1] * vertexStride])));
			ZeroVector3 P2 (scaleVector.ComponentsProduct (ZeroVector3(&Ptr[faceArray[i+2] * vertexStride])));
			
			ZeroVector3 Normal (CrossProduct(P1 - P0, P2 - P0));
			
			Area = 0.5f * Magnitude (Normal);
			
			Center = P0 + P1 + P2;
			Center *= (1.0f /3.0f);
			
			Ixx = P0.x * P0.x + P1.x * P1.x + P2.x * P2.x;	
			Iyy = P0.y * P0.y + P1.y * P1.y + P2.y * P2.y;	
			Izz = P0.z * P0.z + P1.z * P1.z + P2.z * P2.z;	
			
			Ixy = P0.x * P0.y + P1.x * P1.y + P2.x * P2.y;	
			Iyz = P0.y * P0.z + P1.y * P1.z + P2.y * P2.z;	
			Ixz = P0.x * P0.z + P1.x * P1.z + P2.x * P2.z;	
			
			if (Area > STAT_EPSILON * 10.0)	{
				K = Area / 12.0;
				
				Ixx = K * (Ixx + 9.0 * Center.x * Center.x);
				Iyy = K * (Iyy + 9.0 * Center.y * Center.y);
				Izz = K * (Izz + 9.0 * Center.z * Center.z);
				
				Ixy = K * (Ixy + 9.0 * Center.x * Center.y);
				Ixz = K * (Ixz + 9.0 * Center.x * Center.z);
				Iyz = K * (Iyz + 9.0 * Center.y * Center.z);
				Center *= (float)Area;
			} 
			
			Total_Area += Area;
			massCenter += Center;
			Var += ZeroVector3 ((float)Ixx, (float)Iyy, (float)Izz);
			Cov += ZeroVector3 ((float)Ixy, (float)Ixz, (float)Iyz);
		}
		
		if (Total_Area > STAT_EPSILON * 10.0)	{
			K = 1.0 / Total_Area; 
			Var *= (float)K;
			Cov *= (float)K;
			massCenter *= (float)K;
		}
		
		Ixx = Var.x - massCenter.x * massCenter.x;
		Iyy = Var.y - massCenter.y * massCenter.y;
		Izz = Var.z - massCenter.z * massCenter.z;
		
		Ixy = Cov.x - massCenter.x * massCenter.y;
		Ixz = Cov.y - massCenter.x * massCenter.z;
		Iyz = Cov.z - massCenter.y * massCenter.z;
		
		sphere.right = ZeroVector3 ((float)Ixx, (float)Ixy, (float)Ixz);
		sphere.up    = ZeroVector3 ((float)Ixy, (float)Iyy, (float)Iyz);
		sphere.front = ZeroVector3 ((float)Ixz, (float)Iyz, (float)Izz);
		sphere = sphere.EigenVectors(eigenValues);
#endif
	}
}


void ZeroSphere::SetDimensions (
	const float *vertexArray, 
	int strideInBytes, 
	ZeroFace* const faceArray,
	int faceCount, 
	const ZeroMatrix *basis)
{
	int i;
	int j;
	int k;
	float aspect;

	ZeroVector3 scaleVector (1, 1, 1);
	
	int vertexStride = strideInBytes / sizeof (float);
	
	if (basis) {
		*this = *basis;
	} else {
		ZeroVector3 eigen;
		InternalSphere::Statistics (*this, eigen, scaleVector, vertexArray, vertexStride, faceArray, faceCount);
		
		k = 0;
		for (i = 0; i < 3; i ++) {
			if (k >= 6)	{
				break;
			} for (j = i + 1; j < 3; j ++) {
				aspect = InternalSphere::AspectRatio (eigen[i], eigen[j]);
				if (aspect > InternalSphere::ASPECT) {
					scaleVector[i] *= 2.0f; 
					InternalSphere::Statistics (*this, eigen, scaleVector, vertexArray, vertexStride, faceArray, faceCount);
					k ++;
					i = -1;
					break;
				}
			}
		}
	}
	
	// get minimum and maximum
	ZeroVector3 Min; 
	ZeroVector3 Max;
	InternalSphere::BoundingBox (*this, 
		vertexArray, vertexStride, (const int *)faceArray, faceCount * 3, Min, Max);
	
	// set center
	ZeroVector3 massCenter ((Max + Min) * 0.5f);
	posit = TransformVector (massCenter);
	
	// set dimensions
	ZeroVector3 dim ((Max - Min) * 0.5f);
	SetDimensions (
		dim.x + InternalSphere::SPHERE_TOL, 
		dim.y + InternalSphere::SPHERE_TOL, 
		dim.z + InternalSphere::SPHERE_TOL);
}

void ZeroSphere::SetDimensionsFromTriStrips (
	const float *vertexArray, 
	int strideInBytes, 
	const unsigned short *faceArray, 
	int faceCount, 
	const ZeroMatrix *basis)
{
	int i;
	int j;
	int k;
	float aspect;

	ZeroVector3 scaleVector (1, 1, 1);
	
	int vertexStride = strideInBytes / sizeof (float);
	
	if (basis) {
		*this = *basis;
	} else {
		ZeroVector3 eigen;
		InternalSphere::StatisticsFromTriStrips (*this, eigen, scaleVector, vertexArray, vertexStride, faceArray, faceCount);
		
		k = 0;
		for (i = 0; i < 3; i ++) {
			if (k >= 6)	{
				break;
			} for (j = i + 1; j < 3; j ++) {
				aspect = InternalSphere::AspectRatio (eigen[i], eigen[j]);
				if (aspect > InternalSphere::ASPECT) {
					scaleVector[i] *= 2.0f; 
					InternalSphere::StatisticsFromTriStrips (*this, eigen, scaleVector, vertexArray, vertexStride, faceArray, faceCount);
					k ++;
					i = -1;
					break;
				}
			}
		}
	}
	
	// get minimum and maximum
	ZeroVector3 Min; 
	ZeroVector3 Max;
	InternalSphere::BoundingBoxFromTriStrips (*this, 
		vertexArray, vertexStride, faceArray, faceCount, Min, Max);
	
	// set center
	ZeroVector3 massCenter ((Max + Min) * 0.5f);
	posit = TransformVector (massCenter);
	
	// set dimensions
	ZeroVector3 dim ((Max - Min) * 0.5f);
	SetDimensions (
		dim.x + InternalSphere::SPHERE_TOL, 
		dim.y + InternalSphere::SPHERE_TOL, 
		dim.z + InternalSphere::SPHERE_TOL);
}

void ZeroSphere::SetDimensions (
	const float *vertexArray, 
	int strideInBytes,
	int vertexCount,
	const ZeroMatrix *basis) 
{
	int i;
	int j;
	int k;
	int vertexStride;
	float aspect;

	ZeroVector3 scaleVector (1, 1, 1);
	
	vertexStride = strideInBytes / sizeof (float);
	
	if (basis) {
		*this = *basis;
	} else {
		ZeroVector3 eigen;
		InternalSphere::Statistics (*this, eigen, scaleVector, vertexArray, vertexStride, vertexCount);
		
		k = 0;
		for (i = 0; i < 3; i ++) {
			if (k >= 6)	{
				break;
			} 
			for (j = i + 1; j < 3; j ++)	{

				aspect = InternalSphere::AspectRatio (eigen[i], eigen[j]);
				if (aspect > InternalSphere::ASPECT) {
					scaleVector[i] *= 2.0f; 
					InternalSphere::Statistics (*this, eigen, scaleVector, vertexArray, vertexStride, vertexCount);
					k ++;
					i = -1;
					break;
				}
			}
		}
	}
	
	// get minimum and maximum
	ZeroVector3 Min; 
	ZeroVector3 Max;
	InternalSphere::BoundingBox (*this,
		vertexArray, vertexStride, vertexCount, Min, Max);
	
	// set center
	ZeroVector3 massCenter ((Max + Min) * 0.5f);
	posit = TransformVector (massCenter);
	
	// set dimensions
	ZeroVector3 dim ((Max - Min) * 0.5f);
	SetDimensions (
		dim.x + InternalSphere::SPHERE_TOL, 
		dim.y + InternalSphere::SPHERE_TOL, 
		dim.z + InternalSphere::SPHERE_TOL);
}

void ZeroSphere::SetDimensions (
	const float *vertexArray,  
	int strideInBytes,
	const int *indexArray,
	int indexCount,
	const ZeroMatrix *basis)
{
	int i;
	int j;
	int vertexStride;
	float aspect;

	ZeroVector3 scaleVector (1, 1, 1);
	
	vertexStride = strideInBytes / sizeof (float);
	
	if (basis) {
		// use the basis
		*this = *basis;
	} else {
		ZeroVector3 eigen;
		InternalSphere::Statistics (*this, eigen, scaleVector, vertexArray, vertexStride, indexArray, indexCount);
		
		
		int k = 0;
		for (i = 0; i < 3; i ++) {
			if (k >= 6)	{
				break;
			}
			for (j = i + 1; j < 3; j ++)	{
				aspect = InternalSphere::AspectRatio (eigen[i], eigen[j]);
				if (aspect > InternalSphere::ASPECT) {

					scaleVector[i] *= 2.0f; 
					InternalSphere::Statistics (*this, eigen, scaleVector, vertexArray, vertexStride, indexArray, indexCount);
					i = -1;
					k ++;
					break;
				}
			}
		}
	}
	
	// get minimum and maximum
	ZeroVector3 Min; 
	ZeroVector3 Max;
	InternalSphere::BoundingBox (*this, 
		vertexArray, vertexStride, indexArray, indexCount, Min, Max);
	
	// set center
	ZeroVector3 massCenter ((Max + Min) * 0.5f);
	posit = TransformVector (massCenter);
	
	// set dimensions
	ZeroVector3 dim ((Max - Min) * 0.5f);
	SetDimensions (
		dim.x + InternalSphere::SPHERE_TOL, 
		dim.y + InternalSphere::SPHERE_TOL, 
		dim.z + InternalSphere::SPHERE_TOL);
}

static ZeroRenderState renderStatesList[] =
{
	ZeroRenderState (ZERO_RENDERSTATE_ALPHABLENDENABLE, FALSE),
	ZeroRenderState (ZERO_RENDERSTATE_LIGHTING, FALSE),
	ZeroRenderState (ZERO_RENDERSTATE_SRCBLEND, ZERO_BLEND_ONE),
	ZeroRenderState (ZERO_RENDERSTATE_DESTBLEND, ZERO_BLEND_ZERO),
	ZeroRenderState (ZERO_RENDERSTATE_ZENABLE, TRUE),
	ZeroRenderState (ZERO_RENDERSTATE_ZWRITEENABLE, TRUE),
//   ZeroRenderState (ZERO_RENDERSTATE_ZFUNC, ZERO_COMPARE_ALWAYS),
	ZeroRenderState (ZERO_RENDERSTATE_END, 0),
};

void ZeroSphere::DebugRender(
	const ZeroCamera* cameraArg, 
	const ZeroMatrix& worldMatrix, 
	ZeroColor color) const
{
	struct ColorVertex
	{
		float x;
		float y;
		float z;
		ZeroColor color;
	};
	ColorVertex box[8];
	ZeroCamera* camera; 

	camera = (ZeroCamera*) cameraArg; 

	unsigned short index [][2] = {
		{0, 4}, {1, 5}, {2, 6}, {3, 7},
		{0, 1}, {4, 5}, {7, 6}, {3, 2},
		{1, 2}, {5, 6}, {4, 7}, {0, 3},
	};

	// set the world matrix
	ZeroMatrix tmpMat (*this * worldMatrix);
	const ZeroMatrix *matrix = &tmpMat;
	camera->SetWorldMatrix (1, &matrix);

	// create the render descriptor
	ZeroRenderDescriptor desc (camera);
	desc.primitive = ZERO_PRIMITIVE_LINELIST;
	desc.format = ZERO_FORMAT_DIFFUSE;
	desc.flags = ZERO_RENDERFLAG_CLIP;
	desc.material = NULL;
#if (PLATFORM == PLATFORM_PS2)
	desc.vertexCount = sizeof(index) / sizeof(short);
	desc.indexCount = 0;
#else
	desc.vertexCount = 8;
	desc.indexCount = sizeof(index) / sizeof(short);
#endif

	// lock the render descriptor
	camera->Lock(desc, true);

#if (PLATFORM == PLATFORM_PS2)
	ZeroVector3 posit [] = {
		ZeroVector3(-size.x, -size.y, -size.z),
		ZeroVector3( size.x, -size.y, -size.z),
		ZeroVector3( size.x, -size.y,  size.z),
		ZeroVector3(-size.x, -size.y,  size.z),
		ZeroVector3(-size.x,  size.y, -size.z),
		ZeroVector3( size.x,  size.y, -size.z),
		ZeroVector3( size.x,  size.y,  size.z),
		ZeroVector3(-size.x,  size.y,  size.z),
	};
	for (int i = 0; i < desc.vertexCount / 2; i++)
	{
		ZeroVector3 v0(posit[index[i][0]]);
		ZeroVector3 v1(posit[index[i][1]]);
		desc.vertexRecord.AddVertexPos(v0.x, v0.y, v0.z, false);
		desc.vertexRecord.AddVertexColor(color);
		desc.vertexRecord.AddVertexPos(v1.x, v1.y, v1.z, true);
		desc.vertexRecord.AddVertexColor(color);
	}
	desc.vertexRecord.RewindPointersBy(desc.vertexCount);
#else
	// fill in the render descriptor
	desc.vertexRecord.AddVertexPos(-size.x, -size.y, -size.z);
	desc.vertexRecord.AddVertexColor(color);

	desc.vertexRecord.AddVertexPos( size.x, -size.y, -size.z);
	desc.vertexRecord.AddVertexColor(color);

	desc.vertexRecord.AddVertexPos( size.x, -size.y,  size.z);
	desc.vertexRecord.AddVertexColor(color);

	desc.vertexRecord.AddVertexPos(-size.x, -size.y,  size.z);
	desc.vertexRecord.AddVertexColor(color);

	desc.vertexRecord.AddVertexPos(-size.x,  size.y, -size.z);
	desc.vertexRecord.AddVertexColor(color);

	desc.vertexRecord.AddVertexPos( size.x,  size.y, -size.z);
	desc.vertexRecord.AddVertexColor(color);

	desc.vertexRecord.AddVertexPos( size.x,  size.y,  size.z);
	desc.vertexRecord.AddVertexColor(color);

	desc.vertexRecord.AddVertexPos(-size.x,  size.y,  size.z);
	desc.vertexRecord.AddVertexColor(color);

	memcpy(desc.indexArray, &index[0][0], sizeof(index));
#endif

	// unlock the render descriptor
	camera->Unlock(desc);

	// set the render state
	camera->SetRenderStates(renderStatesList);
	camera->SetTextureStages(0, NULL, NULL);

	// render
	camera->Render (desc);
}


float ZeroSphere::RayBoxHit (
	const ZeroVector3& p0, 
	const ZeroVector3& p1, 
	ZeroVector3& unnormalizeNormal) const
{
	// get the local position
	ZeroVector3 p (UntransformVector(p0));

	// get the local direction
	ZeroVector3 d (UnrotateVector(p1 - p0));

	// time to intersection
	float t;

	// position of intersection
	float hitx, hity, hitz;

	// if not parallel to the x plane...
	if (fabsf(d.x) > EPSILON)
	{
		// get time to intersection for the x plane
		if (d.x < 0.0f)
			t = (-p.x + size.x) / (d.x);
		else
			t = (-p.x - size.x) / (d.x);

		// if the time is not negative...
		if (t >= 0.0f)
		{
			// get intersection position
			hity = p.y + d.y * t;
			hitz = p.z + d.z * t;

			// if the intersection is inside the face...
			if ((hity >= -size.y) && (hity <= size.y) &&
				(hitz >= -size.z) && (hitz <= size.z))
			{
				// return the face normal
				unnormalizeNormal = ZeroVector3((d.x < 0.0f) ? 1.0f : -1.0f, 0.0f, 0.0);

				// return the intersection time
				return t;
			}
		}
	}

	// if not parallel to the y plane...
	if (fabsf(d.y) > EPSILON)
	{
		// get time to intersection for the y plane
		if (d.y < 0.0f)
			t = (-p.y + size.y) / (d.y);
		else
			t = (-p.y - size.y) / (d.y);

		// if the time is not negative...
		if (t >= 0.0f)
		{
			// get intersection position
			hitx = p.x + d.x * t;
			hitz = p.z + d.z * t;

			// if the intersection is inside the face...
			if ((hitx >= -size.x) && (hitx <= size.x) &&
				(hitz >= -size.z) && (hitz <= size.z))
			{
				// return the face normal
				unnormalizeNormal = ZeroVector3(0.0f, (d.y < 0.0f) ? 1.0f : -1.0f, 0.0f);

				// return the intersection time
				return t;
			}
		}
	}

	// if not parallel to the z plane...
	if (fabsf(d.z) > EPSILON)
	{
		// get time to intersection for the z plane
		if (d.z < 0.0f)
			t = (-p.z + size.z) / (d.z);
		else
			t = (-p.z - size.z) / (d.z);

		// if the time is not negative...
		if (t >= 0.0f)
		{
			// get intersection position
			hitx = p.x + d.x * t;
			hity = p.y + d.y * t;

			// if the intersection is inside the face...
			if ((hitx >= -size.x) && (hitx <= size.x) &&
				(hity >= -size.y) && (hity <= size.y))
			{
				// return the face normal
				unnormalizeNormal = ZeroVector3(0.0f, 0.0f, (d.z < 0.0f) ? 1.0f : -1.0f);

				// return the intersection time
				return t;
			}
		}
	}

	// return the intersection time
	return 1.1f;
}

float ZeroSphere::RaySphereHit (
	const ZeroVector3& p0Arg, 
	const ZeroVector3& p1Arg, 
	ZeroVector3& unnormalizeNormal) const
{
	float a;
	float b;
	float c;
	float t;
	float discrim;

	ZeroVector3 p0 (p0Arg - posit); 
	ZeroVector3 p1 (p1Arg - posit);  
	ZeroVector3 p10 (p1 - p0); 

	a = DotProduct (p10, p10);
	b = 2.0f * DotProduct (p0, p10);
	c = DotProduct (p0, p0) - size.w * size.w;

	discrim = b * b - 4.0f * a * c;
	if (discrim < 0.0f) {
		return 1.1f;
	}

	discrim = sqrtf (discrim);

	t = 0.5f * GetMin (discrim - b, - discrim - b) / a;
	if (t < 0) {
		return 1.1f;
	}

	_ASSERTE (t <= 1.0000001f);

	unnormalizeNormal	= p0 + (p1Arg - p0Arg) * t;
	return t;
}

bool ZeroSphere::RayBoxTest (const ZeroVector3& p0, 
							 const ZeroVector3& p1) const
{
    float fR;

    ZeroVector3 fD = 0.5f * (p1 - p0);
    ZeroVector3 fP = 0.5f * (p1 + p0) - posit;

    float fDdR = fabsf(DotProduct(fD, right));
    float fPdR = fabsf(DotProduct(fP, right));
    fR = size.x + fDdR;
    if (fPdR > fR)
        return false;

    float fDdU = fabsf(DotProduct(fD, up));
    float fPdU = fabsf(DotProduct(fP, up));
    fR = size.y + fDdU;
    if (fPdU > fR)
        return false;

    float fDdF = fabsf(DotProduct(fD, front));
    float fPdF = fabsf(DotProduct(fP, front));
    fR = size.z + fDdF;
    if (fPdF > fR)
        return false;

    ZeroVector3 kDxP (CrossProduct(fD, fP));

    float fDxPdR = fabsf(DotProduct(kDxP, right));
    fR = size.y*fDdF + size.z*fDdU;
    if (fDxPdR > fR)
        return false;

    float fDxPdU = fabsf(DotProduct(kDxP, up));
    fR = size.x*fDdF + size.z*fDdR;
    if (fDxPdU > fR)
        return false;

    float fDxPdF = fabsf(DotProduct(kDxP, front));
    fR = size.x*fDdU + size.y*fDdR;
    if (fDxPdF > fR)
        return false;

    return true;
}

bool ZeroSphere::RaySphereTest (const ZeroVector3& p0, 
								 const ZeroVector3& p1) const
{
    ZeroVector3 kDiff = posit - p0;
    float fT = DotProduct(kDiff, p1 - p0);

    if (fT > 0.0f)
    {
        float fSqrLen = DistanceSq(p0, p1);
        if (fT >= fSqrLen)
        {
            kDiff -= p1 - p0;
        }
        else
        {
            kDiff -= (p1 - p0) * fT / fSqrLen;
        }
    }

    return MagnitudeSq(kDiff) < size.w * size.w;
}

