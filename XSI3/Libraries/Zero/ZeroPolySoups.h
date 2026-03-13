/****************************************************************************
*
*  File Name  : Bitmap.C
*  Visual C++ 4.0 base by Julio Jerez
*
****************************************************************************/
#ifndef __ZeroPolySoup__
#define __ZeroPolySoup__

#include "ZeroTypes.h"
#include "ZeroFile.h"
#include "ZeroSets.h"
#include "ZeroUnknown.h"
#include "ZeroGeoTypes.h"
#include "ZeroIntersections.h"



class ZeroFile;
class ZeroColor;
class ZeroMatrix;
class ZeroCamera;
class ZeroVector3;

class ZeroPolySoupBuilder 
{
	public:
	struct VertexArray: public ZeroSet<ZeroVector3>
	{
		VertexArray() :ZeroSet<ZeroVector3>(512){}
	};

	struct IndexArray: public ZeroSet<int>
	{
		IndexArray() :ZeroSet<int>(512) {}
	};


	int m_faceCount;
	int m_vertexCount;
	int m_indexCount;
	IndexArray m_faceArray;
	IndexArray m_indexArray;
	VertexArray	m_vertexArray;


	DLL_DECLSPEC ZeroPolySoupBuilder ();
	DLL_DECLSPEC void Begin();
	DLL_DECLSPEC void End();
	DLL_DECLSPEC void AddMesh (const float vertex[], int vertexCount, int strideInBytes, 
							   int faceCount, const int faceIndexCoun[], 
							   const int faceID[], const int faceIndices[], 
							   const ZeroMatrix& worldMatrix);
};


class ZeroPolySoup: public ZeroUnknown
{
	ZERO_RTTI(ZeroUnknown);

	protected:
	DLL_DECLSPEC virtual void Create___ (const ZeroPolySoupBuilder& builder, void (*meterFunc)(const int addCur, const int addMax) = NULL) = 0;

	DLL_DECLSPEC ~ZeroPolySoup ();


	public:
	DLL_DECLSPEC ZeroPolySoup ();

	DLL_DECLSPEC virtual bool Load___ (ZeroFile& file) = 0;
	DLL_DECLSPEC virtual void Save___ (ZeroFile& file) const = 0;
	DLL_DECLSPEC virtual void DebugSave (ZeroFile& file) const = 0;
	DLL_DECLSPEC virtual unsigned GetPolygonId (const int polyIndex[]) { return 0;}

	DLL_DECLSPEC virtual void ForAllSectors (const ZeroVector3& boxMin, const ZeroVector3& boxMax, ZeroIntersectCallback callback, void* context) const = 0;
	DLL_DECLSPEC virtual void ForAllSectorsRayHit (const ZeroVector3& p0, const ZeroVector3& p1, ZeroIntersectCallback callback, void* context) const = 0;
	DLL_DECLSPEC virtual float RayHit (const ZeroVector3& p0, const ZeroVector3& p1, ZeroVector3& unNormalizeNormal) const;
	DLL_DECLSPEC virtual float RayTest (const ZeroVector3& p0, const ZeroVector3& p1) const;
	DLL_DECLSPEC virtual void DebugRender (const ZeroCamera* camera, const ZeroMatrix& worldMatrix, ZeroColor color) const;
};



struct ZeroPolySoupRayHit
{
	ZeroVector3 p0;
	ZeroVector3 p1;
	ZeroVector3 normal;
	float t;

	ZeroPolySoupRayHit (const ZeroVector3& p0Arg, const ZeroVector3& p1Arg)
		:p0 (p0Arg), p1 (p1Arg), normal (0.0f, 1.0f, 0.0f)
	{
		t = 1.1f;
	}


	static ZeroIntersectStatus RayHit  (
		void *context, 
		const int indexArray[], 
		int indexCount,
		const float polygon[], 
		int strideInBytes)
	{	
		float t;
		ZeroVector3	normal;
		const float LEAF_EDGE_PADDING = -0.001f;


		ZeroPolySoupRayHit& me = *(ZeroPolySoupRayHit*) context;
		if (ZeroRayPolygonIntersect (me.p0, me.p1, LEAF_EDGE_PADDING, indexArray, indexCount,
											  polygon, strideInBytes,  t, normal)) {
			if (t < me.t) {
				me.t = t;
				me.normal = normal;
			}
		}

		return continueSearch;
	}

	static ZeroIntersectStatus RayTest  (
		void *context, 
		const int indexArray[], 
		int indexCount,
		const float polygon[], 
		int strideInBytes)
	{	
		float t;
		ZeroVector3	normal;
		const float LEAF_EDGE_PADDING = -0.001f;

		ZeroPolySoupRayHit& me = *(ZeroPolySoupRayHit*) context;
		if (ZeroRayPolygonIntersect (me.p0, me.p1, LEAF_EDGE_PADDING, indexArray, indexCount,
											  polygon, strideInBytes, t, normal)) {
			me.t = t;
			return stopSearch;
		}

		return continueSearch;
	}
};




#endif

