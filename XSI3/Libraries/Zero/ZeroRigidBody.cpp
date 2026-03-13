/****************************************************************************
*
*  Visual C++ 6.0 created by: Julio Jerez
*
****************************************************************************/
#include "ZeroTypes.h"
#include "ZeroDebug.h"
#include "ZeroMathTypes.h"
#include "ZeroRigidBody.h"
#include "ZeroObject.h"

ZERO_RTTI_CONTRUCTOR(ZeroRigidBody);
ZeroMatrix ZeroRigidBody::tmpMatrix (identityMatrix);


ZeroRigidBody::ZeroRigidBody (
	ZeroObject *owner, 
	const ZeroRigidBodyDesc& desc)
	: ZeroTransform (owner),
	 force (0.0f, 0.0f, 0.0f),
	 torque (0.0f, 0.0f, 0.0f),
	 inertia (),
	 inertiaInv (),
	 inertiaCoupling (0.0f, 0.0f, 0.0f, 0.0f),
	 inertiaMatrixInv (),
	 veloc (0.0f, 0.0f, 0.0f),
	 accel (0.0f, 0.0f, 0.0f),
	 omega (0.0f, 0.0f, 0.0f),
	 alpha (0.0f, 0.0f, 0.0f),
	 halfVeloc (0.0f, 0.0f, 0.0f),
	 halfOmega (0.0f, 0.0f, 0.0f)
{
	// set the object sphere
	//mOwner->SetObjectSphere(desc.matrix);

	float scale;
	float density;
	ZeroVector4 axis;

	density = desc.density;
//	density *= 0.001f;

	axis = mOwner->GetDimensions();
	axis.x = max(axis.x * 2.0f, EPSILON);
	axis.y = max(axis.y * 2.0f, EPSILON);
	axis.z = max(axis.z * 2.0f, EPSILON);
	axis.w = max(axis.w * 2.0f, EPSILON);

	mass = density * axis.x * axis.y * axis.z;
	SetCollisionRecipient (true);
	SetCollisionGenerator (true);
	if (mass < INFINITE_MASS) {
		scale = mass / 12.0f;
		scale *= 0.5f;
		inertia.x = scale * (axis.y * axis.y + axis.z * axis.z);
		inertia.y = scale * (axis.x * axis.x + axis.z * axis.z);
		inertia.z = scale * (axis.x * axis.x + axis.y * axis.y);
		inertia.w = GetMin (inertia.x, inertia.y, inertia.z);

		massInv = 1.0f / mass;

		inertiaInv.x = 1.0f / inertia.x;
		inertiaInv.y = 1.0f / inertia.y;
		inertiaInv.z = 1.0f / inertia.z;
		inertiaInv.w = 1.0f / inertia.w;

		inertiaCoupling.x = (inertia.y - inertia.z) * inertiaInv.x; 
		inertiaCoupling.y = (inertia.z - inertia.x) * inertiaInv.y; 
		inertiaCoupling.z = (inertia.x - inertia.y) * inertiaInv.z; 
		inertiaCoupling.w = 0.0f;
	} else {

		mass = INFINITE_MASS * 1.1f;
		massInv = 0.0f;

		inertia.x = mass;
		inertia.y = mass;
		inertia.z = mass;
		inertia.w = mass;
		inertiaInv.x = 0.0f;
		inertiaInv.y = 0.0f;
		inertiaInv.z = 0.0f;
		inertiaInv.w = 0.0f;
		inertiaCoupling.x = 0.0f; 
		inertiaCoupling.y = 0.0f; 
		inertiaCoupling.z = 0.0f; 
		inertiaCoupling.w = 0.0f;
	}
}

ZeroRigidBody::ZeroRigidBody (
	const ZeroRigidBody &transform,
	ZeroObject *ownerArg)
	:ZeroTransform (transform, ownerArg),
	 mass (transform.mass),
	 massInv (transform.massInv),
	 force (transform.force),
	 torque (transform.torque),
	 inertia(transform.inertia),
	 inertiaInv (transform.inertiaInv),
	 inertiaCoupling (transform.inertiaCoupling),
	 inertiaMatrixInv (transform.inertiaMatrixInv),
	 veloc (transform.veloc),
	 accel (transform.accel),
	 omega (transform.omega),
	 alpha (transform.alpha),
	 halfVeloc (transform.halfVeloc),
	 halfOmega (transform.halfOmega)
{
}


