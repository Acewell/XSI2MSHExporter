/****************************************************************************
*
*  Visual C++ 6.0 created by: Julio Jerez
*
****************************************************************************/
#include "ZeroTypes.h"
#include "ZeroFile.h"
#include "ZeroStack.h"
#include "ZeroVector3.h"
#include "ZeroSphere.h"
#include "ZeroGeoUtil.h"
#include "ZeroPolyhedra.h"
#include "ZeroMathTypes.h"


namespace GeoUtil
{
	static int vertexStride;
	static float *vertexArray;

	typedef int (__cdecl * cmp_vertex_func) (const void *elem1, const void *elem2);

	static int __cdecl cmp_vertex_x (const void *elem1, const void *elem2)
	{
		unsigned i1 = *(unsigned *) elem1;
		unsigned i2 = *(unsigned *) elem2;
		
		float v1 = vertexArray[i1 * vertexStride];
		float v2 = vertexArray[i2 * vertexStride];

		if (v1 < v2) {
			return -1;
		}
		
		if (v1 > v2){
			return 1;
		}
		
		return 0;
	}
	
	static int __cdecl cmp_vertex_y (const void *elem1, const void *elem2)
	{
		unsigned i1 = *(unsigned *) elem1;
		unsigned i2 = *(unsigned *) elem2;
		
		float v1 = vertexArray[i1 * vertexStride + 1];
		float v2 = vertexArray[i2 * vertexStride + 1];

		if (v1 < v2) {
			return -1;
		}
		
		if (v1 > v2){
			return 1;
		}
		
		return 0;
	}
	
	static int __cdecl cmp_vertex_z (const void *elem1, const void *elem2)
	{
		unsigned i1 = *(unsigned *) elem1;
		unsigned i2 = *(unsigned *) elem2;
		
		float v1 = vertexArray[i1 * vertexStride + 2];
		float v2 = vertexArray[i2 * vertexStride + 2];

		if (v1 < v2) {
			return -1;
		}
		
		if (v1 > v2){
			return 1;
		}
		
		return 0;
	}
	
	
	static int SortVertices (
		float * const vertexList, 
		const int vertexStride,
		const int vertexCount, 
		unsigned * const indexList,
		const int looseCount,
		const float tolerance)
	{
#if (PLATFORM == PLATFORM_PS2)
		_ASSERTE(0); // We shouldn't be in here!
		return 0;
#else
		int i;
		int i1;
		int j;
		int count;
		int offset;
		cmp_vertex_func cmp_vertex;
		double val;
		double swept;
		float x;
		float y;
		float z;
		double xc;
		double yc;
		double zc;
		double x2c;
		double y2c;
		double z2c;
		double Tol;
		double minDist;
		double sweptWindow;
		float *vertPtr;

		GeoUtil::vertexStride = vertexStride;
		GeoUtil::vertexArray = vertexList;

		ZeroStack<unsigned> indexSort(vertexCount);
		ZeroStack<float> storageList(vertexCount * vertexStride);

		xc = 0;
		yc = 0;
		zc = 0;

		x2c = 0;
		y2c = 0;
		z2c = 0;
		
		ZeroVector3 Min (1e10, 1e10, 1e10);
		ZeroVector3 Max (-1e10, -1e10, -1e10);

		vertPtr = vertexList;
		for (i = 0; i < vertexCount; i ++) {
			x = vertPtr[0];
			y = vertPtr[1];
			z = vertPtr[2];

			xc += x;
			yc += y;
			zc += z;

			x2c += x * x;
			y2c += y * y; 
			z2c += z * z;
			
			if (x < Min.x)
				Min.x = x;
			if (y < Min.y)
				Min.y = y;
			if (z < Min.z)
				Min.z = z;

			if (x > Max.x)
				Max.x = x;
			if (y > Max.y)
				Max.y = y;
			if (z > Max.z)
				Max.z = z;

			indexList[i] = 0xFFFFFFFF;
			indexSort[i] = i;

			vertPtr += vertexStride;
		}
		
		ZeroVector3 Del (Max - Min);
		minDist = GetMin (Del.x, Del.y, Del.z);
		if (minDist < 1.0e-3) {
			minDist = 1.0e-3;
		}
		
		Tol = tolerance * minDist + 1.0e-4;
		sweptWindow = 2.0 * Tol;
		sweptWindow += 1.0e-4;
		
		x2c = vertexCount * x2c - xc * xc;
		y2c = vertexCount * y2c - yc * yc;
		z2c = vertexCount * z2c - zc * zc;
		
		if ((x2c >= y2c) && (x2c >= z2c)) {
			offset = 0;
			cmp_vertex = cmp_vertex_x;
		} else if ((y2c >= x2c) && (y2c >= z2c)) {
			offset = 1;
			cmp_vertex = cmp_vertex_y;
		} else {
			offset = 2;
			cmp_vertex = cmp_vertex_z;
		}

		qsort (&indexSort[0], vertexCount, sizeof (unsigned), cmp_vertex);
		count = 0;
		for (i = 0; i < vertexCount; i ++) {
			if (indexList[indexSort[i]] == 0xFFFFFFFF) {
				swept = vertexList[indexSort[i] * vertexStride + offset] + sweptWindow;
				for (i1 = i + 1; i1 < vertexCount; i1 ++) {
					if (indexList[indexSort[i1]] == 0xFFFFFFFF) {
						val = vertexList[indexSort[i1] * vertexStride + offset];
						if (val >= swept) {
							break;
						}
						for (j = 0; j < looseCount; j++) {
							if (tolerance < fabsf(
								vertexList[indexSort[i] * vertexStride + j] - 
								vertexList[indexSort[i1] * vertexStride + j]))
								break;
						}
						for (; j < vertexStride; j++) {
							if (*(unsigned *)&vertexList[indexSort[i] * vertexStride + j] !=
								*(unsigned *)&vertexList[indexSort[i1] * vertexStride + j])
								break;
						}
						if (j == vertexStride) {
							indexList[indexSort[i1]] = count;
						}
					}
				}
				indexList[indexSort[i]] = count;
				memcpy(
					&storageList[count * vertexStride],
					&vertexList[indexSort[i] * vertexStride],
					sizeof(float) * vertexStride);
				count ++;
			}
		}

		memcpy(vertexList, &storageList[0], sizeof(float) * vertexStride * count);

		return count;
#endif
	}
}

