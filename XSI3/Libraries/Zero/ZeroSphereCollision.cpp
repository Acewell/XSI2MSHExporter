#include "ZeroTypes.h"
#include "ZeroSphereCollision.h"


ZERO_RTTI_CONTRUCTOR(ZeroSphereCollision);							  

// ray hit
float ZeroSphereCollision::RayHit (const ZeroVector3 &aP0,
								   const ZeroVector3 &aP1,
								   ZeroVector3 &aNormal) const
{
	return mSphere.RayBoxHit(aP0, aP1, aNormal);
}

// perform a ray test
bool ZeroSphereCollision::RayTest (const ZeroVector3 &aP0,
								   const ZeroVector3 &aP1) const
{
	return mSphere.RayBoxTest(aP0, aP1);
}
