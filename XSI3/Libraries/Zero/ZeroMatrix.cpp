/****************************************************************************
*
*  Visual C++ 6.0 created by: Julio Jerez
*
****************************************************************************/
#include "ZeroTypes.h"
#include "ZeroDebug.h"
#include "ZeroMatrix.h"
#include "ZeroVector3.h"
#include "ZeroVector4.h"
#include "ZeroMathTypes.h"
#include "ZeroQuaternion.h"
#include "ZeroRecord.h"

#pragma warning (push)
#pragma warning (disable : 4127)
#pragma warning (disable : 4100)

const ZeroMatrix identityMatrix(ZeroVector4 (1.0f, 0.0f, 0.0f, 0.0f),
								ZeroVector4 (0.0f, 1.0f, 0.0f, 0.0f),
								ZeroVector4 (0.0f, 0.0f, 1.0f, 0.0f),
								ZeroVector4 (0.0f, 0.0f, 0.0f, 1.0f));

const ZeroMatrix emptyMatrix(ZeroVector4 (0.0f, 0.0f, 0.0f, 0.0f),
							 ZeroVector4 (0.0f, 0.0f, 0.0f, 0.0f),
							 ZeroVector4 (0.0f, 0.0f, 0.0f, 0.0f),
							 ZeroVector4 (0.0f, 0.0f, 0.0f, 0.0f));


ZeroMatrix::ZeroMatrix(const ZeroQuaternion &q, const ZeroVector3 &p)
{
//	ZeroAssert ((fabsf (q.DotProduct (q) - 1.0f) < 0.001f, "quaternion q(%f %f %f %f) not normalized", q.x, q.y, q.z, q.s));

//	float n = 2.0f / (q.s * q.s + q.x * q.x + q.y * q.y + q.z * q.z);
	float n = 2.0f;
	float xn = q.x * n;
	float yn = q.y * n;
	float zn = q.z * n;
	float sn = q.s * n;
	float xx = q.x * xn;
	float xy = q.x * yn;
	float xz = q.x * zn;
	float xs = q.x * sn;
	float yy = q.y * yn;
	float yz = q.y * zn;
	float ys = q.y * sn;
	float zz = q.z * zn;
	float zs = q.z * sn;
	float ss = q.s * sn;

	right.x = ss + xx - 1.0f;
	right.y = xy + zs;
	right.z = xz - ys;
	right.w = 0.0f;
	up.x    = xy - zs;
	up.y    = ss + yy - 1.0f;
	up.z    = yz + xs;
	up.w    = 0.0f;
	front.x = xz + ys;
	front.y = yz - xs;
	front.z = ss + zz - 1.0f;
	front.w = 0.0f;
	posit.x = p.x;
	posit.y = p.y;
	posit.z = p.z;
	posit.w = 1.0f;

#ifdef WIN32
	_ASSERTE(SanityCheck());
#endif
}

ZeroMatrix::ZeroMatrix(const float neardist, const float fardist, const float fov, const float aspect)
{
	float f = fov * 0.5f;
	float cs = Zero_cos(f) / Zero_sin(f);
	float q = fardist / (fardist - neardist);

	right.x = cs * aspect;
	right.y = 0.0f;
	right.z = 0.0f;
	right.w = 0.0f;
	up.x    = 0.0f;
	up.y    = cs;
	up.z    = 0.0f;
	up.w    = 0.0f;
	front.x = 0.0f;
	front.y = 0.0f;
	front.z = q;
	front.w = 1.0f;
	posit.x = 0.0f;
	posit.y = 0.0f;
	posit.z = -q * neardist;
	posit.w = 0.0f;
#ifdef WIN32
	_ASSERTE (SanityCheck());
#endif
}


inline void ROT(ZeroMatrix &a,
	int i, int j, int k, int l, 
	float s, float tau)
{
	float g;
	float h;

	g = a[i][j]; 
	h = a[k][l]; 
	a[i][j] = g - s * (h + g * tau);
	a[k][l] = h + s * (g - h * tau);
}


