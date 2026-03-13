#include "ZeroTypes.h"
#include "ZeroVector4.h"
#include "ZeroQuaternion.h"
#include "ZeroMathTypes.h"

const ZeroQuaternion identityQuaternion(1.0f, 0.0f, 0.0f, 0.0f);

enum QUAT_INDEX
{
   X_INDEX = 0,
   Y_INDEX = 1,
   Z_INDEX = 2,
};
static QUAT_INDEX QIndex [] = {Y_INDEX, Z_INDEX, X_INDEX};

ZeroQuaternion::ZeroQuaternion(const ZeroMatrix &matrix) 
{
	float *ptr;
	float trace;
	QUAT_INDEX i;
	QUAT_INDEX j; 
	QUAT_INDEX k;
	float magnitude;
	
	trace = matrix.right.x + matrix.up.y + matrix.front.z;
	
	if (trace > 0.0f) {
	   trace = sqrtf (trace + 1.0f);
	   s = 0.5f * trace;
	   trace = 0.5f / trace;
	
	   x = (matrix.front.y - matrix.up.z) * trace;
	   y = (matrix.right.z - matrix.front.x) * trace;
	   z = (matrix.up.x    - matrix.right.y) * trace;
	} else {
	   i = X_INDEX;

	   if (matrix[Y_INDEX][Y_INDEX] > matrix[X_INDEX][X_INDEX] ) {
		   i = Y_INDEX;
	   }
	   if (matrix[Z_INDEX][Z_INDEX] > matrix[i][i] ) {
		   i = Z_INDEX;
	   }
	   j = QIndex [i];
	   k = QIndex [j];
	
	   trace = sqrtf (1.0f + matrix[i][i] - matrix[j][j] - matrix[k][k]);

	   ptr = &x;
	   ptr[i] = 0.5f * trace;
	   trace = 0.5f / trace;
	   ptr[3] = (matrix[k][j] - matrix[j][k]) * trace;
	   ptr[j] = (matrix[j][i] + matrix[i][j]) * trace;
	   ptr[k] = (matrix[k][i] + matrix[i][k]) * trace;
	}
	x = -x;
	y = -y;
	z = -z;

	magnitude = (*this).DotProduct (*this);
	if (fabsf(magnitude - 1.0f) > EPSILON)	{
		*this = (*this).Scale (rsqrt(magnitude));
	}

#ifdef WIN32
	_ASSERTE(SanityCheck());
#endif
}

ZeroVector3 ZeroQuaternion::RotateVector(const ZeroVector3 &v) const
{
	// for now do the naive operation
	ZeroMatrix mat(*this, ZeroVector3(0, 0, 0));
	return mat.RotateVector(v);
}

ZeroVector3 ZeroQuaternion::UnrotateVector(const ZeroVector3 &v) const
{
	ZeroMatrix mat(*this, ZeroVector3(0, 0, 0));
	return mat.UnrotateVector(v);
}

ZeroQuaternion __cdecl Interpolate(
	const ZeroQuaternion &a, 
	const ZeroQuaternion &b, 
	float t)
{
	float dot;
	float omega;
	float scale;
	float scale0;
	float scale1;
	float magnitude;

	dot = a.s * b.s + a.x * b.x + a.y * b.y + a.z * b.z;
	if (dot >= 0.0f) {
		if ((1.0f - dot) < 0.01f) {
			scale0 = 1.0f - t;
			scale1 = t;
		} else {
			omega = Zero_acos(dot);
			scale = 1.0f / Zero_sin(omega);
			scale0 = Zero_sin((1.0f - t) * omega) * scale;
			scale1 = Zero_sin(t * omega) * scale;
		}
	} else {
		dot = -dot;
		if ((1.0f - dot) < 0.01f) {
			scale0 = 1.0f - t;
			scale1 = -t;
		} else {
			omega = Zero_acos(dot);
			scale = 1.0f / Zero_sin(omega);
			scale0 = Zero_sin((1.0f - t) * omega) * scale;
			scale1 = -Zero_sin(t * omega) * scale;
		}
	}

	ZeroQuaternion q(a.Scale (scale0) + b.Scale (scale1));

	magnitude = q.DotProduct (q);
	if (fabsf(magnitude - 1.0f) > EPSILON)	{
		q = q.Scale (rsqrt (magnitude)); 
	}

#ifdef WIN32
	_ASSERTE(q.SanityCheck());
#endif
	return q;
}


ZeroVector3 ZeroQuaternion::GetOmega (const ZeroQuaternion &q1, float dt) const
{
	float dot;
	float omegaMag;
	float halfAngleSin;
	float dirInvMag;

	ZeroQuaternion dq (Inverse() * q1);										 
	ZeroVector4 omega (dq.x, dq.y, dq.z);									 
										                                                 
	dot = ::DotProduct (omega, omega) + 1.0e-6f;							 
	dirInvMag = rsqrt (dot);													 
	halfAngleSin = dirInvMag * dot;										
																						 
	omegaMag = 2.0f * Zero_atan2 (halfAngleSin, dq.s) / dt;		
																						                                                 
	return omega.Scale (omegaMag * dirInvMag);						
}



ZeroQuaternion __cdecl operator*(const ZeroQuaternion &a, const ZeroQuaternion &b)
{
/*
	// Julio's original version
	return ZeroQuaternion(
		a.s * b.s - a.x * b.x - a.y * b.y - a.z * b.z, 
		a.x * b.s + a.s * b.x - a.z * b.y + a.y * b.z, 
		a.y * b.s + a.z * b.x + a.s * b.y - a.x * b.z, 
		a.z * b.s - a.y * b.x + a.x * b.y + a.s * b.z
		);
*/
/*
	// Ken's rearranged version
	return ZeroQuaternion(
		a.s * b.s - a.x * b.x - a.y * b.y - a.z * b.z, 
		a.s * b.x + a.x * b.s + a.y * b.z - a.z * b.y, 
		a.s * b.y - a.x * b.z + a.y * b.s + a.z * b.x, 
		a.s * b.z + a.x * b.y - a.y * b.x + a.z * b.s
		);
*/
	// Ken's reversed version (equivalent to matrix multiply)
	return ZeroQuaternion(
		a.s * b.s - a.x * b.x - a.y * b.y - a.z * b.z, 
		a.s * b.x + a.x * b.s - a.y * b.z + a.z * b.y, 
		a.s * b.y + a.x * b.z + a.y * b.s - a.z * b.x, 
		a.s * b.z - a.x * b.y + a.y * b.x + a.z * b.s
		);
}
