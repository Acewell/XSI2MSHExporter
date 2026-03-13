/****************************************************************************
*
*  Visual C++ 6.0 created by: Julio Jerez
*
****************************************************************************/
#ifndef __ZeroSegment__
#define __ZeroSegment__

#include "ZeroTypes.h"
#include "ZeroUnknown.h"
#include "ZeroRenderDescriptor.h"
#include "ZeroSphere.h"
#include "Msh.h"


class ZeroCamera;
class ZeroObject;
class ZeroUserData;
class ZeroMaterial;
class ZeroRenderItem;
class ZeroVertexRecord;


class ZeroSegmenData
{
public:
	ZeroMaterial *material; 
	bool bIsDynamicVB; 
	bool bIsDynamicContent;
	ZeroMshModelTypes modelType;
	ZeroPrimitiveType primitiveType;
	
	int vertexCount; 
	int indexCount;
	unsigned short *indexArray;
	ZeroVertexRecord vertexRecord;
	void* extraData;
	
	ZeroSegmenData()
		:vertexRecord()
	{
		material = NULL; 
		bIsDynamicVB = false; 
		bIsDynamicContent = false;
		primitiveType = ZERO_PRIMITIVE_NULL;
		vertexCount = 0; 
		indexCount = 0;
		indexArray = NULL;
		modelType = Null_Node;
		extraData = NULL;
	}
};

class ZeroSegment: public ZeroUnknown
{
	ZERO_RTTI(ZeroUnknown);
	
	friend class ZeroGeometry;
	
protected:	
	
	unsigned int mHidden			   : 1;	// is hidden?
	unsigned int mDynamicVB			: 1;	// does the segment use a dynamic vertex buffer?
	unsigned int mDynamicContent	: 1;	// does the segment contain dynamic content?
	unsigned int mTranslucent		: 1;	// is the segment translucent?
	
	bool mCastsShadows;

	ZeroSegment ()
	{
		mHidden = false;
		mDynamicVB = false;
		mTranslucent = false;
		mDynamicContent = false;
		mCastsShadows = false;
	}

	
	
	
	ZeroSegment (const ZeroSegment &segment)
		:ZeroUnknown(segment)
	{
		mHidden = segment.mHidden;
		mDynamicVB = segment.mDynamicVB;
		mTranslucent = segment.mTranslucent;
		mDynamicContent = segment.mDynamicContent;
		mCastsShadows = segment.mCastsShadows;
	}
	
	
	//	ZeroSegment (
	//		 ZeroMaterial *material, 
	//		 bool bIsDynamicVB, 
	//		 bool bIsDynamicContent,
	//		 int vertexCount, 
	//		 int indexCount,
	//		 unsigned vertexFormat,
	//		 ZeroPrimitiveType primitiveType)
	//		 :ZeroUnknown()
	
	ZeroSegment (const ZeroSegmenData& data)
	{
		mHidden = false;
		mTranslucent = false;
		mDynamicVB = data.bIsDynamicVB;
		mDynamicContent = data.bIsDynamicContent;
		mCastsShadows = false;
	}
	
	virtual ~ZeroSegment ()
	{
	}
	
public:
	ZeroUnknown* CreateClone(bool isSim) 
	{
		_ASSERTE (0);
		return NULL;
	}
	
	// use this to create procedural segments
	virtual ZeroSegment *CreateProceduralSegment()
	{
		_ASSERTE (0);
		return NULL;
	}
	
	
	// get the face count
	virtual int GetFaceCount() const
	{
		_ASSERTE (0);
		return 0;
	}
	
	virtual int GetVertexCount() const
	{
		_ASSERTE (0);
		return 0;
	}
	
	// get the segment face table
	virtual int GetFaceTable(ZeroFace aFaceTable[], unsigned int aBase) const
	{
		return 0;
	}
	
	virtual ZeroMaterial* GetMaterial() const
	{
		_ASSERTE (0);
		return NULL;
	}
	
	// set the material
	virtual void AttachMaterial (ZeroMaterial *material)
	{
		_ASSERTE (0);
	}
	
	// scale the segment
	virtual void Scale (float scale)
	{
	}
	
	int CreatePolyhedra (ZeroPolyhedra& polyhedrum, float const* pool, int strideInBytes, const ZeroMatrix& worldMatrix);
	
	
	// set the bounding sphere
	virtual void SetSphere(const ZeroSphere& aSphere)
	{
		_ASSERTE (0);
	}
	
	virtual const ZeroSphere& GetSphere() const
	{
		return identitySphere;
	}
	
	// lock the render descriptor
	// this function return a ZeroRenderDescriptor set with pointers to application readables vertex and index data
	virtual const ZeroRenderDescriptor* DriverLock()
	{
		_ASSERTE (0);
		return NULL;
	}
	
	// unlock the render descriptor
	virtual void DriverUnlock()
	{
		_ASSERTE (0);
	}
	
	virtual const ZeroRenderDescriptor* AppLock()
	{
		_ASSERTE (0);
		return NULL;
	}
	
	// unlock the render descriptor
	virtual void AppUnlock()
	{
		_ASSERTE (0);
	}
	
	
	// this function is called by the loader to upload vertex data into platform specific memory.
	// if this function is called more than once, data allocated by a previous call will be lost.
	// this function is not to be called between Lock-Unlock pair.
	
	// note: for platforms with read only memory for vertex data. Or compress vertex format
	//  		the implementation of this function must keep readable copy of the vertex and index data,
	//			should the application decide to read the vertex data.
	virtual void SetVertexData (const ZeroVertexRecord& sourceData, int vertexCount,	const unsigned short indexList[], int indexCount)
	{
		_ASSERTE (0);
	}
	
	
	void SetHiddenFlag (bool hidden)
	{
		mHidden = hidden;
	}
	
	
	bool GetHiddenFlag () const
	{
		return mHidden;
	}
	
	virtual void InitBuffer()
	{
		_ASSERTE (0);
	}

	void SetShadowProperties(bool _castsShadows)
	{
		mCastsShadows = _castsShadows;
	}
	
	
	virtual unsigned GetRenderPriority (const ZeroCamera* camera, const ZeroMatrix& worldMatrix)
	{
		return 0;
	}
	
	virtual void Render (ZeroCamera *camera, const ZeroRenderItem& context)
	{
		_ASSERTE (0);
	}
	
	
	virtual void  UpdateProceduralVertexData (ZeroCamera *camera, 
		const ZeroVertexRecord& sourceData, const int* indirectVertexList, int vertexIndexCount,
		const unsigned short *indexList, int indexCount)
	{
		_ASSERTE (0);
	}
	
	virtual void  UpdateProceduralVertexData (ZeroCamera *camera, const ZeroVertexRecord& sourceData, int vertexIndexCount)
	{
		_ASSERTE (0);
	}
	
	virtual void  FixupData (ZeroGeometry* geometry)
	{
		
	}
	
	
	bool IsDynamicVb() const
	{
		return mDynamicVB;
	}
	
	bool IsDynamicContents() const
	{
		return mDynamicContent;
	}
	
#if (PLATFORM == PLATFORM_PS2)
	DLL_DECLSPEC void TriangleStrips();
#endif
	
	virtual void FreePool()
	{
		
	}
};

#endif