ZeroMatrix ZeroMatrix::EigenVectors (
	ZeroVector3 &eigenvalues) const
{
	int i;
	int nrot;
	float t;
	float s;
	float h;
	float g;
	float c;
	float sm;
	float tau;
	float theta;
	float thresh;
	float b[3];
	float d[3];
	float z[3];

	ZeroMatrix M (*this);
	ZeroMatrix E (identityMatrix);

	b[0] = M[0][0];
	b[1] = M[1][1];
	b[2] = M[2][2];

	d[0] = M[0][0];
	d[1] = M[1][1];
	d[2] = M[2][2];

	z[0] = 0.0f;
	z[1] = 0.0f;
	z[2] = 0.0f;

	nrot = 0;

	for (i = 0; i < 50; i++) {
		sm = fabsf(M[0][1]) + fabsf(M[0][2]) + fabsf(M[1][2]);
		
		if (sm < EPSILON * 1e-5f) {
			_ASSERTE (fabsf(MagnitudeSq(E.right) - 1.0f) < EPSILON);
			_ASSERTE (fabsf(MagnitudeSq(E.up) - 1.0f) < EPSILON);
			_ASSERTE (fabsf(MagnitudeSq(E.front) - 1.0f) < EPSILON);
			
			if (DotProduct(CrossProduct(E.right, E.up), E.front) < 0.0f) {
				E.front = -E.front;
			}
			
			eigenvalues = ZeroVector3 (d[0], d[1], d[2]);
			return E.Transpose();
		}
		
		if (i < 3) {
			thresh = (float)(0.2 / 9.0) * sm;
		} else {
			thresh = 0.0f;
		}
		
		g = 100.0f * fabsf(M[0][1]);
		if ((i > 3) && (fabsf(d[0]) + g == fabsf(d[0])) && (fabsf(d[1]) + g == fabsf(d[1]))) {
			M[0][1] = 0.0f;
		} else if (fabsf(M[0][1]) > thresh)	{
			h = d[1] - d[0];
			if (fabsf(h) + g == fabsf(h))	{
				t = M[0][1] / h;
			} else {
				theta = 0.5f * h / (M[0][1]);
				t = 1.0f / (fabsf(theta) + sqrtf(1.0f + theta * theta));
				if (theta < 0.0f)	{
					t = -t;
				}
			}
			c = 1.0f / sqrtf( 1 + t * t); 
			s = t * c; 
			tau = s / (1.0f + c); 
			h = t * M[0][1];
			z[0] -= h; 
			z[1] += h; 
			d[0] -= h; 
			d[1] += h;
			M[0][1] = 0.0f;
			ROT(M, 0, 2, 1, 2, s, tau); 
			ROT(E, 0, 0, 0, 1, s, tau); 
			ROT(E, 1, 0, 1, 1, s, tau); 
			ROT(E, 2, 0, 2, 1, s, tau); 
			nrot++;
		}
		
		g = 100.0f * fabsf(M[0][2]);
		if ((i > 3) && (fabsf(d[0]) + g == fabsf(d[0])) && (fabsf(d[2]) + g == fabsf(d[2]))) {
			M[0][2] = 0.0f;
		} else if (fabsf(M[0][2]) > thresh)	{
			h = d[2] - d[0];
			if (fabsf(h) + g == fabsf(h))	{
				t = M[0][2] / h;
			} else {
				theta = 0.5f * h / M[0][2];
				t = 1.0f / (fabsf(theta) + sqrtf(1.0f + theta * theta));
				if (theta < 0.0f)	{
					t = -t;
				}
			}
			c = 1.0f / sqrtf(1 + t * t); 
			s = t * c; 
			tau = s / (1.0f + c);
			h = t * M[0][2];
			z[0] -= h; 
			z[2] += h; 
			d[0] -= h; 
			d[2] += h;
			M[0][2] = 0.0f;
			ROT(M, 0, 1, 1, 2, s, tau); 
			ROT(E, 0, 0, 0, 2, s, tau); 
			ROT(E, 1, 0, 1, 2, s, tau); 
			ROT(E, 2, 0, 2, 2, s, tau); 
		}
		
		g = 100.0f * fabsf(M[1][2]);
		if ((i > 3) && (fabsf(d[1]) + g == fabsf(d[1])) &&  (fabsf(d[2]) + g == fabsf(d[2]))) {
			M[1][2] = 0.0f;
		} else if (fabsf(M[1][2]) > thresh)	{
			h = d[2] - d[1];
			if (fabsf(h) + g == fabsf(h))	{
				t = M[1][2] / h;
			} else {
				theta = 0.5f * h / M[1][2];
				t = 1.0f / (fabsf(theta) + sqrtf(1.0f + theta * theta));
				if (theta < 0.0f)	{
					t = -t;
				}
			}
			c = 1.0f / sqrtf(1 + t * t);
			s = t * c; 
			tau = s / (1.0f + c); 
			h = t * M[1][2];
			z[1] -= h; 
			z[2] += h; 
			d[1] -= h; 
			d[2] += h;
			M[1][2] = 0.0f;
			ROT(M, 0, 1, 0, 2, s, tau); 
			ROT(E, 0, 1, 0, 2, s, tau); 
			ROT(E, 1, 1, 1, 2, s, tau); 
			ROT(E, 2, 1, 2, 2, s, tau); 
			nrot++;
		}
		
		b[0] += z[0]; d[0] = b[0]; z[0] = 0.0f;
		b[1] += z[1]; d[1] = b[1]; z[1] = 0.0f;
		b[2] += z[2]; d[2] = b[2]; z[2] = 0.0f;
	}
	
	eigenvalues = ZeroVector3 (d[0], d[1], d[2]);

	return identityMatrix;
}

#ifndef	_ASM_MATH 





ZeroVector3 ZeroMatrix::CalcPitchYawRollEulerAngles () const
{
	float yaw;
	float roll;
	float pitch;

	if ((fabsf (right.y) < 1.0e-12f) && (fabsf (right.x) < 1.0e-12f)) {
		yaw = -fsign (right.z) * PIBY2;
		pitch	= -Zero_atan2 (front.y, front.x);
		roll	= 0.0f;

#ifdef _DEBUG
ZeroMatrix pitchM (PitchMatrix (pitch));
ZeroMatrix yawM (YawMatrix (yaw));
ZeroMatrix rollM (RollMatrix (roll));
ZeroMatrix xxx (pitchM * yawM * rollM);

_ASSERTE (fabsf (DotProduct (right, xxx.right)) > 0.9999f);
_ASSERTE (fabsf (DotProduct (up,    xxx.up)) > 0.9999f);
_ASSERTE (fabsf (DotProduct (front, xxx.front)) > 0.9999f);
#endif


	} else {
		roll = Zero_atan2 (right.y, right.x); 
		pitch = Zero_atan2 (up.z, front.z); 

		ZeroMatrix rollMatrixInv (RollMatrix (-roll));
		ZeroMatrix pitchMatrixInv (PitchMatrix (-pitch));
		ZeroMatrix yawMatrix (pitchMatrixInv * *this * rollMatrixInv);
		yaw = Zero_atan2 (yawMatrix.front.x, yawMatrix.right.x);
	}

	return ZeroVector3 (pitch, yaw, roll);
}


ZeroVector4 ZeroMatrix::CalcIKValues () const
{
	float pitch;

	pitch = Zero_atan2 (up.z, front.z); 

	ZeroMatrix pitchMatrixInv (PitchMatrix (-pitch));
	ZeroMatrix yawRollMatrix (pitchMatrixInv * *this);

	yawRollMatrix.posit.w = pitch;

	return yawRollMatrix.posit;
}
/*
extern "C" { 
static __cdecl   void MatrixMultiply(ZeroMatrix &A,const ZeroMatrix &B, const ZeroMatrix &C)
{

		__asm__ volatile (
	
			"
			.set noreorder

			

			lqc2		vf1, 0(%1)
			lqc2		vf3, 0(%2)
			lqc2		vf4, 16(%2)
			lqc2		vf5, 32(%2)
			lqc2		vf6, 48(%2)


			li			$4, 4

Loop123:

	 		addi		$4, $4, -1

			vmulax.xyzw	ACC, vf3, vf1x
			vmadday.xyzw ACC, vf4, vf1y
			vmaddaz.xyzw ACC, vf5, vf1z
			vmaddw.xyzw vf2, vf6, vf1w
		
			lqc2		vf1, 16(%1)

			addi		%1, %1, 16
			addi		%0, %0, 16

			bne			$0, $4, Loop123
			sqc2		vf2, -16(%0)

			.set reorder
		" :
		:  "r" (&A),"r" (&B), "r" (&C)
		: "$4"
	);
}
}
*/

#if (PLATFORM == PLATFORM_GAMECUBE)

#define A00_A10 fp0
#define A20_A30 fp1
#define A01_A11 fp2
#define A21_A31 fp3
#define A02_A12 fp4
#define A22_A32 fp5
#define A03_A13 fp6
#define A23_A33 fp7

