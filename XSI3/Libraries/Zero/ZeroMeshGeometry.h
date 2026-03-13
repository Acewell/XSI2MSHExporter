/****************************************************************************
*
*  Visual C++ 6.0 created by: Julio Jerez
*
****************************************************************************/
#ifndef __ZeroMeshGeometry__
#define __ZeroMeshGeometry__

#include "ZeroRtti.h"
#include "ZeroGeometry.h"
#include "ZeroGeoUtil.h"
#include "ZeroCameraSphere.h"
#include "ZeroRenderDescriptor.h"

class ZeroMeshGeometry: public ZeroGeometry
{
	ZERO_RTTI(ZeroGeometry);

    protected:
	ZeroCameraSphere sphere;
	ZeroSegmentList segments;
	bool sphereDirty;

	ZeroSegmentShaderCallback renderFnct;
	ZeroSegmentShaderCustomParametersCallback setCustomData;


    protected:
	DLL_DECLSPEC void PackVertexFormat();
	DLL_DECLSPEC ~ZeroMeshGeometry ();
	
    public:
	DLL_DECLSPEC ZeroMeshGeometry(const char *name);
	DLL_DECLSPEC ZeroMeshGeometry(const ZeroGeometryData &data);

	DLL_DECLSPEC ZeroMeshGeometry(const ZeroMeshGeometry &geometry);
	DLL_DECLSPEC ZeroUnknown *CreateClone(bool isSim)
   {
       AddRef ();
       return (ZeroUnknown *)this;
   }



	ZeroSegmentList *GetSegments()
	{
		return &segments;
	}
	

	DLL_DECLSPEC void AddSegment (ZeroSegment *Segment);

	DLL_DECLSPEC void RemoveSegment (ZeroSegment *Segment);
		
	// return:  0 if shere is inside camera
	// 		    1 if sphere is partially inside camera
	//		   -1 if sphere is completly outside camera
	int TrivialRejection (
		const ZeroCamera *camera, 
		const ZeroMatrix &world,
		const unsigned flags) const
	{
		int clipFlag;
		
		clipFlag = 0;
		if (flags & ZERO_RENDERFLAG_CLIP) {
			clipFlag = sphere.VisibilityTest (camera, world);
			if (clipFlag < 0) {
				return clipFlag;
			}
		}
		return ZERO_RENDERFLAG_CLIP & -clipFlag;
	}
	
	DLL_DECLSPEC void TotalVertexAndFaceCount(int& faceCount, int& vertexCount) const;

	DLL_DECLSPEC void FlatFaceArray (const ZeroMatrix &matrix,
		float* const vertex, int strideInBytes, int &vCount, int maxVertex, 
		ZeroFace* const index, int &iCount, int maxFaces) const;
	
	bool SphereDirty () 
	{ 
		return sphereDirty;
	}

	const ZeroSphere &GetSphere ()
	{
		return sphere;
	}

	void SetSphere(const ZeroSphere& arg)
	{
		sphereDirty	= false;
		sphere = arg;
	}
	
	DLL_DECLSPEC void SetSphere();
	DLL_DECLSPEC void Scale(float scale);

	virtual void SetSegmentShaderFunction (ZeroSegmentShaderCallback fnt)
	{
		renderFnct = fnt;
	}

	virtual ZeroSegmentShaderCallback GetSegmentShaderFunction () const
	{
		return renderFnct;
	}

	virtual void SetItemCustomParameterCallback (ZeroSegmentShaderCustomParametersCallback fnt)
	{
		setCustomData = fnt;
	}

	virtual ZeroSegmentShaderCustomParametersCallback GetItemCustomParameterCallback()
	{
		return setCustomData;
	}

	void Render (const ZeroObject *owner, ZeroCamera *camera, const ZeroMatrix &world, unsigned flags = 0) const;

	void GetExtents(ZeroVector3 &min, ZeroVector3 &max, const ZeroMatrix& worldMatrix);

	float mMaxDistance;
};

#endif
