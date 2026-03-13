/****************************************************************************
*
*  Visual C++ 6.0 created by: Julio Jerez
*
****************************************************************************/
#ifndef __ZeroGeoUtil__
#define __ZeroGeoUtil__

#include "ZeroGeoTypes.h"

class ZeroPlane;
class ZeroPolyhedra;


DLL_DECLSPEC ZeroPolyhedra* __cdecl CreateConvexHull (
	float *const pool, 
	int strideInBytes, 
	int vertexCount); 


DLL_DECLSPEC ZeroPolyhedra* __cdecl CreateConvexHull (
	const ZeroPlane& plane,
	float *const pool, 
	int strideInBytes, 
	int vertexCount); 


DLL_DECLSPEC int __cdecl VertexListToIndexList (
	float * const vertList,		// pointer to vertex structure
	const int strideInBytes,	// size of vertex structure in bytes
	const int vertexCount,		// number of vertex structures
	unsigned * const indexList,	// output index list
	const int looseInBytes,		// size of block of "loose" comparisons
	const float tolerance);		// "loose" comparison tolerance

DLL_DECLSPEC void __cdecl ZeroSavexMesh (
	const char *name, 
	const float *vertex,
	const int strideInBytes,
	const int vertexCount, 
	const ZeroFace *face,
	const int faceCount);


// convert multi triangle strips index array to a triangle list;
// note: indexTriangleArrayOut must be big enoght to hold the triagle array
// agood upper bound is (indexCount * 3);

// return triangle count
DLL_DECLSPEC int __cdecl TriangleStripToTriangleList (int indexCount, const unsigned short indexStripArray[], unsigned short indexTriangleArrayOut[]);



#endif