#define B00_B10 fp8
#define B20_B30 fp9
#define B01_B11 fp10
#define B21_B31 fp11
#define B02_B12 fp12
#define B22_B32 fp13
#define B03_B13 fp14
#define B23_B33 fp15

#define R00_R10 fp16
#define R20_R30 fp17
#define R01_R11 fp18
#define R21_R31 fp19
#define R02_R12 fp20
#define R22_R32 fp21
#define R03_R13 fp22
#define R23_R33 fp23

asm void ZeroMatrixMultiply
(
	register ZeroMatrix &R,        // r3
	register const ZeroMatrix &A,  // r4
	register const ZeroMatrix &B   // r5
)
{	
    nofralloc

    // save FP regs
    stwu        r1, -88(r1)
    stfd        fp14,  8(r1)
    stfd        fp15, 16(r1)
    stfd        fp16, 24(r1)
    stfd        fp17, 32(r1)
    stfd        fp18, 40(r1)
    stfd        fp19, 48(r1)
    stfd        fp20, 56(r1)
    stfd        fp21, 64(r1)
    stfd        fp22, 72(r1)
    stfd        fp23, 80(r1)

    psq_l   A00_A10, 0x00(A),0,0
    psq_l   A01_A11, 0x10(A),0,0
    psq_l   A02_A12, 0x20(A),0,0
    psq_l   A03_A13, 0x30(A),0,0

    psq_l   A20_A30, 0x08(A),0,0
    psq_l   A21_A31, 0x18(A),0,0
    psq_l   A22_A32, 0x28(A),0,0
    psq_l   A23_A33, 0x38(A),0,0

    psq_l   B00_B10, 0x00(B),0,0
    psq_l   B01_B11, 0x10(B),0,0
    psq_l   B02_B12, 0x20(B),0,0
    psq_l   B03_B13, 0x30(B),0,0

    psq_l   B20_B30, 0x08(B),0,0
    psq_l   B21_B31, 0x18(B),0,0
    psq_l   B22_B32, 0x28(B),0,0
    psq_l   B23_B33, 0x38(B),0,0
    
    ps_muls0  R00_R10, B00_B10, A00_A10
    ps_muls0  R20_R30, B20_B30, A00_A10
    ps_muls0  R01_R11, B00_B10, A01_A11
    ps_muls0  R21_R31, B20_B30, A01_A11
    ps_muls0  R02_R12, B00_B10, A02_A12
    ps_muls0  R22_R32, B20_B30, A02_A12
    ps_muls0  R03_R13, B00_B10, A03_A13
    ps_muls0  R23_R33, B20_B30, A03_A13
    
    ps_madds1 R00_R10, B01_B11, A00_A10, R00_R10
    ps_madds1 R20_R30, B21_B31, A00_A10, R20_R30
    ps_madds1 R01_R11, B01_B11, A01_A11, R01_R11
    ps_madds1 R21_R31, B21_B31, A01_A11, R21_R31
    ps_madds1 R02_R12, B01_B11, A02_A12, R02_R12
    ps_madds1 R22_R32, B21_B31, A02_A12, R22_R32
    ps_madds1 R03_R13, B01_B11, A03_A13, R03_R13
    ps_madds1 R23_R33, B21_B31, A03_A13, R23_R33
        
    ps_madds0 R00_R10, B02_B12, A20_A30, R00_R10
    ps_madds0 R20_R30, B22_B32, A20_A30, R20_R30
    ps_madds0 R01_R11, B02_B12, A21_A31, R01_R11
    ps_madds0 R21_R31, B22_B32, A21_A31, R21_R31
    ps_madds0 R02_R12, B02_B12, A22_A32, R02_R12
    ps_madds0 R22_R32, B22_B32, A22_A32, R22_R32
    ps_madds0 R03_R13, B02_B12, A23_A33, R03_R13
    ps_madds0 R23_R33, B22_B32, A23_A33, R23_R33

    ps_madds1 R00_R10, B03_B13, A20_A30, R00_R10
    ps_madds1 R20_R30, B23_B33, A20_A30, R20_R30
    ps_madds1 R01_R11, B03_B13, A21_A31, R01_R11
    ps_madds1 R21_R31, B23_B33, A21_A31, R21_R31
    ps_madds1 R02_R12, B03_B13, A22_A32, R02_R12
    ps_madds1 R22_R32, B23_B33, A22_A32, R22_R32
    ps_madds1 R03_R13, B03_B13, A23_A33, R03_R13
    ps_madds1 R23_R33, B23_B33, A23_A33, R23_R33
    
    psq_st    R00_R10, 0x00(R), 0, 0
    psq_st    R01_R11, 0x10(R), 0, 0
    psq_st    R02_R12, 0x20(R), 0, 0
    psq_st    R03_R13, 0x30(R), 0, 0

    psq_st    R20_R30, 0x08(R), 0, 0
    psq_st    R21_R31, 0x18(R), 0, 0
    psq_st    R22_R32, 0x28(R), 0, 0
    psq_st    R23_R33, 0x38(R), 0, 0

	// restore fpu regs    
    lfd         fp14,  8(r1)
    lfd         fp15, 16(r1)
    lfd         fp16, 24(r1)
    lfd         fp17, 32(r1)
    lfd         fp18, 40(r1)
    lfd         fp19, 48(r1)
    lfd         fp20, 56(r1)
    lfd         fp21, 64(r1)
    lfd         fp22, 72(r1)
    lfd         fp23, 80(r1)
    
    addi        r1, r1, 88

    blr
}
#elif (PLATFORM == PLATFORM_PS2)
void ZeroMatrixMultiply(ZeroMatrix &R, const ZeroMatrix &A, const ZeroMatrix &B)
{
#ifdef _DEBUG
	unsigned int pA = (unsigned int)(&A);
	unsigned int pB = (unsigned int)(&B);
	if((pA | pB) & 0x0F)
		 printf("Alignment busted on PS2. pA = %08X pB = %08X\n",pA,pB);
#endif

__asm__ volatile (		
			"
			.set noreorder

#copy the stack pointer



			lqc2		vf1, 0(%0)
			lqc2		vf3, 0(%1)
			lqc2		vf4, 16(%1)
			lqc2		vf5, 32(%1)
			lqc2		vf6, 48(%1)


			vmulax.xyzw	ACC, vf3, vf1x
			vmadday.xyzw ACC, vf4, vf1y
			vmaddaz.xyzw ACC, vf5, vf1z
			vmaddw.xyzw vf2, vf6, vf1w
		
			lqc2		vf1, 16(%0)

			vmulax.xyzw	ACC, vf3, vf1x
			vmadday.xyzw ACC, vf4, vf1y
			sqc2		vf2, 0(%2)
			vmaddaz.xyzw ACC, vf5, vf1z
			vmaddw.xyzw vf2, vf6, vf1w

			lqc2		vf1, 32(%0)

			vmulax.xyzw	ACC, vf3, vf1x
			vmadday.xyzw ACC, vf4, vf1y
			sqc2		vf2, 16(%2)
			vmaddaz.xyzw ACC, vf5, vf1z
			vmaddw.xyzw vf2, vf6, vf1w


			lqc2		vf1, 48(%0)

			vmulax.xyzw	ACC, vf3, vf1x
			vmadday.xyzw ACC, vf4, vf1y
			sqc2		vf2, 32(%2)
			vmaddaz.xyzw ACC, vf5, vf1z
			vmaddw.xyzw vf2, vf6, vf1w
			
			nop
			nop
			
			sqc2		vf2, 48(%2)




			.set reorder
		" :
		:  "r" (&A),"r" (&B), "r" (&R)
		: "$9"
	);
}
#else
void ZeroMatrixMultiply(ZeroMatrix &R, const ZeroMatrix &A, const ZeroMatrix &B)
{
	R = ZeroMatrix(ZeroVector4(A.right.x * B.right.x + A.right.y * B.up.x + A.right.z * B.front.x + A.right.w * B.posit.x,
	                           A.right.x * B.right.y + A.right.y * B.up.y + A.right.z * B.front.y + A.right.w * B.posit.y,
	                           A.right.x * B.right.z + A.right.y * B.up.z + A.right.z * B.front.z + A.right.w * B.posit.z,
	                           A.right.x * B.right.w + A.right.y * B.up.w + A.right.z * B.front.w + A.right.w * B.posit.w),

	               ZeroVector4(A.up.x * B.right.x + A.up.y * B.up.x + A.up.z * B.front.x + A.up.w * B.posit.x,
	                           A.up.x * B.right.y + A.up.y * B.up.y + A.up.z * B.front.y + A.up.w * B.posit.y,
    	                       A.up.x * B.right.z + A.up.y * B.up.z + A.up.z * B.front.z + A.up.w * B.posit.z,
	                           A.up.x * B.right.w + A.up.y * B.up.w + A.up.z * B.front.w + A.up.w * B.posit.w),

	               ZeroVector4(A.front.x * B.right.x + A.front.y * B.up.x + A.front.z * B.front.x + A.front.w * B.posit.x,
	                           A.front.x * B.right.y + A.front.y * B.up.y + A.front.z * B.front.y + A.front.w * B.posit.y,
	                           A.front.x * B.right.z + A.front.y * B.up.z + A.front.z * B.front.z + A.front.w * B.posit.z,
	                           A.front.x * B.right.w + A.front.y * B.up.w + A.front.z * B.front.w + A.front.w * B.posit.w),

	               ZeroVector4(A.posit.x * B.right.x + A.posit.y * B.up.x + A.posit.z * B.front.x + A.posit.w * B.posit.x,
	                           A.posit.x * B.right.y + A.posit.y * B.up.y + A.posit.z * B.front.y + A.posit.w * B.posit.y,
	                           A.posit.x * B.right.z + A.posit.y * B.up.z + A.posit.z * B.front.z + A.posit.w * B.posit.z,
	                           A.posit.x * B.right.w + A.posit.y * B.up.w + A.posit.z * B.front.w + A.posit.w * B.posit.w));
}
#endif

