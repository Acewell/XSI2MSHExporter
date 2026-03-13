#include "ZeroTypes.h"
#include "ZeroTransform.h"
#include "ZeroInfinityMassPhysics.h"


ZeroInfiniteMassPhysics::ZeroInfiniteMassPhysics (ZeroObject *owner)
	: ZeroTransform (owner)
{
	// initialize dynamics flags
	mApplyDynamicsFlag = false;
	mCollisionRecipientFlag = true;
	mCollisionGeneratorFlag = false;
}


ZeroInfiniteMassPhysics::ZeroInfiniteMassPhysics (
	const ZeroInfiniteMassPhysics &transform, 
	ZeroObject *ownerArg)
	: ZeroTransform (transform, ownerArg)
{
}

