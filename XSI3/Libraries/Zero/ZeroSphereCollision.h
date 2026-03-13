#ifndef __ZeroSphereCollision__
#define __ZeroSphereCollision__

#include "ZeroRtti.h"
#include "ZeroCollision.h"

class ZeroSphereCollision : public ZeroCollision
{
	ZERO_RTTI(ZeroCollision);

protected:
	// collision sphere
	ZeroSphere mSphere;

protected:
	// destructor
	~ZeroSphereCollision()
	{
	}

public:
	// constructor (type name)
	ZeroSphereCollision(const char *aName, ZeroObject *aOwner, const ZeroSphere &aSphere)
		: ZeroCollision(aName, aOwner), mSphere(aSphere)
	{
	}

	// constructor (type id)
	ZeroSphereCollision(unsigned int aId, ZeroObject *aOwner, const ZeroSphere &aSphere)
		: ZeroCollision(aId, aOwner), mSphere(aSphere)
	{
	}

	// copy constructor
	ZeroSphereCollision(const ZeroSphereCollision &aOriginal, ZeroObject *aOwner)
		: ZeroCollision(aOriginal, aOwner), mSphere(aOriginal.mSphere)
	{
	}

	// clone this object
	ZeroUnknown *CreateClone(ZeroObject *aOwner, bool isSim)
	{
		return new (isSim) ZeroSphereCollision (*this, aOwner);
	}

	// perform a ray hit
	virtual float RayHit (const ZeroVector3 &aP0, const ZeroVector3 &aP1, ZeroVector3 &aNormal) const;

	// perform a ray test
	virtual bool RayTest (const ZeroVector3 &aP0, const ZeroVector3 &aP1) const;

	// get the bounding sphere
	virtual const ZeroSphere &GetSphere (void) const
	{
		return mSphere;
	}
};

#endif
