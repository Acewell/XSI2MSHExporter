//********************************************************************
// Direct geometry Hardware independent library
//	By Julio Jerez
// VC: 6.0
//********************************************************************
#include "ZeroTypes.h"


#include "ZeroDebug.h"
#include "ZeroVector3.h"
#include "ZeroMatrix.h"
#include "ZeroIntersections.h"




struct RayStruct
{
	ZeroVector3 l0;
	ZeroVector3 l1;
	ZeroVector3 l1l0;


	RayStruct (const ZeroVector3& p0, const ZeroVector3& p1)
	{
		l0 = p0;
		l1 = p1;
		l1l0 = p1 - p0;
	}

	bool PlaneClip (
		const ZeroVector3& normal, 
		const ZeroVector3& origin, 
		float& t0,
		float& t1)
	{
		float test0;
		float test1;

		ZeroAssert ((t0 < t1, "t0 must be less than t1"));
		ZeroVector3 p0 (l0 + l1l0.Scale (t0));
		ZeroVector3 p1 (l0 + l1l0.Scale (t1));

		test0 = DotProduct (p0 - origin, normal);
		test1 = DotProduct (p1 - origin, normal);

		if ((test0 < 0.0f) && (test1 < 0.0f)) {
			return false;
		}

		if ((test0 < 0.0f) && (test1 >= 0.0f)) {
			t0 = DotProduct (origin - l0, normal) / DotProduct (normal, l1l0);
		} else if ((test0 >= 0.0f) && (test1 < 0.0f)) {
			t1 = DotProduct (origin - l0, normal) / DotProduct (normal, l1l0);
		}

		return t0 < t1 - EPSILON;
	}


	void PlaneClipPerimeter (
		const ZeroVector3& normal, 
		const ZeroVector3& origin, 
		float& t0,
		float& t1)
	{
		float test0;
		float test1;

		ZeroAssert ((t0 < t1, "t0 must be less than t1"));
		ZeroVector3 p0 (l0 + l1l0.Scale (t0));
		ZeroVector3 p1 (l0 + l1l0.Scale (t1));

		test0 = DotProduct (p0 - origin, normal);
		test1 = DotProduct (p1 - origin, normal);

		if ((test0 > 0.0f) && (test1 > 0.0f)) {
			t0 = 1.0f;
			t1 = 0.0f;
			return;
		}

		if ((test0 >= 0.0f) && (test1 < 0.0f)) {
			t0 = DotProduct (origin - l0, normal) / DotProduct (normal, l1l0);
		} else if ((test0 < 0.0f) && (test1 >= 0.0f)) {
			t1 = DotProduct (origin - l0, normal) / DotProduct (normal, l1l0);
		}
	}

};


bool __cdecl ZeroLinesIntersect (
	const ZeroVector3& p0, 
	const ZeroVector3& p1, 
	float& t,
	const ZeroVector3& q0, 
	const ZeroVector3& q1, 
	float& s,
	float distToEdgeTolerance)
{
	float nMag;
	float tmax;
	ZeroVector3 p01 (p1 - p0); 
	ZeroVector3 q01 (q1 - q0); 

	ZeroVector3 n (CrossProduct (q01, p01));

	nMag = DotProduct (n, n);
	if (nMag < 1.0e-4f) {
		return false;
	}

	ZeroVector3 p0q0 (p0 - q0);
	ZeroVector3 p0q0p01 (CrossProduct (p0q0, p01));

	s = DotProduct (p0q0p01, n) / nMag;
	if (s < distToEdgeTolerance) {
		return false;
	}

	tmax = 1.0f - distToEdgeTolerance;

	if (s > tmax) {
		return false;
	}

	ZeroVector3 p0q0q01 (CrossProduct (p0q0, q01));
	t = DotProduct (p0q0q01, n) / nMag;

	if (t > tmax) {
		return false;
	}

	if (t < distToEdgeTolerance) {
		return false;
	}

	return true;

}