ZeroRigidBody::~ZeroRigidBody ()
{
}


void ZeroRigidBody::SweptVolume (float dt, ZeroVector3 &pmin, ZeroVector3 &pmax) const
{
	const ZeroSphere sphere(mOwner->GetObjectSphere());

	float w = 
		sphere.size.x * fabsf(sphere.right.x) + 
		sphere.size.y * fabsf(sphere.up.x) + 
		sphere.size.z * fabsf(sphere.front.x);  
	float h = 
		sphere.size.x * fabsf(sphere.right.y) + 
		sphere.size.y * fabsf(sphere.up.y) + 
		sphere.size.z * fabsf(sphere.front.y);  
	float d = 
		sphere.size.x * fabsf(sphere.right.z) + 
		sphere.size.y * fabsf(sphere.up.z) + 
		sphere.size.z * fabsf(sphere.front.z);  

	ZeroVector3 p0 (sphere.posit);
	ZeroVector3 p1 (sphere.posit + veloc * dt);

	pmin.x = GetMin(p0.x, p1.x) - w;
	pmin.y = GetMin(p0.y, p1.y) - h;
	pmin.z = GetMin(p0.z, p1.z) - d;

	pmax.x = GetMax(p0.x, p1.x) + w;
	pmax.y = GetMax(p0.y, p1.y) + h;
	pmax.z = GetMax(p0.z, p1.z) + d;
}


void ZeroRigidBody::ApplyDynamics (float dt)
{
	//SetTimeStep (dt);
	
#if (PLATFORM == PLATFORM_GAMECUBE)
	ASSERT(dt > 0.0f);

	ASSERT(!isnan(force.x));
	ASSERT(!isnan(force.y));
	ASSERT(!isnan(force.z));
	
	ASSERT(!isnan(torque.x));
	ASSERT(!isnan(torque.y));
	ASSERT(!isnan(torque.z));
	
	ASSERT(!isnan(veloc.x));
	ASSERT(!isnan(veloc.y));
	ASSERT(!isnan(veloc.z));
	
	ASSERT(!isnan(omega.x));
	ASSERT(!isnan(omega.y));
	ASSERT(!isnan(omega.z));
#endif

	alpha = torque.ComponentsProduct (inertiaInv);
	accel = force * massInv;

	veloc += halfVeloc;
	omega += halfOmega;

	halfVeloc = accel * dt * 0.5f;
	veloc += halfVeloc;

	halfOmega = alpha * dt * 0.5f;
	omega += halfOmega;

	ZeroVector4 angularMomentumPrev(omega.ComponentsProduct(inertia));
	float angularMomentumPrevMagSq(DotProduct(angularMomentumPrev, angularMomentumPrev));
	if (angularMomentumPrevMagSq > EPSILON)
	{
		ZeroVector4 couple(
			omega.x + inertiaCoupling.x * omega.y * omega.z * dt,
			omega.y + inertiaCoupling.y * omega.z * omega.x * dt,
			omega.z + inertiaCoupling.z * omega.x * omega.y * dt
			);
		ZeroVector4 angularMomentumNext(couple.ComponentsProduct(inertia));
		float angularMomentumNextMagSq(DotProduct(angularMomentumNext, angularMomentumNext));
		omega = couple * sqrtf(angularMomentumPrevMagSq / angularMomentumNextMagSq);
	}

	_ASSERTE (DotProduct(omega, omega) < 30 * PI2 * 30 * PI2);
	
#if (PLATFORM == PLATFORM_GAMECUBE)
	ASSERT(!isnan(omega.x));
	ASSERT(!isnan(omega.y));
	ASSERT(!isnan(omega.z));
#endif

//	static xxx;
//	xxx ++;
//	if (xxx == 167) {
//		xxx *= 1;
//	}
//	ZeroTrace (("%d %f %f %f %f %f %f\n", xxx, omega.x, omega.y, omega.z, veloc.x, veloc.y, veloc.z))
}

