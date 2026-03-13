//********************************************************************
// Direct geometry Hardware independent library
//	By Julio Jerez
// VC: 6.0
//********************************************************************
#ifndef __ZeroIntersections__
#define __ZeroIntersections__

#include "ZeroTypes.h"



class ZeroMatrix;
class ZeroVector3;

enum ZeroIntersectStatus
{
	stopSearch,
	continueSearch
};
typedef ZeroIntersectStatus (*ZeroIntersectCallback) (void *context, 
																		const int index[], int count, 
																		const float vertex[], int strideInBytes);



// calcucate ontirsection between to line segments in 3d space;
// note: in the lines do not do not define a plane the intersection
//			is the perpendicular to both segments 
DLL_DECLSPEC bool __cdecl ZeroLinesIntersect (
							const ZeroVector3& p0, const ZeroVector3& p1, float& t,
							const ZeroVector3& q0, const ZeroVector3& q1, float& s,
							float distToEdgeTolerance = 0);




//caculate the intersection between a line segment and a polygon
DLL_DECLSPEC bool __cdecl ZeroRayPolygonIntersect (
						   const ZeroVector3& ray_p0, 
							const ZeroVector3& ray_p1, 
						   float distToEdgeTolerance,
							const int index[],
							int indexCount,
							const float vertex[], 
							int strideInBytes,
						   float& tout, 
							ZeroVector3& unNormalizedNormal);



// calculate shortest distance from a point to a well define convex polygon
// (well defined mean that the polygon is strictly convex, all vertex angles < 180degree)
// return false if the point is in the negative side of the polygon
// return true otherwise, pointOut is set to a point is the polygon surface
DLL_DECLSPEC bool  __cdecl ZeroPointToPolygonDistance (
							const ZeroVector3& point, 
							const int index[],
							int indexCount,
							const float vertex[], 
							int strideInBytes,
							float bailDistance,
							ZeroVector3& pointOut);


// calculate the closest distance a line segment and a polygon
// return parameter t on line segment, 
DLL_DECLSPEC void __cdecl ZeroPointRayDistance (
						   const ZeroVector3& p,
							const ZeroVector3& l0, const ZeroVector3& l1,
							ZeroVector3& pointp0p1);


DLL_DECLSPEC void __cdecl ZeroRayRayDistance (
							const ZeroVector3& p0, const ZeroVector3& p1,
							const ZeroVector3& q0, const ZeroVector3& q1,
							ZeroVector3& pointp0p1,
							ZeroVector3& pointq0q1);


DLL_DECLSPEC bool  __cdecl ZeroRayPolygonDistance (
						   const ZeroVector3& ray_p0, 
							const ZeroVector3& ray_p1, 
							const int index[],
							int indexCount,
							const float vertex[], 
							int strideInBytes,
							ZeroVector3& pointInLine,
							ZeroVector3& pointInPolygon);



#endif
