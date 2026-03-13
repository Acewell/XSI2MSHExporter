//********************************************************************
// Direct geometry Hardware independent library
//	By Julio Jerez
// VC: 6.0
//********************************************************************
#include "ZeroTypes.h"

#ifdef TOOL_DEVELOPMENT 

#include "ZeroHeap.h"
#include "ZeroStack.h"
#include "ZeroPlane.h"
#include "ZeroDebug.h"
#include "ZeroMatrix.h"
#include "ZeroSphere.h"
#include "ZeroVector3.h"
#include "ZeroGeoUtil.h"
#include "ZeroGeoTypes.h"
#include "ZeroPolyhedra.h"
//#include "ZeroPathFinder.h"

#define __ENABLE_SANITY_CHECK 

namespace InternalPolyhedra
{
	#pragma warning (push)
	#pragma warning (disable : 4201)

	const double MAX_COST = 1024.0;	
	const double EDGE_PENALTY	= (1.0 / 64.0);
	const int MAX_EDGE_KEY_COUNT = 4096;
	const int MAX_SHARED_EDGE_ENTRY = 256;

	struct PairKey
	{
		union	{
			LONGLONG val;
			struct {
				int i1;
				int i0;
			} fields;
		};

		PairKey ()
		{
		}

	
		PairKey (LONGLONG Val)
		{
			val = Val;
		}
	
		PairKey (int v0, int v1)
		{
			fields.i0 = v0;
			fields.i1 = v1;
		}
	};
	#pragma warning (pop)


	struct SHARED_EDGE_PAIR
	{
		ZeroVisTree<ZeroEdge, LONGLONG>::ZeroTreeNode *myNode;
		ZeroVisTree<ZeroEdge, LONGLONG>::ZeroTreeNode *hisNode;
	};


	struct CHANGED_EDGE_KEY
	{
		ZeroEdge *edge;
		PairKey oldKey;
		PairKey newKey;
	
		CHANGED_EDGE_KEY (ZeroEdge *edgeArg, int i0, int i1, int New_i0, int New_i1)
			:oldKey (i0, i1), newKey (New_i0, New_i1)
		{
			edge = edgeArg;
		}
	
	
		CHANGED_EDGE_KEY (ZeroEdge *edgeArg)
			:oldKey (edgeArg->incidentVertex, edgeArg->twin->incidentVertex), newKey (0, 0) 
		{
			edge = edgeArg;
		}
	
		void SetNewKey (ZeroEdge *edgeArg)
		{
			newKey = PairKey (edgeArg->incidentVertex, edgeArg->twin->incidentVertex);
		}
	};

	struct EDGE_HANDLE
	{
		unsigned inList;
		ZeroEdge *edge;
	
		EDGE_HANDLE	(ZeroEdge *newEdge)
		{
			inList = false;
			edge = newEdge;
		}
	
		EDGE_HANDLE	(const EDGE_HANDLE &dataHandle)
		{
			EDGE_HANDLE	*handle;
	
			inList = true;
	
			edge = dataHandle.edge;
			handle = (EDGE_HANDLE *)edge->userData;
	
			if (handle) {
	
				_ASSERTE (handle != this);
				handle->edge = NULL;
			}
			edge->userData = (unsigned) this;
		}
	
		~EDGE_HANDLE ()
		{
			EDGE_HANDLE	*handle;
			if (inList) {
				if (edge) {
					handle = (EDGE_HANDLE *)edge->userData;
					if (handle == this) {
						edge->userData = NULL;
					}
				}
			}
			edge = NULL;
		}
	};


	
	struct VERTEX_METRIC_STRUCT
	{
		double elem[10];
	
		VERTEX_METRIC_STRUCT (ZeroPlane &plane) 
		{
			elem[0] = plane.x * plane.x;  
			elem[1] = plane.y * plane.y;  
			elem[2] = plane.z * plane.z;  
			elem[3] = plane.w * plane.w;  
			elem[4] = 2.0 * plane.x * plane.y;  
			elem[5] = 2.0 * plane.x * plane.z;  
			elem[6] = 2.0 * plane.x * plane.w;  
			elem[7] = 2.0 * plane.y * plane.z;  
			elem[8] = 2.0 * plane.y * plane.w;  
			elem[9] = 2.0 * plane.z * plane.w;  
		}
	
		void Clear ()
		{
			memset (elem, 0, sizeof(elem));
		}
	
		void Accumulate (ZeroPlane &plane) 
		{
			elem[0] += plane.x * plane.x;  
			elem[1] += plane.y * plane.y;  
			elem[2] += plane.z * plane.z;  
			elem[3] += plane.w * plane.w;  
			elem[4] += 2.0 * plane.x * plane.y;  
			elem[5] += 2.0 * plane.x * plane.z;  
			elem[6] += 2.0 * plane.x * plane.w;  
			elem[7] += 2.0 * plane.y * plane.z;  
			elem[8] += 2.0 * plane.y * plane.w;  
			elem[9] += 2.0 * plane.z * plane.w;  
		}
	
	
		void Accumulate (VERTEX_METRIC_STRUCT &p) 
		{
			elem[0] += p.elem[0]; 
			elem[1] += p.elem[1]; 
			elem[2] += p.elem[2]; 
			elem[3] += p.elem[3]; 
			elem[4] += p.elem[4]; 
			elem[5] += p.elem[5]; 
			elem[6] += p.elem[6]; 
			elem[7] += p.elem[7]; 
			elem[8] += p.elem[8]; 
			elem[9] += p.elem[9]; 
		}
	
		double Evalue (ZeroVector3 &p) 
		{
			double Acc;
			Acc = elem[0] * p.x * p.x + 
					elem[1] * p.y * p.y + 
					elem[2] * p.z * p.z + 
					elem[4] * p.x * p.y + 
					elem[5] * p.x * p.z + 
					elem[7] * p.y * p.z + 
					elem[6] * p.x + elem[8] * p.y + elem[9] * p.z + elem[3];  
	
		   _ASSERTE (Acc >= -1.0e-2);
		   return Acc;
		}
	};

	static void RemoveHalfEdge (
		ZeroPolyhedra *polyhedra,
		ZeroEdge *edge,
		int v1)
	{
		EDGE_HANDLE	*handle;
	
		handle = (EDGE_HANDLE *) edge->userData;
		if (handle) { 
			handle->edge = NULL;
		}
	
		InternalPolyhedra::PairKey key (edge->incidentVertex, v1);
	
		_ASSERTE (polyhedra->Find (key.val));
	
		polyhedra->Remove (key.val);
	}

	static void RemoveHalfEdge (
		ZeroPolyhedra *polyhedra,
		ZeroEdge *edge)
	{
		_ASSERTE (edge->twin);
		RemoveHalfEdge (polyhedra, edge, edge->twin->incidentVertex);
	}

	static bool MatchTwins (ZeroPolyhedra *polyhedra) 
	{
		bool ret;
		ZeroEdge *edge;
		ZeroPolyhedra::Iterator iter (*polyhedra);
	
		ret = true;

		// Conect all twin edge
		for (iter.Begin(); iter; iter ++) {
			edge = &(*iter)->info;
			if (!edge->twin) {
				edge->twin = polyhedra->FindEdge (edge->next->incidentVertex, edge->incidentVertex);
				if (edge->twin) {
					edge->twin->twin = edge; 
				}
				ret &= (edge->twin != NULL);
			}
		}

		#ifdef __ENABLE_SANITY_CHECK 
		_ASSERTE (!ret || polyhedra->SanityCheck());
		#endif
		return ret;
	}

	static void CloseOpenBounds (ZeroPolyhedra *polyhedra)
	{
		int edgeCount;
		ZeroEdge *ptr;
		ZeroEdge *edge;
		ZeroEdge** edgeArray;
		ZeroPolyhedra::ZeroTreeNode *node;
		ZeroPolyhedra::Iterator iter (*polyhedra);
		ZeroStack<ZeroEdge*> edgeArrayPool(polyhedra->GetCount()*2 + 1024);
	
		edgeCount = 0;
		edgeArray = &edgeArrayPool[0];
		for (iter.Begin(); iter; iter ++) {
			edge = &(*iter)->info;
			if (!edge->twin) {
				InternalPolyhedra::PairKey code (edge->next->incidentVertex, edge->incidentVertex);
				ZeroEdge tmpEdge (edge->next->incidentVertex, -1);
				tmpEdge.incidentFace = -1; 
				node = polyhedra->Insert (tmpEdge, code.val); 
				edge->twin = &node->info;
				edge->twin->twin = edge; 
				edgeArray[edgeCount] = edge->twin;
				edgeCount ++;
			}
		}
	
		for (iter.Begin(); iter; iter ++) {
			edge = &(*iter)->info;
			if (!edge->prev) {
				for (ptr = edge->twin; ptr->next; ptr = ptr->next->twin);
				ptr->next = edge;
				edge->prev = ptr;
			}
		}
	
		#ifdef __ENABLE_SANITY_CHECK 
			_ASSERTE (polyhedra->SanityCheck ());
		#endif
	}


	static void NormalizeVertex (
	   int count,
		ZeroVector3 target[],
		const float source[],
		int stride)
	{
		int i;
		
		ZeroVector3 min;
		ZeroVector3 max;
		GetMinMax (min, max,	source, stride * sizeof (float), count);

		ZeroVector3 centre (0, 0, 0);
				
		for (i = 0; i < count; i ++) {
			target[i].x = centre.x + source[i * stride + 0];
			target[i].y = centre.y + source[i * stride + 1];
			target[i].z = centre.z + source[i * stride + 2];
		}
	}

	static ZeroPlane EdgePlane (
		ZeroEdge *face,
		ZeroVector3 *pool)
	{
		ZeroEdge *edge;
		ZeroVector3 normal (0, 0, 0);
		edge = face;
		ZeroVector3 p0(pool[edge->incidentVertex]);
		edge = edge->next;
		ZeroVector3 p1(pool[edge->incidentVertex]);
		edge = edge->next;
		while (edge != face) {
			ZeroVector3 p2(pool[edge->incidentVertex]);
			normal += CrossProduct(p1 - p0, p2 - p0);
			p1 = p2;
			edge = edge->next;
		}

		ZeroPlane plane (normal, -DotProduct(normal, p0));
		float mag = Magnitude(plane);
		if (mag < 1.0e-12) {
			ZeroWarn((0U, "Degenerate face!"));
			mag = 1.0e-12f;
		}
		mag = 1.0f / mag;
	
		plane.x *= mag;
		plane.y *= mag;
		plane.z *= mag;
		plane.w *= mag;
	
		return plane;
	}


	static ZeroPlane UnboundedLoopPlane (
		int i0, 
		int i1, 
		int i2, 
		ZeroVector3 *pool)
	{
		float mag;
		float dist;
			
		ZeroVector3 p0 (&pool[i0].x);
		ZeroVector3 p1 (&pool[i1].x);
		ZeroVector3 p2 (&pool[i2].x);
		ZeroVector3 E0 (p1 - p0); 
		ZeroVector3 E1 (p2 - p0); 
	
		ZeroVector3 N (CrossProduct(CrossProduct(E0, E1), E0)); 
		ZeroWarn((MagnitudeSq(CrossProduct(NormalizeFast(p0 - p1), NormalizeFast(p1 - p2))) > EPSILON * EPSILON ,
			"Degenerate face!"));
		dist = - DotProduct(N, p0);
		ZeroPlane plane (N, dist);
	
		mag = DotProduct(plane, plane);
		if (mag < 1.0e-12) {
			mag = 1.0e-12f;
		}
		mag = 10.0f / sqrtf (mag);
	
		plane.x *= mag;
		plane.y *= mag;
		plane.z *= mag;
		plane.w *= mag;
	
		return plane;
	}

	static void CalculateAllMetrics (
		ZeroPolyhedra *polyhedra,
		VERTEX_METRIC_STRUCT *table,
		ZeroVector3 *pool)
	{
		int i0;
		int i1;
		int i2;
		int edgeMark; 
		ZeroEdge *ptr;
		ZeroEdge *edge;
		ZeroPlane constrainPlane;
	
		edgeMark = polyhedra->IncLRU();
	
		ZeroPolyhedra::Iterator iter (*polyhedra);
		for (iter.Begin(); iter; iter ++) {
			edge = &(*iter)->info;
	
			_ASSERTE (edge);
	
//			if (polyhedra->GetConstrainedPlane (constrainPlane, edge, pool)) {
//				table[edge->incidentVertex].Accumulate(constrainPlane);
//			}
	
			if (edge->mark != edgeMark) {
	
				if (edge->incidentFace > 0) {
					constrainPlane = EdgePlane (edge, pool);
					VERTEX_METRIC_STRUCT tmp (constrainPlane);
	
					ptr = edge;
					do {
						edge->mark = edgeMark;
						i0 = edge->incidentVertex;
						table[i0].Accumulate(tmp);
	
						edge = edge->next;
					} while (edge != ptr);
	
				} else {
					_ASSERTE (edge->twin->incidentFace > 0);
					i0 = edge->twin->incidentVertex;
					i1 = edge->twin->next->incidentVertex;
					i2 = edge->twin->prev->incidentVertex;
		
	  				edge->mark = edgeMark;
					constrainPlane = UnboundedLoopPlane (i0, i1, i2, pool);
					VERTEX_METRIC_STRUCT tmp (constrainPlane);
	
					i0 = edge->incidentVertex;
					table[i0].Accumulate(tmp);
	
					i0 = edge->twin->incidentVertex;
					table[i0].Accumulate(tmp);
				}
			}
		}
	}


	static bool EdgePenalty (
		ZeroVector3 *pool,
		ZeroEdge *edge) 
	{
		int i0;
		int i1;
		int i2;
		int Face;
		int faceA;
		int faceB;
		bool penalty;
		double dx0;
		double dy0;
		double dz0;
		double dx1;
		double dy1;
		double dz1;
		double mag0;
		double mag1;
		double dot;
		double n0[3];
		double n1[3];
		ZeroEdge *ptr;
		ZeroEdge *adj;
		ZeroVector3 *p;
		ZeroVector3 *p0;
		ZeroVector3 *p1;
		ZeroVector3 *p2;


		i0 = edge->incidentVertex;
		i1 = edge->next->incidentVertex;

		p0 = &pool[i0]; 
		p1 = &pool[i1]; 
	
		dx0 = p1->x - p0->x;
		dy0 = p1->y - p0->y;
		dz0 = p1->z - p0->z;

		dot = dx0 * dx0 + dy0 * dy0 + dz0 * dz0;
		if (dot < 1.0e-6) {
			return true;
		}

		if ((edge->incidentFace > 0) && (edge->twin->incidentFace > 0)) {
			ptr = edge;
			do {
				if ((ptr->incidentFace <= 0) || (ptr->twin->incidentFace <= 0)){
					adj = edge->twin;
					ptr = edge;
					do {
						if ((ptr->incidentFace <= 0) || (ptr->twin->incidentFace <= 0)){
							return true;
						}
						ptr = ptr->twin->next;
					} while (ptr != adj);
				}
				ptr = ptr->twin->next;
			} while (ptr != edge);
		}
	
		faceA = edge->incidentFace;
		faceB = edge->twin->incidentFace;
	
		i0 = edge->twin->incidentVertex;
		p = &pool[i0]; 
	
		penalty = true;
		ptr = edge;
		do {
			adj = ptr->twin;
	
			Face = adj->incidentFace;
	
			if (Face == faceB) {
				ptr = ptr->twin->next;
				continue;
			}
	
			if (Face == faceA) {
				ptr = ptr->twin->next;
				continue;
			}
	
			if (Face < 0) {
				ptr = ptr->twin->next;
				continue;
			}
	
			if (adj->next->incidentFace != Face) {
				ptr = ptr->twin->next;
				continue;
			}
	
			if (adj->prev->incidentFace != Face) {
				ptr = ptr->twin->next;
				continue;
			}
	
			i0 = adj->next->incidentVertex;
			p0 = &pool[i0]; 
	
			i1 = adj->incidentVertex;
			p1 = &pool[i1]; 
	
			i2 = adj->prev->incidentVertex;
			p2 = &pool[i2]; 
	
			dx0 = p1->x - p0->x;
			dy0 = p1->y - p0->y;
			dz0 = p1->z - p0->z;
			dx1 = p2->x - p0->x;
			dy1 = p2->y - p0->y;
			dz1 = p2->z - p0->z;
			n0[0] = dy0 * dz1 - dz0 * dy1;
		   n0[1] = dz0 * dx1 - dx0 * dz1;
		   n0[2] = dx0 * dy1 - dy0 * dx1;
	
			dx0 = p1->x - p->x;
			dy0 = p1->y - p->y;
			dz0 = p1->z - p->z;
			dx1 = p2->x - p->x;
			dy1 = p2->y - p->y;
			dz1 = p2->z - p->z;
			n1[0] = dy0 * dz1 - dz0 * dy1;
		   n1[1] = dz0 * dx1 - dx0 * dz1;
		   n1[2] = dx0 * dy1 - dy0 * dx1;
	
			mag0 = n0[0] * n0[0] + n0[1] * n0[1] + n0[2] * n0[2];
			mag0 = sqrt (mag0);
	
			mag1 = n1[0] * n1[0] + n1[1] * n1[1] + n1[2] * n1[2];
			mag1 = sqrt (mag1);
	
			dot = n0[0] * n1[0] + n0[1] * n1[1] + n0[2] * n1[2];
	
			penalty = false;
			if (dot <= (mag0 * mag1 * EDGE_PENALTY) || (mag0 > 16.0 * mag1)) {
				penalty = true;
				break;
			}
	
			ptr = ptr->twin->next;
		} while (ptr != edge);
	
		return penalty;
	}

