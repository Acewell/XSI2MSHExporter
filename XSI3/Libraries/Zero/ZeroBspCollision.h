#ifndef __ZeroBspCollision__
#define __ZeroBspCollision__
/*
#include "ZeroRtti.h"
#include "ZeroSphereCollision.h"

class ZeroBsp;

class ZeroBspCollision : public ZeroSphereCollision
{
	ZERO_RTTI(ZeroSphereCollision);
protected:
	// collision bsp
	ZeroBsp *mBsp;

protected:
	// destructor
	~ZeroBspCollision()
	{
		AttachObject((ZeroUnknown **)&mBsp, NULL);
	}

public:
	// constructor (type name)
	ZeroBspCollision(const char *aName, ZeroObject *aOwner, const ZeroSphere &aSphere, ZeroBsp *aBsp)
		: ZeroSphereCollision(aName, aOwner, aSphere), mBsp(NULL)
	{
		AttachObject((ZeroUnknown **)&mBsp, (ZeroUnknown *)aBsp);
	}

	// constructor (type id)
	ZeroBspCollision(unsigned int aId, ZeroObject *aOwner, const ZeroSphere &aSphere, ZeroBsp *aBsp)
		: ZeroSphereCollision(aId, aOwner, aSphere), mBsp(NULL)
	{
		AttachObject((ZeroUnknown **)&mBsp, (ZeroUnknown *)aBsp);
	}

	// copy constructor
	ZeroBspCollision(const ZeroBspCollision &aOriginal, ZeroObject *aOwner)
		: ZeroSphereCollision(aOriginal, aOwner), mBsp(NULL)
	{
		AttachObject((ZeroUnknown **)&mBsp, (ZeroUnknown *)aOriginal.mBsp);
	}

	// clone this object
	ZeroUnknown *CreateClone(ZeroObject *aOwner, bool isSim)
	{
		return new (isSim) ZeroBspCollision (*this, aOwner);
	}

	// perform a ray hit
	virtual float RayHit (const ZeroVector3 &aP0, const ZeroVector3 &aP1, ZeroVector3 &aNormal) const;

	// perform a ray test
	virtual bool RayTest (const ZeroVector3 &aP0, const ZeroVector3 &aP1) const;

	// perform a sector iteration
	virtual void ForAllSectors (const ZeroVector3 &aMin, const ZeroVector3 &aMax, ZeroIntersectCallback aCallback, void *aContext) const;

	// debugging render
	virtual void DebugRender(const ZeroCamera* camera, const ZeroMatrix& worldMatrix, ZeroColor color) const;

	// debugging render bsp
	virtual void DebugRenderBsp(const ZeroCamera* camera, const ZeroMatrix& worldMatrix, ZeroColor color) const
	{
		DebugRender( camera, worldMatrix, color );
	}
};
*/
#endif
