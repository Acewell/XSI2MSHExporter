/****************************************************************************
*
*  File Name  : Bitmap.C
*  Visual C++ 4.0 base by Julio Jerez
*
****************************************************************************/
#ifndef __ZeroAABB__
#define __ZeroAABB__

#include "ZeroTypes.h"
#include "ZeroPolySoups.h"


class ZeroAABBTree;

class ZeroAABB: public ZeroPolySoup
{
	int m_vertexCount;
	int m_indexCount;

	ZeroVector3* m_vertexPool;
	int *m_indexPool;

	ZeroVector3 m_scale;
	ZeroAABBTree* m_aabb;

	ZERO_RTTI(ZeroPolySoup);

	protected:
	DLL_DECLSPEC void Create___ (const ZeroPolySoupBuilder& builder, void (*meterFunc)(const int addCur, const int addMax) = NULL);

	DLL_DECLSPEC ~ZeroAABB ();

	public:
	DLL_DECLSPEC ZeroAABB ();

	int GetIndexCount() const
	{
		return m_indexCount;
	}

	int* GetIndexPool() const
	{
		return m_indexPool;
	}

	void Create (const ZeroPolySoupBuilder& builder, void (*meterFunc)(const int addCur, const int addMax))
	{
		Create___ (builder, meterFunc);
	}

	DLL_DECLSPEC virtual bool Load___ (ZeroFile& file);
	DLL_DECLSPEC virtual void Save___ (ZeroFile& file) const;
	DLL_DECLSPEC virtual void DebugSave (ZeroFile& file) const;
	DLL_DECLSPEC virtual unsigned GetPolygonId (const int polyIndex[])
	{
		return polyIndex[-1];
	}


	DLL_DECLSPEC virtual void ForAllSectors (const ZeroVector3& boxMin, const ZeroVector3& boxMax, ZeroIntersectCallback callback, void* context) const;
	DLL_DECLSPEC virtual void ForAllSectorsRayHit (const ZeroVector3& p0, const ZeroVector3& p1, ZeroIntersectCallback callback, void* context) const;
};




#endif

