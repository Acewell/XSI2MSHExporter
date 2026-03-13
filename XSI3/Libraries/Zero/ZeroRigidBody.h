/****************************************************************************
*
*  Visual C++ 6.0 created by: Julio Jerez
*
****************************************************************************/
#ifndef __ZeroRigidBody__
#define __ZeroRigidBody__

#include "ZeroTransform.h"

struct ZeroRigidBodyDesc
{
//	ZeroSphere matrix;
	float density;
	ZeroVector3 veloc;
	ZeroVector3 omega;

	ZeroRigidBodyDesc	()
		: density (1.0f)
		, veloc (0, 0, 0)
		, omega (0, 0, 0)
	{
	}
};


// forward declaration
class ZeroRigidBodyLoader;

// root transform class definition
class ZeroRigidBody: public ZeroTransform
{
	ZERO_RTTI(ZeroTransform);

	float mass;
	float massInv;
	ZeroVector3 force;
	ZeroVector3 torque;
	ZeroVector4 inertia;			
	ZeroVector4 inertiaInv;		
	ZeroVector4 inertiaCoupling;
	ZeroMatrix inertiaMatrixInv;
	static ZeroMatrix tmpMatrix;

	void UpdateInertiaMatrix();
	void UpdateVelocities(float dt);

	protected:
	ZeroVector3 veloc;
	ZeroVector3 accel;
	ZeroVector3 omega;
	ZeroVector3 alpha;
	ZeroVector3 halfVeloc;
	ZeroVector3 halfOmega;

	ZeroUnknown *CreateClone (bool isSim)
	{
		return new (isSim) ZeroRigidBody (*this);
	}
	DLL_DECLSPEC void ApplyKinematics (float dt);


	DLL_DECLSPEC ~ZeroRigidBody ();
	DLL_DECLSPEC ZeroRigidBody (const ZeroRigidBody &transform, ZeroObject *ownerArg);

	public:
	DLL_DECLSPEC ZeroRigidBody (ZeroObject *owner, const ZeroRigidBodyDesc& desc);
	ZeroTransform *CreateClone(ZeroObject *ownerArg, bool isSim)
	{
		return new (isSim) ZeroRigidBody (*this, ownerArg);
	}

	float GetMass (void) const
	{
		return mass;
	}

	float GetMassInv (void) const
	{
		return massInv;
	}

	const ZeroVector3 &GetInertia (void) const
	{
		return inertia;
	}

	const ZeroVector3 &GetInertiaInv (void) const
	{
		return inertiaInv;
	}

	const ZeroMatrix &GetInertialMatrixInv() const
	{
		return inertiaMatrixInv;
	}

	DLL_DECLSPEC void DebugRender (const ZeroCamera* camera, const ZeroMatrix &transform, ZeroColor color) const;
	DLL_DECLSPEC void SweptVolume (float dt, ZeroVector3 &pmin, ZeroVector3 &pmax) const;

	ZeroVector3 GetVelocity () const
	{
		return veloc;
	}

	void SetVelocity (const ZeroVector3 &velocArg)
	{
		veloc = velocArg;
	}

	ZeroVector3 GetOmega (void) const
	{
		return omega;
	}

	void SetOmega (const ZeroVector3 &omegaArg)
	{
		omega =	omegaArg;
	}

	ZeroVector3 GetAcceleration () const
	{
		return accel;
	}

	ZeroVector3 GetAlpha () const
	{
		return alpha;
	}

	DLL_DECLSPEC void SetCollisionMatrix (ZeroMatrix &Mat, const ZeroVector3 &contact); 

	void ApplyNetForce (const ZeroVector3 &forceArg) 
	{
		force = forceArg;
	}

	void ApplyNetTorque (const ZeroVector3 &torqueArg) 
	{
		torque = torqueArg;
	}

	DLL_DECLSPEC void ApplyDynamics (float dt);
};

#endif
