#ifndef __ZeroPolyCollision__
#define __ZeroPolyCollision__

#include "ZeroRtti.h"
#include "ZeroSphereCollision.h"

class ZeroPolySoup;

class ZeroPolyCollision : public ZeroSphereCollision
{
	ZERO_RTTI(ZeroSphereCollision);
protected:
	// collision data
	ZeroPolySoup *mPoly;

protected:
	// destructor
	~ZeroPolyCollision()
	{
		AttachObject((ZeroUnknown **)&mPoly, NULL);
	}

public:
	// constructor (type name)
	ZeroPolyCollision(const char *aName, ZeroObject *aOwner, const ZeroSphere &aSphere, ZeroPolySoup *aPoly)
		: ZeroSphereCollision(aName, aOwner, aSphere), mPoly(NULL)
	{
		AttachObject((ZeroUnknown **)&mPoly, (ZeroUnknown *)aPoly);
	}

	// constructor (type id)
	ZeroPolyCollision(unsigned int aId, ZeroObject *aOwner, const ZeroSphere &aSphere, ZeroPolySoup *aPoly)
		: ZeroSphereCollision(aId, aOwner, aSphere), mPoly(NULL)
	{
		AttachObject((ZeroUnknown **)&mPoly, (ZeroUnknown *)aPoly);
	}

	// copy constructor
	ZeroPolyCollision(const ZeroPolyCollision &aOriginal, ZeroObject *aOwner)
		: ZeroSphereCollision(aOriginal, aOwner), mPoly(NULL)
	{
		AttachObject((ZeroUnknown **)&mPoly, (ZeroUnknown *)aOriginal.mPoly);
	}

	// clone this object
	ZeroUnknown *CreateClone(ZeroObject *aOwner, bool isSim)
	{
		return new (isSim) ZeroPolyCollision (*this, aOwner);
	}

	// perform a ray hit
	virtual float RayHit (const ZeroVector3 &aP0, const ZeroVector3 &aP1, ZeroVector3 &aNormal) const;

	// perform a ray test
	virtual bool RayTest (const ZeroVector3 &aP0, const ZeroVector3 &aP1) const;

	// perform a sector iteration
	virtual void ForAllSectors (const ZeroVector3 &aMin, const ZeroVector3 &aMax, ZeroIntersectCallback aCallback, void *aContext) const;

	virtual void DebugRender(const ZeroCamera* camera, const ZeroMatrix& worldMatrix, ZeroColor color) const;
	void DebugRenderPoly(const ZeroCamera* camera, const ZeroMatrix& worldMatrix, ZeroColor color) const
	{
		DebugRender( camera, worldMatrix, color );
	}
};

#endif