ZeroMatrix __cdecl operator*(const ZeroMatrix &A, const ZeroMatrix &B)
{
	ZeroMatrix R;
	ZeroMatrixMultiply(R, A, B);
	return R;
}												

ZeroMatrix &ZeroMatrix::operator=(register const ZeroMatrix &M)
{
#if (PLATFORM == PLATFORM_GAMECUBE)
	register ZeroMatrix &D = *this;
	asm
	{
	    psq_l   fp0, 0x00(M),0,0
	    psq_l   fp1, 0x08(M),0,0
	    psq_l   fp2, 0x10(M),0,0
	    psq_l   fp3, 0x18(M),0,0
	    psq_l   fp4, 0x20(M),0,0
	    psq_l   fp5, 0x28(M),0,0
	    psq_l   fp6, 0x30(M),0,0
	    psq_l   fp7, 0x38(M),0,0

	    psq_st  fp0, 0x00(D), 0, 0
	    psq_st  fp1, 0x08(D), 0, 0
	    psq_st  fp2, 0x10(D), 0, 0
	    psq_st  fp3, 0x18(D), 0, 0
	    psq_st  fp4, 0x20(D), 0, 0
	    psq_st  fp5, 0x28(D), 0, 0
	    psq_st  fp6, 0x30(D), 0, 0
	    psq_st  fp7, 0x38(D), 0, 0
	}
#else
	right.x = M.right.x;
	right.y = M.right.y;
	right.z = M.right.z;
	right.w = M.right.w;
	up.x = M.up.x;
	up.y = M.up.y;
	up.z = M.up.z;
	up.w = M.up.w;
	front.x = M.front.x;
	front.y = M.front.y;
	front.z = M.front.z;
	front.w = M.front.w;
	posit.x = M.posit.x;
	posit.y = M.posit.y;
	posit.z = M.posit.z;
	posit.w = M.posit.w;
#endif
	return (*this);
}


#if (PLATFORM == PLATFORM_GAMECUBE)
#define M00_M10 fp0
#define M20_M30 fp1
#define M01_M11 fp2
#define M21_M31 fp3
#define M02_M12 fp4
#define M22_M32 fp5
#define M03_M13 fp6
#define M23_M33 fp7

#define VX_VY   fp8
#define VZ_VW   fp9

#define RX_RY   fp10
#define RZ_RW   fp11
#endif