void ZeroRigidBody::ApplyKinematics (float dt)
{
	float angle (Magnitude(omega));
	if (angle > EPSILON)
	{
		ZeroQuaternion r (omega / angle, angle * dt);

		const ZeroQuaternion &rotation = mOwner->GetObjectRotation();

		ZeroQuaternion q;
		q.s = rotation.s * r.s - rotation.x * r.x - rotation.y * r.y - rotation.z * r.z;
		q.x = rotation.x * r.s + rotation.s * r.x - rotation.z * r.y + rotation.y * r.z;
		q.y = rotation.y * r.s + rotation.z * r.x + rotation.s * r.y - rotation.x * r.z;
		q.z = rotation.z * r.s - rotation.y * r.x + rotation.x * r.y + rotation.s * r.z;

		mOwner->SetObjectRotation(q.Scale(1.0f / sqrtf(q.DotProduct(q))));
	}

	mOwner->SetObjectPosition(mOwner->GetObjectPosition() + veloc * dt);
	
#if (PLATFORM == PLATFORM_GAMECUBE)
	ASSERT(!isnan(mOwner->GetObjectPosition().x));
	ASSERT(!isnan(mOwner->GetObjectPosition().y));
	ASSERT(!isnan(mOwner->GetObjectPosition().z));
#endif
	
/*
	int i;
	int count;
	ZeroVector3 r;
	ZeroQuaternion q;
	float mag;
	static const float MINANGLE2 = 0.01f;

	ZeroVector4 posit (matrix.posit + veloc.Scale (dt));

	dt *= 0.5f;
	r = omega * dt;

	count = 1;
	mag = MagnitudeSq(r);
	while (mag > MINANGLE2) {
		r *= 0.5f;
		mag *= 0.25f;
		count <<= 1;
	} 

	for (i = 0; i < count; i ++) {
		q.s = rotation.s - rotation.x * r.x - rotation.y * r.y - rotation.z * r.z;
		q.x = rotation.x + rotation.s * r.x - rotation.z * r.y + rotation.y * r.z;
		q.y = rotation.y + rotation.z * r.x + rotation.s * r.y - rotation.x * r.z;
		q.z = rotation.z - rotation.y * r.x + rotation.x * r.y + rotation.s * r.z;

		mag = q.DotProduct(q);

		rotation = q.Scale(1.0f / sqrtf(mag));
	}
	matrix = ZeroMatrix(rotation, posit);
*/
}


void ZeroRigidBody::UpdateInertiaMatrix()
{
	const ZeroMatrix matrix(mOwner->GetObjectMatrix());
	tmpMatrix.right = matrix.right.Scale (inertiaInv.x);
	tmpMatrix.up = matrix.up.Scale (inertiaInv.y);
	tmpMatrix.front = matrix.front.Scale (inertiaInv.z);
	inertiaMatrixInv = matrix.Transpose() * tmpMatrix;
}


void ZeroRigidBody::SetCollisionMatrix (
	ZeroMatrix &mat, 
	const ZeroVector3 &p)
{
	UpdateInertiaMatrix();
	tmpMatrix.right.x = 0.0;
	tmpMatrix.right.y = p.z;
	tmpMatrix.right.z = -p.y;

	tmpMatrix.up.x = -p.z;
	tmpMatrix.up.y = 0.0;
	tmpMatrix.up.z = p.x;

	tmpMatrix.front.x = p.y;
	tmpMatrix.front.y = -p.x;
	tmpMatrix.front.z = 0.0;

	mat = tmpMatrix * inertiaMatrixInv * tmpMatrix;

	mat.right = mat.right.Scale (-1.0);
	mat.up = mat.up.Scale (-1.0);
	mat.front = mat.front.Scale (-1.0);

	mat.right.x += massInv;
	mat.up.y += massInv;
	mat.front.z += massInv;
}



void ZeroRigidBody::DebugRender (
	const ZeroCamera* camera, 
	const ZeroMatrix &transform, 
	ZeroColor color) const
{
	mOwner->GetObjectSphere().DebugRender (camera, identityMatrix, color);
}