#ifdef TOOL_DEVELOPMENT 

namespace ConvexHull
{
	const int HULL_RESOLUTION = (1<<15);
	
	struct HULL_VERTEX 
	{
		long double x;
		long double y;
		long double z;
		HULL_VERTEX () {}
		HULL_VERTEX (const float *p)
		{
			x = p[0];
			y = p[1];
			z = p[2];
		};
	};
	
	
	static int vertexStride;
	static float *vertexArray;
	static ZeroVector3 centre;
	static ZeroMatrix planeMatrix;
	
	static void NormalizeVertex (
		float *tArray,
		int tStride,
		float *sArray,
		int sStride,
		int vCount)
	{
		int i;
		int x;
		int y;
		int z;

		double x0;
		double dx;
		double y0;
		double dy;
		double z0;
		double dz;
		
		ZeroVector3 Min;
		ZeroVector3 Max;
		GetMinMax (Min, Max, sArray, sStride * sizeof (float), vCount);
		
		x0 = (Min.x + Max.x) * 0.5f;
		y0 = (Min.y + Max.y) * 0.5f;
		z0 = (Min.z + Max.z) * 0.5f;

		dx	= Max.x - Min.x;
		if (dx < 0.001f) {
			dx	= 0.001f;
		}
	
		dy	= Max.y - Min.y;
		if (dy < 0.001f) {
			dy	= 0.001f;
		}
	
		dz	= Max.z - Min.z;
		if (dz < 0.001f) {
			dz	= 0.001f;
		}
		
		dx	= HULL_RESOLUTION * 2.0f / dx;
		dy	= HULL_RESOLUTION * 2.0f / dy;
		dz	= HULL_RESOLUTION * 2.0f / dz;
		
		for (i = 0; i < vCount; i ++) {
			x = (int)((sArray[0] - x0) * dx);
			y = (int)((sArray[1] - y0) * dy);
			z = (int)((sArray[2] - z0) * dz);               
	
			tArray[0] = (float)x; 
			tArray[1] = (float)y; 
			tArray[2] = (float)z; 
			
			sArray += sStride;
			tArray += tStride;
		}
	}
	
	
	static int __cdecl cmp_angles (const void *elem1, const void *elem2)
	{
		int i1;
		int i2;
		int *Index1;
		int *Index2;
		
		Index1 = (int *) elem1;
		Index2 = (int *) elem2;
		
		i1 = *Index1;
		i2 = *Index2;
		
		ZeroVector3 p1 (&vertexArray[i1 * vertexStride]);
		ZeroVector3 p2 (&vertexArray[i2 * vertexStride]);
		
		p1 = planeMatrix.UnrotateVector(p1);
		p2 = planeMatrix.UnrotateVector(p2);
		
		ZeroVector3 e0 (p1 - centre);
		ZeroVector3 e1 (p2 - centre);
		ZeroVector3 Area (CrossProduct(e1, e0));
		if (Area.y > 0.0) {
			return -1;
		}
		if (Area.y < 0.0) {
			return 1;
		}
		return 0;
	}
	
