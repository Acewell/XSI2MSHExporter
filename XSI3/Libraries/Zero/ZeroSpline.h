#ifndef _ZeroSpline_
#define _ZeroSpline_

#include "ZeroTypes.h"
#include "ZeroVector3.h"


namespace ZeroSpline
{
	const int curveOrder = 3;

	DLL_DECLSPEC int GetKnotIndex(const float *knotList, const int knotCount,
								  const float u);

	DLL_DECLSPEC ZeroVector3 GetPoint(const ZeroVector3 *pointList, const int pointCount,
									  const float *knotList, const int knotCount,
									  int knotIndex, const float u);

	DLL_DECLSPEC ZeroVector3 GetTangent(const ZeroVector3 *pointList, const int pointCount,
										const float *knotList, const int knotCount,
										int knotIndex, const float u);

	DLL_DECLSPEC void GetPoints(const ZeroVector3 *pointList, const int pointCount,
								const float *knotList, const int knotCount,
								float *dstArray, const int dstStride, const int dstCount);

	DLL_DECLSPEC void ComputeKnots(const ZeroVector3 *pointList, const int pointCount,
								   float *knotList, const int knotCount);
}

#endif
