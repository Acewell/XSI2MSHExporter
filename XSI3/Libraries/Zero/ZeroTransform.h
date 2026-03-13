/****************************************************************************
*
*  Visual C++ 6.0 created by: Julio Jerez
*
****************************************************************************/
#ifndef __ZeroTransform__
#define __ZeroTransform__


#include "ZeroTypes.h"
#include "ZeroRtti.h"
#include "ZeroUnknown.h"
#include "ZeroMatrix.h"
#include "ZeroVector3.h"
#include "ZeroSphere.h"
#include "ZeroGeoTypes.h"
#include "ZeroQuaternion.h"


// forward declarations
class ZeroObject;
class ZeroUserData;
class RepBuffer;

// base transform class definition
class ZeroTransform: public ZeroUnknown
{
	ZERO_RTTI(ZeroUnknown);

protected:
	ZeroObject *mOwner;
	unsigned mApplyDynamicsFlag			: 1;
	unsigned mCollisionGeneratorFlag	: 1;
	unsigned mCollisionRecipientFlag	: 1;
	unsigned mMovablePhysicsFlag		: 1;

protected:
	// clone this object
	ZeroUnknown *CreateClone(bool isSim)
	{
		_ASSERTE (0);
		return NULL;
//		return new (isSim) ZeroTransform (*this, owner);
	}

	// destructor
	~ZeroTransform ()
	{
	}


public:
	// constructor
	ZeroTransform (ZeroObject *aOwner)
		: ZeroUnknown()
		, mOwner(aOwner)
		, mApplyDynamicsFlag(true)
		, mCollisionGeneratorFlag(false)
		, mCollisionRecipientFlag(true)
		, mMovablePhysicsFlag(true)
	{
	}

	// copy constructor
	ZeroTransform (const ZeroTransform &aOriginal, ZeroObject *aOwner)
		: ZeroUnknown(aOriginal)
		, mOwner(aOwner)
		, mApplyDynamicsFlag(aOriginal.mApplyDynamicsFlag)
		, mCollisionGeneratorFlag(aOriginal.mCollisionGeneratorFlag)
		, mCollisionRecipientFlag(aOriginal.mCollisionRecipientFlag)
		, mMovablePhysicsFlag(aOriginal.mMovablePhysicsFlag)
	{
	}

	void SetAlwaysApplyDynamics (bool aFlag)
	{
		mApplyDynamicsFlag = aFlag;
	}

	void SetCollisionRecipient (bool aFlag)
	{
		mCollisionRecipientFlag = aFlag;
	}

	void SetCollisionGenerator (bool aFlag)
	{
		mCollisionGeneratorFlag = aFlag;
	}

	void SetMovablePhysics (bool aFlag)
	{
		mMovablePhysicsFlag = aFlag;
	}

	bool AlwaysApplyDynamics () const
	{
		return mApplyDynamicsFlag;
	}

	bool CollisionRecipient () const
	{
		return mCollisionRecipientFlag;
	}

	bool CollisionGenerator () const
	{
		return mCollisionGeneratorFlag;
	}

	bool MovablePhysics () const
	{
		return mMovablePhysicsFlag;
	}

	// create a clone
	virtual ZeroTransform *CreateClone(ZeroObject *aOwner, bool isSim)
	{
		return  new (isSim) ZeroTransform (*this, aOwner);
	}

	// post cloning initializtion
	virtual void CloneFixUp (const ZeroTransform &aClone)
	{
	}


	// get the owner object
	ZeroObject *GetOwner () const
	{
		return mOwner;
	}

