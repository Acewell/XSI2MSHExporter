#include "ZeroTypes.h"
#include "ZeroCamera.h"
#include "ZeroCameraSphere.h"

// visibility test pointers
ZeroCameraSphere::CachedVisibilityTest ZeroCameraSphere::PlaneTestArray[6] = 
{
	&ZeroCameraSphere::FrontTest,
	&ZeroCameraSphere::RearTest,
	&ZeroCameraSphere::LeftTest,
	&ZeroCameraSphere::RightTest,
	&ZeroCameraSphere::TopTest,
	&ZeroCameraSphere::BottomTest,
};

ZeroCameraSphere::ZeroCameraSphere ()
: ZeroSphere ()
{
	planeTest = &ZeroCameraSphere::FrontTest;
}

ZeroCameraSphere::ZeroCameraSphere (const ZeroMatrix &matrix)
: ZeroSphere (matrix)
{
	planeTest = &ZeroCameraSphere::FrontTest;
}

ZeroCameraSphere::ZeroCameraSphere (const ZeroSphere &sphere)
: ZeroSphere (sphere)
{
	planeTest = &ZeroCameraSphere::FrontTest;
}


int ZeroCameraSphere::FrontTest (const ZeroMatrix &matrix, const ZeroPlane* plane) const
{
	float R;
	float dR;
	FLOATSIGN flag0;
	FLOATSIGN flag1;

	dR = size.x * fabsf (matrix.right.z) + 
		 size.y * fabsf (matrix.up.z) + 
		 size.z * fabsf (matrix.front.z);
	R = plane[5].z * matrix.posit.z + plane[5].w; 
	
	flag0.f = R + dR;
	flag1.f = R - dR;
	flag0.i = flag0.i >> 31 & 1;
	flag1.i = flag1.i >> 31 & 1;
	return -(flag0.i & flag1.i) | flag0.i | flag1.i;
}

int ZeroCameraSphere::RearTest (const ZeroMatrix &matrix, const ZeroPlane* plane) const
{
	float R;
	float dR;
	FLOATSIGN flag0;
	FLOATSIGN flag1;

	dR = size.x * fabsf (matrix.right.z) + 
		 size.y * fabsf (matrix.up.z) + 
		 size.z * fabsf (matrix.front.z);
	R = plane[4].z * matrix.posit.z + plane[4].w; 
	
	flag0.f = R + dR;
	flag1.f = R - dR;
	flag0.i = flag0.i >> 31 & 1;
	flag1.i = flag1.i >> 31 & 1;
	return -(flag0.i & flag1.i) | flag0.i | flag1.i;
}

int ZeroCameraSphere::LeftTest (const ZeroMatrix &matrix, const ZeroPlane* plane) const
{
	float R;
	float dR;
	FLOATSIGN flag0;
	FLOATSIGN flag1;

	dR = size.x * fabsf (matrix.right.x * plane[0].x + matrix.right.z * plane[0].z) + 
		 size.y * fabsf (matrix.up.x    * plane[0].x + matrix.up.z    * plane[0].z) +
		 size.z * fabsf (matrix.front.x * plane[0].x + matrix.front.z * plane[0].z);
	R = plane[0].x * matrix.posit.x + plane[0].z * matrix.posit.z; 
	
	flag0.f = R + dR;
	flag1.f = R - dR;
	flag0.i = flag0.i >> 31 & 1;
	flag1.i = flag1.i >> 31 & 1;
	return -(flag0.i & flag1.i) | flag0.i | flag1.i;
}