	static int SpliteList (
		int count, 
		unsigned* const index, 
		float *pool,
		int stride)
	{
		int i0;
		int i1;
		int i2;
		float test;
		ZeroSphere sphere;

		sphere.SetDimensions (pool, stride * sizeof (float), (int*)index, count);
		
		ZeroVector3 n (sphere.right);
		if ((sphere.size.y > sphere.size.x) && (sphere.size.y > sphere.size.z)) {
			n  = sphere.up;
		}
		if ((sphere.size.z > sphere.size.x) && (sphere.size.z > sphere.size.y)) {
			n  = sphere.front;
		}
	 	ZeroPlane plane (n, - DotProduct(n, sphere.posit));
		
		
		i0 = 0;
		i1 = count - 1;
		while (i0 < i1) {
			for (test = PlaneDist(plane, &pool[index[i0] * stride]); test < 0.0;) {
				i0 ++;
				test = PlaneDist(plane, &pool[index[i0] * stride]);
			}
			for (test = PlaneDist(plane, &pool[index[i1] * stride]); test >= 0.0; ) {
				i1 --;
				test = PlaneDist(plane, &pool[index[i1] * stride]);
			}
			
			if (i0 < i1) {
				i2 = index[i0];
				index[i0] = index[i1];
				index[i1] = i2;
			}
		}
		
#ifdef __ENABLE_SANITY_CHECK 
		int i;
		for (i = 0; i < i0; i ++) {
			_ASSERTE (PlaneDist(plane, &pool[index[i] * stride]) < 0.0);
		}
		for (;i < count; i ++) {
			_ASSERTE (PlaneDist(plane, &pool[index[i] * stride]) >= 0.0);
		}
#endif
		
		return i0;
	}
	
	
	static bool FaceVisible (
		ZeroVector3 &point,
		ZeroPolyhedra* polyhedra, 
		ZeroEdge *face, 
		float *pool,
		int stride)
	{
		int i0;
		int i1;
		int i2;
		long double vol;
		HULL_VERTEX	e0;
		HULL_VERTEX	e1;
		HULL_VERTEX	e2;
		const long double tol = -0.0;
		
		i0 = face->incidentVertex;
		i1 = face->next->incidentVertex;
		i2 = face->prev->incidentVertex;
		
		HULL_VERTEX P (&point.x);
		HULL_VERTEX P0 (&pool [i0 * stride]);
		HULL_VERTEX P1 (&pool [i1 * stride]);
		HULL_VERTEX P2 (&pool [i2 * stride]);
		
		e0.x = P0.x - P.x;
		e0.y = P0.y - P.y;
		e0.z = P0.z - P.z;
		
		e1.x = P1.x - P.x;
		e1.y = P1.y - P.y;
		e1.z = P1.z - P.z;
		
		e2.x = P2.x - P.x;
		e2.y = P2.y - P.y;
		e2.z = P2.z - P.z;
		
		vol = e0.x * (e1.y * e2.z - e1.z * e2.y) + 
			e0.y * (e1.z * e2.x - e1.x * e2.z) +
			e0.z * (e1.x * e2.y - e1.y * e2.x);
		
		return (vol < tol);
	}
	
	
	