ZeroVector4 ZeroMatrix::RotateVector (const ZeroVector4 &v) const
{
// because of ZeroPlane
// TODO: Fix it
#if (PLATFORM == PLATFORM_PS2)
	ZeroVector4 ret;
	__asm__ volatile
	(
		"
			.set noreorder

			lqc2		vf1, 0(%0)

			lqc2		vf3, 0(%1)
			lqc2		vf4, 16(%1)
			lqc2		vf5, 32(%1)

			vmulax.xyzw	ACC, vf3, vf1x
			vmadday.xyzw ACC, vf4, vf1y
			vmaddz.xyzw vf2, vf5, vf1z
		
			sqc2		vf2, 0(%2)

			.set reorder
		" :
		: "r" (&v), "r" (this) , "r" (&ret)
	);
	return ret;
#elif (PLATFORM == PLATFORM_GAMECUBE)
	register ZeroVector4 ret;

	// this is just so I can access the variables in the asm
	register const ZeroMatrix  &M = *this;
	register const ZeroVector4 &V = v;
	register ZeroVector4 &R       = ret;
	__asm 
	{
		// load the matrix
	    psq_l   M00_M10, 0x00(M),0,0
	    psq_l   M20_M30, 0x08(M),0,0
	    psq_l   M01_M11, 0x10(M),0,0
	    psq_l   M21_M31, 0x18(M),0,0
	    psq_l   M02_M12, 0x20(M),0,0
	    psq_l   M22_M32, 0x28(M),0,0
		
		// load the vector
	    psq_l   VX_VY, 0x00(V),0,0
	    psq_l   VZ_VW, 0x08(V),0,0
	    
	    // get X & Y
	    ps_muls0  RX_RY, M00_M10, VX_VY
	    ps_madds1 RX_RY, M01_M11, VX_VY, RX_RY
	    ps_madds0 RX_RY, M02_M12, VZ_VW, RX_RY
	    
	    ps_muls0  RZ_RW, M20_M30, VX_VY
	    ps_madds1 RZ_RW, M21_M31, VX_VY, RZ_RW
	    ps_madds0 RZ_RW, M22_M32, VZ_VW, RZ_RW

		// store the result
	    psq_st  RX_RY, 0x00(R), 0, 0
	    psq_st  RZ_RW, 0x08(R), 0, 0
	}
	return R;
#else
	return ZeroVector4 (
		v.x * right.x + v.y * up.x + v.z * front.x,
		v.x * right.y + v.y * up.y + v.z * front.y,
		v.x * right.z + v.y * up.z + v.z * front.z
		);
#endif
}

ZeroVector4 ZeroMatrix::TransformVectorH (const ZeroVector4 &v) const
{
// because of ZeroPlane
// TODO: Fix it
#if (PLATFORM == PLATFORM_PS2)
	ZeroVector4 ret;

	__asm__ volatile
	(
	"
		.set noreorder

		lqc2		vf1, 0(%0)

		lqc2		vf3, 0(%1)
		lqc2		vf4, 16(%1)
		lqc2		vf5, 32(%1)
		lqc2		vf6, 48(%1)

		vmulax.xyzw	ACC, vf3, vf1x
		vmadday.xyzw ACC, vf4, vf1y
		vmaddaz.xyzw ACC, vf5, vf1z
		vmaddw.xyzw	vf2, vf6, vf1w
	
		sqc2		vf2, 0(%2)

		.set reorder
	" :
	: "r" (&v), "r" (this) , "r" (&ret)
	);
	return ret;
#elif (PLATFORM == PLATFORM_GAMECUBE)
	register ZeroVector4 ret;

	// this is just so I can access the variables in the asm
	register const ZeroMatrix  &M = *this;
	register const ZeroVector4 &V = v;
	register ZeroVector4 &R       = ret;
	__asm 
	{
		// load the matrix
	    psq_l   M00_M10, 0x00(M),0,0
	    psq_l   M20_M30, 0x08(M),0,0
	    psq_l   M01_M11, 0x10(M),0,0
	    psq_l   M21_M31, 0x18(M),0,0
	    psq_l   M02_M12, 0x20(M),0,0
	    psq_l   M22_M32, 0x28(M),0,0
	    psq_l   M03_M13, 0x30(M),0,0
	    psq_l   M23_M33, 0x38(M),0,0
		
		// load the vector
	    psq_l   VX_VY, 0x00(V),0,0
	    psq_l   VZ_VW, 0x08(V),0,0
	    
	    // get X & Y
	    ps_muls0  RX_RY, M00_M10, VX_VY
	    ps_madds1 RX_RY, M01_M11, VX_VY, RX_RY
	    ps_madds0 RX_RY, M02_M12, VZ_VW, RX_RY
	    ps_madds1 RX_RY, M03_M13, VZ_VW, RX_RY
	    
	    ps_muls0  RZ_RW, M20_M30, VX_VY
	    ps_madds1 RZ_RW, M21_M31, VX_VY, RZ_RW
	    ps_madds0 RZ_RW, M22_M32, VZ_VW, RZ_RW
	    ps_madds1 RZ_RW, M23_M33, VZ_VW, RZ_RW

		// store the result
	    psq_st  RX_RY, 0x00(R), 0, 0
	    psq_st  RZ_RW, 0x08(R), 0, 0
	}
	return R;
#else
	return ZeroVector4 (
		v.x * right.x + v.y * up.x + v.z * front.x + v.w * posit.x,
		v.x * right.y + v.y * up.y + v.z * front.y + v.w * posit.y,
		v.x * right.z + v.y * up.z + v.z * front.z + v.w * posit.z
		);
#endif
}

ZeroVector4 ZeroMatrix::FullTransformVector( const ZeroVector4 &v ) const
{
#if (PLATFORM == PLATFORM_GAMECUBE)
	register ZeroVector4 ret;

	// this is just so I can access the variables in the asm
	register const ZeroMatrix  &M = *this;
	register const ZeroVector4 &V = v;
	register ZeroVector4       &R = ret;
	__asm 
	{
		// load the matrix
	    psq_l   M00_M10, 0x00(M),0,0
	    psq_l   M20_M30, 0x08(M),0,0
	    psq_l   M01_M11, 0x10(M),0,0
	    psq_l   M21_M31, 0x18(M),0,0
	    psq_l   M02_M12, 0x20(M),0,0
	    psq_l   M22_M32, 0x28(M),0,0
	    psq_l   M03_M13, 0x30(M),0,0
	    psq_l   M23_M33, 0x38(M),0,0
		
		// load the vector
	    psq_l   VX_VY, 0x00(V),0,0
	    psq_l   VZ_VW, 0x08(V),0,0
	    
	    // get X & Y
	    ps_muls0  RX_RY, M00_M10, VX_VY
	    ps_madds1 RX_RY, M01_M11, VX_VY, RX_RY
	    ps_madds0 RX_RY, M02_M12, VZ_VW, RX_RY
	    ps_madds1 RX_RY, M03_M13, VZ_VW, RX_RY
	    
	    ps_muls0  RZ_RW, M20_M30, VX_VY
	    ps_madds1 RZ_RW, M21_M31, VX_VY, RZ_RW
	    ps_madds0 RZ_RW, M22_M32, VZ_VW, RZ_RW
	    ps_madds1 RZ_RW, M23_M33, VZ_VW, RZ_RW

		// store the result
	    psq_st  RX_RY, 0x00(R), 0, 0
	    psq_st  RZ_RW, 0x08(R), 0, 0
	}
	return R;
#else
	return ZeroVector4(
		v.x * right.x + v.y * up.x + v.z * front.x + v.w * posit.x,
		v.x * right.y + v.y * up.y + v.z * front.y + v.w * posit.y,
		v.x * right.z + v.y * up.z + v.z * front.z + v.w * posit.z,
		v.x * right.w + v.y * up.w + v.z * front.w + v.w * posit.w
	);
#endif
}

