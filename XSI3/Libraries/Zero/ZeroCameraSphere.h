#ifndef __ZeroCameraSphere__
#define __ZeroCameraSphere__

#include "ZeroSphere.h"

class ZeroCameraSphere: public ZeroSphere
{
	typedef int (ZeroCameraSphere::*CachedVisibilityTest) (const ZeroMatrix &point, const ZeroPlane* plane) const;

	mutable CachedVisibilityTest planeTest;
	static CachedVisibilityTest PlaneTestArray[6];

	int FrontTest  (const ZeroMatrix &point, const ZeroPlane *plane) const;
	int RearTest   (const ZeroMatrix &point, const ZeroPlane *plane) const;
	int LeftTest   (const ZeroMatrix &point, const ZeroPlane *plane) const;
	int RightTest  (const ZeroMatrix &point, const ZeroPlane *plane) const;
	int TopTest    (const ZeroMatrix &point, const ZeroPlane *plane) const;
	int BottomTest (const ZeroMatrix &point, const ZeroPlane *plane) const;
	int VisibilityTestLow (const ZeroCamera *camera, const ZeroMatrix &view) const;

	public:
	DLL_DECLSPEC ZeroCameraSphere();
	DLL_DECLSPEC ZeroCameraSphere(const ZeroMatrix &matrix);
	DLL_DECLSPEC ZeroCameraSphere(const ZeroSphere &sphere);

	DLL_DECLSPEC int VisibilityTest (const ZeroCamera* camera) const;
	DLL_DECLSPEC int VisibilityTest (const ZeroCamera* camera, const ZeroMatrix &world) const; 
};

#endif