	/*
	// get the transform rotation
	const ZeroQuaternion &GetRotation () const
	{
		return rotation;
	}

	// set the transform rotation
	void SetRotation (const ZeroQuaternion &rotationArg)
	{
		rotation = rotationArg;
	}

	// get the transform position
	const ZeroVector3 &GetPosition () const
	{
		return position;
	}

	// set the transform position
	void SetPosition (const ZeroVector3 &positionArg)
	{
		position = positionArg;
	}

	// get the transform matrix
	const ZeroMatrix GetMatrix () const
	{
		return ZeroMatrix(rotation, position);
	}

	// set the transform matrix
	virtual void SetMatrix (const ZeroMatrix &matrixArg)
	{
		rotation = ZeroQuaternion(matrixArg);
		position = matrixArg.posit;
	}

	// get the transform sphere
	const ZeroSphere GetSphere () const
	{
		return ZeroSphere(GetMatrix(), GetBoxDimension());
	}

	// set the transform sphere
	virtual void SetSphere (const ZeroSphere &sphereArg)
	{
		SetMatrix(sphereArg);
	}
	*/

	// get the swept volume
	virtual void SweptVolume (float dt, ZeroVector3 &pmin, ZeroVector3 &pmax) const;

	// get the world extents
	virtual void GetExtents (ZeroVector3 &pmin, ZeroVector3 &pmax) const;

	/*
	// get bounding box dimension
	virtual const ZeroVector4 &GetBoxDimension (void) const
	{
		return identitySphere.size;
	}
	*/


	// visual update
	virtual void ApplyVisual (float dt)
	{
	}


	// get simulation velocity
	virtual ZeroVector3 GetVelocity (void) const
	{
		return ZeroVector3 (0, 0, 0);
	}

	// set simulation velocity
	virtual void SetVelocity (const ZeroVector3 &v)
	{
	}

	// get simulation omega
	virtual ZeroVector3 GetOmega (void) const
	{
		return ZeroVector3 (0, 0, 0);
	}

	// set simulation omega
	virtual void SetOmega (const ZeroVector3 &w)
	{
	}

	// get simulation acceleration
	virtual ZeroVector3 GetAcceleration (void) const
	{
		return ZeroVector3 (0, 0, 0);
	}

	// get simulation alpha
	virtual ZeroVector3 GetAlpha (void) const
	{
		return ZeroVector3 (0, 0, 0);
	}

	// get the object's mass
	virtual float GetMass (void) const
	{
		// objects with no physics are considered static objects
		return INFINITE_MASS * 1.1f;
	}

	// get the object's inverse mass
	virtual float GetMassInv (void) const
	{
		return 0;
	}

	// get the inverse inertial matrix
	virtual const ZeroMatrix &GetInertialMatrixInv() const
	{
		return emptyMatrix;
	}


	// calculate forces and torques
	virtual void ApplyDynamics (float dt)
	{
	}

	// apply forces and torques
	virtual void ApplyKinematics (float dt)
	{
	}

	// get the static coefficient of friction
	virtual float GetStaticFriction (void)
	{
		return 0.8f;
	}

	// get the dynamic coefficient of friction
	virtual float GetDynamicFriction (void)
	{
		return 0.3f;
	}

	// get the normal coefficient of restitution
	virtual float GetNormalRestitution (void)
	{
		return 0.5f;
	}

	// get the tangent coefficient of restitution
	virtual float GetTangentRestitution (void)
	{
		return 0.5f;
	}

	// set the collision matrix
	virtual void SetCollisionMatrix (ZeroMatrix &Mat, const ZeroVector3 &Contact)
	{
		Mat.right = ZeroVector3 (0, 0, 0);
		Mat.up    = ZeroVector3 (0, 0, 0);
		Mat.front = ZeroVector3 (0, 0, 0);
		Mat.posit = ZeroVector3 (0, 0, 0);
	}

	virtual void DebugRender (const ZeroCamera* camera, const ZeroMatrix &transform, ZeroColor color) const {}

	// collision callback
	virtual void CollisionCallBack (ZeroTransform *him, float totalEneryLost)
	{
	}
	virtual void CollisionCallBack (ZeroTransform *him, float totalEneryLost, const ZeroVector3 &position, const ZeroVector3 &normal)
	{
	}

	// replay stuff
	virtual void Store(RepBuffer *buf);
	virtual void Restore(RepBuffer *buf, float blend = 1.f );
};

#endif
