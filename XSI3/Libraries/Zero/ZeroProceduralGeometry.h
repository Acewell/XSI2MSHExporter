/****************************************************************************
*
*  Visual C++ 6.0 created by: Julio Jerez
*
****************************************************************************/
#ifndef __ZeroProceduralGeometry__
#define __ZeroProceduralGeometry__

#include "ZeroRtti.h"
#include "ZeroTypes.h"
#include "ZeroUnknown.h"
#include "ZeroGeoTypes.h"
#include "ZeroGeometry.h"


class ZeroProceduralGeometry: public ZeroGeometry
{
	ZERO_RTTI(ZeroGeometry);

	protected:
	virtual void CloneFixUp (const ZeroObject& myOwner, const ZeroObject& myClone);
	ZeroGeometry* sharedGeometry;

	ZeroProceduralGeometry(const ZeroProceduralGeometry& clone);
	~ZeroProceduralGeometry();

	public:
	ZeroProceduralGeometry(); 

	ZeroProceduralGeometry(const ZeroGeometryData& data); 
	ZeroUnknown *CreateClone (bool isSim);
	
	void Scale (float scale)
   {
      ZeroGeometry::Scale (scale); 
		if(sharedGeometry) {
			sharedGeometry->Scale(scale);
		}
   }

	void TotalVertexAndFaceCount(int& faceCount, int& vertexCount) const
	{
		if(sharedGeometry) {
			sharedGeometry->TotalVertexAndFaceCount(faceCount, vertexCount);
		}
	}
	
	void FlatFaceArray (const ZeroMatrix &matrix,
		float* const vertex, int strideInBytes, int &vCount, int maxVertex, 
		ZeroFace* const index, int &iCount, int maxFaces) const
	{
		if(sharedGeometry) {
			sharedGeometry->FlatFaceArray (matrix, vertex, strideInBytes, vCount, maxVertex, index, iCount, maxFaces);
		}
	}

	virtual ZeroSegmentList* GetSegments()
	{
		return sharedGeometry ? sharedGeometry->GetSegments() : NULL;
	}

	void Render(const ZeroObject *owner,  ZeroCamera *camera, const ZeroMatrix& worldMatrix, unsigned flags) const;


};



#endif