int ZeroCameraSphere::RightTest (const ZeroMatrix &matrix, const ZeroPlane *plane) const
{
	float R;
	float dR;
	FLOATSIGN flag0;
	FLOATSIGN flag1;

	dR = size.x * fabsf (matrix.right.x * plane[1].x + matrix.right.z * plane[1].z) + 
		 size.y * fabsf (matrix.up.x    * plane[1].x + matrix.up.z    * plane[1].z) +
		 size.z * fabsf (matrix.front.x * plane[1].x + matrix.front.z * plane[1].z);
	R = plane[1].x * matrix.posit.x + plane[1].z * matrix.posit.z; 
	

	flag0.f = R + dR;
	flag1.f = R - dR;
	flag0.i = flag0.i >> 31 & 1;
	flag1.i = flag1.i >> 31 & 1;
	return -(flag0.i & flag1.i) | flag0.i | flag1.i;
}

int ZeroCameraSphere::BottomTest (const ZeroMatrix &matrix, const ZeroPlane *plane) const
{
	float R;
	float dR;
	FLOATSIGN flag0;
	FLOATSIGN flag1;

	dR = size.x * fabsf (matrix.right.y * plane[2].y + matrix.right.z * plane[2].z) + 
		 size.y * fabsf (matrix.up.y    * plane[2].y + matrix.up.z    * plane[2].z) +
		 size.z * fabsf (matrix.front.y * plane[2].y + matrix.front.z * plane[2].z);
	R = plane[2].y * matrix.posit.y + plane[2].z * matrix.posit.z; 

	flag0.f = R + dR;
	flag1.f = R - dR;
	flag0.i = flag0.i >> 31 & 1;
	flag1.i = flag1.i >> 31 & 1;
	return -(flag0.i & flag1.i) | flag0.i | flag1.i;
}


int ZeroCameraSphere::TopTest (const ZeroMatrix &matrix, const ZeroPlane *plane) const
{
	float R;
	float dR;
	FLOATSIGN flag0;
	FLOATSIGN flag1;

	dR = size.x * fabsf (matrix.right.y * plane[3].y + matrix.right.z * plane[3].z) + 
		 size.y * fabsf (matrix.up.y    * plane[3].y + matrix.up.z    * plane[3].z) +
		 size.z * fabsf (matrix.front.y * plane[3].y + matrix.front.z * plane[3].z);
	R = plane[3].y * matrix.posit.y + plane[3].z * matrix.posit.z; 

	flag0.f = R + dR;
	flag1.f = R - dR;
	flag0.i = flag0.i >> 31 & 1;
	flag1.i = flag1.i >> 31 & 1;
	return -(flag0.i & flag1.i) | flag0.i | flag1.i;
}

// code values

int ZeroCameraSphere::VisibilityTestLow (
	const ZeroCamera *camera,
	const ZeroMatrix &view) const
{
	int i;
	int code;
	const ZeroPlane* planes;
	const ZeroPlane* guardPlanes;

	planes = camera->GetViewVolume();

	code = (this->*planeTest) (view, planes);
	if (code != -1) {
		for (i = 0; i < 6; i ++) {
			code |= (this->*PlaneTestArray[i]) (view, planes);
			if (code == -1) {
				planeTest = PlaneTestArray[i];
				return -1;
			}
		}

		if (code) {
			guardPlanes = camera->GetGuadBandViewVolume();
			if (guardPlanes) {
				code = 0;
				for (i = 0; i < 6; i ++) {
					code |= (this->*PlaneTestArray[i]) (view, guardPlanes);
					_ASSERTE (code >= 0);
					if (code) {
						return code;
					}
				}
			}
		}
	}

   return code;

}

int ZeroCameraSphere::VisibilityTest (const ZeroCamera* camera) const
{
	ZeroMatrix view;
	ZeroMatrixMultiply(view, *this, camera->GetViewMatrix());
	return VisibilityTestLow (camera, view);
}

int ZeroCameraSphere::VisibilityTest (const ZeroCamera* camera, const ZeroMatrix &world) const 
{
	ZeroMatrix view;
	ZeroMatrixMultiply(view, *this, world);
	ZeroMatrixMultiply(view, view, camera->GetViewMatrix());
	return VisibilityTestLow (camera, view);
}
