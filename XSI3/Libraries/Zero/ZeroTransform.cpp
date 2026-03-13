/****************************************************************************
*
*  Visual C++ 6.0 created by: Julio Jerez
*
****************************************************************************/

#include "ZeroTypes.h"
#include "ZeroTransform.h"
#include "ZeroObject.h"

ZERO_RTTI_CONTRUCTOR(ZeroTransform);							  

// get the swept volume
void ZeroTransform::SweptVolume (float dt, ZeroVector3 &pmin, ZeroVector3 &pmax) const
{
	mOwner->GetObjectSphere().GetExtents (pmin, pmax);
}

// get the world extents
void ZeroTransform::GetExtents (ZeroVector3 &pmin, ZeroVector3 &pmax) const
{
	mOwner->GetObjectSphere().GetExtents (pmin, pmax);
}

// store data
void ZeroTransform::Store(RepBuffer *buf)
{
}

// restore data
void ZeroTransform::Restore(RepBuffer *buf, float blend)
{
}