	static void SweepOutline (
		int apex, 
		ZeroPolyhedra* hull, 
		ZeroEdge *outline) 
	{
		int faceIndex;
		ZeroEdge *ptr;
		ZeroEdge *Edge;
		ZeroEdge *twin;
		ZeroEdge *Edge0;
		ZeroEdge *nextOutline;
		
		faceIndex = 4;
		
		Edge0 = NULL;
		ptr = outline;
		do {
			nextOutline = ptr->next;
			
			Edge = hull->AddEdge (apex, nextOutline->incidentVertex);
			twin = hull->AddEdge (nextOutline->incidentVertex, apex);
			
			Edge->twin = twin;
			twin->twin = Edge;
			
			ptr->next = twin;
			twin->prev = ptr;
			
			nextOutline->prev = Edge;
			Edge->next = nextOutline;
			
			if (Edge0) {
				twin->next = Edge0;
				Edge0->prev = twin;
			}
			
			Edge->incidentFace = faceIndex;
			twin->incidentFace = faceIndex - 1;
			nextOutline->incidentFace = faceIndex;
			
			faceIndex ++;
			Edge0 = Edge;
			
			ptr = nextOutline;
		} while (ptr != outline);
		
		Edge0->prev = outline->next;
		outline->next->next = Edge0;
		outline->next->incidentFace = Edge0->incidentFace;
		
#ifdef __ENABLE_SANITY_CHECK 
		ZeroPolyhedraDescriptor desc(hull);
		_ASSERTE (desc.unboundedLoops.GetCount() == 0);
		_ASSERTE (hull->SanityCheck());
#endif
	}
	
	
	static bool HullAddVertex (
		ZeroPolyhedra* polyhedra, 
		int index, 
		float *pool,
		int stride)
	{
		bool alive;
		int mark;
		bool exterior;
		ZeroEdge *face;
		ZeroEdge *outline;
		ZeroPolyhedra::ZeroTreeNode *node;
		
		exterior	= false;
		mark = polyhedra->IncLRU();
		
		ZeroVector3 P (&pool[index * stride]);
		ZeroPolyhedra::Iterator iter(*polyhedra);
		for (iter.Begin(); iter;) {
			node = *iter;
			node->AddRef();
			face = &node->info;
			
			if (face->mark != mark) {
				face->mark = mark;
				face->next->mark = mark;
				face->prev->mark = mark;
				
				if (FaceVisible (P, polyhedra, face, pool, stride)) {
					exterior	= true;
					
					face->incidentFace = -1;
					face->prev->incidentFace = -1;
					face->next->incidentFace = -1;
					
					if (face->prev->twin->incidentFace < 0) {
						polyhedra->DeleteEdge (face->prev);
					}
					
					if (face->next->twin->incidentFace < 0) {
						polyhedra->DeleteEdge (face->next);
					}
					
					if (face->twin->incidentFace < 0) {
						polyhedra->DeleteEdge (face);
					}
				}
			}
			
			alive	= node->IsAlive();
			node->Release();
			if (alive) {
				iter ++;
			} else {
				iter.Begin();
			}
		}
		
		if (exterior) {
			outline = NULL;
			for (iter.Begin(); iter; iter ++) {
				outline = &(*iter)->info;
				if (outline->incidentFace < 0) {
					break;
				}
			}
			
			_ASSERTE (outline);
			SweepOutline (index, polyhedra, outline);
		}
		return exterior;
	}
	
	
	static void Hull3D (
		ZeroPolyhedra* polyhedra,
		int count, 
		unsigned *index, 
		float *pool,
		int stride)
	{
		int i;
		
		ZeroPolyhedra::Iterator iter(*polyhedra);
		for (i = 3; i < count; i ++) {
			HullAddVertex (polyhedra, index[i], pool, stride);
		}
	}
	
