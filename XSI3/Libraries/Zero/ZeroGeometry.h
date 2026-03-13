/****************************************************************************
*
*  Visual C++ 6.0 created by: Julio Jerez
*
****************************************************************************/
#ifndef __ZeroGeometry__
#define __ZeroGeometry__

#include "ZeroTypes.h"
#include "ZeroRtti.h"
#include "ZeroUnknown.h"
#include "ZeroList.h"
#include "ZeroCamera.h"
#include "ZeroMatrix.h"
#include "ZeroSphere.h"
#include "ZeroGeoTypes.h"


class ZeroObject;
class ZeroSegment;
class ZeroClothData;



class ZeroSegmentList: public ZeroVisList<ZeroSegment*>
{
	public:
	ZeroSegmentList()
		: ZeroVisList<ZeroSegment *>()
	{
	}
};

class ZeroGeometryData
{
	public:
	unsigned modelType;
	int envelopsCount;
	int* envelopsMap; 
	ZeroClothData* clothData;
	ZeroGeometry* dataGeometry;

	ZeroGeometryData()
	{
		modelType = 0;
		envelopsCount = 0;
		envelopsMap = NULL; 
		dataGeometry = NULL;
		clothData = NULL;

	}
};


class ZeroGeometry: public ZeroUnknown
{

	void EnumerateByType (ZeroObject *me, int& count, const ZeroObject** objectsOut, unsigned typeId) const;

	ZERO_RTTI(ZeroUnknown);

	protected:
	static int maxSegmentFaceSize;
	static int maxSegmentVertexSize;
	
	DLL_DECLSPEC ~ZeroGeometry ()
	{
	}
	
	public:
	ZeroGeometry(const char *name)
		:ZeroUnknown(name) 
	{
	}

	ZeroGeometry(const ZeroGeometryData &data)
		:ZeroUnknown() 
	{
	}

	
	DLL_DECLSPEC ZeroGeometry(const ZeroGeometry &geometry)
		: ZeroUnknown(geometry)
	{
	}


	ZeroUnknown *CreateClone(bool isSim)
	{
		AddRef();
		return this;
	}

	virtual void CloneFixUp (const ZeroObject& myOwner, const ZeroObject& myClone)
	{
	}

	
	//shadow volume
//	virtual ZeroUnknown* GetShadowVolume(){ return NULL;};
//	virtual void CreateShadowVolume() { };
	virtual void SetShadowLight(ZeroVector3 &Light) { };
	
	static DLL_DECLSPEC void __cdecl SetSegmentVertexAndFaceSize (int vertexSize, int faceSize);
	static DLL_DECLSPEC void __cdecl GetSegmentVertexAndFaceSize (int& vertexSize, int& faceSize);

	virtual ZeroSegmentList *GetSegments()
	{
		return NULL;
	}


	virtual void AddSegment (ZeroSegment *Segment)
	{
	}

	virtual void RemoveSegment (ZeroSegment *Segment)
	{
	}

	virtual void PackVertexFormat()
	{
	}

	virtual int TrivialRejection (
		const ZeroCamera *camera, 
		const ZeroMatrix &world,
		const unsigned flags) const
	{
		return -1;
	}

	virtual void TotalVertexAndFaceCount(int& faceCount, int& vertexCount) const
	{
		faceCount = 0;
		vertexCount = 0;
	}
	
	virtual void FlatFaceArray (const ZeroMatrix &matrix,
		float* const vertex, int strideInBytes, int &vCount, int maxVertex, 
		ZeroFace* const index, int &iCount, int maxFaces) const
	{
		vCount = 0;
		iCount = 0;
	}
	
	virtual bool SphereDirty()
	{
		return true;
	}

	virtual const ZeroSphere &GetSphere()
	{
		return identitySphere;
	}

	virtual void SetSphere(const ZeroSphere &arg)
	{
	}

	virtual void SetSphere()
	{
	}

	virtual void Scale(float scale)
	{
	}


	virtual void GetExtents(ZeroVector3 &min, ZeroVector3 &max, const ZeroMatrix& worldMatrix)
	{
	}

	//LOD functionality
	virtual bool SetLOD(float aDist)
	{
		return true;
	}


	virtual void SetSegmentShaderFunction (ZeroSegmentShaderCallback fnt)
	{
		_ASSERTE (0);
	}

	virtual ZeroSegmentShaderCallback GetSegmentShaderFunction () const
	{
		_ASSERTE (0);
		return NULL;
	}


	virtual void SetItemCustomParameterCallback (ZeroSegmentShaderCustomParametersCallback fnt)
	{
		_ASSERTE (0);
	}

	virtual ZeroSegmentShaderCustomParametersCallback GetItemCustomParameterCallback()
	{
		_ASSERTE (0);
		return NULL;
	}


	virtual void Render (const ZeroObject *owner, ZeroCamera *camera, const ZeroMatrix &world, unsigned flags = 0) const
	{
		_ASSERTE (0);
	}


	virtual void ApplyTransform(const ZeroMatrix& aMatrix);

	int EnumerateByType (ZeroObject *me, const ZeroObject** objectsOut, unsigned typeId) const
	{	
		int count;
		count = 0;
		EnumerateByType (me, count, objectsOut, typeId);
		return count;
	}



//	virtual void DebugRender (ZeroCamera *camera, const ZeroMatrix& worldMatrix, ZeroColor color) const
//	{
//	}
//	virtual void DebugRender (ZeroCamera *camera, const ZeroMatrix& worldMatrix,ZeroMatrix* matrixStack,unsigned count, ZeroColor color) const
//	{
//	}

	static float sRenderDistance;
};

#endif