	static void CalculateVertexMetrics ( 
		ZeroPolyhedra *polyhedra,
		VERTEX_METRIC_STRUCT *table,
		ZeroVector3 *pool,
		ZeroEdge *edge)
	{
		int i0;
		int i1;
		int i2;
		ZeroEdge *ptr;
		ZeroPlane constrainPlane;
	
		i0 = edge->incidentVertex;
	
		ZeroVector3 p0 (&pool[i0].x);
		table[i0].Clear ();
	
		ptr = edge;
		do {
	
//			if (polyhedra->GetConstrainedPlane (constrainPlane, ptr, pool)) {
//				 table[i0].Accumulate (constrainPlane);
//			}
	
			if (ptr->incidentFace > 0) {
				constrainPlane = EdgePlane (ptr, pool);
				table[i0].Accumulate (constrainPlane);
	
			} else {
//			  	_ASSERTE (ptr->twin->incidentFace > 0);
	
			  	i1 = ptr->twin->incidentVertex;
			  	i2 = ptr->twin->prev->incidentVertex;
				constrainPlane = UnboundedLoopPlane (i0, i1, i2, pool);
				table[i0].Accumulate (constrainPlane);
	
//			  	_ASSERTE (ptr->prev->incidentFace < 0);
			  	i1 = ptr->prev->incidentVertex;
			  	i2 = ptr->prev->twin->prev->incidentVertex;
				constrainPlane = UnboundedLoopPlane (i0, i1, i2, pool);
				table[i0].Accumulate (constrainPlane);
			}
	
			ptr = ptr->twin->next;
		} while (ptr != edge);
	}

	static ZeroEdge* FindConcaveTip (
		ZeroPolyhedra& polyhedra,
		ZeroEdge *face,
		ZeroVector3 &normal,
		const float array[],
		const int stride,
		ZeroHeap <ZeroEdge*, float>& heap)
	{
		float f;
		float dotn;
		float minAngle;
		ZeroEdge *ear;
		ZeroEdge *ptr;

		ptr = face;
		ZeroVector3 p0 (&array[ptr->prev->incidentVertex * stride]);
		ZeroVector3 p1 (&array[ptr->incidentVertex * stride]);
		ZeroVector3 d0 (p1 - p0);
		f = sqrtf (DotProduct (d0, d0));
		if (f < 1.0e-10f) {
			f = 1.0e-10f;
		}
		d0 *= (1.0f / f);

		minAngle = 10.0;
		do {
			ZeroVector3 p2 (&array[ptr->next->incidentVertex * stride]);
			ZeroVector3 d1 (p2 - p1);
			f = sqrtf (DotProduct (d1, d1));
 			if (f < 1.0e-10f) {
				f = 1.0e-10f;
			}
			d1 *= (1.0f / f);
			ZeroVector3 n (CrossProduct (d0, d1));

			dotn = DotProduct(normal, n);
			if (dotn > 0) {	// was >=
				heap.Push (ptr, dotn);
			}

			if (dotn < minAngle) {
				minAngle	= dotn;
			}

			d0 = d1;
			p1 = p2;
			ptr = ptr->next;
		} while (ptr != face);

		_ASSERTE (heap.GetCount());

		if (minAngle > 0.1) {
			return heap[0];
		}

		ear = NULL;
		while (heap.GetCount()) {
			ear = heap[0];
			heap.Pop();

			ZeroVector3 p0 (&array[ear->prev->incidentVertex * stride]);
			ZeroVector3 p1 (&array[ear->incidentVertex * stride]);
			ZeroVector3 p2 (&array[ear->next->incidentVertex * stride]);

			ZeroVector3 p10 (p1 - p0);
			ZeroVector3 p21 (p2 - p1);
			ZeroVector3 p02 (p0 - p2);

			for (ptr = ear->next->next; ptr != ear->prev; ptr = ptr->next) {
				ZeroVector3 p (&array[ptr->incidentVertex * stride]);

				ZeroVector3 n (CrossProduct ((p - p0), p10));
				if (DotProduct(normal, n) < 1.0e-6) {
					ZeroVector3 n (CrossProduct ((p - p1), p21));
					if (DotProduct(normal, n) < 1.0e-6) {
						ZeroVector3 n (CrossProduct ((p - p2), p02));
						if (DotProduct(normal, n) < 1.0e-6) {
							break;
						}
					}
				}
			}

			if (ptr == ear->prev) {
				break;
			}
		}

		_ASSERTE (ear);
		return ear;
	}

	static bool TriangulateFace (
		ZeroPolyhedra& polyhedra,
		ZeroEdge *face,
		const float array[],
		const int stride,
		ZeroHeap <ZeroEdge*, float>& heap)
	{
		float mag;
		ZeroEdge *ear;
		ZeroEdge *edge;
		ZeroEdge *twin;

		ZeroVector3 normal (0, 0, 0);
		edge = face;
		ZeroVector3 p0(&array[edge->incidentVertex * stride]);
		edge = edge->next;
		ZeroVector3 p1(&array[edge->incidentVertex * stride]);
		edge = edge->next;
		while (edge != face) {
			ZeroVector3 p2(&array[edge->incidentVertex * stride]);
			normal += CrossProduct(p1 - p0, p2 - p0);
			p1 = p2;
			edge = edge->next;
		}

		mag = DotProduct (normal, normal);
		if (mag < 0.0000001) {
		 	polyhedra.DeleteFace(face);
		 	heap.Flush ();
			return false;
		}

		normal = normal.Scale (1.0f / sqrtf (mag));

		while (face->next->next->next != face) {
			ear = FindConcaveTip (polyhedra, face, normal, array, stride, heap); 
			if (!ear) {
				polyhedra.DeleteFace(face);
		 		heap.Flush ();
				return false;
			}
			if ((face == ear) || (face == ear->prev)) {
				face = face->prev->prev;
			}
			edge = polyhedra.AddEdge (ear->next->incidentVertex, ear->prev->incidentVertex);
			if (!edge) {
				polyhedra.DeleteFace(face);
		 		heap.Flush ();
				return false;
			}
			twin = polyhedra.AddEdge (ear->prev->incidentVertex, ear->next->incidentVertex);
			_ASSERTE (twin);

			edge->userData = ear->next->userData;
			twin->userData = ear->prev->userData;

			edge->twin = twin;
			twin->twin = edge;

			twin->prev = ear->prev->prev;
			twin->next = ear->next;
			ear->prev->prev->next = twin;
			ear->next->prev = twin;

			edge->next = ear->prev;
			edge->prev = ear;
			ear->prev->prev = edge;
			ear->next = edge;

			edge->incidentFace = ear->incidentFace;
			edge->mark = ear->mark;

			twin->incidentFace = ear->incidentFace;
			twin->mark = ear->mark;

			heap.Flush ();
		}
		return true;
	}


	static void GetAdjacentCoplanarFacesPerimeter (
		ZeroPolyhedra& perimeter,
		ZeroPolyhedra& polyhedra,
		ZeroEdge *face, 
		const float pool[],  
		int strideInBytes,
		ZeroEdge **stack,
		int* faceIndex)
	{
		int index;
		int faceMark;
		int faceIndexCount;
		float dot;
		ZeroEdge *ptr;
		ZeroEdge *edge;
		const float normalDeviation = 0.9999f;
		ZeroStack<int>facesIndex (4096);

		_ASSERTE (face->incidentFace > 0);

		polyhedra.IncLRU();
		faceMark = polyhedra.IncLRU();

		ZeroVector3 normal (polyhedra.FaceNormal (face, pool, strideInBytes));
		dot = DotProduct (normal, normal);
		if (dot < 1.0e-12) {
			ptr = face;
			faceIndexCount	= 0;
			do {
				faceIndex[faceIndexCount] = ptr->incidentVertex;
				faceIndexCount	++;
				ptr->mark = faceMark;
				ptr = ptr->next;
			} while (ptr != face);
			perimeter.AddFace (faceIndexCount, faceIndex);
			return;
		}
		normal = normal.Scale (1.0f / sqrtf (dot));

		stack[0] = face;
		index = 1;
		perimeter.BeginFace();
		while (index) {
			index --;
			edge = stack[index];

			if (edge->mark == faceMark) {
				continue;
			}
	
			ZeroVector3 normal1 (polyhedra.FaceNormal (edge, pool, strideInBytes));
			dot = DotProduct (normal1, normal1);
			if (dot < 1.0e-12f) {
				dot = 1.0e-12f;
			}
			normal1 = normal1.Scale (1.0f / sqrtf (dot));
	
			dot = DotProduct (normal1, normal);
			if (dot >= normalDeviation) {
				ptr = edge;
				faceIndexCount	= 0;
				do {
					faceIndex[faceIndexCount] = ptr->incidentVertex;
					faceIndexCount	++;
					ptr->mark = faceMark;
					if ((ptr->twin->incidentFace > 0) && (ptr->twin->mark != faceMark)) {
						stack[index] = ptr->twin;
						index ++;
						_ASSERTE (index < (int)polyhedra.GetCount() / 2);
					}
					ptr = ptr->next;
				} while (ptr != edge);
				perimeter.AddFace (faceIndexCount, faceIndex);
			}
		}
		perimeter.EndFace();

		ZeroPolyhedra::Iterator iter (perimeter);
		for (iter.Begin(); iter; ) {
			edge = &(*iter)->info;
			iter ++;
			if ((edge->incidentFace > 0) && (edge->twin->incidentFace > 0)) {
				if ( perimeter.GetNodeFromInfo (*edge->twin)	== iter.GetNode()) {
					iter ++;
				}
				perimeter.DeleteEdge (edge);
			}
		}
	}




	static bool IsEssensialPointDiagonal (
		ZeroEdge* diagonal,
		const ZeroVector3& normal,
		const float pool[],  
		int stride)
	{
		float dot;
		ZeroVector3 p0 (&pool[diagonal->userData * stride]);
		ZeroVector3 p1 (&pool[diagonal->twin->next->twin->userData * stride]);
		ZeroVector3 p2 (&pool[diagonal->prev->userData * stride]);

		ZeroVector3 e1 (p1 - p0);
		dot = DotProduct (e1, e1);
		if (dot < 1.0e-12) {
			return false;
		}
		e1 = e1.Scale (1.0f / sqrtf(dot));

		ZeroVector3 e2 (p2 - p0);
		dot = DotProduct (e2, e2);
		if (dot < 1.0e-12) {
			return false;
		}
		e2 = e2.Scale (1.0f / sqrtf(dot));

		ZeroVector3 n1 (CrossProduct (e1, e2)); 

		dot = DotProduct (normal, n1);
		if (dot > -5.0e-3) {
			return false;
		}
		return true;
	}

  
	static bool IsEssensialDiagonal (
		ZeroEdge* diagonal,
		const ZeroVector3& normal,
		const float pool[],  
		int stride)
	{
		return IsEssensialPointDiagonal (diagonal, normal,	pool, stride) || 
				 IsEssensialPointDiagonal (diagonal->twin, normal,	pool, stride); 
	}


	static int GetInteriorDiagonals (
		ZeroPolyhedra& polyhedra,
		ZeroEdge **diagonals)
	{

		int count;
		int mark;
		ZeroEdge *edge;
		ZeroPolyhedra::Iterator iter (polyhedra);

		count  = 0;
		mark = polyhedra.IncLRU();
		for (iter.Begin(); iter; iter ++) {
			edge = &(*iter)->info;
			if (edge->mark == mark) {
				continue;
			}

			if (edge->incidentFace < 0) {
				continue;
			}

			edge->mark = mark;
			edge->twin->mark = mark;

			if (edge->twin->incidentFace > 0) {
				diagonals[count] = edge;
				count ++;
			}
		}
		return count;
	}

	static void SimplePolygonConvexPartition (
		ZeroPolyhedra& polyhedra,
		const ZeroVector4 normal, 
		const float vertex[], 
		int strideInBytes,
		ZeroEdge* diagonals[],
		ZeroList<LONGLONG>& deadEdges)
	{
		int i;
		int stride;
		int diagonalCount;
		LONGLONG key;
		ZeroEdge* diagonal;


		stride = strideInBytes / sizeof (float);
		diagonalCount = GetInteriorDiagonals (polyhedra, diagonals);
		for (i = 0; i < diagonalCount; i ++) {
			diagonal = diagonals[i];
			if (!IsEssensialDiagonal(diagonal, normal, vertex, stride)) {
				key = (LONGLONG (diagonal->userData) << 32) + diagonal->twin->userData;
				deadEdges.Append (key);
				polyhedra.DeleteEdge (diagonal);
			}
		}
	}

	static bool CanConectToOtherBorder (
		ZeroPolyhedra& polyhedra, 
		ZeroEdge *edge, 
		ZeroEdge *otherBorder)
	{

		int mark;
		ZeroEdge *ptr;
		ZeroEdge *ptr1;


//		ptr = edge;
//		do {
//			if (ptr->incidentVertex != (int)ptr->userData) {
//				_ASSERTE (0);
//				return false;
//			}
//			ptr = ptr->twin->next;
//		} while (ptr != edge);


		mark = otherBorder->mark;
		ptr = edge;
		do {
			ptr1 = ptr->twin;
			do {
				if ((ptr1->mark == mark) || (ptr1->twin->mark == mark)) {
//					ZeroEdge *ptr2;
//					ptr2 = ptr1;
//					do {
//						if (ptr2->incidentVertex != (int)ptr2->userData) {
//							break;
//						}
//						ptr2 = ptr2->twin->next;
//					} while (ptr2 != ptr1);
//					if (ptr2->incidentVertex == (int)ptr2->userData) {
						return true;
//					}
				}
				ptr1 = ptr1->twin->next;
			} while (ptr1 != ptr->twin);


			ptr = ptr->twin->next;
		} while (ptr != edge);
		return false;
	}



	static ZeroEdge *FindSpliteEdge (
		ZeroPolyhedra& polyhedra,
		ZeroEdge* innerLoop, 
		ZeroEdge* outerLoop, 
		const float vertex[], 
		int strideInBytes)
	{

		int mark;
		int stride;
		float dist2;
		float bestDist2;
		ZeroEdge *ptr;
		ZeroEdge *ptr1;
		ZeroEdge *spliteEdge;

		spliteEdge = NULL;

		bestDist2 = 1.0e10f;

		stride = strideInBytes / sizeof (float);
		ZeroVector4 p0 (&vertex[innerLoop->userData * stride]);
		mark = outerLoop->mark;
		ptr = innerLoop;
		do {
			ptr1 = ptr->twin;
			do {
				if ((ptr1->mark == mark) || (ptr1->twin->mark == mark)) {
					ZeroVector4 p1 (&vertex[ptr->twin->userData * stride]);
					ZeroVector4 dp (p1 - p0);
					dist2	= DotProduct (dp, dp);
					if (dist2 < bestDist2) {
						bestDist2 = dist2;
						spliteEdge = ptr;
					}
					break;
				}
				ptr1 = ptr1->twin->next;
			} while (ptr1 != ptr->twin);


			ptr = ptr->twin->next;
		} while (ptr != innerLoop);


		_ASSERTE (spliteEdge);
		return spliteEdge;

	}


	static void InsertAliasEdge (
		ZeroPolyhedra& polyhedra,
		ZeroEdge* splite, 
		int& lastVertexIndex)
	{
		int v0;
		int v1;

		ZeroEdge* ptr;
		ZeroEdge* edge;
		ZeroEdge* twin;
		ZeroEdge* border;


		lastVertexIndex = lastVertexIndex + 1;
		v0 = lastVertexIndex;

		lastVertexIndex = lastVertexIndex + 1;
		v1 = lastVertexIndex;

		edge = polyhedra.AddEdge (v0, v1);
		twin = polyhedra.AddEdge (v1, v0);


		edge->twin = twin;
		twin->twin = edge;

		edge->userData = splite->userData;
		twin->userData = splite->twin->userData;

		edge->incidentFace = -1;
		twin->incidentFace = splite->twin->incidentFace;
		splite->twin->incidentFace = -1;


		for (border = splite; border->incidentFace > 0; border = border->prev->twin);
		splite->twin->next->prev = twin;
		twin->next = splite->twin->next;

		edge->prev = border->prev;
		border->prev->next = edge;

		splite->twin->next = border;
		border->prev = splite->twin;
		


		for (border = splite; border->incidentFace > 0; border = border->next->twin);
		splite->twin->prev->next = twin;
		twin->prev = splite->twin->prev;

		edge->next = border->next;
		border->next->prev = edge;

		splite->twin->prev = border;
		border->next = splite->twin;


		ptr = edge->twin->next;
		do {
			PairKey newEdgeKey(edge->incidentVertex, ptr->twin->incidentVertex);
			PairKey oldEdgeKey(splite->incidentVertex, ptr->twin->incidentVertex);
			polyhedra.ReplaceKey (oldEdgeKey.val, newEdgeKey.val);


			PairKey newTwinKey(ptr->twin->incidentVertex, edge->incidentVertex);
			PairKey oldTwinKey(ptr->twin->incidentVertex, splite->incidentVertex);
			polyhedra.ReplaceKey (oldTwinKey.val, newTwinKey.val);

			ptr = ptr->twin->next;
		} while (ptr !=  edge);


		ptr = twin->twin->next;
		do {
			PairKey newEdgeKey(twin->incidentVertex, ptr->twin->incidentVertex);
			PairKey oldEdgeKey(splite->twin->incidentVertex, ptr->twin->incidentVertex);
			polyhedra.ReplaceKey (oldEdgeKey.val, newEdgeKey.val);


			PairKey newTwinKey(ptr->twin->incidentVertex, twin->incidentVertex);
			PairKey oldTwinKey(ptr->twin->incidentVertex, splite->twin->incidentVertex);
			polyhedra.ReplaceKey (oldTwinKey.val, newTwinKey.val);

			ptr = ptr->twin->next;
		} while (ptr != twin);


		v0 = edge->incidentVertex;
		ptr = edge;
		do {
			ptr->incidentVertex = v0;
			ptr = ptr->twin->next;
		} while (ptr !=  edge);


		v0 = twin->incidentVertex;
		ptr = twin;
		do {
			ptr->incidentVertex = v0;
			ptr = ptr->twin->next;
		} while (ptr != twin);

//		_ASSERTE (polyhedra.SanityCheck());

	}