	static void Hull2D (
		ZeroPolyhedra* polyhedra,
		int count, 
		unsigned *index, 
		float *pool,
		int stride)
	{
		int i;
		int j;
		int Min;
		double Min_x;
		double Min_z;
		int stackCount;
		
		ZeroVector3 p0(&pool[index[0] * stride]);
		ZeroVector3 p1(&pool[index[1] * stride]);
		ZeroVector3 p2(&pool[index[2] * stride]);
		
		ZeroVector3 e0 (p1 - p0);
		ZeroVector3 e1 (p2 - p0);
		
		ZeroVector3 n (CrossProduct(e0, e1));
		_ASSERTE (DotProduct(e0, e0) > 0);
		_ASSERTE (DotProduct(n, n) > 0);
		
		planeMatrix.up = Normalize(n);
		planeMatrix.front = Normalize(e0);
		planeMatrix.right = CrossProduct(planeMatrix.up, planeMatrix.front);
		
		Min = -1;
		Min_z = 1e10;
		Min_x = 1e10;
		for (i = 0; i < count; i ++) {
			j = index[i];
			ZeroVector3 P(&pool[j * stride]);
			P = planeMatrix.UnrotateVector(P);
			if ((P.z < Min_z) || ((P.z == Min_z) && (P.x < Min_x))) {
				Min = i;
				Min_z = P.z;
				Min_x = P.x;
			}
		}
		
		i = index[0];
		index[0]	= index[Min];
		index[Min] = i;
		
		vertexArray = pool;
		vertexStride = stride;
		centre = planeMatrix.UnrotateVector(ZeroVector3 (&pool[index[0] * stride]));
		
		qsort (&index[1], count - 1, sizeof (int), cmp_angles);
		
		p0 = ZeroVector3 (&pool[index[0] * stride]);
		j = 1;
		for (i = 2; i < count; i ++) {
			if (cmp_angles (&index[i], &index[j])) {
				j ++;
				index[j] = index[i];
			} else {
				float Dist1;
				float Dist2;
				p2 = ZeroVector3 (&pool[index[i] * stride]);
				p1 = ZeroVector3 (&pool[index[j] * stride]);
				Dist1 = DistanceSq(p0, p1);
				Dist2 = DistanceSq(p0, p2);
				if (Dist2 > Dist1) {
					index[j] = index[i];
				}
			}
		}
		count = j + 1;
		
		stackCount = 2;
		for (i = 2; i < count; i ++) {
			p0 = planeMatrix.UnrotateVector(ZeroVector3 (&pool[index[stackCount - 2] * stride]));
			p1 = planeMatrix.UnrotateVector(ZeroVector3 (&pool[index[stackCount - 1] * stride]));
			p2 = planeMatrix.UnrotateVector(ZeroVector3 (&pool[index[i] * stride]));
			ZeroVector3 e2 (p2 - p1); 
			ZeroVector3 e1 (p1 - p0); 
			ZeroVector3 N (CrossProduct(e2, e1)); 
			if (N.y > 0) {
				index[stackCount] = index[i];
				stackCount ++;
			} else {
				stackCount --;
			}
		}
		
		polyhedra->RemoveAll(); 
		polyhedra->BeginFace();
		for (i = 2; i < stackCount; i ++) {
			polyhedra->AddFace (index[0], index[i - 1], index[i]);
			polyhedra->AddFace (index[stackCount - 1], index[stackCount - i], index[stackCount - i - 1]);
		}
		polyhedra->EndFace();
	}
	
	static void MergeFlat (
		ZeroPolyhedra* polyhedra, 
		ZeroPolyhedra* mergePolyhedra, 
		float *pool,
		int stride)
	{
		int i;
		int j;
		int mark;
		int count;
		unsigned *indexPtr;
		ZeroEdge *ptr;
		ZeroEdge *Edge;
		
		ZeroPolyhedraDescriptor desc0 (polyhedra);
		ZeroPolyhedraDescriptor desc1 (mergePolyhedra);
		ZeroStack<unsigned> indexPool (desc0.vertexCount + desc1.vertexCount);
		
		count = 0;
		indexPtr = &indexPool[0];
		
		mark = polyhedra->IncLRU();
		ZeroPolyhedra::Iterator iter0(*polyhedra);
		for (iter0.Begin(); iter0; iter0	++) {
			Edge = &(*iter0)->info;
			if (Edge->mark != mark) {
				ptr = Edge;
				do {
					ptr->mark = mark;
					ptr = ptr->twin->next;
				} while (ptr != Edge);
				indexPtr[count] = Edge->incidentVertex;
				count ++;
			}
		}
		
		mark = mergePolyhedra->IncLRU();
		ZeroPolyhedra::Iterator iter1(*mergePolyhedra);
		for (iter1.Begin(); iter1; iter1	++) {
			Edge = &(*iter1)->info;
			if (Edge->mark != mark) {
				ptr = Edge;
				do {
					ptr->mark = mark;
					ptr = ptr->twin->next;
				} while (ptr != Edge);
				indexPtr[count] = Edge->incidentVertex;
				count ++;
			}
		}
		
		ZeroVector3 p0(&pool[indexPtr[0] * stride]);
		ZeroVector3 p1(&pool[indexPtr[1] * stride]);
		ZeroVector3 e0(p1 - p0);
		
		for (i = 2; i < count; i ++) {
			ZeroVector3 p2(&pool[indexPtr[i] * stride]);
			ZeroVector3 e1(p2 - p0);
			ZeroVector3 n (CrossProduct(e1, e0));
			
			if (MagnitudeSq(n) > 0.01f) {
				j = indexPtr[i];
				indexPtr[i] = indexPtr[2];
				indexPtr[2] = j;
				break;
			}
		}
		
		Hull2D (polyhedra, count, indexPtr, pool, stride);
	}
	