bool __cdecl ZeroPointToPolygonDistance (
	const ZeroVector3& p, 
	const int index[],
	int indexCount,
	const float vertex[], 
	int strideInBytes,
	float bailDistance,
	ZeroVector3& out)
{
	int i;
	int i0;
	int i1;
	int i2;

	int stride;
	float side;
	float dist;
	float mag2;
	float bail2;
	float side01;
	float side10;
	float side21;
	float side12;

	stride = strideInBytes / sizeof (float);

	i0 = index[0] * stride;
	i1 = index[1] * stride;
	i2 = index[2] * stride;

	ZeroVector4 v0 (&vertex[i0]);
	ZeroVector4 v1 (&vertex[i1]);
	ZeroVector4 v2 (&vertex[i2]);

	ZeroVector4 e10 (v1 - v0);
	ZeroVector4 e21 (v2 - v1);
	ZeroVector4 normal (CrossProduct(e10, e21));

	ZeroVector4 point (p); 
	ZeroVector4 p10 (point.x - v0.x, point.y - v0.y, point.z - v0.z);

	dist = DotProduct (normal, p10);
	if (dist < 0.0f) {
		return false;
	}
	
	mag2 = DotProduct (normal, normal);
	bail2 = bailDistance * bailDistance;
	if ((dist * dist) > (mag2 * bail2)) {
		return false;
	}

	ZeroVector4 pointOut (point - normal.Scale (dist / mag2));

	ZeroVector4 Out0 (pointOut - v0);
	ZeroVector4 Out1 (pointOut - v1);

	side01 = DotProduct (e10, Out0);
	side10 = -DotProduct (e10, Out1);
	if ((side01 >= 0.0f) && (side10 >= 0.0f)) {
		ZeroVector4 perpend10 (CrossProduct(normal, e10));
		side = DotProduct (perpend10, Out0);
		if (side < 0.0f) {
			ZeroVector4 pointOut (v0 + e10.Scale (side01 / DotProduct (e10, e10)));
			ZeroVector4 vDist (point - pointOut);
			if (DotProduct (vDist, vDist) > bail2) {
				return false;
			}
			out.x	= pointOut.x;
			out.y	= pointOut.y;
			out.z	= pointOut.z;
			return true;
		}
	}

	for (i = 1; i < indexCount; i ++) {
		i2 = i + 1;
		if (i2 == indexCount) {
			i2 = 0;
		}
		i2 = index[i2] * stride;
		ZeroVector4 v2 (&vertex[i2]);

		ZeroVector4 e21 (v2 - v1);
		ZeroVector4 Out2 (pointOut - v2);
		side12 = DotProduct (e21, Out1);
		side21 = -DotProduct (e21, Out2);
		if ((side12 >= 0.0f) && (side21 >= 0.0f)) {
			ZeroVector4 perpend21 (CrossProduct(normal, e21));
			side = DotProduct (perpend21, Out1);
			if (side < 0.0f) {
				ZeroVector4 pointOut (v1 + e21.Scale (side12 / DotProduct (e21, e21)));
				ZeroVector4 vDist (point - pointOut);
				if (DotProduct (vDist, vDist) > bail2) {
					return false;
				}

				out.x	= pointOut.x;
				out.y	= pointOut.y;
				out.z	= pointOut.z;
				return true;
			}
		}

		if ((side10 <= 0.0f)	&& (side12 <= 0.0f)) {
			ZeroVector4 vDist (point - v1);
			if (DotProduct (vDist, vDist) > bail2) {
				return false;
			}
			out.x	= v1.x;
			out.y	= v1.y;
			out.z	= v1.z;
			return true;
		}
		v1 = v2;
		Out1 = Out2;
		side10 = side21;
	}

	if ((side10 <= 0.0f)	&& (side01 <= 0.0f)) {
		ZeroVector4 vDist (point - v0);
		if (DotProduct (vDist, vDist) > bail2) {
			return false;
		}
		out.x	= v0.x;
		out.y	= v0.y;
		out.z	= v0.z;
		return true;
	}

	out.x	= pointOut.x;
	out.y	= pointOut.y;
	out.z	= pointOut.z;
	return true;
}




void __cdecl ZeroPointRayDistance (
	const ZeroVector3& p,
	const ZeroVector3& l0, 
	const ZeroVector3& l1,
	ZeroVector3& pointp0p1)
{
	float t;
	float d;

	ZeroVector3 pl0 (p - l0);
	ZeroVector3 l10 (l1 - l0);

	d = DotProduct (l10, l10);
	t = DotProduct (l10, pl0);

	if (t < 0.0f) {
		pointp0p1 = l0;
	} else if (t > d) {
		pointp0p1 = l1;
	} else {
		pointp0p1 = l0 + l10.Scale (t / d);
	}
}