ZeroVector4 ZeroMatrix::TransformVector (const ZeroVector4 &v) const
{
// because of ZeroPlane
// TODO: Fix it
#if (PLATFORM == PLATFORM_PS2)
	ZeroVector4 ret;
	__asm__ volatile
	(
	"
		.set noreorder

		lqc2		vf1, 0(%0)

		lqc2		vf3, 0(%1)
		lqc2		vf4, 16(%1)
		lqc2		vf5, 32(%1)
		lqc2		vf6, 48(%1)

		vmulax.xyzw	ACC, vf3, vf1x
		vmadday.xyzw ACC, vf4, vf1y
		vmaddaz.xyzw ACC, vf5, vf1z
		vmaddw.xyzw	vf2, vf6, vf0w
	
		sqc2		vf2, 0(%2)

		.set reorder
	" :
	: "r" (&v), "r" (this) , "r" (&ret)
	);
	return ret;
#elif (PLATFORM == PLATFORM_GAMECUBE)
	register ZeroVector4 ret;

	// this is just so I can access the variables in the asm
	register const ZeroMatrix  &M = *this;
	register const ZeroVector4 &V = v;
	register ZeroVector4 &R       = ret;
	__asm 
	{
		// load the matrix
	    psq_l   M00_M10, 0x00(M),0,0
	    psq_l   M20_M30, 0x08(M),0,0
	    psq_l   M01_M11, 0x10(M),0,0
	    psq_l   M21_M31, 0x18(M),0,0
	    psq_l   M02_M12, 0x20(M),0,0
	    psq_l   M22_M32, 0x28(M),0,0
	    psq_l   M03_M13, 0x30(M),0,0
	    psq_l   M23_M33, 0x38(M),0,0
		
		// load the vector
	    psq_l   VX_VY, 0x00(V),0,0
	    psq_l   VZ_VW, 0x08(V),0,0
	    
	    // get X & Y
	    ps_muls0  RX_RY, M00_M10, VX_VY
	    ps_madds1 RX_RY, M01_M11, VX_VY, RX_RY
	    ps_madds0 RX_RY, M02_M12, VZ_VW, RX_RY
	    ps_add    RX_RY, M03_M13, RX_RY
	    
	    ps_muls0  RZ_RW, M20_M30, VX_VY
	    ps_madds1 RZ_RW, M21_M31, VX_VY, RZ_RW
	    ps_madds0 RZ_RW, M22_M32, VZ_VW, RZ_RW
	    ps_add    RZ_RW, M23_M33, RZ_RW

		// store the result
	    psq_st  RX_RY, 0x00(R), 0, 0
	    psq_st  RZ_RW, 0x08(R), 0, 0
	}
	return R;
#else
return RotateVector(v) + posit;
#endif
}

//#ifdef WIN32

void ZeroMatrix::TransformVectors (
	void const * dstPtr, 
	int dstStride,
	void const * srcPtr, 
	int srcStride,
	int count, int dummy) const
{
	int i;
	float x;
	float y;
	float z;
	float *dst;
	float *src;

	dst = (float *)dstPtr; 
	src = (float *)srcPtr; 

	dstStride /= sizeof (float);
	srcStride /= sizeof (float);


	for (i = 0; i < count; i ++) {
		x = src[0];
		y = src[1];
		z = src[2];

#if (PLATFORM != PLATFORM_GAMECUBE)
		_ASSERTE (_finite(x));
		_ASSERTE (_finite(y));
		_ASSERTE (_finite(z));
#endif

		dst[0] = x * right.x + y * up.x + z * front.x + posit.x;
		dst[1] = x * right.y + y * up.y + z * front.y + posit.y;
		dst[2] = x * right.z + y * up.z + z * front.z + posit.z;
		dst += dstStride;
		src += srcStride;
	}
}

