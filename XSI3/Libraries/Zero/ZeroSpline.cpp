#include "ZeroTypes.h"
#include "ZeroSpline.h"

namespace ZeroSpline
{
	//
	// GET A KNOT INDEX
	//
	int GetKnotIndex(const float *knotList, const int knotCount,
					 const float u)
	{
		// perform a binary search to find the knot index
		int minIndex = curveOrder - 1;
		int maxIndex = knotCount - curveOrder;
		int knotIndex = (minIndex + maxIndex) >> 1;
		for (;;)
		{
			if (u < knotList[knotIndex])
			{
				maxIndex = knotIndex - 1;
				knotIndex = (minIndex + maxIndex) >> 1;
				continue;
			}
			if (u >= knotList[knotIndex+1])
			{
				minIndex = knotIndex + 1;
				knotIndex = (minIndex + maxIndex) >> 1;
				continue;
			}
			break;
		}

		return knotIndex;
	}

	//
	// EVALUATE THE SPLINE POSITION AT A GIVEN PARAMETER VALUE
	//
	ZeroVector3 GetPoint(const ZeroVector3 *pointList, const int pointCount,
						 const float *knotList, const int knotCount,
						 int knotIndex, const float u)
	{
		int i1, i2;

		// make sure the knot count is correct
		_ASSERTE(knotCount == pointCount + curveOrder);

		// make sure the knot index is correct
		_ASSERTE(knotIndex >= curveOrder - 1);
		_ASSERTE(knotIndex <= knotCount - curveOrder);
		_ASSERTE(u >= knotList[knotIndex]);
		_ASSERTE(u < knotList[knotIndex + 1]);

		// get the corresponding point index
		int pointIndex = knotIndex - curveOrder + 1;

		// calculate the coefficients
		float C[curveOrder];
		float L[curveOrder];
		float R[curveOrder];
		C[0] = 1.0f;
		for (i1 = 1; i1 < curveOrder; i1++)
		{
			L[i1] = u - knotList[knotIndex + 1 - i1];
			R[i1] = knotList[knotIndex + i1] - u;
			float c = 0.0f;
			for (i2 = 0; i2 < i1; i2++)
			{
				float t = C[i2] / (R[i2 + 1] + L[i1 - i2]);
				C[i2] = c + R[i2 + 1] * t;
				c = L[i1 - i2] * t;
			}
			C[i1] = c;
		}

		// generate the position
		ZeroVector3 p = pointList[pointIndex] * C[0];
		for (i1 = 1; i1 < curveOrder; i1++)
		{
			p += pointList[pointIndex + i1] * C[i1];
		}
		return p;
	}

	//
	// EVALUATE THE SPLINE TANGENT AT A GIVEN PARAMETER VALUE
	//
	ZeroVector3 GetTangent(const ZeroVector3 *pointList, const int pointCount,
						   const float *knotList, const int knotCount,
						   int knotIndex, const float u)
	{
		int i1, i2;

		// make sure the knot count is correct
		_ASSERTE(knotCount == pointCount + curveOrder);

		// make sure the knot index is correct
		_ASSERTE(knotIndex >= curveOrder - 1);
		_ASSERTE(knotIndex <= knotCount - curveOrder);
		_ASSERTE(u >= knotList[knotIndex]);
		_ASSERTE(u < knotList[knotIndex + 1]);

		// get the corresponding point index
		int pointIndex = knotIndex - curveOrder + 1;

		// calculate the coefficients
		float C[curveOrder];
		float L[curveOrder];
		float R[curveOrder];
		C[0] = 1.0f;
		for (i1 = 1; i1 < curveOrder; i1++)
		{
			L[i1] = u - knotList[knotIndex + 1 - i1];
			R[i1] = knotList[knotIndex + i1] - u;
			float c = 0.0f;
			for (i2 = 0; i2 < i1; i2++)
			{
				float t = C[i2] / (R[i2 + 1] + L[i1 - i2]);
				C[i2] = c + R[i2 + 1] * t;
				c = L[i1 - i2] * t;
			}
			C[i1] = c;
		}

		// generate the direction
		ZeroVector3 p = pointList[pointIndex] * C[1];
		for (i1 = 2; i1 < curveOrder; i1++)
		{
			p += pointList[pointIndex + i1] * C[i1] * (float)i1;
		}
		return p;
	}

	//
	// EVALUATE A SPLINE INTO AN ARRAY OF POINTS
	//
	void GetPoints(const ZeroVector3 *pointList, const int pointCount,
				   const float *knotList, const int knotCount,
				   float *dstArray, const int dstStride, const int dstCount)
	{
		static const int curveOrder = 3;

		// initial parameter value
		float u = 0.0f;

		// incremental parameter value
		float du = (1.0f - EPSILON) / (dstCount - 1);

		// initial knot index
		int knotIndex = curveOrder - 1;

		// for each output point
		for (int i = 0; i < dstCount; i++)
		{
			// advance the knot index if necessary
			while (u > knotList[knotIndex + 1])
			{
				knotIndex++;
				_ASSERTE(knotIndex <= knotCount - curveOrder);
			}

			// generate the position
			ZeroVector3 p (GetPoint(pointList, pointCount, knotList, knotCount, knotIndex, u));
			dstArray[i * dstStride] = p.x;
			dstArray[i * dstStride + 1] = p.y;
			dstArray[i * dstStride + 2] = p.z;

			// advance the parameter
			u += du;
		}
	}

	//
	// COMPUTE KNOTS GIVEN AN ARRAY OF POINTS
	//
	void ComputeKnots(const ZeroVector3 *pointList, const int pointCount,
					  float *knotList, const int knotCount)
	{
		int i, j;

		// make sure the point count is high enough
		_ASSERTE(pointCount >= curveOrder);

		// make sure the knot count is correct
		_ASSERTE(knotCount == pointCount + curveOrder);

		// bail if the path is too short
		if (pointCount < curveOrder)
			return;

		// distance buffer
		float dist[curveOrder-1];

		// total distance
		float d = 0.0f;

		// generate the first knots
		for (i = 0; i < curveOrder - 1; i++)
		{
			dist[i] = Distance(pointList[i], pointList[i+1]);
			knotList[i] = 0.0f;
		}

		// generate the interior knots
		for (; i < knotCount - curveOrder; i++)
		{
			for (j = 0; j < curveOrder - 2; j++)
			{
				d += dist[j];
				dist[j] = dist[j+1];
			}
			dist[curveOrder-2] = Distance(pointList[i-curveOrder], pointList[i-curveOrder+1]);
			d += dist[curveOrder-2];
			knotList[i] = d;
		}

		// generate the last knots
		for (j = 0; j < curveOrder - 1; j++)
		{
			d += dist[j];
		}
		for (; i < knotCount; i++)
		{
			knotList[i] = d;
		}

		// normalize the knots
		for (i = 0; i < knotCount; i++)
		{
			knotList[i] /= d;
		}
	}
}