void __cdecl ZeroRayRayDistance (
	const ZeroVector3& p0, 
	const ZeroVector3& p1,
	const ZeroVector3& q0, 
	const ZeroVector3& q1,
	ZeroVector3& pointp0p1,
 	ZeroVector3& pointq0q1)
{
	float t;
  	float s;
  	float b0;
  	float b1;
  	float a00;
  	float a11;
  	float a10;
  	float det; 
  	
  	ZeroVector3 p10 (p1 - p0);
  	ZeroVector3 q10 (q1 - q0);
  	ZeroVector3 p0q0 (p0 - q0);
  	
  	
  	a00 = DotProduct (p10, p10);
  	a11 = DotProduct (q10, q10);
  	a10 = -DotProduct (q10, p10);
  	
  	b0 = -DotProduct (p10, p0q0);
  	b1 = DotProduct (q10, p0q0);
  	
  	det	= a00 * a11 - a10 * a10;
  	
	if (fabsf (det) < 1.0e-4f) {
		pointp0p1 = p0;
		ZeroPointRayDistance (p0, q0, q1, pointq0q1);
		return;
	}

  	det = 1.0f / det;

  	t = (b0 * a11 - a10 * b1) * det;
  	s = (a00 * b1 - b0 * a10) * det;
  	
  	
  	ZeroVector3 tmp_p;
  	ZeroVector3 tmp_q;

	if (t > 1.0f) {
		if (s > 1.0f) {
  			ZeroPointRayDistance (p1, q0, q1, tmp_q);
			ZeroPointRayDistance (q1, p0, p1, tmp_p);
			
			ZeroVector3 p1q (p1 - tmp_q);
			ZeroVector3 q1p (q1 - tmp_p);
			if (DotProduct (p1q, p1q) < DotProduct (q1p, q1p)) {
			  pointp0p1	= p1;
			  pointq0q1	= tmp_q;
			} else {
			  pointp0p1	= tmp_p;
			  pointq0q1	= q1;
			}

		} else if (s < 0.0f) {
			ZeroPointRayDistance (p1, q0, q1, tmp_q);
			ZeroPointRayDistance (q0, p0, p1, tmp_p);
	
			ZeroVector3 p1q (p1 - tmp_q);
			ZeroVector3 q0p (q0 - tmp_p);
			if (DotProduct (p1q, p1q) < DotProduct (q0p, q0p)) {
			  pointp0p1	= p1;
			  pointq0q1	= tmp_q;
			} else {
			  pointp0p1	= tmp_p;
			  pointq0q1	= q0;
			}

		} else {
			pointp0p1	= p1;
			ZeroPointRayDistance (p1, q0, q1, pointq0q1);
		}

  } else if (t < 0.0f){
	  if (s < 0.0f) {
  			ZeroPointRayDistance (p0, q0, q1, tmp_q);
			ZeroPointRayDistance (q0, p0, p1, tmp_p);
			
			ZeroVector3 p0q (p0 - tmp_q);
			ZeroVector3 q0p (q0 - tmp_p);
			if (DotProduct (p0q, p0q) < DotProduct (q0p, q0p)) {
			  pointp0p1	= p0;
			  pointq0q1	= tmp_q;
			} else {
			  pointp0p1	= tmp_p;
			  pointq0q1	= q0;
			}


	  } else if (s > 1.0f) {

			ZeroPointRayDistance (p0, q0, q1, tmp_q);
			ZeroPointRayDistance (q1, p0, p1, tmp_p);

			ZeroVector3 p0q (p0 - tmp_q);
			ZeroVector3 q1p (q1 - tmp_p);
			if (DotProduct (p0q, p0q) < DotProduct (q1p, q1p)) {
			  pointp0p1	= p0;
			  pointq0q1	= tmp_q;
			} else {
			  pointp0p1	= tmp_p;
			  pointq0q1	= q1;
			}
	  } else {
		  pointp0p1	= p0;
		  ZeroPointRayDistance (p0, q0, q1, pointq0q1);
	  }

  } else {

	  if (s < 0.0f) {
		  ZeroPointRayDistance (q0, p0, p1, pointp0p1);
		  pointq0q1	= q0;
	  } else if (s > 1.0f) {
		  ZeroPointRayDistance (q1, p0, p1, pointp0p1);
		  pointq0q1	= q1;
	  } else {
		  pointp0p1	= p0 + p10.Scale (t);
		  pointq0q1 = q0 + q10.Scale (s);
	  }
  }
}