	static float FindStartingPoint (
		ZeroPolyhedra& polyhedra, 
		ZeroEdge **start, 
		ZeroEdge *otherBorder, 
		const ZeroVector4& normal,
		const float vertex[], 
		int strideInBytes)
	{
		int mark;
		int stride;
		ZeroEdge *ptr;
		ZeroEdge *edge;
		float value;
		float bestTurn;


		mark = polyhedra.IncLRU();
		ptr = otherBorder;
		do {
			ptr->mark = mark;
			ptr = ptr->next;
		} while (ptr != otherBorder);


		edge = *start;
		stride = strideInBytes / sizeof (float);

		ZeroVector3 p0 (&vertex[edge->prev->userData * stride]);
		ZeroVector3 p1 (&vertex[edge->userData * stride]);
		ZeroVector3 dp0 (p1 - p0);
		dp0 = dp0.Scale (rsqrt (DotProduct (dp0, dp0) + 1.0e-6f));

		bestTurn	= 0.0f;

		ptr = edge;
		do {
			ZeroVector3 p2 (&vertex[ptr->twin->userData * stride]);
			ZeroVector3 dp1 (p2 - p1);
			dp1 = dp1.Scale (rsqrt (DotProduct (dp1, dp1) + 1.0e-6f));

			ZeroVector4 turnNormal (CrossProduct (dp1, dp0));
			value = DotProduct (normal, turnNormal);
			if (value < bestTurn) {
				if (CanConectToOtherBorder (polyhedra, ptr, otherBorder)) {
					bestTurn	= value;
					*start = ptr;
				}
			}

			p1 = p2;
			dp0 = dp1;
			ptr = ptr->next;
		} while (ptr != edge);

		return bestTurn;
	}


//	static void GetConcaveCorners  (
//		ZeroPolyhedra& polyhedra, 
//		ZeroVisList<ZeroEdge*>& concaveOut,
//		ZeroEdge *start, 
//		ZeroEdge *otherBorder, 
//		const ZeroVector4& normal,
//		const float vertex[], 
//		int strideInBytes)
//	{
//		int mark;
//		int stride;
//		ZeroEdge *ptr;
//		ZeroEdge *edge;
//		float value;
//
//
//		mark = polyhedra.IncLRU();
//		ptr = otherBorder;
//		do {
//			ptr->mark = mark;
//			ptr = ptr->next;
//		} while (ptr != otherBorder);
//
//
//		edge = start;
//		stride = strideInBytes / sizeof (float);
//
//		ZeroVector3 p0 (&vertex[edge->prev->userData * stride]);
//		ZeroVector3 p1 (&vertex[edge->userData * stride]);
//		ZeroVector3 dp0 (p1 - p0);
//		dp0 = dp0.Scale (rsqrt (DotProduct (dp0, dp0) + 1.0e-6f));
//
//
//		ZeroHeap<ZeroEdge*, float> heap (64, 10.0f);
//
//		ptr = edge;
//		do {
//			ZeroVector3 p2 (&vertex[ptr->twin->userData * stride]);
//			ZeroVector3 dp1 (p2 - p1);
//			dp1 = dp1.Scale (rsqrt (DotProduct (dp1, dp1) + 1.0e-6f));
//
//			ZeroVector4 turnNormal (CrossProduct (dp1, dp0));
//			value = DotProduct (normal, turnNormal);
//			if (value < -1.e-3f) {
//				if (CanConectToOtherBorder (polyhedra, ptr, otherBorder)) {
//					heap.Push (ptr, fabsf (value)); 
//					if (heap.GetCount() > 16) {
//						break;
//					}
//				}
//			}
//
//			p1 = p2;
//			dp0 = dp1;
//			ptr = ptr->next;
//		} while (ptr != edge);
//
//
//		while (heap.GetCount()) {
//			concaveOut.Append (heap[0]);
//			heap.Pop();
//		}
//	}


	static bool CombineOpenLoops (
		ZeroPolyhedra& polyhedra, 
		ZeroEdge *openFace0, 
		ZeroEdge *openFace1,
		const float vertex[],
		int strideInBytes,
		const ZeroVector4& normal,
		int& lastVertexIndex)
	{
		float cost0;
		float cost1;
		ZeroEdge *innerLoop; 
		ZeroEdge *outerLoop; 
		ZeroEdge *spliteEdge; 

		cost0 = FindStartingPoint (polyhedra, &openFace0, openFace1, normal, vertex, strideInBytes);
		cost1 = FindStartingPoint (polyhedra, &openFace1, openFace0, normal, vertex, strideInBytes);
		if ((cost0 > -1.0e-5f) && (cost1 > -1.0e-5)) {
			return false;
		}

		innerLoop = NULL; 
		outerLoop = NULL; 
		if (cost0 < cost1) {
			innerLoop = openFace0;
			outerLoop = openFace1;
		} else {
			innerLoop = openFace1;
			outerLoop = openFace0;
		}
		spliteEdge = FindSpliteEdge (polyhedra, innerLoop, outerLoop, vertex, strideInBytes);
		InsertAliasEdge (polyhedra, spliteEdge, lastVertexIndex);


//		ZeroVisList<ZeroEdge*> concave0;
//		ZeroVisList<ZeroEdge*> concave1;
//		GetConcaveCorners (polyhedra, concave0, openFace0, openFace1, normal, vertex, strideInBytes);
//		GetConcaveCorners (polyhedra, concave1, openFace1, openFace0, normal, vertex, strideInBytes);

		return true;
	}



	static void FindOpenEdges (ZeroPolyhedra& polyhedra, ZeroVisList<ZeroEdge*>& openEdgeList)
	{

		int mark;
		ZeroEdge *ptr;
		ZeroEdge *edge;

		mark = polyhedra.IncLRU();

		ZeroPolyhedra::Iterator iter (polyhedra);
		for (iter.Begin (); iter; iter ++) {
			edge = &(*iter)->info;

			if (edge->incidentFace > 0) {
				continue;
			}

			if (edge->mark != mark) {
				openEdgeList.Append(edge);
			}

			ptr = edge;
			do {
				ptr->mark = mark;
				ptr = ptr->next;
			} while (ptr != edge);
		}
	}


	static bool ConvexPartition (
		ZeroPolyhedra& flatPolyhedra,
		const float vertex[], 
		int strideInBytes,
		ZeroEdge* diagonalsOut[],
		ZeroList<LONGLONG>& deadEdges,
		int lastVertexIndex)
	{
		float dot;
		ZeroEdge *edge;
		
		_ASSERTE (flatPolyhedra.GetCount());

		ZeroPolyhedra::Iterator iter(flatPolyhedra);
		for (iter.Begin(); iter; iter ++) {
			edge = &(*iter)->info;
			edge->userData = edge->incidentVertex;
		}


		edge = &flatPolyhedra.GetRoot()->info;
		if (edge->incidentFace < 0) {
			edge = edge->twin;
		}
		_ASSERTE (edge->incidentFace > 0);



		ZeroVector4 normal (flatPolyhedra.FaceNormal(edge, vertex, strideInBytes));
		dot = DotProduct (normal, normal);
		_ASSERTE (dot > 1.0e-8f);
		normal = normal.Scale (rsqrt (dot));
		
		ZeroVisList<ZeroEdge*> openEdgeList;
		FindOpenEdges (flatPolyhedra, openEdgeList);
		while (openEdgeList.GetCount() > 1) {
			ZeroEdge *openFace0;
			ZeroEdge *openFace1;
			ZeroVisList<ZeroEdge*>::Iterator iter0 (openEdgeList);
			ZeroVisList<ZeroEdge*>::Iterator iter1 (openEdgeList);

			for (iter0.Begin(); iter0; iter0 ++) {
				openFace0 = *iter0;

				iter1.Set (iter0.GetNode());
				for (iter1 ++; iter1; iter1 ++) {
					openFace1 = *iter1;
					if (CombineOpenLoops (flatPolyhedra, openFace0, openFace1, vertex, strideInBytes, normal, lastVertexIndex)) {
						openEdgeList.Remove (iter1.GetNode());
						goto listIsReduced;
					}
				}
			}
			listIsReduced:;
			if (!iter0)	{
				return false;
			}
		}

		SimplePolygonConvexPartition (flatPolyhedra, normal, vertex, strideInBytes, diagonalsOut, deadEdges);
		return true;
	}




	static void ComplexConvexPartition (
		ZeroPolyhedra& flatPolyhedra,
		const float vertex[], 
		int strideInBytes,
		ZeroEdge* diagonalsOut[],
		ZeroList<LONGLONG>& deadEdgesOut,
		int lastVertexIndex)
	{
		ZeroVisList<LONGLONG> deadEdges;
		ZeroPolyhedra polyhedra (flatPolyhedra);

		if (ConvexPartition (polyhedra,	vertex, strideInBytes, diagonalsOut, deadEdges,	lastVertexIndex)) {
			ZeroVisList<LONGLONG>::Iterator iter (deadEdges);
			for (iter.Begin(); iter; iter ++) {
				deadEdgesOut.Append (*iter);
			}

			return;
		} else {
			int count;
			int mark;
			int edgeCount;
			ZeroEdge *ptr;
			ZeroEdge *ptr1;
			ZeroEdge *edge;
			ZeroEdge *startFace;


			polyhedra.RemoveAll();

			ZeroPolyhedraDescriptor	desc (&flatPolyhedra);

			ZeroStack<ZeroEdge*> edgeStack (flatPolyhedra + 100); 

			startFace = &flatPolyhedra.GetRoot()->info;
			if (startFace->incidentFace < 0) {
				startFace = startFace->twin; 
			}

			mark = flatPolyhedra.IncLRU ();
			edgeStack[0] = startFace;
			edgeCount = 1;
			edge = startFace;
			do	{
				edge->mark = mark;
				edge = edge ->next;
			}	while (edge != startFace);


			count = 0;
			while (edgeCount)	{
				edge = edgeStack[--edgeCount];

				count ++;
				if ((count * 2) >= desc.triangleCount) {
						break;
				}

				ptr = edge;
				do	{
					if (ptr->twin->incidentFace > 0)	{
						if (ptr->twin->mark != mark) {
							edgeStack[edgeCount++] = ptr->twin;
							_ASSERTE (edgeCount < (int)flatPolyhedra.GetCount()/2);
							ptr1 = ptr->twin;
							do	{
								ptr1 ->mark = mark;
								ptr1 = ptr1 ->next;
							} 	while (ptr1 != ptr->twin);
						}
					}
		
					ptr = ptr->next;
				} while (ptr != edge);
			}

			ZeroPolyhedra polyhedraA;
			ZeroPolyhedra polyhedraB;

			polyhedraA.BeginFace();
			polyhedraB.BeginFace();

			ZeroPolyhedra::Iterator iter (flatPolyhedra);

			for (iter.Begin(); iter; iter ++) {
				edge = &(*iter)->info;

				if (edge->incidentFace < 0) {
					continue;
				}

				if (edge->mark == mark) {
					polyhedraA.AddFace (edge->prev->incidentVertex, edge->incidentVertex, edge->next->incidentVertex);
				} else {
					polyhedraB.AddFace (edge->prev->incidentVertex, edge->incidentVertex, edge->next->incidentVertex);
				}
			}

			flatPolyhedra.RemoveAll();
			polyhedraA.EndFace();
			polyhedraB.EndFace();

			ComplexConvexPartition (polyhedraA,	vertex, strideInBytes, diagonalsOut, deadEdgesOut, lastVertexIndex);
			ComplexConvexPartition (polyhedraB,	vertex, strideInBytes, diagonalsOut, deadEdgesOut, lastVertexIndex);
		}
	}




	static int TriangleStrips (
		ZeroEdge *edge,
		int faceColor,
		unsigned outputBuffer[])
	{
		int count;

		_ASSERTE (edge->incidentFace > 0); 

		count = 0;
		outputBuffer[count] = edge->incidentVertex;
		count	++;

		outputBuffer[count] = edge->next->incidentVertex;
		count	++;

		for (;;)	{
			edge->mark = faceColor;
			edge->next->mark = faceColor;
			edge->prev->mark = faceColor;
			outputBuffer[count] = edge->prev->incidentVertex;
			count	++;

			edge = edge->next->twin;
			if (edge->mark >= faceColor) {
				break;
			}
			if (edge->incidentFace < 0) {
				edge->mark = faceColor;
				break;
			}


			edge->mark = faceColor;
			edge->next->mark = faceColor;
			edge->prev->mark = faceColor;
			outputBuffer[count] = edge->prev->incidentVertex;
			count	++;
			edge = edge->prev->twin;
			if (edge->mark >= faceColor) {
				break;
			}
			if (edge->incidentFace < 0) {
				edge->mark = faceColor;
				break;
			}
		} 

		return count;
	}


	static int CalculateStripLength (
		ZeroEdge *edge,
		int faceColor)
	{
		int count;

		_ASSERTE (edge->incidentFace > 0); 

		count	= 0;
		for (;;)	{
			edge->mark = faceColor;
			edge->next->mark = faceColor;
			edge->prev->mark = faceColor;
			count	++;

			edge = edge->next->twin;
			if (edge->mark >= faceColor) {
				break;
			}
			if (edge->incidentFace < 0) {
				edge->mark = faceColor;
				break;
			}


			edge->mark = faceColor;
			edge->next->mark = faceColor;
			edge->prev->mark = faceColor;
			count	++;
			edge = edge->prev->twin;
			if (edge->mark >= faceColor) {
				break;
			}
			if (edge->incidentFace < 0) {
				edge->mark = faceColor;
				break;
			}
		} 

		return count;
	}



	ZeroEdge* FindStripStart(ZeroVisList<ZeroEdge*>& faceList, int faceColor)
	{
		int i;
		int count1;
		int count2;
		int adjacents;
		int minAdjacents;
		ZeroEdge *edge;
		ZeroEdge *bestEdge;
		ZeroEdge *secondBestEdge;
		ZeroVisList<ZeroEdge*>::ZeroListNode *node;

		bestEdge = NULL;
		secondBestEdge	= NULL;
		ZeroVisList<ZeroEdge*>::Iterator iter(faceList);


		for (iter.Begin(); iter; ) {
			edge = *iter;
			node = iter.GetNode();
			iter ++;
			if (edge->mark != (faceColor + 2)) {
				edge->mark = faceColor-1;
			} else {
				faceList.Remove (node);
			}
		}

		minAdjacents = 10;
		for (iter.Begin(); iter; iter ++) {
			edge = *iter;

			_ASSERTE (edge->incidentFace > 0);
				
			adjacents = 0;
			for (i = 0; i < 3; i ++) {
				if (edge->twin->incidentFace > 0) {
					if (edge->twin->mark <= faceColor) {
						 adjacents ++;
					}
				}
				edge = edge->next;
			}
				
			if (adjacents >= 1) {
				if (adjacents < minAdjacents) {
					minAdjacents = adjacents;
					bestEdge = edge;
					if (minAdjacents == 1) {
						break;
					}
				}
			} else {
				secondBestEdge	= edge;
			}
		}

		if (!bestEdge) {
			return secondBestEdge;
		}
			
			
		if (bestEdge) {
			switch (minAdjacents) {
				case 1:
				{	
					if ((bestEdge->twin->incidentFace < 0) || (bestEdge->twin->mark > faceColor)) {
						bestEdge = bestEdge->next;
				  		if ((bestEdge->twin->incidentFace < 0) || (bestEdge->twin->mark > faceColor)) {
							bestEdge = bestEdge->next;
						}
					}
		
					count1 = CalculateStripLength (bestEdge->prev, faceColor + 1);
					count2 = CalculateStripLength (bestEdge->twin, faceColor + 1);
					if (count1 > count2) {
						bestEdge = bestEdge->prev;
					} else {
						bestEdge = bestEdge->twin;
					}
					break;
				}

				case 2:
				{
					if ((bestEdge->twin->incidentFace < 0) || (bestEdge->twin->mark > faceColor)) {
						bestEdge = bestEdge->next;
						if ((bestEdge->twin->incidentFace < 0) || (bestEdge->twin->mark > faceColor)) {
							bestEdge = bestEdge->next;
						}
					}
					break;
				}
			}
		}
		
	
		return bestEdge;
	}



	static int CalculateQuadLength (ZeroEdge **edgeOut, int faceColor)
	{
		int count;
		ZeroEdge* ptr;
		ZeroEdge* edge;

		edge = *edgeOut;
		_ASSERTE (edge->incidentFace > 0); 

		edge = edge->twin;
		ptr = edge;
		do { 
			if ((ptr->incidentFace < 0) || (ptr->mark == faceColor)) {
				break;
			}

			if (ptr->next->next->next == ptr) {
				ptr = ptr->next->twin;
			} else {
				ptr = ptr->next->next->twin;
			}
		} while (ptr != edge);
		edge = ptr->twin;
		*edgeOut = edge;

		count	= 0;
		ptr = edge;
		do { 
			if ((ptr->incidentFace < 0) || (ptr->mark == faceColor)) {
				break;
			}

			count	++;
			if (ptr->next->next->next == ptr) {
				ptr = ptr->next->twin;
			} else {
				ptr = ptr->next->next->twin;
			}
		} while (ptr != edge);

		return count;
	}


	static ZeroEdge* FindQuadStart(
		ZeroPolyhedra& polyhedra,
		int faceColor,
		ZeroList<ZeroEdge*>& edgeStart)
	{
		int length0;
		int length1;
		int adjacents;
		int minAdjacents;
		ZeroEdge *edge;
		ZeroEdge *bestEdge;

		bestEdge = NULL;
		if (edgeStart) {
			_ASSERTE (0);
//			bestEdge = edgeStart.GetFirst()->info;
//			edgeStart.Remove (edgeStart.GetFirst());
		} else {
			ZeroPolyhedra::Iterator iter(polyhedra);
			for (iter.Begin(); iter; iter ++) {
				edge = &(*iter)->info;
				if (edge->mark != (faceColor + 2)) {
					edge->mark = faceColor-1;
				}
			}

			minAdjacents = 100;
			for (iter.Begin(); iter; iter ++) {
				ZeroEdge *ptr;

				edge = &(*iter)->info;
				if (edge->mark >= faceColor) {
					continue;
				}

				if (edge->incidentFace < 0) {
					continue;
				}

				adjacents = 0;

				ptr = edge;
				do {
					ptr->mark = faceColor;
					if (ptr->twin->incidentFace > 0) {
						if (ptr->twin->mark <= faceColor) {
							 adjacents ++;
						}
					}
					ptr = ptr->next;
				} while (ptr != edge);

				if (adjacents < minAdjacents) {
					minAdjacents = adjacents;
					bestEdge = edge;
				}
			}


			if (bestEdge) {
				if (bestEdge->next->next->next != bestEdge) {
					edge = bestEdge->next;
					length0 = CalculateQuadLength (&bestEdge, faceColor + 2);
					length1 = CalculateQuadLength (&edge, faceColor + 2);
					if (length1	> length0) {
						bestEdge	= edge;
					}
				}
			}
		}

		return bestEdge;
	}


