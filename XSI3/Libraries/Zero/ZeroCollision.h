#ifndef __ZeroCollision__
#define __ZeroCollision__


#include "ZeroTypes.h"
#include "ZeroRtti.h"
#include "ZeroUnknown.h"
#include "ZeroIntersections.h"
#include "ZeroSphere.h"
#include "ZeroGeoTypes.h"

class ZeroObject;

class ZeroCollision : public ZeroUnknown
{
	ZERO_RTTI(ZeroUnknown);
protected:
	// owner object
	ZeroObject *mOwner;

protected:
	// destructor
	~ZeroCollision ()
	{
	}

public:
	// constructor (type name)
	ZeroCollision(const char *aName, ZeroObject *aOwner)
		: ZeroUnknown(aName), mOwner(aOwner)
	{
	}

	// constructor (type id)
	ZeroCollision(unsigned int aId, ZeroObject *aOwner)
		: ZeroUnknown(aId), mOwner(aOwner)
	{
	}

	// copy constructor
	ZeroCollision (const ZeroCollision &aOriginal, ZeroObject *aOwner)
		: ZeroUnknown(aOriginal), mOwner(aOwner)
	{
	}

	// clone this object
	virtual ZeroUnknown *CreateClone(ZeroObject *aOwner, bool isSim)
	{
		return new (isSim) ZeroCollision (*this, aOwner);
	}

	// get the owner object
	ZeroObject *GetOwner (void) const
	{
		return mOwner;
	}

	// perform a ray hit
	virtual float RayHit (const ZeroVector3& aP0, const ZeroVector3& aP1, ZeroVector3& aNormal) const
	{
		return 1.1f;
	}

	// perform a ray test
	virtual bool RayTest (const ZeroVector3 &aP0, const ZeroVector3 &aP1) const
	{
		return false;
	}

	// perform a sector iteration
	virtual void ForAllSectors (const ZeroVector3 &aMin, const ZeroVector3 &aMax, ZeroIntersectCallback aCallback, void *aContext) const
	{
	}

	// get the bounding sphere
	virtual const ZeroSphere &GetSphere (void) const
	{
		return identitySphere;
	}

	// debugging render
	virtual void DebugRender(const ZeroCamera* camera, const ZeroMatrix& worldMatrix, ZeroColor color) const
	{
		GetSphere().DebugRender (camera, worldMatrix, color);
	}

	// debugging render bsp
	virtual void DebugRenderBsp(const ZeroCamera* camera, const ZeroMatrix& worldMatrix, ZeroColor color) const
	{
	}
};

#endif