bool __cdecl ZeroRayPolygonDistance (
	const ZeroVector3& r0, 
	const ZeroVector3& r1, 
	const int index[],
	int indexCount,
	const float vertex[], 
	int strideInBytes,
	ZeroVector3& pointInLine,
	ZeroVector3& pointInPolygon)
{
	int i;
	int i0;
	int i1;
	int i2;
	int stride;
	float t0;
	float t1;

	stride = strideInBytes / sizeof (float);

	i0 = index[0] * stride;
	i1 = index[1] * stride;
	i2 = index[2] * stride;

	ZeroVector3 v0 (&vertex[i0]);
	ZeroVector3 v1 (&vertex[i1]);
	ZeroVector3 v2 (&vertex[i2]);

	ZeroVector3 e10 (v1 - v0);
	ZeroVector3 e21 (v2 - v1);
	ZeroVector3 normal (CrossProduct (e10, e21));


   ZeroVector3 ray_p0 (r0);
	ZeroVector3 dist0 (ray_p0 - v0); 
	t0 = DotProduct (dist0, normal);  
	if (t0 < 0.0f) {
		return false;
	}

  	ZeroVector3 ray_p1 (r1);
	ZeroVector3 dist1 (ray_p1 - v0); 
	t1 = DotProduct (dist1, normal);  
	if (t1 < 0.0f) {
	  	ZeroVector3 p10 (ray_p1 - ray_p0);
		ray_p1 = ray_p0 - p10.Scale ( t0 * 0.999f/ DotProduct (p10, normal));
	}

	RayStruct ray(ray_p0, ray_p1);

	i0 = index[indexCount - 1] * stride;
	v0 = &vertex[i0];

	float perimeter_t0;
	float perimeter_t1;



	float dist2;
	float minDist2;
	minDist2 = 1e10;


	perimeter_t0 = 0.0f;
	perimeter_t1 = 1.0f;
	for (i = 0; i < indexCount; i ++) {
		i1 = index[i] * stride;
		v1 = &vertex[i1];

		ZeroVector3 e0 (v1 - v0);
		ZeroVector3 w (CrossProduct (e0, normal));
		if (perimeter_t0 < perimeter_t1) {
	 		ray.PlaneClipPerimeter (w, v0, perimeter_t0, perimeter_t1);
		}

		// check against polygon edge
		t0 = 0.0f;
		t1 = 1.0f;
 		if (ray.PlaneClip (w, v0, t0, t1)) {
			if (ray.PlaneClip (e0, v0, t0, t1)) {
				ZeroVector3 e1 (v0 - v1);
				if (ray.PlaneClip (e1, v1, t0, t1)) {
					if ((t0 == 0.0f) && (t1 == 1.0f)) {
						ZeroRayRayDistance (ray_p0, ray_p1, v0, v1, pointInLine, pointInPolygon);
						return true;
					} else {
					  	ZeroVector3 p0;
					  	ZeroVector3 p1;
					   ZeroVector3 l0 (ray.l0 + ray.l1l0.Scale (t0));
					  	ZeroVector3 l1 (ray.l0 + ray.l1l0.Scale (t1));
					  
						ZeroRayRayDistance (l0, l1, v0, v1, p0, p1);

						ZeroVector3 del (p1- p0);
					  	dist2 = DotProduct (del, del);
					   if (dist2 < minDist2) {
							pointInLine = p0;
							pointInPolygon = p1;
							minDist2	= dist2;
						}
					}
				}
			}
		}

		// check against polygon edge	vertex
		t0 = 0.0f;
		t1 = 1.0f;
 		if (ray.PlaneClip (e0, v1, t0, t1)) {
			i1 = i + 1;
			if (i1 >= indexCount) {
				i1 = 0;
			}
			i1 = index[i1] * stride;
			v2 = &vertex[i1];
			ZeroVector3 e1 (v1 - v2);
	 		if (ray.PlaneClip (e1, v1, t0, t1)) {
				if ((t0 == 0.0f) && (t1 == 1.0f)) {
					pointInPolygon	= v1;
					ZeroPointRayDistance (v1, ray_p0, ray_p1, pointInLine);
					return true;
				} else {
				  	ZeroVector3 p0;
				   ZeroVector3 l0 (ray.l0 + ray.l1l0.Scale (t0));
				  	ZeroVector3 l1 (ray.l0 + ray.l1l0.Scale (t1));
					  
					ZeroPointRayDistance (v1, l0, l1, p0);
					ZeroVector3 del (p0- v1);

				  	dist2 = DotProduct (del, del);
				   if (dist2 < minDist2) {
						pointInLine = p0;
						pointInPolygon = v1;
						minDist2	= dist2;
					}
				}
			}
		}


		v0 = v1;
	}

	if (perimeter_t0 < perimeter_t1) {
	   ZeroVector3 p0 (ray.l0 + ray.l1l0.Scale (perimeter_t0));
	  	ZeroVector3 p1 (ray.l0 + ray.l1l0.Scale (perimeter_t1));

		ZeroVector3 dist0 (p0 - v0); 
		ZeroVector3 dist1 (p1 - v0); 

		float d0;
		float d1;

		d0 = DotProduct (dist0, normal);  
		d1 = DotProduct (dist1, normal);

		if (d1 < d0) {
			d0 = d1;
			p0	= p1;
		}


		ZeroVector3 del (normal.Scale (d0 / DotProduct (normal, normal)));
	  	dist2 = DotProduct (del, del);

		if (dist2 < minDist2) {
			minDist2 = dist2;
			pointInLine = p0;
			pointInPolygon = p0 - del;
		}
	}

	return minDist2 < 1e10f;
}