/*
#else

void ZeroMatrix::TransformVectors (
	void const * dstPtr, 
	int dstStride,
	void const * srcPtr, 
	int srcStride,
	int count,int precision ) const
{
	int i;
	float x,fx;
	float y,fy;
	float z,fz;
	float *dst;
	float *src;
	int	  *compressedDst;
	int	  *compressedSrc;
	short ix,iy,iz,flag;
	int combinedXY;
	int combinedZFlags;

	dst = (float *)dstPtr; 
	src = (float *)srcPtr; 
	
	compressedDst = (int *)dstPtr; 
	compressedSrc = (int *)srcPtr; 

	dstStride /= sizeof (float);
	srcStride /= sizeof (float);


	for (i = 0; i < count; i ++) {
		if(!precision){
			x = src[0];
			y = src[1];
			z = src[2];
			dst[0] = x * right.x + y * up.x + z * front.x + posit.x;
			dst[1] = x * right.y + y * up.y + z * front.y + posit.y;
			dst[2] = x * right.z + y * up.z + z * front.z + posit.z;
		}
		else
		{
			ix = (short)(*(int*)(&(compressedSrc[0])) & 0x0000FFFF);
			iy = (short)((*(int*)(&(compressedSrc[0])) & 0xFFFF0000) >> 16);
			iz = (short)(*(int*)(&(compressedSrc[1])) & 0x0000FFFF);
			flag = (short)((*(int*)(&(compressedSrc[1])) & 0xFFFF0000) >> 16);

			x = ((float)ix) / ((float)(1<<(15-precision)));
			y = ((float)iy) / ((float)(1<<(15-precision)));
			z = ((float)iz) / ((float)(1<<(15-precision)));

			fx = x * right.x + y * up.x + z * front.x + posit.x;
			fy = x * right.y + y * up.y + z * front.y + posit.y;
			fz = x * right.z + y * up.z + z * front.z + posit.z;

			ix = (int) (fx * (1<<(15-precision)));
			iy = (int) (fy * (1<<(15-precision)));
			iz = (int) (fz * (1<<(15-precision)));
			

			combinedXY = (((iy & 0x0000FFFF)<<16) | (ix & 0x0000FFFF) );
			combinedZFlags = (((flag & 0x0000FFFF)<<16) | (iz & 0x0000FFFF) );
				
			compressedDst[0] = combinedXY;
			compressedDst[1] = combinedZFlags;
			
		}

		
		dst += dstStride;
		src += srcStride;
		compressedDst += dstStride;
		compressedSrc += srcStride;

	}
}

#endif
*/
void ZeroMatrix::IndexedTransformVectorsAndAccumulate (
	void const *dstPtr, 
	int destStrideInBytes, 
	int const *dstIndexArray,
	ZeroVector4 const* srcPtr, 
	int count) const
{
/*
	for (i = 0; i < vCount; i ++) {
		index = indexPtr[i];

		dstVertexPtr = (ZeroVector3*)&vPtr[index];
		dstNormalPtr = (ZeroVector3*)&nPtr[index];

		x = srcVertexPtr[i].x;
		y = srcVertexPtr[i].y;
		z = srcVertexPtr[i].z;
		w = srcVertexPtr[i].w;

		dstVertexPtr->x += x * matrix.right.x + y * matrix.up.x + z * matrix.front.x + w * matrix.posit.x;
		dstVertexPtr->y += x * matrix.right.y + y * matrix.up.y + z * matrix.front.y + w * matrix.posit.y;
		dstVertexPtr->z += x * matrix.right.z + y * matrix.up.z + z * matrix.front.z + w * matrix.posit.z;

		x = srcNormalPtr[i].x;
		y = srcNormalPtr[i].y;
		z = srcNormalPtr[i].z;
		w = 0.0;

		dstNormalPtr->x += x * matrix.right.x + y * matrix.up.x + z * matrix.front.x + w * matrix.posit.x;
		dstNormalPtr->y += x * matrix.right.y + y * matrix.up.y + z * matrix.front.y + w * matrix.posit.y;
		dstNormalPtr->z += x * matrix.right.z + y * matrix.up.z + z * matrix.front.z + w * matrix.posit.z;
	}
*/
}



#else

#define	STACK_FRAME(pushcount) (4 + pushcount*4)
#define NAKED __declspec(naked)

typedef struct
{
	float x;
	float y;
	float z;
}
TRIPLET;

typedef struct
{
	float x;
	float y;
	float z;
	float w;
}
QUADRUPLET;

typedef struct
{
	QUADRUPLET right;
	QUADRUPLET up;
	QUADRUPLET front;
	QUADRUPLET posit;
}
MATRIX_STRUCT;

static NAKED Transform_Component_Sparse ()
{
	__asm {
		fld   (MATRIX_STRUCT [ecx]).right.x				 // a00
		fmul  dword ptr (TRIPLET [edx]).x			  	 // x*a00

		fld   (MATRIX_STRUCT [ecx]).up.x				 // a10 x*a00
		fmul  dword ptr (TRIPLET [edx]).y			  	 // y*a10 x*a00

		fxch  st(1)										// x*a00 y*a10 
		fadd  dword ptr (MATRIX_STRUCT [ecx]).posit.x	// x*a00+a30 y*a10 

		fld   (MATRIX_STRUCT [ecx]).front.x				// a20 x*a00+a30 y*a10 
		fmul  dword ptr (TRIPLET [edx]).z			  	// a20*z x*a00+a30 y*a10 
														
		fxch  st(2)										// y*a10 x*a00+a30  a20*z 
		faddp st(1), st(0)								// x*a00+y*a10+a30  a20*z 
		faddp st(1), st(0)								// x*a00+y*a10+a20*z+a30  
		ret
	}
}

static NAKED Transform_Dense_Vector ()
{
	__asm {
		fld   (MATRIX_STRUCT [eax]).right.x				 // a00
		fmul  dword ptr (QUADRUPLET [edx]).x			 // x*a00

		fld   (MATRIX_STRUCT [eax]).up.x				 // a10 x*a00
		fmul  dword ptr (QUADRUPLET [edx]).y			 // y*a10 x*a00

		fld   (MATRIX_STRUCT [eax]).front.x				 // a20 y*a10 x*a00
		fmul  dword ptr (QUADRUPLET [edx]).z			 // z*a20 y*a10 x*a00

		fxch  st(1)										 // y*a10 z*a20 x*a00
		faddp st(2), st(0) 								 // z*a20 x*a00+y*a10 

		fld   (MATRIX_STRUCT [eax]).posit.x				 // a31 z*a20 x*a00+y*a10 
		fmul  dword ptr (QUADRUPLET [edx]).w			 // w*a31 z*a20 x*a00+y*a10 
														
		fxch  st(1)										// z*a20 w*a31  x*a00+y*a10 
		faddp st(1), st(0)								// w*a31  x*a00+y*a10+z*a20  
		faddp st(1), st(0)								// x*a00+y*a10+z*a20+w*a31    


		fld   (MATRIX_STRUCT [eax]).right.y				 // a00
		fmul  dword ptr (QUADRUPLET [edx]).x			 // x*a00

		fld   (MATRIX_STRUCT [eax]).up.y				 // a10 x*a00
		fmul  dword ptr (QUADRUPLET [edx]).y			 // y*a10 x*a00

		fld   (MATRIX_STRUCT [eax]).front.y				 // a20 y*a10 x*a00
		fmul  dword ptr (QUADRUPLET [edx]).z			 // z*a20 y*a10 x*a00

		fxch  st(1)										 // y*a10 z*a20 x*a00
		faddp st(2), st(0) 								 // z*a20 x*a00+y*a10 

		fld   (MATRIX_STRUCT [eax]).posit.y				 // a31 z*a20 x*a00+y*a10 
		fmul  dword ptr (QUADRUPLET [edx]).w			 // w*a31 z*a20 x*a00+y*a10 
														
		fxch  st(1)										// z*a20 w*a31  x*a00+y*a10 
		faddp st(1), st(0)								// w*a31  x*a00+y*a10+z*a20  
		faddp st(1), st(0)								// x*a00+y*a10+z*a20+w*a31    


		fld   (MATRIX_STRUCT [eax]).right.z				 // a00
		fmul  dword ptr (QUADRUPLET [edx]).x			 // x*a00

		fld   (MATRIX_STRUCT [eax]).up.z				 // a10 x*a00
		fmul  dword ptr (QUADRUPLET [edx]).y			 // y*a10 x*a00

		fld   (MATRIX_STRUCT [eax]).front.z				 // a20 y*a10 x*a00
		fmul  dword ptr (QUADRUPLET [edx]).z			 // z*a20 y*a10 x*a00

		fxch  st(1)										 // y*a10 z*a20 x*a00
		faddp st(2), st(0) 								 // z*a20 x*a00+y*a10 

		fld   (MATRIX_STRUCT [eax]).posit.z				 // a31 z*a20 x*a00+y*a10 
		fmul  dword ptr (QUADRUPLET [edx]).w			 // w*a31 z*a20 x*a00+y*a10 
														
		fxch  st(1)										// z*a20 w*a31  x*a00+y*a10 
		faddp st(1), st(0)								// w*a31  x*a00+y*a10+z*a20  
		faddp st(1), st(0)								// x*a00+y*a10+z*a20+w*a31    


		fld   (MATRIX_STRUCT [eax]).right.w				 // a00
		fmul  dword ptr (QUADRUPLET [edx]).x			 // x*a00

		fld   (MATRIX_STRUCT [eax]).up.w				 // a10 x*a00
		fmul  dword ptr (QUADRUPLET [edx]).y			 // y*a10 x*a00

		fld   (MATRIX_STRUCT [eax]).front.w				 // a20 y*a10 x*a00
		fmul  dword ptr (QUADRUPLET [edx]).z			 // z*a20 y*a10 x*a00

		fxch  st(1)										 // y*a10 z*a20 x*a00
		faddp st(2), st(0) 								 // z*a20 x*a00+y*a10 

		fld   (MATRIX_STRUCT [eax]).posit.w				 // a31 z*a20 x*a00+y*a10 
		fmul  dword ptr (QUADRUPLET [edx]).w			 // w*a31 z*a20 x*a00+y*a10 
														
		fxch  st(1)										// z*a20 w*a31  x*a00+y*a10 
		faddp st(1), st(0)								// w*a31  x*a00+y*a10+z*a20  
		faddp st(1), st(0)								// x*a00+y*a10+z*a20+w*a31    

		ret
	}
}