	static void Merge (
		ZeroPolyhedra* polyhedra, 
		ZeroPolyhedra* mergePolyhedra, 
		float *pool,
		int stride)
	{
		int mark;
		bool wasMerged;
		ZeroEdge *ptr;
		ZeroEdge *Edge;
		
		mark = mergePolyhedra->IncLRU();
		ZeroPolyhedra::Iterator iter(*mergePolyhedra);
		
		if (*polyhedra) {
			
			wasMerged = false;
			for (iter.Begin(); iter; iter	++) {
				Edge = &(*iter)->info;
				if (Edge->mark != mark) {
					ptr = Edge;
					do {
						ptr->mark = mark;
						ptr = ptr->twin->next;
					} while (ptr != Edge);
					wasMerged |= HullAddVertex (polyhedra, Edge->incidentVertex, pool, stride);
				}
			}
			if (!wasMerged) {
				MergeFlat (polyhedra, mergePolyhedra, pool, stride);
			}
		} else {
			polyhedra->BeginFace();
			for (iter.Begin(); iter; iter	++) {
				Edge = &(*iter)->info;
				if (Edge->mark != mark) {
					Edge->mark = mark;
					Edge->next->mark = mark;
					Edge->prev->mark = mark;
					polyhedra->AddFace (Edge->incidentVertex, 
						Edge->next->incidentVertex,
						Edge->prev->incidentVertex);
				}
			}
			polyhedra->EndFace();
		}
	}
	
	static ZeroPolyhedra* BruteForceHull (
		int count, 
		unsigned *index, 
		float *pool,
		int stride)
	{
		int i;
		int j;
		int mark;
		ZeroEdge *face;
		ZeroPolyhedra* polyhedra;
		
		ZeroVector3 p0(&pool[index[0] * stride]);
		ZeroVector3 p1(&pool[index[1] * stride]);
		
		ZeroVector3 e0(p1 - p0);
		for (i = 2; i < count; i ++) {
			ZeroVector3 p2(&pool[index[i] * stride]);
			ZeroVector3 e1(p2 - p0);
			ZeroVector3 n (CrossProduct(e1, e0));
			
			if (MagnitudeSq(n) > 0.01f) {
				j = index[i];
				index[i] = index[2];
				index[2] = j;
				break;
			}
		}
		
		polyhedra = new ZeroPolyhedra;
		polyhedra->BeginFace();
		polyhedra->AddFace (index[0], index[1], index[2]);
		polyhedra->AddFace (index[2], index[1], index[0]);
		polyhedra->EndFace();
		
		ZeroPolyhedra::Iterator iter(*polyhedra);
		for (i = 3; i < count; i ++) {
			ZeroVector3 P (&pool[index[i] * stride]);
			
			mark = polyhedra->IncLRU();
			for (iter.Begin(); iter; iter ++) {
				face = &(*iter)->info;
				
				if (face->mark != mark) {
					face->mark = mark;
					face->next->mark = mark;
					face->prev->mark = mark;
					
					if (FaceVisible (P, polyhedra, face, pool, stride)) {
						j = index[i];
						index[i] = index[3];
						index[3] = j;
						i = count + 100;
						break;
					}
				}
			}
		}
		
		if (i > count) {
			Hull3D (polyhedra, count, index, pool, stride);
		} else {
			Hull2D (polyhedra, count, index, pool, stride);
		}
		
#ifdef __ENABLE_SANITY_CHECK 
		ZeroPolyhedraDescriptor desc (polyhedra);
		_ASSERTE (desc.unboundedLoops.GetCount() == 0);
		_ASSERTE (polyhedra->SanityCheck());
#endif
		
		return polyhedra;
	}