bool __cdecl ZeroRayPolygonIntersect (
	const ZeroVector3& ray_p0, 
	const ZeroVector3& ray_p1, 
	float distToEdgeTolerance,
	const int index[],
	int indexCount,
	const float vertex[], 
	int strideInBytes,
	float& tout, 
	ZeroVector3& unNormalizedNormal)
{
	int i;
	int i0;
	int i1;
	int i2;
	int stride;
	union	FloatInt
	{
		int i;
		float f;
	};

	float t;
	float b1;
	float b2;
	float a11;
	float a21;
	float a22;
	FloatInt det;
	FloatInt num;
	FloatInt den;
	FloatInt alpha1;
	FloatInt alpha2;

	stride = strideInBytes / sizeof (float);

	i0 = index[0] * stride;
	i1 = index[1] * stride;
	i2 = index[2] * stride;

	ZeroVector4 v0 (&vertex[i0]);
	ZeroVector4 v1 (&vertex[i1]);
	ZeroVector4 v2 (&vertex[i2]);

	ZeroVector4 e10 (v1 - v0);
	ZeroVector4 e20 (v2 - v0);
	ZeroVector4 normal (CrossProduct (e10 , e20));

#ifdef _DEBUG
	float xx;
	xx = rsqrt (DotProduct (e10, e10));
	ZeroVector4 _e10 (e10.Scale (xx));

	xx = rsqrt (DotProduct (e20, e20));
	ZeroVector4 _e20 (e20.Scale (xx));

	ZeroVector4 _normal (CrossProduct (_e10, _e20));
//	_ASSERTE (DotProduct (_normal, _normal) > (1.0e-1f * 1.0e-1f));
#endif

	ZeroVector4 ray (ray_p0 - ray_p1);

	den.f = DotProduct (normal, ray);
	if (den.i <= 0) {
		return false;
	}

	ZeroVector4	del (v0 - ray_p1);
	num.f = DotProduct (normal, del);
	if (num.i < 0) {
		return false;
	}
	if (num.i > den.i) {
		return false;
	}

	t = num.f / den.f;
	ZeroVector4 p (ray_p1 + ray.Scale (t));

	a11 = DotProduct (e10, e10);
	a21 = DotProduct (e20, e10);
	a22 = DotProduct (e20, e20);
	det.f = a11 * a22 - a21 * a21;

	_ASSERTE (det.f > 0.0f);
	
	ZeroVector4 dp0 (p - v0);
	b1 = DotProduct (e10, dp0);
	b2 = DotProduct (e20, dp0);

	alpha1.f = b1 * a22 - b2 * a21;
	alpha2.f = b2 * a11 - b1 * a21;
	if (alpha1.i >= 0) {
		if (alpha2.i >= 0) {
			alpha1.f += alpha2.f;
			if (alpha1.i <= det.i) {
				tout = 1.0f - t;
				unNormalizedNormal = normal;
				return true;
			}
		}
	}

	for (i = 3; i < indexCount; i ++) {
		b1 = b2;
		a11 = a22;
		e10 = e20;
					 
		i2 = index[i] * stride;
		ZeroVector4 v2 (&vertex[i2]);
		e20 = v2 - v0;

		a21 = DotProduct (e20, e10);
		a22 = DotProduct (e20, e20);
		det.f = a11 * a22 - a21 * a21;
//#ifdef _DEBUG
//		if (det.f < -EPSILON)
//		{
//			ZeroTrace(("ZeroRayPolygonIntersect() det.f < -1.0e-4f\n"));
//		} 
//#endif

		if (det.i > 0) {
			ZeroVector4 dp0 (p - v0);
			b2 = DotProduct (e20, dp0);
		
			alpha1.f = b1 * a22 - b2 * a21;
			alpha2.f = b2 * a11 - b1 * a21;
			if (alpha1.i >= 0) {
				if (alpha2.i >= 0) {
					alpha1.f += alpha2.f;
					if (alpha1.i <= det.i) {
						tout = 1.0f - t;t;
						unNormalizedNormal = normal;
						return true;
					}
				}
			}
		}
	}
	return false;
}