NAKED ZeroVector3 ZeroMatrix::Vector_Transform (const ZeroVector3 &v) const 
{
	__asm {
		mov		eax, [esp + STACK_FRAME(0)]
		mov		edx, [esp + STACK_FRAME(0) + 4]
		call	Transform_Component_Sparse							//x0
		lea		ecx, (MATRIX_STRUCT [ecx]).right.y		    
		call	Transform_Component_Sparse							//y0 x0
		lea		ecx, (MATRIX_STRUCT [ecx]).right.y
		call	Transform_Component_Sparse							//z0 y0 x0
		mov		edx, [eax]
		fxch	st(2)										//x0 y0 z0
		fstp	dword ptr (TRIPLET [eax]).x					//y0 z0
		fstp	dword ptr (TRIPLET [eax]).y					//z0
		fstp	dword ptr (TRIPLET [eax]).z	  

#ifdef _SANITY_CHECK
		push	ecx
		push	eax
		push	edx
		mov		ecx, eax
		call	Vector_Sanity_Check
		pop		edx
		pop		eax
		pop		ecx
#endif		
		ret		8
	}
}




NAKED void ZeroMatrix::Transform_Vectors (
	const float *Dst, 
	const float *Src, 
	int Stride, 
	int Count) const
{
	__asm {
		push	esi
		push	edi
		push	ebx

		mov		eax, [esp + STACK_FRAME(3)]			//	Get Dst
		mov		edx, [esp + STACK_FRAME(3) + 4]		//	Get Src
		mov		edi, [esp + STACK_FRAME(3) + 8]		//	Get Stride
		mov		ebx, [esp + STACK_FRAME(3) + 12]	 //	Get Count

		mov		esi, [eax]
		mov		esi, [eax + edi]

		test	ebx, ebx
		mov		esi, ecx
		jmp		End_Transform_Loop
	Begin_Transform_Loop:

		call	Transform_Component_Sparse							//x0
		lea		ecx, (MATRIX_STRUCT [ecx]).right.y		    
		call	Transform_Component_Sparse							//y0 x0
		lea		ecx, (MATRIX_STRUCT [ecx]).right.y
		call	Transform_Component_Sparse							//z0 y0 x0
		add		edx, edi

		mov		ecx, esi
		fstp	dword ptr (TRIPLET [eax]).z					//y0 z0
		fstp	dword ptr (TRIPLET [eax]).y					//z0
		fstp	dword ptr (TRIPLET [eax]).x					

#ifdef _SANITY_CHECK
		push	ecx
		push	eax
		push	edx
		mov		ecx, eax
		call	Vector_Sanity_Check
		pop		edx
		pop		eax
		pop		ecx
#endif		

		add		eax, edi
		dec		ebx
	End_Transform_Loop:	
		jg		Begin_Transform_Loop
		pop		ebx
		pop		edi
		pop		esi

		ret		16 
	}
}



NAKED ZeroMatrix operator* (const ZeroMatrix &A, const ZeroMatrix &B)
{
	__asm {
		push	esi				
		mov		eax, [ecx]

		mov		esi, 4
	Begin_Matrix_Loop:	

//		mov		eax, [ecx + size QUADRUPLET]		//may cause crash on Win2k, read-out-of-bounds

		mov		eax, [esp + STACK_FRAME(1)]
		call	Transform_Dense_Vector						  // w z y x
		add		edx, size QUADRUPLET

		fxch	st(3)										  // x z y w
		fstp	dword ptr (QUADRUPLET [ecx]).x				  
		fstp	dword ptr (QUADRUPLET [ecx]).z				   
		fstp	dword ptr (QUADRUPLET [ecx]).y					
		fstp	dword ptr (QUADRUPLET [ecx]).w					
		add		ecx, size QUADRUPLET

		dec		esi
		jnz		Begin_Matrix_Loop
		pop		esi
		lea		eax, [ecx - size MATRIX_STRUCT]

#ifdef _SANITY_CHECK
		push	ecx
		push	eax
		push	edx
		mov		ecx, eax
		call	Matrix_Sanity_Check
		pop		edx
		pop		eax
		pop		ecx
#endif		

		ret		4
	}
}

#endif

#pragma warning (pop)

DLL_DECLSPEC void ZeroMatrix::RecordValues(int mode)
{
	ZeroValue::Type t = (ZeroValue::Type)mode;
	ZeroValue::Record(posit.x, t);
	ZeroValue::Record(posit.z, t);
	ZeroValue::Record(front.x, t);
	ZeroValue::Record(front.z, t);
	ZeroValue::Record(up.y, t);
}
