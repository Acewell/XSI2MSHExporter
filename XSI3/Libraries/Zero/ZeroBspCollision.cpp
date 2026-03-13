#include "ZeroTypes.h"
#include "ZeroBspCollision.h"
#include "ZeroGeoTypes.h"

/*
ZERO_RTTI_CONTRUCTOR(ZeroBspCollision);							  

// ray hit
float ZeroBspCollision::RayHit (const ZeroVector3 &aP0,
								const ZeroVector3 &aP1,
								ZeroVector3 &aNormal) const
{
	return mBsp->RayHit(aP0, aP1, aNormal);
}

// ray test
bool ZeroBspCollision::RayTest (const ZeroVector3 &aP0,
								 const ZeroVector3 &aP1) const
{
	ZeroVector3 fTmp;
	return mBsp->RayHit(aP0, aP1, fTmp) < 1.0f;
}

// sector iteration
void ZeroBspCollision::ForAllSectors (const ZeroVector3 &aMin,
									  const ZeroVector3 &aMax,
									  ZeroIntersectCallback aCallback,
									  void *aContext) const
{
	mBsp->ForAllSectors(aMin, aMax, aCallback, aContext);
}

// debugging render
void ZeroBspCollision::DebugRender(const ZeroCamera* camera, 
								   const ZeroMatrix& worldMatrix, 
								   ZeroColor color) const
{
	mBsp->DebugRender(camera, worldMatrix, color);
}
*/
