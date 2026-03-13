/****************************************************************************
*
*  Visual C++ 6.0 created by: Julio Jerez
*
****************************************************************************/
#ifndef __ZeroTransformContraintGeometry__
#define __ZeroTransformContraintGeometry__

#include "ZeroRtti.h"
#include "ZeroTypes.h"
#include "ZeroUnknown.h"
#include "ZeroMatrix.h"
#include "ZeroSphere.h"
#include "ZeroGeoTypes.h"
#include "ZeroGeometry.h"
#include "ZeroProceduralGeometry.h"


class ZeroCamera;
class ZeroObject;


class ZeroTransformContraintGeometry: public ZeroProceduralGeometry
{
	ZERO_RTTI(ZeroProceduralGeometry);
	friend class ZeroSkinGeometry;

	protected:
	mutable ZeroMatrix worldMatrix;

	ZeroTransformContraintGeometry(const ZeroTransformContraintGeometry& aClone);
	~ZeroTransformContraintGeometry();

	public:
	ZeroTransformContraintGeometry(); 

	ZeroTransformContraintGeometry(const ZeroGeometryData& data); 

	ZeroUnknown *CreateClone (bool isSim);
	
	void Scale (float scale)
   {
		worldMatrix.posit = worldMatrix.posit.Scale (scale);
      ZeroProceduralGeometry::Scale (scale); 
   }


	const ZeroMatrix& GetWorldMatrix() const
	{
		return worldMatrix;
	}


	virtual void Render (const ZeroObject *owner, ZeroCamera *camera, const ZeroMatrix &world, unsigned flags = 0) const;
};


#endif