	static bool FlatTest (
		int count, 
		unsigned* const index, 
		float* const pool,
		int stride)
	{
		int i;
		double mag2;

		if (count < 1024) {
			return true;
		}

		ZeroVector3 p0(&pool[index[0] * stride]);
		ZeroVector3 p1(&pool[index[1] * stride]);
		ZeroVector3 e0(p1 - p0);

		for (i = 2; i < count; i ++) {
			mag2 = DotProduct (e0, e0);
			if (mag2 > 0.001) {
				break;
			}
			ZeroVector3 p1(&pool[index[i] * stride]);
			e0 = p1 - p0;
		}

		ZeroVector3 p2(&pool[index[i] * stride]);
		ZeroVector3 e1(p2 - p0);
		for (; i < count; i ++) {
			ZeroVector3 n (CrossProduct (e1, e0));
			mag2 = DotProduct (n, n);
			if (mag2 > 0.01) {
				break;
			}
			ZeroVector3 p2(&pool[index[i] * stride]);
			e1 = p2 - p0;
		}

		ZeroVector3 n(CrossProduct (e1, e0));
		n = n.Scale (1.0f / sqrtf (DotProduct (n, n)));
		
		for (i = 1; i < count; i ++) {
			ZeroVector3 p(&pool[index[i] * stride]);

			mag2 = fabsf (DotProduct (n, (p - p0)));
			if (mag2 > 0.1) {
				return false;
			}
		}

		return true;
	}

	
	static ZeroPolyhedra* CreateConvexHull (
		int count, 
		unsigned* const index, 
		float *pool,
		int stride)
	{
		int middle;
		bool bruteForce;
		ZeroPolyhedra* polyhedra;
		ZeroPolyhedra* tmpPolyhedra;
		
		bruteForce = FlatTest (count, index, pool, stride);
		
		if (bruteForce) {
			polyhedra = BruteForceHull (count, index, pool, stride);
		} else {
			middle = SpliteList (count, index, pool, stride);
			if ((middle <= 64) || ((count - middle) <= 64)) {
				polyhedra = BruteForceHull (count, index, pool, stride);
			} else {
				polyhedra = CreateConvexHull (middle, index, pool, stride);
				tmpPolyhedra = CreateConvexHull (count - middle, &index[middle], pool, stride);
				Merge (polyhedra, tmpPolyhedra, pool, stride);
				delete tmpPolyhedra;
			}
		}
		
		
#ifdef __ENABLE_SANITY_CHECK 
		ZeroPolyhedraDescriptor desc0(polyhedra);
		_ASSERTE (desc0.unboundedLoops.GetCount() == 0);
		_ASSERTE (polyhedra->SanityCheck());
#endif
		
		return polyhedra;
	}
}


ZeroPolyhedra * __cdecl CreateConvexHull (
	float * const pool, 
	int strideInBytes, 
	int vertexCount) 
{
	int i;
	int stride;
	unsigned int *index;
	unsigned int Precision;
	float *tmpPtr;
	ZeroPolyhedra *hull;
	ZeroVector3 *sortVertex; 
	ZeroStack<unsigned int> indexPool (vertexCount + 100);
	ZeroStack<ZeroVector3> sortVertexPool (vertexCount + 100);

	Precision = ZeroSetPrecision (ZeroDoublePrecision);

	stride = strideInBytes / sizeof(float);
	tmpPtr = pool;
	index = &indexPool[0];
	sortVertex = &sortVertexPool[0]; 
	for (i = 0; i < vertexCount; i ++) {
		sortVertex[i].x = tmpPtr[0];
		sortVertex[i].y = tmpPtr[1];
		sortVertex[i].z = tmpPtr[2];
		tmpPtr += stride;
	}

	vertexCount = VertexListToIndexList (&sortVertex[0].x, sizeof(ZeroVector3), vertexCount, 
													 index, sizeof(ZeroVector3), 0.005f);

	tmpPtr = pool;
	for (i = 0;	i < vertexCount; i ++) {
		index[i] = i;
		tmpPtr[0] = sortVertex[i].x;
		tmpPtr[1] = sortVertex[i].y;
		tmpPtr[2] = sortVertex[i].z;
		tmpPtr += stride;
	}

	tmpPtr = &sortVertex[0].x; 
	stride = sizeof (ZeroVector3) / sizeof (float);
	ConvexHull::NormalizeVertex (tmpPtr, stride, pool, stride, vertexCount);


#ifdef __ENABLE_SANITY_CHECK 
	for (i = 0; i < vertexCount - 1; i ++) {
		ZeroVector3 p0 (&tmpPtr[i * stride]);
		for (int j = i + 1; j < vertexCount; j ++) {
			ZeroVector3 p1 (&tmpPtr[j * stride]);
			ZeroVector3 dp (p1 - p0);
			_ASSERTE (DotProduct(dp, dp) > 0.001);
		}
	}
#endif		

	hull = ConvexHull::CreateConvexHull (vertexCount, index, tmpPtr, stride);

	ZeroSetPrecision (Precision);

	return hull;
}



ZeroPolyhedra* __cdecl CreateConvexHull (
	const ZeroPlane& plane,
	float *const pool, 
	int strideInBytes, 
	int vertexCount)
{
	return NULL;
}