	static void TriangleQuadStrips (
		ZeroPolyhedra& polyhedra,
		ZeroEdge *edge,
		int faceColor,
		ZeroList<ZeroEdge*>& edgeStart)
	{
		int count;
		int v0;
		int v1;
		ZeroEdge *ptr;
		_ASSERTE (edge->incidentFace > 0); 

		for (;;)	{
			if (edge->mark == faceColor) {
				break;
			}

			if (edge->incidentFace < 0) {
				break;
			}

			count = 0;
			ptr = edge;
			do {
				count ++; 
				ptr->mark = faceColor;
				ptr = ptr->next;
			} while (edge != ptr);

			if (count == 3) {
				edge = edge->next->twin;
			} else {
				_ASSERTE (count == 4);
				ZeroEdge *newEdge;
				ZeroEdge *newTwin;

#if 0
				v0 = edge->incidentVertex; 
				v1 = edge->next->next->incidentVertex; 

				ptr = edge;
				edge = edge->next->next->twin;

				newEdge = polyhedra.AddEdge (v1, v0);
				if (newEdge) {
					newTwin = polyhedra.AddEdge (v0, v1);
					_ASSERTE (newTwin);

					newEdge->incidentFace = ptr->incidentFace; 
					newTwin->incidentFace = ptr->incidentFace; 
					
					newEdge->mark = faceColor;
					newTwin->mark = faceColor;
					
					newEdge->twin = newTwin;
					newTwin->twin = newEdge;

					newEdge->next = ptr;
					newEdge->prev = ptr->next;
					
					newTwin->next = ptr->prev->prev;
					newTwin->prev = ptr->prev;
					
					ptr->prev->prev->prev = newTwin;
					ptr->prev->next = newTwin;

					ptr->prev = newEdge;
					ptr->next->next = newEdge;
#else

				v0 = edge->next->incidentVertex; 
				v1 = edge->prev->incidentVertex; 

				ptr = edge;
				edge = edge->next->next->twin;

				newEdge = polyhedra.AddEdge (v0, v1);
				if (newEdge) {
					newTwin = polyhedra.AddEdge (v1, v0);
					_ASSERTE (newTwin);

					newEdge->incidentFace = ptr->incidentFace; 
					newTwin->incidentFace = ptr->incidentFace; 
					
					newEdge->mark = faceColor;
					newTwin->mark = faceColor;
					
					newEdge->twin = newTwin;
					newTwin->twin = newEdge;

					newEdge->next = ptr->prev;
					newEdge->prev = ptr;
					
					newTwin->next = ptr->next;
					newTwin->prev = ptr->prev->prev;
					
					ptr->prev->prev->next = newTwin;
					ptr->next->prev = newTwin;

					ptr->prev->prev = newEdge;
					ptr->next = newEdge;

#endif
				}
			}

//			if ((edge->prev->twin->incidentFace > 0) && (edge->prev->twin->mark != faceColor)){
//				edgeStart.Append (edge->prev->twin);
//			}
		}
	}

}



ZeroPolyhedraDescriptor::ZeroPolyhedraDescriptor (const ZeroPolyhedra *Polyhedra)
	:unboundedLoops ()
{
	Update (Polyhedra);
}

ZeroPolyhedraDescriptor::~ZeroPolyhedraDescriptor ()
{
}

void ZeroPolyhedraDescriptor::Update (const ZeroPolyhedra *polyhedraArg)
{
	int saveMark;
	int faceCountLocal;
	int edgeCountLocal;
	int vertexCountLocal;
	int triangleCountLocal;
	int maxVertexIndexLocal;
	ZeroEdge *ptr;
	ZeroEdge *edge;
	ZeroPolyhedra *polyhedra;

	polyhedra = (ZeroPolyhedra *)polyhedraArg;

	faceCountLocal = 0;
	edgeCountLocal = 0;
	vertexCountLocal	= 0;
	triangleCountLocal = 0;
	maxVertexIndexLocal = -1;

	saveMark = polyhedra->edgeMark;
	if (saveMark < 16) {
		saveMark	= 16;
	}
	polyhedra->edgeMark = 16;
	ZeroPolyhedra::Iterator iter(*polyhedra);
	for (iter.Begin(); iter; iter ++) {
		edge =  &(*iter)->info;
		edge->mark = 0;
		edgeCountLocal ++;
		if (edge->incidentVertex > maxVertexIndexLocal) {
			maxVertexIndexLocal = edge->incidentVertex;
		}
	}

	unboundedLoops.RemoveAll();
	for (iter.Begin(); iter; iter ++) {
		edge = &(*iter)->info;

		if (edge->incidentFace < 0) {
			if (~edge->mark & 1) {
				unboundedLoops.Append (edge);
				ptr = edge;
				do {
					ptr->mark |= 1;
					ptr = ptr->next;
				} while (ptr != edge);
			}
		} else {
			if (~edge->mark & 2) {
				ptr = edge;
				triangleCountLocal -= 2; 
				do {
					triangleCountLocal ++; 
					ptr->mark |= 2;
					ptr = ptr->next;
				} while (ptr != edge);
			}
		}

		if (~edge->mark & 4) {
			vertexCountLocal ++; 
			ptr = edge;
			do {
				ptr->mark |= 4;
				ptr = ptr->twin->next;
			} while (ptr != edge);
		}

		if (~edge->mark & 8) {
			ptr = edge;
			faceCountLocal ++; 
			do {
				ptr->mark |= 8;
				ptr = ptr->next;
			} while (ptr != edge);
		}
	}

	edgeCount = edgeCountLocal;
	faceCount = faceCountLocal;
	vertexCount = vertexCountLocal;
	triangleCount = triangleCountLocal;  
	maxVertexIndex = maxVertexIndexLocal + 1;

	polyhedra->edgeMark = saveMark;
}

ZeroPolyhedra::ZeroPolyhedra ()
	:ZeroVisTree <ZeroEdge, LONGLONG>()
{
	edgeMark = 0;
	faceSecuence = 0;
}

ZeroPolyhedra::ZeroPolyhedra (const ZeroPolyhedra &polyhedra)
	:ZeroVisTree <ZeroEdge, LONGLONG>()
{
	ZeroEdge* ptr;
	ZeroEdge* edge;
	int indexCount;
	ZeroStack<int> face(1024 * 16);
	ZeroStack<unsigned> userData(1024 * 16);

	edgeMark = 0;
	faceSecuence = 0;

	BeginFace();
	Iterator iter(polyhedra);
	for (iter.Begin(); iter; iter ++) {
		edge = &(*iter)->info;
		if (edge->incidentFace > 0) {
			if (!FindEdge (edge->incidentVertex, edge->twin->incidentVertex)) {
				ptr = edge;
				indexCount = 0;
				do {
					face[indexCount] = ptr->incidentVertex;
					userData[indexCount]	= ptr->userData;
					indexCount ++;
					ptr = ptr->next;
				} while (ptr != edge);
				AddFace (indexCount, &face[0], &userData[0]);
			}
		}
	}
	EndFace();

#ifdef __ENABLE_SANITY_CHECK 
	_ASSERTE (SanityCheck ());
#endif
}


ZeroPolyhedra::~ZeroPolyhedra ()
{
}

void ZeroPolyhedra::DeleteAllFace()
{
	edgeMark = 0;
	faceSecuence = 0;
	RemoveAll();
}


bool ZeroPolyhedra::SanityCheck () const
{
	int i;
	int coorCount;
	ZeroEdge *ptr;
	ZeroEdge *edge;
	ZeroTreeNode *node;
	ZeroStack<int> coor(65536);

	Iterator iter (*this);
	for (iter.Begin(); iter; iter ++) {
		node = *iter;
		if (!node->IsAlive()) {
			return false;
		}

		edge = &(*iter)->info;

		if ((edge->incidentFace < 0) && (edge->twin->incidentFace < 0)) {
			return false;
		}


		if (edge->mark > edgeMark) {
			return false;
		}

		node = *iter;
		InternalPolyhedra::PairKey key (edge->incidentVertex, edge->twin->incidentVertex);
		if (key.val != node->key) {
			return false;
		}

		ptr = edge;
		do {
			if (ptr->incidentVertex != edge->incidentVertex) {
				return false;
			}
			ptr = ptr->twin->next;
		} while (ptr != edge);

		ptr = edge;
		coorCount = 0;
		do {
			if ((coorCount * sizeof (int))> coor.SizeInBytes()) {
				return false;
			}
			if (ptr->incidentFace != edge->incidentFace) {
//				return false;
			}
			coor [coorCount] = ptr->incidentVertex;
			coorCount ++;

			ptr = ptr->next;
		} while (ptr != edge);

		ptr = edge->prev;
		i = 0;
		do {
			if ((i * sizeof (int))> coor.SizeInBytes()) {
				return false;
			}
			if (ptr->incidentFace != edge->incidentFace) {
//				return false;
			}

			if (coor [coorCount - i - 1] != ptr->incidentVertex) {
				return false;
			}

			i ++;
			ptr = ptr->prev;
		} while (ptr != edge->prev);

		if (edge->twin->twin != edge) {
			return false;
		}

		if (edge->prev->next != edge) {
			return false;
		}

		if (edge->next->prev != edge) {
			return false;
		}
	}

	return ZeroVisTree <ZeroEdge, LONGLONG>::SanityCheck();
}


void ZeroPolyhedra::BeginFace ()
{
}

void ZeroPolyhedra::EndFace ()
{
	InternalPolyhedra::MatchTwins (this);
	InternalPolyhedra::CloseOpenBounds (this);
}


ZeroEdge* ZeroPolyhedra::AddFace (
	int count, 
	int *index)
{
	int i;
	int i0;
	int i1;
	ZeroTreeNode *node;
	ZeroEdge *edge0;
	ZeroEdge *edge1;
	ZeroEdge *first;

	i0 = index[count-1];
	for (i = 0; i < count; i ++) {
		i1 = index[i];
		if (i0 == i1) {
			return NULL;
		}
		if (FindEdge (i0, i1)) {
			return NULL;
		}
		i0 = i1;
	}

	faceSecuence ++;

	i0 = index[count-1];
	i1 = index[0];
	InternalPolyhedra::PairKey code (i0, i1);
	ZeroEdge tmpEdge (i0, faceSecuence);
	node = Insert (tmpEdge, code.val); 
	edge0 = &node->info;
	first = edge0;

	for (i = 1; i < count; i ++) {
		i0 = i1;
		i1 = index[i];
		InternalPolyhedra::PairKey code (i0, i1);
		ZeroEdge tmpEdge (i0, faceSecuence);
		node = Insert (tmpEdge, code.val); 
		edge1 = &node->info;

		edge0->next	= edge1;
		edge1->prev	= edge0;
		edge0 = edge1;
	}

	first->prev = edge0;
	edge0->next = first;

	return first->next;
}


void ZeroPolyhedra::DeleteFace(ZeroEdge *edge)
{
	int i;
	int count;
	ZeroEdge* ptr;
	InternalPolyhedra::PairKey keys[512];

	if (edge->incidentFace < 0) {
		return;
	}

	count = 0;
	ptr = edge;
	do {
		ptr->incidentFace = -1;
		if (ptr->twin->incidentFace == -1) {
			keys[count] = InternalPolyhedra::PairKey (ptr->incidentVertex, ptr->twin->incidentVertex);
			count	++;
		}
		ptr = ptr->next;
	} while (ptr != edge);

	for (i = 0; i < count; i ++) {
		edge = FindEdge (keys[i].fields.i0, keys[i].fields.i1);
		if (edge) { 
			_ASSERTE (edge->next->incidentFace < 0);
			DeleteEdge (edge);
		}
	}


#ifdef __ENABLE_SANITY_CHECK 
	_ASSERTE (SanityCheck ());
#endif

}


ZeroEdge *ZeroPolyhedra::FindEdge (int i0, int i1)	const
{
	ZeroTreeNode *node;

	InternalPolyhedra::PairKey key (i0, i1);
	node = Find (key.val);
	return node ? &node->info : NULL;
}


ZeroEdge* ZeroPolyhedra::AddEdge (int v0, int v1)
{
	ZeroTreeNode *node;

	InternalPolyhedra::PairKey  pairKey (v0, v1);
	ZeroEdge tmpEdge (v0, -1);

	node = Insert (tmpEdge, pairKey.val); 
//	_ASSERTE (node);
	return node ? &node->info : NULL;
}

bool ZeroPolyhedra::FlipEdge (ZeroEdge *edge)
{
	ZeroEdge *prevEdge;
	ZeroEdge *prevTwin;
	ZeroTreeNode *node;

	if (edge->next->next->next != edge) {
		return false;
	}

	if (edge->twin->next->next->next != edge->twin) {
		return false;
	}


	if (FindEdge(edge->prev->incidentVertex, edge->twin->prev->incidentVertex)) {
		return false;
	}

	prevEdge = edge->prev;
	prevTwin = edge->twin->prev;

	InternalPolyhedra::PairKey edgeKey (prevTwin->incidentVertex, prevEdge->incidentVertex);
	InternalPolyhedra::PairKey twinKey (prevEdge->incidentVertex, prevTwin->incidentVertex);

	node = ReplaceKey (GetNodeFromInfo (*edge), edgeKey.val);
	_ASSERTE (node);

	node = ReplaceKey (GetNodeFromInfo (*edge->twin), twinKey.val);
	_ASSERTE (node);

	edge->incidentVertex = prevTwin->incidentVertex;
	edge->userData = prevTwin->userData;

	edge->twin->incidentVertex = prevEdge->incidentVertex;
	edge->twin->userData = prevEdge->userData;

	prevEdge->next = edge->twin->next;
	prevTwin->prev->prev = edge->prev;

	prevTwin->next = edge->next;
	prevEdge->prev->prev = edge->twin->prev;

	edge->prev = prevTwin->prev;
	edge->next = prevEdge;

	edge->twin->prev = prevEdge->prev;
	edge->twin->next = prevTwin;

	prevTwin->prev->next = edge;
	prevTwin->prev = edge->twin;

	prevEdge->prev->next = edge->twin;
	prevEdge->prev = edge;

	edge->next->incidentFace = edge->incidentFace;
	edge->prev->incidentFace = edge->incidentFace;

	edge->twin->next->incidentFace = edge->twin->incidentFace;
	edge->twin->prev->incidentFace = edge->twin->incidentFace;


#ifdef __ENABLE_SANITY_CHECK 
	_ASSERTE (SanityCheck ());
#endif

	return true;
}




void ZeroPolyhedra::DeleteEdge (ZeroEdge *edge)
{
	ZeroEdge *twin;

	twin = edge->twin;

	edge->prev->next = twin->next;
	twin->next->prev = edge->prev;
	edge->next->prev = twin->prev;
	twin->prev->next = edge->next;

	Remove (GetNodeFromInfo(*edge));
	Remove (GetNodeFromInfo(*twin));
}


void ZeroPolyhedra::DeleteEdge (int v0, int v1)
{
	ZeroEdge *edge;
	ZeroTreeNode *node;

	InternalPolyhedra::PairKey pairKey (v0, v1);
	node = Find(pairKey.val);
	edge = node ? &node->info : NULL;
	if (!edge) {
		return;
	}
	DeleteEdge (edge);
}


ZeroEdge *ZeroPolyhedra::CollapseEdge(ZeroEdge *edge)
{
	int v0; 
	int v1;

	ZeroEdge* ptr;
	ZeroEdge *twin;
	ZeroEdge *retEdge;
	ZeroTreeNode *node;

	ZeroEdge *badEdge;
	ZeroEdge *lastEdge;
	ZeroEdge *firstEdge;


	v0 = edge->incidentVertex;
	v1 = edge->twin->incidentVertex;


#ifdef __ENABLE_SANITY_CHECK 
	InternalPolyhedra::PairKey TwinKey (v1, v0);
	node = Find (TwinKey.val);
	twin = node ? &node->info : NULL;
	_ASSERTE (twin);
	_ASSERTE (edge->twin == twin);
	_ASSERTE (twin->twin == edge);
	_ASSERTE (edge->incidentFace != 0);
	_ASSERTE (twin->incidentFace != 0);
#endif

	retEdge = edge->twin->prev->twin;
	if (retEdge	== edge->twin->next) {
		return NULL;
	}
	if (retEdge	== edge->twin) {
		return NULL;
	}
	if (retEdge	== edge->next) {
		retEdge = edge->prev->twin;
		if (retEdge	== edge->twin->next) {
			return NULL;
		}
	 	if (retEdge	== edge->twin) {
			return NULL;
		}
	}

	lastEdge	= NULL;
	firstEdge = NULL;
	if ((edge->incidentFace >= 0)	&& (edge->twin->incidentFace >= 0)) {	
		lastEdge	= edge->prev->twin;
		firstEdge = edge->twin->next->twin->next;
	} else if (edge->twin->incidentFace >= 0) {
		firstEdge = edge->twin->next->twin->next;
		lastEdge	= edge;
	} else {
		lastEdge	= edge->prev->twin;
		firstEdge = edge->twin->next;
	}

	for (ptr = firstEdge; ptr != lastEdge; ptr = ptr->twin->next) {
		badEdge = FindEdge (edge->twin->incidentVertex, ptr->twin->incidentVertex);
		if (badEdge) {
			return NULL;
		}
	} 


	twin = edge->twin;
	if (twin->next == twin->prev->prev) {
		twin->prev->twin->twin = twin->next->twin;
		twin->next->twin->twin = twin->prev->twin;

		InternalPolyhedra::RemoveHalfEdge (this, twin->prev);
		InternalPolyhedra::RemoveHalfEdge (this, twin->next);
	} else {
		twin->next->prev = twin->prev;
		twin->prev->next = twin->next;
	}

	if (edge->next == edge->prev->prev) {
		edge->next->twin->twin = edge->prev->twin;
		edge->prev->twin->twin = edge->next->twin;
		InternalPolyhedra::RemoveHalfEdge (this, edge->next);
		InternalPolyhedra::RemoveHalfEdge (this, edge->prev);
	} else {
		edge->next->prev = edge->prev;
		edge->prev->next = edge->next;
	}

	_ASSERTE (twin->twin->incidentVertex == v0);
	_ASSERTE (edge->twin->incidentVertex == v1);
	InternalPolyhedra::RemoveHalfEdge (this, twin, v0);
	InternalPolyhedra::RemoveHalfEdge (this, edge, v1);

	edge = retEdge;
	do {
		InternalPolyhedra::PairKey pairKey (v0, edge->twin->incidentVertex);
		node = Find (pairKey.val);
		if (node) {
			if (&node->info == edge) {
				InternalPolyhedra::PairKey key (v1, edge->twin->incidentVertex);
				edge->incidentVertex = v1;
				node = ReplaceKey (node, key.val);
				_ASSERTE (node);
			} 
		}

		InternalPolyhedra::PairKey TwinKey (edge->twin->incidentVertex, v0);
		node = Find (TwinKey.val);
		if (node) {
			if (&node->info == edge->twin) {
				InternalPolyhedra::PairKey key (edge->twin->incidentVertex, v1);
				node = ReplaceKey (node, key.val);
				_ASSERTE (node);
			}
		}

		edge = edge->twin->next;
	} while (edge != retEdge);

	return retEdge;
}



ZeroVector3 ZeroPolyhedra::FaceNormal (
	ZeroEdge *face, 
	const float array[], 
	int strideInBytes) const
{
	int stride = strideInBytes / sizeof (float);

#if 0
	if (face->next->next->next == face) {
		ZeroVector3 p0 (&array[face->incidentVertex * stride]);
		ZeroVector3 p1 (&array[face->next->incidentVertex * stride]);
		ZeroVector3 p2 (&array[face->prev->incidentVertex * stride]);
		return CrossProduct((p1 - p0), (p2 - p0));
	}
#endif

	ZeroEdge *edge;
	ZeroVector3 normal (0, 0, 0);
	edge = face;
	ZeroVector3 p0(&array[edge->incidentVertex * stride]);
	edge = edge->next;
	ZeroVector3 p1(&array[edge->incidentVertex * stride]);
	edge = edge->next;
	while (edge != face) {
		ZeroVector3 p2(&array[edge->incidentVertex * stride]);
		normal += CrossProduct(p1 - p0, p2 - p0);
		p1 = p2;
		edge = edge->next;
	}
	ZeroWarn((MagnitudeSq(normal) > EPSILON * EPSILON, "Degenerate face!"));

	return normal;
}

void ZeroPolyhedra::SpliteFace (
	int newIndex,
	ZeroEdge *face)
{
	unsigned i;
	unsigned i0;
	unsigned i1;
	unsigned i2;
	unsigned index;
	ZeroEdge *ptr;
	ZeroEdge *edge;
	ZeroEdge *twin;
	ZeroEdge *array[512];
	ZeroEdge *poly[526];

	index = 0;
	ptr = face; 
	do {
		poly [index] = ptr;
		edge = AddEdge (newIndex, ptr->incidentVertex);
		twin = AddEdge (ptr->incidentVertex, newIndex);

		edge->twin = twin;
		twin->twin = edge;

		edge->incidentFace = face->incidentFace;
		twin->incidentFace = face->incidentFace;

		array[index * 2 + 0] = edge;
		array[index * 2 + 1] = twin;

		index ++;
		ptr = ptr->next;
	} while (ptr != face);

	for (i = 0; i < index; i ++) {
		i0 = (i + index - 1) % index;
		i1 = (i + index + 0) % index;
		i2 = (i + index + 1) % index;

		edge = array[i1 * 2 + 0];
		edge->prev = array[i2 * 2 + 1];
		edge->next = poly[i1];
		array[i2 * 2 + 1]->next = edge;
		poly[i1]->prev = edge;

		twin = array[i1 * 2 + 1];
		twin->next = array[i0 * 2 + 0];
		twin->prev = poly[i0];

		array[i0 * 2 + 0]->prev = twin;
		poly[i0]->next = twin;
	}

#ifdef __ENABLE_SANITY_CHECK 
	_ASSERTE (SanityCheck ());
#endif
}



void ZeroPolyhedra::SpliteEdge (
	int newIndex,
	ZeroEdge *edge,
	ZeroEdge **edgein,
	ZeroEdge **edgeout)
{
	int i0;
	int i1;
	int f0;
	int f1;
	ZeroEdge* edge0;
	ZeroEdge* twin0;
	ZeroEdge* edge1;
	ZeroEdge* twin1;

	ZeroEdge* edge00;
	ZeroEdge* edge01;
	ZeroEdge* twin00;
	ZeroEdge* twin01;

	edge00 = edge->prev;
	edge01 = edge->next;
	twin00 = edge->twin->next;
	twin01 = edge->twin->prev;
 
	i0 = edge->incidentVertex;
	i1 = edge->twin->incidentVertex;

	f0 = edge->incidentFace;
	f1 = edge->twin->incidentFace;

	DeleteEdge (edge);

	edge0 = AddEdge (i0, newIndex);
	edge1 = AddEdge (newIndex, i1);

	twin0 = AddEdge (newIndex, i0);
	twin1 = AddEdge (i1, newIndex);
	_ASSERTE (edge0);
	_ASSERTE (edge1);
	_ASSERTE (twin0);
	_ASSERTE (twin1);

	edge0->twin = twin0;
	twin0->twin = edge0;

	edge1->twin = twin1;
	twin1->twin = edge1;

	edge0->next = edge1;
	edge1->prev = edge0;

	twin1->next = twin0;
	twin0->prev = twin1;

	edge0->prev = edge00;
	edge00->next = edge0;

	edge1->next = edge01;
	edge01->prev = edge1;

	twin0->next = twin00;
	twin00->prev = twin0;

	twin1->prev = twin01;
	twin01->next = twin1;

	edge0->incidentFace = f0;
	edge1->incidentFace = f0;

	twin0->incidentFace = f1;
	twin1->incidentFace = f1;

	if (edgein)	{
		*edgein = edge0;
	}
	if (edgeout) {
		*edgeout = edge1;
	}

#ifdef __ENABLE_SANITY_CHECK 
//	_ASSERTE (SanityCheck ());
#endif
}


void ZeroPolyhedra::SpliteEdgeAndTriangulate (
	int newIndex,
	ZeroEdge *edge,
	ZeroEdge **edgein,
	ZeroEdge **edgeout)
{
	ZeroEdge *edge0;
	ZeroEdge *twin0;
	ZeroEdge *ancle;
	ZeroEdge *faceEdge;

	ancle = edge->next;

	SpliteEdge (newIndex, edge, edgein, edgeout);

	edge = ancle->prev;
	ancle = edge;
	do {
		faceEdge = edge->twin;
		if (faceEdge->incidentFace > 0)	{
			if (faceEdge->next->next->next != faceEdge) {

				edge0 = AddEdge (newIndex, faceEdge->prev->incidentVertex);
				twin0 = AddEdge (faceEdge->prev->incidentVertex, newIndex);

				twin0->incidentFace = faceEdge->incidentFace;
				faceEdge->prev->incidentFace = faceSecuence;
				faceEdge->incidentFace = faceSecuence;
				edge0->incidentFace = faceSecuence;
				faceSecuence ++;

				edge0->twin = twin0;
				twin0->twin = edge0;

				twin0->next = faceEdge->next;
				faceEdge->next->prev = twin0;

				twin0->prev = faceEdge->prev->prev;
				faceEdge->prev->prev->next = twin0;

				edge0->prev = faceEdge;
				faceEdge->next = edge0;

				edge0->next = faceEdge->prev;
				faceEdge->prev->prev = edge0;
			}
		}

		edge = edge->twin->next;
	} while (edge != ancle);


#ifdef __ENABLE_SANITY_CHECK 
	_ASSERTE (SanityCheck ());
#endif 
}



ZeroSphere ZeroPolyhedra::CalculateSphere (
	const float vertex[], 
	int strideInBytes, 
	const ZeroMatrix *basis)
{
	int fCount;
	ZeroSphere sphere;

	ZeroStack<ZeroFace> faceList (GetCount() / 2 + 1024); 

	fCount = WireMeshing (&faceList[0], vertex, strideInBytes, false);

	_ASSERTE (fCount < ((int)GetCount() / 2 + 1024));
	sphere.SetDimensions (vertex, strideInBytes, &faceList[0], fCount, basis);
	return sphere;
}



bool ZeroPolyhedra::IsEdgeCollinear (
	ZeroEdge *edge, 
	const float pool[],
	int strideInBytes) const
{
	int i0;
	int i1;
	int i2;
	float val;
	int stride;

	stride = strideInBytes / sizeof (float);


	i0 = edge->prev->incidentVertex;
	i1 = edge->incidentVertex;
	i2 = edge->next->incidentVertex;

	ZeroVector3 p0 (&pool[i0 * stride]);
	ZeroVector3 p1 (&pool[i1 * stride]);
	ZeroVector3 p2 (&pool[i2 * stride]);
	ZeroVector3 e0 (p1 - p0);
	val = DotProduct(e0, e0);
	if (val < (1.0e-5f * 1.0e-5f))	{
		return true;
	}

	e0 = e0 * (1.0f / sqrtf(val));

	ZeroVector3 e1 (p2 - p1);
	val = DotProduct(e1, e1);
	if (val < (1.0e-5f * 1.0e-5f))	{
		return true;
	}
	e1 = e1 * (1.0f / sqrtf(val));

	ZeroVector3 normal (CrossProduct(e0, e1));
	val = DotProduct(normal, normal);
	// sin (5.0) degree2
	return val < (0.087 * 0.087); 
}

void ZeroPolyhedra::PackVertex (
	ZeroPolyhedraDescriptor &desc,
	void* const destArray__, 
	void* const unpackArray__, 
	int strideInBytes)
{
	int i;
	int m;
	int n;
	int edgeCount;
	int vertexCount;
	int mark;
	ZeroEdge *ptr;
	ZeroEdge *edge;
	ZeroTreeNode *node;
	ZeroTreeNode **tree;
	char* destArray; 
	char* unpackArray; 

	destArray = (char*)destArray__, 	  
	unpackArray = (char*) unpackArray__,  

	mark = IncLRU();

	ZeroStack<ZeroTreeNode *> treePool(desc.edgeCount);
	tree = &treePool[0];

	edgeCount = 0;
	vertexCount = 0;
	Iterator iter (*this);
	for (iter.Begin(); iter; ) {
		node = *iter;
		iter ++;

		tree[edgeCount] = node;
		node->AddRef();
		Remove (node);

		_ASSERTE (edgeCount < desc.edgeCount);
		edgeCount ++;

		edge = &node->info;
		if (edge->mark != mark) {

			ptr = edge;
			m = edge->incidentVertex * strideInBytes;
			n = vertexCount * strideInBytes;
			memmove (&destArray[n], &unpackArray[m], strideInBytes);
			do {
				ptr->mark = mark;
				ptr->incidentVertex = vertexCount;
				ptr = ptr->twin->next;

			} while (ptr != edge);
			vertexCount ++;
		}	
	}

	for (i = 0; i < edgeCount; i ++) {
		node = tree[i];
		node->Unkill();

		edge = &node->info;
		InternalPolyhedra::PairKey key(edge->incidentVertex, edge->twin->incidentVertex);
		Insert (node, key.val);
		node->Release();
	}
	desc.maxVertexIndex = desc.vertexCount;

#ifdef __ENABLE_SANITY_CHECK 
	ZeroPolyhedraDescriptor checkDesc (this);
	_ASSERTE (checkDesc.maxVertexIndex == desc.maxVertexIndex); 
	_ASSERTE (checkDesc.vertexCount == desc.vertexCount); 
	_ASSERTE (checkDesc.faceCount == desc.faceCount); 
	_ASSERTE (checkDesc.edgeCount == desc.edgeCount); 
	_ASSERTE (checkDesc.unboundedLoops.GetCount() == desc.unboundedLoops.GetCount()); 
	_ASSERTE (SanityCheck ());
#endif
}

void ZeroPolyhedra::PackVertex (
	ZeroPolyhedraDescriptor &desc,
	float destArray[], 
	int dstStrideInBytes,
	const float unpackArray[], 
	int srcStrideInBytes)
{
	int i;
	int m;
	int n;
	int edgeCount;
	int vertexCount;
	int mark;
	ZeroEdge *ptr;
	ZeroEdge *edge;
	ZeroTreeNode *node;
	ZeroTreeNode **tree;

	mark = IncLRU();

	dstStrideInBytes /= sizeof (float);
	srcStrideInBytes /= sizeof (float);


	ZeroStack<ZeroTreeNode *> treePool(desc.edgeCount);
	tree = &treePool[0];

	edgeCount = 0;
	vertexCount = 0;
	Iterator iter (*this);
	for (iter.Begin(); iter; ) {
		node = *iter;
		iter ++;

		tree[edgeCount] = node;
		node->AddRef();
		Remove (node);

		_ASSERTE (edgeCount < desc.edgeCount);
		edgeCount ++;

		edge = &node->info;
		if (edge->mark != mark) {

			ptr = edge;
			m = edge->incidentVertex * srcStrideInBytes;
			n = vertexCount * dstStrideInBytes;
			destArray[n + 0] = unpackArray[m + 0];
			destArray[n + 1] = unpackArray[m + 1];
			destArray[n + 2] = unpackArray[m + 2];
			do {
				ptr->mark = mark;
				ptr->incidentVertex = vertexCount;
				ptr = ptr->twin->next;

			} while (ptr != edge);
			vertexCount ++;
		}	
	}

	for (i = 0; i < edgeCount; i ++) {
		node = tree[i];
		node->Unkill();

		edge = &node->info;
		InternalPolyhedra::PairKey key(edge->incidentVertex, edge->twin->incidentVertex);
		Insert (node, key.val);
		node->Release();
	}
	desc.maxVertexIndex = desc.vertexCount;

#ifdef __ENABLE_SANITY_CHECK 
	ZeroPolyhedraDescriptor checkDesc (this);
	_ASSERTE (checkDesc.maxVertexIndex == desc.maxVertexIndex); 
	_ASSERTE (checkDesc.vertexCount == desc.vertexCount); 
	_ASSERTE (checkDesc.faceCount == desc.faceCount); 
	_ASSERTE (checkDesc.edgeCount == desc.edgeCount); 
	_ASSERTE (checkDesc.unboundedLoops.GetCount() == desc.unboundedLoops.GetCount()); 
	_ASSERTE (SanityCheck ());
#endif
}








int ZeroPolyhedra::WireMeshing (
	ZeroFace* const face, 
	const float vertex[], 
	int strideInBytes,
	bool useOptimalTrangulation)
{
	int mark; 
	int fCount; 
	ZeroEdge *edge;


	ZeroPolyhedra tmpPoly (*this);

	if (useOptimalTrangulation) {
		tmpPoly.OptimalTriangulation (vertex, strideInBytes);
	} else {
		tmpPoly.Triangulate (vertex, strideInBytes);
	}

	mark = tmpPoly.IncLRU();

	fCount = 0; 
	Iterator iter (tmpPoly);
	for (iter.Begin(); iter; iter ++) {
		edge = &(*iter)->info;

		if (edge->mark == mark) {
			continue;
		}
		if (edge->incidentFace < 0) {
			continue;
		}

	   	edge->mark = mark;
	   	edge->next->mark = mark;
	   	edge->prev->mark = mark;
				
		face[fCount].index[0] = edge->incidentVertex;
		face[fCount].index[1] = edge->next->incidentVertex;
		face[fCount].index[2] = edge->prev->incidentVertex;
		fCount ++; 
	}
	return fCount;
}


void ZeroPolyhedra::SavexMesh (
	const char *name,
	const float pool[], 
	int strideInBytes) const
{
	ZeroPolyhedra tmp (*this);
	ZeroPolyhedraDescriptor desc (&tmp);

	ZeroStack<ZeroFace> faceArray(desc.edgeCount + 1024); 
	ZeroStack<ZeroVector3> vertexPool(desc.vertexCount); 

	tmp.PackVertex (desc, &vertexPool[0].x, sizeof (ZeroVector3), pool, strideInBytes);

	tmp.WireMeshing (&faceArray[0], &vertexPool[0].x, sizeof (ZeroVector3), true);

	ZeroSavexMesh (name, 
				   &vertexPool[0].x, sizeof (ZeroVector3), desc.vertexCount, 
				   &faceArray[0], desc.triangleCount);
}




void ZeroPolyhedra::Optimize (
	const float array[], 
	int strideInBytes, 
	float tol)
{
	int mark;
	int index0;
	int index1;
	int stride;
	int edgeCount;
	int vertexCount;
	const int edgePadding = 1024 * 64;
	float tol2;
	double cost;
	double validateCost;
	ZeroEdge *ptr;
	ZeroEdge *edge;
	ZeroVisList <InternalPolyhedra::EDGE_HANDLE>::ZeroListNode *handleNodePtr;

	if (!GetCount())
		return;

	stride = strideInBytes / sizeof (float);

#ifdef __ENABLE_SANITY_CHECK 
	_ASSERTE (SanityCheck ());
#endif

	ZeroPolyhedraDescriptor desc (this); 
//	faceCount = desc.faceCount;
	edgeCount = desc.edgeCount + edgePadding;
	vertexCount = desc.maxVertexIndex;

	ZeroStack<ZeroVector3> vertexPool (vertexCount); 
	ZeroStack<InternalPolyhedra::VERTEX_METRIC_STRUCT> vertexMetrics (vertexCount + 512); 

	ZeroVisList <InternalPolyhedra::EDGE_HANDLE> edgeHandleList;
	ZeroStack<char> heapPool (edgeCount * (sizeof (double) + sizeof (InternalPolyhedra::EDGE_HANDLE*) + sizeof (int))); 
	ZeroHeap <ZeroVisList <InternalPolyhedra::EDGE_HANDLE>::ZeroListNode* , double> bigHeapArray(edgeCount, InternalPolyhedra::MAX_COST * 256.0, &heapPool[0]);

	InternalPolyhedra::NormalizeVertex (vertexCount, &vertexPool[0], array, stride);

	memset (&vertexMetrics[0], 0, vertexCount * sizeof (InternalPolyhedra::VERTEX_METRIC_STRUCT));
	InternalPolyhedra::CalculateAllMetrics (this, &vertexMetrics[0], &vertexPool[0]);


	tol2 = tol * tol;
	Iterator iter (*this);
	for (iter.Begin(); iter; iter ++) {
		edge = &(*iter)->info;

	  	index0 = edge->incidentVertex;
  		index1 = edge->twin->incidentVertex;
  
	  	InternalPolyhedra::VERTEX_METRIC_STRUCT &metric = vertexMetrics[index0];
		ZeroVector3 p	(&vertexPool[index1].x);
		cost = metric.Evalue (p); 
		cost += InternalPolyhedra::EdgePenalty (&vertexPool[0], edge) * tol2;

		if (cost	< tol2) {
	  		_ASSERTE ((InternalPolyhedra::MAX_COST - cost) > 0.0);

			InternalPolyhedra::EDGE_HANDLE handle (edge);

			edgeHandleList.Prepend (handle);

			handleNodePtr = edgeHandleList.GetFirst();
			bigHeapArray.Push (handleNodePtr, InternalPolyhedra::MAX_COST - cost);
		}
	}

	while (bigHeapArray.GetCount()) {
	  	// collapse this edge
		cost = InternalPolyhedra::MAX_COST - bigHeapArray.Value();
		handleNodePtr = bigHeapArray[0];

		edge = handleNodePtr->info.edge;
  		bigHeapArray.Pop();
		edgeHandleList.Remove (handleNodePtr);

		if (edge) {

			InternalPolyhedra::CalculateVertexMetrics (this, &vertexMetrics[0], &vertexPool[0], edge);

			index0 = edge->incidentVertex;
			index1 = edge->twin->incidentVertex;
			InternalPolyhedra::VERTEX_METRIC_STRUCT &metric = vertexMetrics[index0];
			ZeroVector3 p	(&vertexPool[index1].x);
			validateCost = metric.Evalue (p); 
			validateCost += InternalPolyhedra::EdgePenalty (&vertexPool[0], edge) * tol2; 

			if (fabs (validateCost - cost) < 1.0e-5f) {
#ifdef __ENABLE_SANITY_CHECK 
				_ASSERTE (SanityCheck ());
#endif

	  			edge = CollapseEdge(edge);

#ifdef __ENABLE_SANITY_CHECK 
				_ASSERTE (SanityCheck ());
#endif

  				if (edge) {
					
  					// Update vertex metrics
					InternalPolyhedra::CalculateVertexMetrics (this, &vertexMetrics[0], &vertexPool[0], edge);
					
  					// Update metrics for all surrounding vertex
  					ptr = edge;
  					do {
						InternalPolyhedra::CalculateVertexMetrics (this, &vertexMetrics[0], &vertexPool[0], ptr->twin);
  						ptr = ptr->twin->next;
  					} while (ptr != edge);
					
  					// calculate edge cost of all incident edges
					mark = IncLRU();
  					ptr = edge;
  					do {
						_ASSERTE (ptr->mark != mark);
						ptr->mark = mark;
					
  						index0 = ptr->incidentVertex;
  						index1 = ptr->twin->incidentVertex;
						
						InternalPolyhedra::VERTEX_METRIC_STRUCT &metric = vertexMetrics[index0];
  						ZeroVector3 p (&vertexPool[index1].x);
						
  	 					cost = metric.Evalue (p); 
						cost += InternalPolyhedra::EdgePenalty (&vertexPool[0], ptr) * tol2; 

						
						if (cost	< tol2) {
							_ASSERTE ((InternalPolyhedra::MAX_COST - cost) > 0.0);
							InternalPolyhedra::EDGE_HANDLE handle (ptr);
							edgeHandleList.Prepend (handle);
							handleNodePtr = edgeHandleList.GetFirst();
							bigHeapArray.Push (handleNodePtr, InternalPolyhedra::MAX_COST - cost);
						} else {
							InternalPolyhedra::EDGE_HANDLE *handle;
							handle = (InternalPolyhedra::EDGE_HANDLE*)ptr->userData;
							if (handle) {
								handle->edge = NULL;
							}
							ptr->userData = NULL;
						}
						
  						ptr = ptr->twin->next;
  					} while (ptr != edge);
					
  					// calculate edge cost of all incident edges to a surrunding vertex
					ptr = edge;
					do {
    					ZeroEdge *ptr1;
	   			 	ZeroEdge *incidentEdge;
					
  						incidentEdge = ptr->twin;		
					  	ptr1 = incidentEdge;
					  	do {
  							index0 = ptr1->incidentVertex;
  							index1 = ptr1->twin->incidentVertex;
					
							if (ptr1->mark != mark) {
								ptr1->mark = mark;
	  							InternalPolyhedra::VERTEX_METRIC_STRUCT &metric = vertexMetrics[index0];
  								ZeroVector3 p (&vertexPool[index1].x);
					
  	 							cost = metric.Evalue (p); 
								cost += InternalPolyhedra::EdgePenalty (&vertexPool[0], ptr1) * tol2; 
					
								if (cost	< tol2) {
	  								_ASSERTE ((InternalPolyhedra::MAX_COST - cost) > 0.0);
									InternalPolyhedra::EDGE_HANDLE	handle (ptr1);
									edgeHandleList.Prepend (handle);
									handleNodePtr = edgeHandleList.GetFirst();
									bigHeapArray.Push (handleNodePtr, InternalPolyhedra::MAX_COST - cost);
								} else {
									InternalPolyhedra::EDGE_HANDLE	*handle;
									handle = (InternalPolyhedra::EDGE_HANDLE*)ptr1->userData;
									if (handle) {
										handle->edge = NULL;
									}
									ptr1->userData = NULL;
								}
							}
					
							if (ptr1->twin->mark != mark) {
								ptr1->twin->mark = mark;
	  							InternalPolyhedra::VERTEX_METRIC_STRUCT &metric = vertexMetrics[index1];
  								ZeroVector3 p (&vertexPool[index0].x);
					
  	 							cost = metric.Evalue (p); 
								cost += InternalPolyhedra::EdgePenalty (&vertexPool[0], ptr1->twin) * tol2; 

								if (cost	< tol2) {
									_ASSERTE ((InternalPolyhedra::MAX_COST - cost) > 0.0);
									InternalPolyhedra::EDGE_HANDLE	handle (ptr1->twin);
									edgeHandleList.Prepend (handle);
									handleNodePtr = edgeHandleList.GetFirst();
									bigHeapArray.Push (handleNodePtr, InternalPolyhedra::MAX_COST - cost);
								} else {
									InternalPolyhedra::EDGE_HANDLE	*handle;
									handle = (InternalPolyhedra::EDGE_HANDLE*)ptr1->twin->userData;
									if (handle) {
										handle->edge = NULL;
									}
									ptr1->twin->userData = NULL;
								}
							}
						
  							ptr1 = ptr1->twin->next;
  						} while (ptr1 != incidentEdge);
					
						ptr = ptr->twin->next;
					} while (ptr != edge);
				}
			}
		}
	}

	desc.Update(this);
//	Trace ("edge Removed = %d  Total edge = %d\n", edgeCount - edgePadding - desc.edgeCount, edgeCount - edgePadding);
//	Trace ("Faces Removed = %d  Total Faces = %d\n", faceCount - desc.faceCount, faceCount);
}



void ZeroPolyhedra::DeleteDegenerateFaces (
//	ZeroPolyhedraDescriptor &desc, 
	const float pool[], 
	int strideInBytes, float area)
{
	int i;
	int i0;
	int i1;
	int i2;
	int mark;
	int stride;
	int count;
	float area2;
	float faceArea;
	ZeroEdge *edge;
	ZeroPolyhedra::ZeroTreeNode* faceNode;
	ZeroPolyhedra::ZeroTreeNode** faceArray;

#ifdef __ENABLE_SANITY_CHECK 
	_ASSERTE (SanityCheck ());
#endif
	ZeroStack <ZeroPolyhedra::ZeroTreeNode*> faceArrayPool(GetCount() / 3 + 1000);

	count = 0;
	faceArray = &faceArrayPool[0];
	mark = IncLRU();
	Iterator iter (*this);
	for (iter.Begin(); iter; iter ++) {
		edge = &(*iter)->info;
		if ((edge->mark != mark) && (edge->incidentFace > 0)) {
			faceArray[count] = *iter;
			(*iter)->AddRef();
			count ++;
			edge->mark = mark;
			edge->next->mark = mark;
			edge->prev->mark = mark;
		}
	}

	stride = strideInBytes / sizeof (float);
	area2 = area * area;

	for (i = 0; i < count; i ++) {
		faceNode = faceArray[i];
		if (faceNode->IsAlive()) {
			edge = &faceNode->info;

			i0 = edge->incidentVertex * stride;
	  		i1 = edge->next->incidentVertex * stride;
  			i2 = edge->prev->incidentVertex * stride;

			ZeroVector3 p0 (&pool[i0]);
			ZeroVector3 p1 (&pool[i1]);
			ZeroVector3 p2 (&pool[i2]);
			
			ZeroVector3 normal (CrossProduct((p2 - p0), (p1 - p0)));
			faceArea = DotProduct(normal, normal);
			if (faceArea < area2) {
				DeleteFace (edge);
			}
		}
		faceNode->Release();
	}

#ifdef __ENABLE_SANITY_CHECK 
	mark = IncLRU();
	for (iter.Begin(); iter; iter ++) {
		edge = &(*iter)->info;
		if ((edge->mark != mark) && (edge->incidentFace > 0)) {
			_ASSERTE (edge->next->next->next == edge);

			edge->mark = mark;
			edge->next->mark = mark;
			edge->prev->mark = mark;

		  	i0 = edge->incidentVertex * stride;
  			i1 = edge->next->incidentVertex * stride;
  			i2 = edge->prev->incidentVertex * stride;

			ZeroVector3 p0 (&pool[i0]);
			ZeroVector3 p1 (&pool[i1]);
			ZeroVector3 p2 (&pool[i2]);
	
			ZeroVector3 normal (CrossProduct ((p2 - p0), (p1 - p0)));
			faceArea = DotProduct (normal, normal);
			_ASSERTE (faceArea >= area2);
		}
	}
	_ASSERTE (SanityCheck ());
#endif

//	desc.Update(this);
}


void ZeroPolyhedra::CollapseDegenerateFaces (
	ZeroPolyhedraDescriptor &desc, 
	const float pool[], 
	int strideInBytes, float area)
{
	int i0;
	int i1;
	int i2;
	int mark;
	int stride;
	float cost;
	float area2;
	float e10Len;
	float e21Len;
	float e02Len;
	float faceArea;
	bool someChanges;
	ZeroEdge *ptr;
	ZeroEdge *face;
	ZeroEdge *edge;


#ifdef __ENABLE_SANITY_CHECK 
	_ASSERTE (SanityCheck ());
#endif

	stride = strideInBytes / sizeof (float);
	area2 = area * area;
	ZeroStack<char> heapPool (desc.faceCount * (sizeof (float) + sizeof (InternalPolyhedra::PairKey) + sizeof (int))); 
	ZeroHeap <InternalPolyhedra::PairKey, float> bigHeapArray(desc.faceCount, area2 * 2.0f, &heapPool[0]);

	Iterator iter (*this);
	do {
		someChanges	= false;
		mark = IncLRU();
		for (iter.Begin(); iter; iter ++) {
			edge = &(*iter)->info;

			if ((edge->mark != mark) && (edge->incidentFace > 0)) {
				_ASSERTE (edge->next->next->next == edge);

				edge->mark = mark;
				edge->next->mark = mark;
				edge->prev->mark = mark;

			  	i0 = edge->incidentVertex * stride;
  				i1 = edge->next->incidentVertex * stride;
  				i2 = edge->prev->incidentVertex * stride;

				ZeroVector3 p0 (&pool[i0]);
				ZeroVector3 p1 (&pool[i1]);
				ZeroVector3 p2 (&pool[i2]);
	
				ZeroVector3 normal (CrossProduct((p2 - p0), (p1 - p0)));
				faceArea = DotProduct(normal, normal);
				if (faceArea < area2) {
					someChanges	= true;
					InternalPolyhedra::PairKey key (edge->incidentVertex, edge->twin->incidentVertex);
					bigHeapArray.Push (key, area2 - faceArea);
				}
			}
		}
	
		while (bigHeapArray.GetCount()) {
		  	// collapse this edge
			cost = area2 - bigHeapArray.Value();
			InternalPolyhedra::PairKey key (bigHeapArray[0]);
			bigHeapArray.Pop();
	
			face = FindEdge (key.fields.i0, key.fields.i1);
			if (face) {
			  	i0 = face->incidentVertex * stride;
	  			i1 = face->next->incidentVertex * stride;
	  			i2 = face->prev->incidentVertex * stride;
	
				ZeroVector3 p0 (&pool[i0]);
				ZeroVector3 p1 (&pool[i1]);
				ZeroVector3 p2 (&pool[i2]);
	
				ZeroVector3 e10 (p1 - p0);
				ZeroVector3 e21 (p2 - p1);
				ZeroVector3 e02 (p0 - p2);
	
				e10Len = DotProduct(e10, e10);
				e21Len = DotProduct(e21, e21);
				e02Len = DotProduct(e02, e02);
	
				edge = face;
				if ((e21Len < e10Len) && (e21Len < e02Len)){
					edge = face->next;
				}
				if ((e02Len < e10Len) && (e02Len < e21Len)){
					edge = face->prev;
				}
		  		ptr = CollapseEdge(edge);
				if (!ptr) {
			  		ptr = CollapseEdge(edge->twin);
					if (!ptr) {
				  		ptr = CollapseEdge(edge->next);
						if (!ptr) {
					  		ptr = CollapseEdge(edge->next->twin);
							if (!ptr) {
						  		ptr = CollapseEdge(edge->prev->next);
								if (!ptr) {
							  		ptr = CollapseEdge(edge->prev->twin);
									if (!ptr) {
										DeleteFace (edge);
									}
								}
							}
						}
					}
				}
	
	#ifdef __ENABLE_SANITY_CHECK 
				_ASSERTE (SanityCheck ());
	#endif
	
			}
		}
	} while (someChanges);

	desc.Update(this);
}

void ZeroPolyhedra::GetConnectedFaces (
	ZeroVisList<ZeroEdge*>& faceList, 
	ZeroEdge *startFace,
	int markValue) const
{
	if (startFace->incidentFace < 0)	{
		startFace = startFace->twin;
		_ASSERTE (startFace->incidentFace > 0);
	}

	int mark = (markValue < 0) ? IncLRU() : markValue;

	ZeroStack<ZeroEdge*> edgeStack (GetCount()/2 + 100); 

	edgeStack[0] = startFace;
	int edgeCount = 1;

	ZeroEdge *e = startFace;
	do	{
		e->mark = mark;
		e = e->next;
	} while (e != startFace);

	while (edgeCount)	{
		ZeroEdge *edge = edgeStack[--edgeCount];
		faceList.Append (edge);

		ZeroEdge *ptr = edge;
		do	{
			if (ptr->twin->incidentFace > 0)	{
				if (ptr->twin->mark != mark)	{
					edgeStack[edgeCount++] = ptr->twin;
					_ASSERTE (edgeCount < (int)GetCount()/2);
					ZeroEdge *e = ptr->twin;
					do	{
						e->mark = mark;
						e = e->next;
					} while (e != ptr->twin);
				}
			}
			ptr = ptr->next;
		} while (ptr != edge);
	}
}




int ZeroPolyhedra::TriangleList (
	unsigned outputBuffer[], 
	int maxSize) const
{
	int mark; 
	int indexCount; 
	ZeroEdge *edge;

	ZeroPolyhedra tmp(*this);

	mark = tmp.IncLRU();

	indexCount = 0; 
	Iterator iter (tmp);
	for (iter.Begin(); iter; iter ++) {
		edge = &(*iter)->info;

		if (edge->mark == mark) {
			continue;
		}
		if (edge->incidentFace < 0) {
			continue;
		}

   	edge->mark = mark;
   	edge->next->mark = mark;
   	edge->prev->mark = mark;

		if ((indexCount + 3) > maxSize) {
			break;
		}
				
		outputBuffer[0] = edge->incidentVertex;
		outputBuffer[1] = edge->next->incidentVertex;
		outputBuffer[2] = edge->prev->incidentVertex;
		outputBuffer += 3;
		indexCount += 3;
	}
	return indexCount;
}


void ZeroPolyhedra::TriangulateFace (
	ZeroEdge *face,
	const float array[],
	int strideInBytes)
{
	int stride = strideInBytes / sizeof(float);

	ZeroStack<char> memPool (256 * (sizeof (float) + sizeof(ZeroEdge))); 
	ZeroHeap <ZeroEdge*, float> heap(250, 10, &memPool[0]);

	InternalPolyhedra::TriangulateFace (*this, face, array, stride, heap);
}

void ZeroPolyhedra::Triangulate (
	const float vertex[], 
	int strideInBytes)
{
	int mark;
	int stride;
	int count;
	ZeroEdge *ptr;
	ZeroEdge *edge;

	ZeroStack<char> memPool (1024 * (sizeof (float) + sizeof(ZeroEdge))); 
	ZeroHeap <ZeroEdge*, float> heap(1000, 10, &memPool[0]);

	mark = IncLRU();
	stride = strideInBytes / sizeof(float);

	Iterator iter (*this);
	for (iter.Begin(); iter; iter ++) {
		edge = &(*iter)->info;

		if (edge->mark == mark) {
			continue;
		}
		if (edge->incidentFace < 0) {
			continue;
		}

		count = 0;
		ptr = edge;
 		do {
			count ++;
			ptr->mark = mark;
			ptr = ptr->next;
		} while (ptr != edge);

		if (count > 3)	{
			if (!InternalPolyhedra::TriangulateFace (*this, edge, vertex, stride, heap)) {
				iter.Begin();
				if (!iter) {
					break;
				}
			}
		}
	}
}



void ZeroPolyhedra::Quadrangulate (const float vertex[], int strideInBytes)
{
	int mark;
	int stride;
	unsigned cost;
	unsigned maxCost;
	ZeroVisTree<ZeroEdge*, LONGLONG> essensialEdge;

	Iterator iter (*this);
	for (iter.Begin(); iter; iter ++) {
		ZeroEdge *edge;
		edge = &(*iter)->info;
		InternalPolyhedra::PairKey code (edge->incidentVertex, edge->twin->incidentVertex);
		essensialEdge.Insert (edge, code.val);
	}

	Triangulate (vertex, strideInBytes);

	stride = strideInBytes / sizeof (float);
	ZeroHeap<ZeroEdge*, unsigned> heapCost (GetCount(), 0xffffffff);
	maxCost = 1<<30;
	mark = IncLRU();
	for (iter.Begin(); iter; iter ++) {
		ZeroEdge *edge;
		ZeroEdge *twin;

		unsigned edgeCost;
		unsigned twinCost;
		unsigned shapeCost;
		unsigned normalCost;
		float normalDot;
		float edgeAngle0;
		float edgeAngle1;
		float twinAngle0;
		float twinAngle1;
		float shapeFactor;
		float medianAngle;

		ZeroTree<ZeroEdge*, LONGLONG>::ZeroTreeNode *essencial;

		edge = &(*iter)->info;
		twin = edge->twin;

		if (edge->mark == mark) {
			continue;
		}

		if ((edge->incidentFace < 0) || (twin->incidentFace < 0)) {
			continue;
		}

		edge->mark = mark;
		twin->mark = mark;

		ZeroVector3 edgeNormal (FaceNormal (edge, vertex, strideInBytes));
		if (DotProduct (edgeNormal, edgeNormal) < 1.0e-8) {
			continue;
		}
		ZeroVector3 twinNormal (FaceNormal (twin, vertex, strideInBytes));

		if (DotProduct (twinNormal, twinNormal) < 1.0e-8) {
			continue;
		}

		edgeNormal = edgeNormal.Scale (rsqrt (DotProduct(edgeNormal, edgeNormal)));
		twinNormal = twinNormal.Scale (rsqrt (DotProduct(twinNormal, twinNormal)));

		normalDot = DotProduct (edgeNormal, twinNormal);
		normalCost = clamp (2048 - (int)(2048.0f * normalDot), 0, 2048);
		if (normalCost	> 300) {
			normalCost = 4000000;
		}

		ZeroVector3 p0 (&vertex[edge->incidentVertex * stride]);
		ZeroVector3 p1 (&vertex[edge->twin->incidentVertex * stride]);
		ZeroVector3 p2 (&vertex[edge->prev->incidentVertex * stride]);
		ZeroVector3 p3 (&vertex[twin->prev->incidentVertex * stride]);

		ZeroVector3 e10 (p1 - p0);
		ZeroVector3 e20 (p2 - p0);
		ZeroVector3 e30 (p3 - p0);

		e10 = e10.Scale (rsqrt (DotProduct(e10, e10) + 1.e-10f));
		e20 = e20.Scale (rsqrt (DotProduct(e20, e20) + 1.e-10f));
		e30 = e30.Scale (rsqrt (DotProduct(e30, e30) + 1.e-10f));

		edgeAngle0 = RAD2DEG * atan2f (DotProduct (edgeNormal, CrossProduct (e10, e20)), DotProduct (e20, e10));
		edgeAngle1 = RAD2DEG * atan2f (DotProduct (twinNormal, CrossProduct (e30, e10)), DotProduct (e10, e30));

		if ((edgeAngle0 + edgeAngle1) < 160.0f) {
			_ASSERTE ((edgeAngle0 + edgeAngle1) > 0.0f);
			medianAngle = 4.0f * edgeAngle0 * edgeAngle1 / (edgeAngle0 + edgeAngle1);

			_ASSERTE (medianAngle >= 0.0f);
			_ASSERTE (medianAngle < 360.0f);
			edgeCost = abs (clamp (90 - (int)medianAngle, -90, 90));
		} else {
			edgeCost	= 4000000;
		}

		ZeroVector3 t10 (p0 - p1);
		ZeroVector3 t20 (p3 - p1);
		ZeroVector3 t30 (p2 - p1);

		t10 = t10.Scale (rsqrt (DotProduct(t10, t10) + 1.e-10f));
		t20 = t20.Scale (rsqrt (DotProduct(t20, t20) + 1.e-10f));
		t30 = t30.Scale (rsqrt (DotProduct(t30, t30) + 1.e-10f));

		twinAngle0 = RAD2DEG * atan2f (DotProduct (twinNormal, CrossProduct (t10, t20)), DotProduct (t20, t10));
		twinAngle1 = RAD2DEG * atan2f (DotProduct (edgeNormal, CrossProduct (t30, t10)), DotProduct (t10, t30));

		if ((twinAngle0 + twinAngle1) < 160.0f) {
			_ASSERTE ((twinAngle0 + twinAngle1) > 0.0f);
			medianAngle = 4.0f * twinAngle0 * twinAngle1 / (twinAngle0 + twinAngle1);

			_ASSERTE (medianAngle > 0.0f);
			_ASSERTE (medianAngle < 360.0f);
			twinCost = abs (clamp (90 - (int)medianAngle, -90, 90));
		} else {
			twinCost	= 4000000;
		}


		float a0;
		float a1;
		float a2;
		float a3;
		float angleSum;
		float angleSum2;

		a0 = edgeAngle0 + edgeAngle1;
		a1 = twinAngle0 + twinAngle1;

		ZeroVector3 oe10 (p0 - p2);
		ZeroVector3 oe20 (p1 - p2);

		oe10 = oe10.Scale (rsqrt (DotProduct (oe10, oe10) + 1.e-10f));
		oe20 = oe20.Scale (rsqrt (DotProduct (oe20, oe20) + 1.e-10f));
		a2 = RAD2DEG * atan2f (DotProduct (edgeNormal, CrossProduct(oe10, oe20)), DotProduct (oe20, oe10));

		ZeroVector3 ot10 (p1 - p3);
		ZeroVector3 ot20 (p0 - p3);

		ot10 = ot10.Scale (rsqrt (DotProduct (ot10, ot10) + 1.e-10f));
		ot20 = ot20.Scale (rsqrt (DotProduct (ot20, ot20) + 1.e-10f));
		a3 = RAD2DEG * atan2f (DotProduct (twinNormal, CrossProduct (ot10, ot20)), DotProduct (ot20, ot10));

		angleSum	= (a0 + a1 + a2 + a3) * 0.25f;
		angleSum2 = (a0 * a0 + a1 * a1 + a2 * a2 + a3 * a3) * 0.25f;
		float tmp=angleSum2 - angleSum * angleSum;
		if(tmp<EPSILON)
			shapeFactor = 0.f;
		else
			shapeFactor = powf (sqrtf (tmp), 1.25f);

		shapeCost = clamp ((int)(shapeFactor * 4.0f), 0, 4096);

		cost = normalCost + edgeCost + twinCost + shapeCost;
		InternalPolyhedra::PairKey code (edge->incidentVertex, edge->twin->incidentVertex);
		essencial = essensialEdge.Find(code.val);
		if (essencial) {
			cost += 1000000; // used to be 1000, but 1000000 prevents the edge from ever being deleted - Mike Z
			// artists were having problems
		}
		heapCost.Push (edge, maxCost - cost);
	}


	mark = IncLRU();
	while (heapCost.GetCount ()) {
		unsigned cost;
		ZeroEdge *edge;

		edge = heapCost[0];
		cost = maxCost - heapCost.Value ();
		if (cost	> 100000) {
			break;
		}

		heapCost.Pop();

		if (edge->mark != mark) {
			edge->mark = mark;
			edge->twin->mark = mark;

			edge->next->mark = mark;
			edge->next->twin->mark = mark;

			edge->prev->mark = mark;
			edge->prev->twin->mark = mark;

			edge->twin->next->mark = mark;
			edge->twin->next->twin->mark = mark;

			edge->twin->prev->mark = mark;
			edge->twin->prev->twin->mark = mark;

			DeleteEdge (edge);
		}

	}
}




void ZeroPolyhedra::OptimalTriangulation (
	const float vertex[], 
	int strideInBytes)
{
	int setMark;
	int faceColorMark;
	ZeroEdge *edge;
	ZeroVisList<ZeroEdge*> edgeStart;

	Quadrangulate (vertex, strideInBytes);

	faceColorMark = IncLRU();
	IncLRU();
	setMark = IncLRU();
	
	for (edge = InternalPolyhedra::FindQuadStart(*this, faceColorMark, edgeStart); edge; edge = InternalPolyhedra::FindQuadStart(*this, faceColorMark, edgeStart)) {
		InternalPolyhedra::TriangleQuadStrips (*this, edge, setMark, edgeStart);
	}
}



void ZeroPolyhedra::DeleteOverlapingFaces (
	const float pool[], 
	int strideInBytes, 
	float distTol)
{
	int i;
	int mark;
	int perimeterCount;
	ZeroEdge *edge;
	ZeroPolyhedra *perimeters;

	if (!GetCount()) {
		return;
	}

	ZeroStack<int>faceIndexPool (4096);
	ZeroStack<ZeroEdge*> stackPool (GetCount() / 2 + 100);
	ZeroStack<ZeroPolyhedra> flatPerimetersPool (GetCount() / 3 + 100);

	perimeterCount = 0;
	perimeters = &flatPerimetersPool[0];

	mark = IncLRU();
	Iterator iter (*this);
	for (iter.Begin(); iter; iter++) {
		edge = &(*iter)->info;
		if (edge->incidentFace < 0) {
			continue;
		}

		if (edge->mark >= mark) {
			continue;
		}

		ZeroPolyhedra dommy;
		perimeters[perimeterCount] = dommy;
		InternalPolyhedra::GetAdjacentCoplanarFacesPerimeter (perimeters[perimeterCount], *this, edge, pool, strideInBytes, &stackPool[0], &faceIndexPool[0]);
		perimeterCount ++;
	}

	// write code here


	for (i = 0 ; i < perimeterCount; i ++) {
		perimeters[i].DeleteAllFace();
	}
}


void ZeroPolyhedra::CombineOpenFaces (
	const float pool[], 
	int strideInBytes, 
	float tol)
{
	int i;
	int mark;
	int index;
	int count;
	int stride;
	int vertexIndex;
	bool conflictEdge;
	double x;
	double y;
	double z;
	double x0;
	double x1;
	double xc;
	double yc;
	double zc;
	double x2c;
	double y2c;
	double z2c;
	double tol2;
	double dist2;

	ZeroEdge *ptr;
	ZeroEdge *edgeA;
	ZeroEdge *edgeB;
	ZeroTreeNode *nodeA;
	ZeroTreeNode *nodeB;
	const double MAX_HEAP_VAL = 1.0e8;
	const double MAX_VAL = MAX_HEAP_VAL * 0.125;

	xc = 0;
	yc = 0;
	zc = 0;
	x2c = 0;
	y2c = 0;
	z2c = 0;
	count = 0;

	stride = strideInBytes / sizeof (float);

	mark = IncLRU();
	Iterator iter (*this);
	for (iter.Begin(); iter; iter++) {
		edgeA = &(*iter)->info;
		if (edgeA->mark == mark) {
			continue;
		}
		if (edgeA->incidentFace > 0) {
			continue;
		}

		edgeB = edgeA;
		do {
			edgeB->mark = mark;
			edgeB = edgeB->twin->next;
		} while (edgeB != edgeA);

		index = edgeA->incidentVertex * stride;
		x = pool[index + 0];
		y = pool[index + 1];
		z = pool[index + 2];
		xc += x;
		yc += y;
		zc += z;
		x2c += x * x;
		y2c += y * y; 
		z2c += z * z;
		count	++;
	}

	x2c = count * x2c - xc * xc;
	y2c = count * y2c - yc * yc;
	z2c = count * z2c - zc * zc;

	index = 0;
	if ((y2c > x2c) && (y2c > z2c)) {
		index = 1;
	}
	if ((z2c > x2c) && (z2c > y2c)) {
		index = 2;
	}

	ZeroStack<char> bigHeapPool ((count + 1024) * (sizeof (double) + sizeof (ZeroEdge*) + sizeof (int))); 
	ZeroStack<char> smallHeapPool((count + 1024) * (sizeof (double) + sizeof (ZeroEdge*) + sizeof (int))); 

	ZeroHeap <ZeroTreeNode*, double> bigHeap (count + 16, MAX_HEAP_VAL, &bigHeapPool[0]);
	ZeroHeap <ZeroTreeNode*, double> smallHeap (count + 16, MAX_HEAP_VAL, &smallHeapPool[0]);

	if (fabs (tol) < 0.0001) {
		tol = 0.0001f;
	}
	tol2 = tol * tol;

	mark = IncLRU();
	for (iter.Begin(); iter; iter++) {
		edgeA = &(*iter)->info;
		if (edgeA->mark == mark) {
			continue;
		}
		if (edgeA->incidentFace > 0) {
			continue;
		}

		edgeB = edgeA;
		do {
			if (edgeB->incidentFace < 0) {
				x0 = pool[edgeB->incidentVertex * stride + index];
				x1 = pool[edgeB->twin->incidentVertex * stride + index];

				nodeB = GetNodeFromInfo(*edgeB);
				bigHeap.Push (nodeB, MAX_VAL - min (x0, x1));
				nodeB->AddRef();
				_ASSERTE (nodeB->IsAlive());
			}

			edgeB->mark = mark;
			edgeB = edgeB->twin->next;
		} while (edgeB != edgeA);
	}

	while (bigHeap.GetCount()) {
		nodeA = bigHeap[0];

		if (nodeA->IsAlive()) {
			edgeA = &nodeA->info;

	  		xc = MAX_VAL - bigHeap.Value() - 0.5f;
			while (smallHeap.GetCount() && (!smallHeap[0]->IsAlive() || (xc > (MAX_VAL - smallHeap.Value())))) {
				nodeB = smallHeap[0];
				smallHeap.Pop();
				nodeB->Release();
			}
	
			ZeroVector3 p0 (&pool[edgeA->incidentVertex * stride]);
			ZeroVector3 p1 (&pool[edgeA->twin->incidentVertex * stride]);
  			for (i = 0; i < smallHeap.GetCount(); i ++) {
				nodeB = smallHeap[i];
				if (nodeB->IsAlive()) {
					edgeB = &nodeB->info;

					ZeroVector3 q0 (&pool[edgeB->incidentVertex * stride]);
					ZeroVector3 q1 (&pool[edgeB->twin->incidentVertex * stride]);
					ZeroVector3 dist (p0- q1);
					dist2 = DotProduct(dist, dist);
					if (dist2 <= tol2) {
						ZeroVector3 dist (p1- q0);
						dist2 = DotProduct(dist, dist);
						if (dist2 <= tol2) {
					
							vertexIndex = edgeA->incidentVertex;
							ptr = edgeB->twin;
							do {
								ptr->incidentVertex = vertexIndex;
								ptr = ptr->twin->next;
							} while (ptr != edgeB->twin);
					
					
							vertexIndex = edgeA->twin->incidentVertex;
							ptr = edgeB;
							do {
								ptr->incidentVertex = vertexIndex;
								ptr = ptr->twin->next;
							} while (ptr != edgeB);
					
							edgeA->prev->next = edgeB->next;
							edgeB->next->prev = edgeA->prev;
					
							edgeA->next->prev = edgeB->prev;
							edgeB->prev->next = edgeA->next;
					
							edgeA->twin->twin = edgeB->twin;
							edgeB->twin->twin = edgeA->twin;
					
							Remove (nodeA);
							Remove (nodeB);
							break;
						}
					}
				} 
			}
			
			if (i == smallHeap.GetCount()) {
				x0 = pool[edgeA->incidentVertex * stride + index];
		   	x1 = pool[edgeA->twin->incidentVertex * stride + index];
				smallHeap.Push (nodeA, MAX_VAL - max (x0, x1));
				nodeA->AddRef();
			}
		}

		bigHeap.Pop();
		nodeA->Release();
	}		

	while (smallHeap.GetCount()) {
		nodeB = smallHeap[0];
		smallHeap.Pop();
		nodeB->Release();
	}

	count = 0;
	ZeroStack <ZeroTreeNode*> badKeyNodes(GetCount() + 100);
	for (iter.Begin(); iter; iter++) {
		nodeA = iter.GetNode();
		edgeA = &nodeA->info;

		InternalPolyhedra::PairKey newKey (edgeA->incidentVertex, edgeA->twin->incidentVertex);
		InternalPolyhedra::PairKey oldKey (nodeA->key);

		if (newKey.val != oldKey.val) {
			badKeyNodes[count] = nodeA;
			nodeA->AddRef();
			count ++;
		}
	}

	conflictEdge = false;
	for (i = 0; i < count; i ++) {
		nodeA = badKeyNodes[i];
		if (nodeA->IsAlive()) {
			edgeA = &nodeA->info;

			InternalPolyhedra::PairKey newKey (edgeA->incidentVertex, edgeA->twin->incidentVertex);
			nodeB = Find (newKey.val);
			if (nodeB) { 
				edgeB = &nodeB->info;
				DeleteEdge (edgeB);
				conflictEdge = true;
			} 
			if (nodeA->IsAlive()){
				ReplaceKey (nodeA, newKey.val);
			}
		} 
		nodeA->Release();
	}

	if (conflictEdge) {
		mark = IncLRU();
		for (iter.Begin(); iter; iter++) {
			edgeA = &(*iter)->info;
			if (edgeA->mark == mark) {
				continue;
			}

			i = edgeA->incidentFace;
			edgeB = edgeA;
			do {
				edgeB->mark = mark;
				edgeB->incidentFace = i;
				edgeB = edgeB->next;
			} while (edgeB != edgeA);
		}
	}

	for (iter.Begin(); iter; ) {
		nodeA = iter.GetNode();
		iter++;
		edgeA = &nodeA->info;
		if (edgeA->incidentFace < 0) {
			if (edgeA->twin->incidentFace < 0) {
				if (edgeA->twin == &(*iter)->info) {
					iter ++;
				}
				DeleteEdge (edgeA);
			}
		}
	}


	#ifdef __ENABLE_SANITY_CHECK 
	_ASSERTE (SanityCheck ());
	#endif
}


void ZeroPolyhedra::Merge (
	ZeroPolyhedraDescriptor &myDesc, 
  	float myPool[], 
	int myStrideInBytes,
  	const ZeroPolyhedra& he, 
	const float hisPool[], 
	int hisStrideInBytes)
{
	int i;
	int i0;
	int i1;
	int mark;
	int myStride;
	int hisStride;
	int triaCount;
	int faceCount;
	int edgeCount;
	int vertexCount;
	int faceIndexCount;
	int vertexIndexCount;
	int* faceIndex;
	int* indexMapPtr;
	ZeroEdge *ptr;
	ZeroEdge *edge;
	ZeroEdge *retEdge;
	ZeroEdge **facePtr;
	ZeroTreeNode *node;
	ZeroEdge *borderEdge;

	myStride = myStrideInBytes / sizeof (float);
	hisStride = hisStrideInBytes / sizeof (float);
	ZeroPolyhedraDescriptor hisDesc (&he); 

	ZeroStack<int> faceIndexPool(4096);
	ZeroStack<int> indexMapArray(he.GetCount() / 2);

	indexMapPtr = &indexMapArray[0];
	faceIndex = &faceIndexPool[0];
 
	vertexCount = 0;
	vertexIndexCount = myDesc.maxVertexIndex;

#ifdef __ENABLE_SANITY_CHECK 
	_ASSERTE (SanityCheck ());
	_ASSERTE (he.SanityCheck ());
#endif

	Iterator iter (he);
	mark = he.IncLRU();
	for (iter.Begin(); iter; iter ++) {
		edge = &(*iter)->info;
		if (edge->mark != mark) {
			memcpy (&myPool[vertexIndexCount * myStride], 
					  &hisPool[edge->incidentVertex * hisStride], sizeof (ZeroVector3));

			_ASSERTE (edge->incidentVertex < hisDesc.maxVertexIndex);
			indexMapPtr[edge->incidentVertex] = vertexIndexCount;
			vertexIndexCount ++;
			vertexCount ++;
			ptr = edge;
			do {
				ptr->mark = mark;
				ptr = ptr->twin->next;
			} while (ptr != edge);
		}
	}

	faceCount = 0;
	triaCount = 0;
	edgeCount = 0;
	mark = he.IncLRU();

	ZeroStack<ZeroEdge*> faceArray(hisDesc.faceCount);
	facePtr = &faceArray[0];
	for (iter.Begin(); iter; iter ++) {
		edge = &(*iter)->info;
		if (edge->mark != mark) {
			if (edge->incidentFace > 0) {
				faceIndexCount = 0;
				ptr = edge;
				do {
					faceIndex[faceIndexCount] = indexMapPtr[ptr->incidentVertex];
					faceIndexCount ++;
					ptr->mark = mark;
					ptr = ptr->next;
				} while (ptr != edge);

				retEdge = AddFace (faceIndexCount, faceIndex);
				if (retEdge) {
					_ASSERTE (retEdge);
					_ASSERTE (retEdge == FindEdge (faceIndex[0], faceIndex[1]));
					facePtr[faceCount] = retEdge;
					faceCount ++;
					edgeCount += faceIndexCount;
					triaCount += faceIndexCount - 2;
				}
			}
		}
	}

	for (i = 0; i < faceCount; i ++) {
		edge = facePtr[i];
		do	{
			if (!edge->twin) {
				edge->twin = FindEdge (edge->next->incidentVertex, edge->incidentVertex);
				if (edge->twin) {
					edge->twin->twin = edge; 
				}
			}
			edge = edge->next;
		} while (edge != facePtr[i]);
	}


	ZeroVisList<ZeroEdge*>::Iterator edgeIter (hisDesc.unboundedLoops);
	for (edgeIter.Begin(); edgeIter; edgeIter++) {
		faceCount ++;
		edge = *edgeIter;
		ptr = edge;
		do {
			i0 = indexMapPtr[ptr->twin->incidentVertex];
			i1 = indexMapPtr[ptr->incidentVertex];
	
			borderEdge = FindEdge (i0, i1);
			_ASSERTE (borderEdge);
			_ASSERTE (!borderEdge->twin);
	
			InternalPolyhedra::PairKey code (i1, i0);
			ZeroEdge tmpEdge (i1, -1);
			node = Insert (tmpEdge, code.val); 
			borderEdge->twin = &node->info;
			borderEdge->twin->twin = borderEdge; 

			edgeCount ++;
			ptr = ptr->next;  
		} while (ptr != edge);
	}

	for (edgeIter.Begin(); edgeIter; edgeIter++) {
		edge = *edgeIter;
		ptr = edge;
		do {
			i0 = indexMapPtr[ptr->incidentVertex];
			i1 = indexMapPtr[ptr->twin->incidentVertex];
			borderEdge = FindEdge (i0, i1);
			if (!borderEdge->prev) {
				ZeroEdge *ptr1;
				for (ptr1 = borderEdge->twin; ptr1->next; ptr1 = ptr1->next->twin);
				ptr1->next = borderEdge;
				borderEdge->prev = ptr1;
			}
			ptr = ptr->next;  
		} while (ptr != edge);

		myDesc.unboundedLoops.Append (borderEdge);
	}

	myDesc.edgeCount += edgeCount;
	myDesc.faceCount += faceCount;
	myDesc.triangleCount += triaCount;
	myDesc.vertexCount += vertexCount;
	myDesc.maxVertexIndex += vertexCount;


#ifdef __ENABLE_SANITY_CHECK 
	ZeroPolyhedraDescriptor desc (this);
	_ASSERTE (myDesc.vertexCount == desc.vertexCount); 
	_ASSERTE (myDesc.faceCount == desc.faceCount); 
	_ASSERTE (myDesc.edgeCount == desc.edgeCount); 
	_ASSERTE (myDesc.unboundedLoops.GetCount() == desc.unboundedLoops.GetCount()); 
	_ASSERTE (SanityCheck ());
#endif
}




int ZeroPolyhedra::TriangleList (
	unsigned outputBuffer[], 
	int maxSize,
	int vertexCacheSize) const
{
	int mark;
	int index;
	int tmpIndex;
	int twinIndex;
	int faceCount;
	int cost;
	int lowestPrize;
	int cacheHit;
	int cacheMiss;
	ZeroEdge *ptr;
	ZeroEdge *edge;
	ZeroVisList<int> vertexCache;
	ZeroVisList<ZeroEdge *> priorityEdgeList;

	ZeroVisList<ZeroEdge *>::ZeroListNode *node;
	ZeroVisList<ZeroEdge *>::ZeroListNode *bestEdge;


	cacheHit = 0;
	cacheMiss = 0;
	vertexCache.Append (-1);

	faceCount = 0;
	mark = IncLRU();
	Iterator iter (*this);
	for (iter.Begin(); iter; iter ++) {
		edge = &(*iter)->info;
		if (edge->incidentFace < 0) {
			continue;
		}

		if (edge->mark == mark) {
			continue;
		}

		ptr = edge;
		do {
			ptr->mark = mark;
			index = ptr->incidentVertex;
			*outputBuffer = index;
			outputBuffer ++;

			if (ptr->twin->mark != mark) {
				if (ptr->twin->incidentFace > 0) {
					priorityEdgeList.Append (ptr->twin);
				}
			}

			ZeroVisList<int>::Iterator vertexIter(vertexCache);
			for (vertexIter.Begin(); vertexIter; vertexIter++) {
				tmpIndex	= *vertexIter;
				if (index == tmpIndex) {
					cacheHit ++;
					break;
				}
			}

			if (!vertexIter) {
				vertexCache.Append (index);
				cacheMiss ++;
				if (vertexCache.GetCount() > vertexCacheSize) {
					 vertexCache.Remove (vertexCache.GetFirst());
				}
			}

			ptr = ptr->next;
		} while (ptr != edge);
		faceCount ++;

		if (!priorityEdgeList) {
			continue;
		}

		ZeroVisList<int>::Iterator vertexIter(vertexCache);
		for (vertexIter.Begin(); vertexIter; ) {
			index = *vertexIter;
			vertexIter ++;

			bestEdge	= NULL;
			lowestPrize = 100000;
			ZeroVisList<ZeroEdge *>::Iterator iter(priorityEdgeList);
			for (iter.Begin(); iter; ) {
				node = iter.GetNode();
				iter ++;
				ptr = node->info;
				if (ptr->mark == mark) {
					priorityEdgeList.Remove (node);
					continue;
				}

				if ((ptr->incidentVertex == index) || (ptr->twin->incidentVertex == index)) {
					if (ptr->incidentVertex == index) {
						twinIndex = ptr->twin->incidentVertex;   
					} else {
						twinIndex = ptr->incidentVertex;   
					}
					ZeroVisList<int>::Iterator vertexIter(vertexCache);
					cost = 0;
					for (vertexIter.Begin(); vertexIter; vertexIter ++) {
						tmpIndex	= *vertexIter;
						if (twinIndex == tmpIndex) {
							break;
						};
						cost ++;
					}
					if (cost < lowestPrize) {
						lowestPrize	= cost;
						bestEdge	= node;
					}
				}
			}

			if (bestEdge) {
				edge = bestEdge->info;
				priorityEdgeList.Remove(bestEdge);

				ptr = edge;
				do {
					ptr->mark = mark;
					*outputBuffer = ptr->incidentVertex;
					outputBuffer ++;

					if (ptr->twin->mark != mark) {
						if (ptr->twin->incidentFace > 0) {
							priorityEdgeList.Append (ptr->twin);
						}
					}

					index = ptr->incidentVertex;
					ZeroVisList<int>::Iterator vertexIter(vertexCache);
					for (vertexIter.Begin(); vertexIter; vertexIter++) {
						tmpIndex	= *vertexIter;
						if (index == tmpIndex) {
							cacheHit ++;
							break;
						}
					}

					if (!vertexIter) {
						cacheMiss ++;
						vertexCache.Append (index);
						if (vertexCache.GetCount() > vertexCacheSize) {
							 vertexCache.Remove (vertexCache.GetFirst());
						}
					}

					ptr = ptr->next;
				} while (ptr != edge);
				faceCount ++;
				vertexIter.Begin();
			}
			if (!priorityEdgeList) {
				break;
			}
		}
	}

//	ZeroTrace ("cacheHit = %d, cacheMiss = %d, total = %d\n", cacheHit, cacheMiss, cacheMiss + cacheHit);
	_ASSERTE (!priorityEdgeList);

	return faceCount * 3;
}


int ZeroPolyhedra::TriangleStrips (
	unsigned outputBuffer[], 
	int maxBufferSize, 
	int vertexCacheSize) const
{
	int setMark;
	int indexCount;
	int stripsIndex;
	int faceColorMark;
	ZeroEdge *edge;
	ZeroVisList<ZeroEdge*> allFaces;

	ZeroPolyhedra tmp(*this);

	setMark = tmp.IncLRU();

	Iterator iter (tmp);
	for (iter.Begin(); iter; iter ++) {
		edge = &(*iter)->info;
		if (edge->incidentFace > 0) {
			if (edge->mark < setMark) {
				edge->mark = setMark;
				edge->twin->mark = setMark;
				edge->prev->mark = setMark;
				if (edge->twin->incidentFace < 0) {
					allFaces.Prepend (edge);
				} else {
					allFaces.Append (edge);
				}
			}
		}
	}

	faceColorMark = tmp.IncLRU();
	tmp.IncLRU();
	setMark = tmp.IncLRU();


#ifdef _DEBUG
	static int performanceIndexCount = 0;
	static int performanceTriangleCount = 0;
#endif

	
	indexCount = 0;
	for (edge = InternalPolyhedra::FindStripStart(allFaces, faceColorMark); edge; edge = InternalPolyhedra::FindStripStart(allFaces, faceColorMark)) {
		stripsIndex = InternalPolyhedra::TriangleStrips (edge, setMark, &outputBuffer[indexCount + 1]);

		if (stripsIndex > 0)	{
#ifdef _DEBUG
			performanceIndexCount += stripsIndex;
			performanceTriangleCount += (stripsIndex - 2);
#endif

			outputBuffer[indexCount] = stripsIndex;
			indexCount += stripsIndex + 1;
			if (indexCount > maxBufferSize) {
				_ASSERT(0);
				break;
			}
		}
	}

	ZeroTrace (("average indices per triangles= %f\n", (float)performanceIndexCount / (float)performanceTriangleCount));

	return indexCount;
}








void ZeroPolyhedra::GetCoplanarFaces (
	ZeroVisList<ZeroEdge*>& faceList,
	ZeroEdge *startFace,
	const float pool[],
	int strideInBytes,
	float normalDeviation,
	int mark) const
{
	float dot;
	int edgeCount;
	ZeroEdge *ptr;
	ZeroEdge *ptr1;
	ZeroEdge *edge;

#ifdef __ENABLE_SANITY_CHECK 
	_ASSERTE (SanityCheck ());
#endif
	if (startFace->incidentFace < 0) {
		startFace = startFace->twin;
		_ASSERTE (startFace->incidentFace > 0);
	}


	ZeroVector3 startNormal (NormalizeFast(FaceNormal (startFace, pool, strideInBytes)));

	ZeroStack<ZeroEdge*> edgeStack (GetCount()/2 + 100); 

	edgeStack[0] = startFace;


	edgeCount = 1;
	edge = startFace;
	do	{
		edge ->mark = mark;
		edge = edge ->next;
	}	while (edge != startFace);

	while (edgeCount)	{
		edge = edgeStack[--edgeCount];
		faceList.Append (edge);

		ptr = edge;
		do	{
			if (ptr->twin->incidentFace > 0)	{
				if (ptr->twin->mark != mark) {
					ZeroVector3 faceNormal (NormalizeFast(FaceNormal (ptr->twin, pool, strideInBytes)));
					dot = DotProduct(startNormal, faceNormal);
					if (dot >= normalDeviation) {

						edgeStack[edgeCount++] = ptr->twin;
						_ASSERTE (edgeCount < (int)GetCount()/2);

						ptr1 = ptr->twin;
						do	{
							ptr1 ->mark = mark;
							ptr1 = ptr1 ->next;
						} 	while (ptr1 != ptr->twin);
					}
				}
			}

			ptr = ptr->next;
		} while (ptr != edge);
	}
}


void ZeroPolyhedra::ConvexPartition (const float vertex[], int strideInBytes)
{

	int lastIndex;
	int mark;
	ZeroEdge *edge;

	Triangulate (vertex, strideInBytes);
	Optimize (vertex, strideInBytes, 1.e-3f);
//	Optimize (vertex, strideInBytes, 2.e-1f);
	DeleteDegenerateFaces (vertex, strideInBytes, 1.0e-4f);
//return;

	if (!GetCount()) {
		return;
	}

#ifdef __ENABLE_SANITY_CHECK 
	_ASSERTE (SanityCheck ());
#endif

	ZeroStack<ZeroEdge*> diagonalsPool (GetCount() / 2 + 100);
	ZeroStack<int>face (2048);

	ZeroPolyhedraDescriptor	desc (this);

	lastIndex = desc.maxVertexIndex;
	
	mark = IncLRU();
	Iterator iter (*this);
	while (iter) {
		edge = &(*iter)->info;
		iter++;

		if (edge->incidentFace < 0) {
			continue;
		}

		if (edge->mark == mark) {
			continue;
		}

		ZeroVisList<ZeroEdge*> faceList;

		GetCoplanarFaces (faceList, edge, vertex, strideInBytes, 0.9999f, mark);
			

		ZeroPolyhedra flatFace; 
		flatFace.BeginFace();
		ZeroList<ZeroEdge*>::Iterator faceListIter (faceList);
		for (faceListIter.Begin(); faceListIter; faceListIter++) {
			int count;
			ZeroEdge *ptr;

			count = 0;
			edge = *faceListIter;
			_ASSERTE (edge->incidentFace > 0);

			ptr = edge;
			do {
				face[count] = ptr->incidentVertex;
				count ++;
				_ASSERTE (count < 2048);
				ptr = ptr->next;
			} while (ptr != edge);

			flatFace.AddFace (count, &face[0]);
		}
		flatFace.EndFace();


		_ASSERTE (flatFace.GetCount());
			
		ZeroVisList<LONGLONG> deadEdges;

		InternalPolyhedra::ComplexConvexPartition (flatFace, vertex, strideInBytes, &diagonalsPool[0], deadEdges, lastIndex);


		ZeroVisList<LONGLONG>::Iterator deadEdgeIter (deadEdges);
		while (deadEdgeIter) {
			int v0;
			int v1;
			LONGLONG key;

			key = *deadEdgeIter;
			deadEdgeIter++;

			v0 = int(key>>32);
			v1 = int(key);
			DeleteEdge (v0, v1);
		}
	}

}

#endif
