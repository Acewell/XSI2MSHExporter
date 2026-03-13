#include "ZeroTypes.h"
#include "ZeroPolyCollision.h"
#include "ZeroPolySoups.h"
#include "ZeroGeoTypes.h"


ZERO_RTTI_CONTRUCTOR(ZeroPolyCollision);							  

// ray hit
float ZeroPolyCollision::RayHit (const ZeroVector3 &aP0,
								 const ZeroVector3 &aP1,
								 ZeroVector3 &aNormal) const
{
	return mPoly->RayHit(aP0, aP1, aNormal);
}

// ray test
bool ZeroPolyCollision::RayTest (const ZeroVector3 &aP0,
								 const ZeroVector3 &aP1) const
{
	return mPoly->RayTest(aP0, aP1) < 1.0f;
}

// sector iteration
void ZeroPolyCollision::ForAllSectors (const ZeroVector3 &aMin,
									  const ZeroVector3 &aMax,
									  ZeroIntersectCallback aCallback,
									  void *aContext) const
{
	mPoly->ForAllSectors(aMin, aMax, aCallback, aContext);
}

// debugging render
void ZeroPolyCollision::DebugRender(const ZeroCamera* camera, 
								   const ZeroMatrix& worldMatrix, 
								   ZeroColor color) const
{
	mPoly->DebugRender(camera, worldMatrix, color);
}