//
// SAVE A MESH IN X FORMAT
//
void __cdecl ZeroSavexMesh (const char *name, 
							const float *vertex, 
							const int strideInBytes, 
							const int vertexCount, 
							const ZeroFace *face, 
							const int faceCount)
{
	int i;
	int stride;
	float x;
	float y;
	float z;
	

	ZeroFile file(name, "wb");
	if (!file) {
		return;
	}

	stride = strideInBytes / sizeof (float); 

	ZeroVector3 min; 
	ZeroVector3 max; 
	GetMinMax (min, max, vertex, strideInBytes, vertexCount);
	
	ZeroVector3 centre ((max + min) * 0.5f);

	file.Print ("xof 0302txt 0064\n\n");
	file.Print ("Header {\n");
	file.Print ("  1;\n");
	file.Print ("  0;\n");
	file.Print ("  1;\n");
	file.Print ("}\n\n");

	file.Print ("Frame frm_Scene_Root {\n");
	file.Print (" FrameTransformMatrix {\n");
	file.Print ("  1.000000,0.000000,0.000000,0.000000,\n");
	file.Print ("  0.000000,1.000000,0.000000,0.000000,\n");
	file.Print ("  0.000000,0.000000,1.000000,0.000000,\n");
	file.Print ("  0.000000,0.000000,0.000000,1.000000;\n");
	file.Print (" }\n\n");

	file.Print (" Mesh object {\n");

	file.Print ("  %d;\n", vertexCount);
	for (i = 0; i < vertexCount - 1; i ++) {
		x = vertex[0] - centre.x;
		y = vertex[1] - centre.y;
		z = vertex[2] - centre.z;
		file.Print ("  %f;%f;%f;,\n", x, y, z);
		vertex += stride;
	}
	x = vertex[0] - centre.x;
	y = vertex[1] - centre.y;
	z = vertex[2] - centre.z;
	file.Print ("  %f;%f;%f;;\n\n", x, y, z);

	file.Print ("  %d;\n", faceCount);
	for (i = 0; i < faceCount - 1; i ++) {
		file.Print ("  3;%d,%d,%d;,\n", face[i].index[0], face[i].index[1], face[i].index[2]); 
	}
	file.Print ("  3;%d,%d,%d;;\n", face[i].index[0], face[i].index[1], face[i].index[2]); 

	file.Print (" }\n");
	file.Print ("}\n");
}



int __cdecl TriangleStripToTriangleList (
	int indexCount, 
	const unsigned short indexStripArray[],
	unsigned short indexTriangleArrayOut[])
{

	int i;
	int count;
	bool counterClockWise;
	unsigned short reg0;
	unsigned short reg1;
	unsigned short reg2;

	reg0 = 0;
	reg1 = 0;
	count = 0;
	counterClockWise = false;

	for (i = 0; i < indexCount; i ++) {
		reg2 = indexStripArray[i];
		if (reg2	& 0x8000) {
			reg2 &= 0x7fff;
			counterClockWise = true;
		} else {
			if (counterClockWise) {
				indexTriangleArrayOut[count * 3 + 0] = reg0;
				indexTriangleArrayOut[count * 3 + 1] = reg1;
				indexTriangleArrayOut[count * 3 + 2] = reg2;
			} else {
				indexTriangleArrayOut[count * 3 + 0] = reg0;
				indexTriangleArrayOut[count * 3 + 2] = reg1;
				indexTriangleArrayOut[count * 3 + 1] = reg2;
			}
			count ++;
			counterClockWise = ! counterClockWise;
		}

		reg0 = reg1;
		reg1 = reg2;
	}
	return count;
}

#endif //#ifdef TOOL_DEVELOPMENT 

//
// COMBINE VERTICES AND GENERATE AN INDEX LIST
//
#ifdef TOOL_DEVELOPMENT
int __cdecl VertexListToIndexList (
	float * const vertexList, 
	const int strideInBytes,
	const int vertexCount, 
	unsigned * const indexList,
	const int looseInBytes,
	const float tolerance)
{
	int count;
	unsigned precision;

	precision = ZeroSetPrecision(ZeroDoublePrecision);
	
	count = GeoUtil::SortVertices (vertexList, strideInBytes / sizeof(float), vertexCount,
											 indexList, looseInBytes / sizeof(float), tolerance);

	ZeroSetPrecision (precision);

	return count;
}
#elif (PLATFORM == PLATFORM_PS2)
// This function is always needed on the PS2, regardless of the TOOL_DEVELOPMENT
// flag - NM 4/23/02
int __cdecl VertexListToIndexList (
	float * const vertexList, 
	const int strideInBytes,
	const int vertexCount, 
	unsigned * const indexList,
	const int looseInBytes,
	const float tolerance)
{
	int count;
	unsigned precision;

	precision = ZeroSetPrecision(ZeroDoublePrecision);
	
	count = GeoUtil::SortVertices (vertexList, strideInBytes / sizeof(float), vertexCount,
											 indexList, looseInBytes / sizeof(float), tolerance);

	ZeroSetPrecision (precision);

	return count;
}

#endif   // #elif (PLATFORM == PLATFORM_PS2)

