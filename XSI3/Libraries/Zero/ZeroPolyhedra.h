//********************************************************************
// Direct geometry Hardware independent library
//	By Julio Jerez
// VC: 6.0
//********************************************************************
#ifndef __ZeroPolyhedra__
#define __ZeroPolyhedra__


#ifdef TOOL_DEVELOPMENT 

#include "ZeroList.h"
#include "ZeroTree.h"
#include "ZeroVector3.h"


class ZeroFace;
class ZeroEdge;
class ZeroPlane;
class ZeroMatrix;
class ZeroSphere;
class ZeroPolyhedra;

class ZeroPolyhedraDescriptor
{
	public:
	int faceCount;
	int edgeCount;
	int vertexCount;
	int triangleCount;
	int maxVertexIndex;
	ZeroVisList <ZeroEdge*> unboundedLoops;
	
	DLL_DECLSPEC  ZeroPolyhedraDescriptor (const ZeroPolyhedra *polyhedra);
	DLL_DECLSPEC ~ZeroPolyhedraDescriptor ();
	DLL_DECLSPEC  void Update (const ZeroPolyhedra *polyhedra);
};


class ZeroEdge
{
	public:
	int mark;
	int incidentFace;
	int incidentVertex;
	unsigned userData;
	ZeroEdge *twin;
	ZeroEdge *next;
	ZeroEdge *prev;
	
	ZeroEdge ()
	{
	}
	
	ZeroEdge (int vertex, int face, unsigned userDataArg = 0)
	{
		mark = 0;
		twin = NULL;
		next = NULL;
		prev = NULL;
		userData = userDataArg;
		incidentVertex = vertex;
		incidentFace = face;
	}
};


class ZeroPolyhedra: public ZeroVisTree <ZeroEdge, LONGLONG>
{
	friend class ZeroPolyhedraDescriptor;
	
	mutable int edgeMark;
	int faceSecuence;
	ZeroEdge *CollapseEdge(ZeroEdge *edge);
	
	public:
	DLL_DECLSPEC ZeroPolyhedra ();
	DLL_DECLSPEC ZeroPolyhedra (const ZeroPolyhedra &polyhedra);
	
	DLL_DECLSPEC ~ZeroPolyhedra ();
	
	DLL_DECLSPEC bool SanityCheck() const;
	
	
	int IncLRU () const
	{
		return ++edgeMark;
	}

	void IncFaceCounter()
	{
		faceSecuence ++;
	}	
	
	DLL_DECLSPEC void BeginFace ();
	DLL_DECLSPEC ZeroEdge* AddFace (int count, int index[]);
	ZeroEdge* AddFace (int count, int index[], unsigned user[])
	{
		int i;
		ZeroEdge *first;

		first = AddFace (count, index);

		if (first) {
			for (i = 0; i < count; i ++) {
				first->userData = user[i];
				first = first->next;
			}
		}
		return first;
	}

	ZeroEdge* AddFace (int v0, int v1, int v2)
	{
		int vertex[3];
		vertex [0] = v0;
		vertex [1] = v1;
		vertex [2] = v2;
		return AddFace (3, vertex);
	}
	DLL_DECLSPEC void EndFace ();
	
	DLL_DECLSPEC ZeroEdge* AddEdge (int v0, int v1);
	DLL_DECLSPEC void DeleteEdge (int v0, int v1);
	DLL_DECLSPEC void DeleteEdge (ZeroEdge *edge);
	
	DLL_DECLSPEC void DeleteAllFace();
	DLL_DECLSPEC void DeleteFace(ZeroEdge *edge);
	DLL_DECLSPEC void DeleteDegenerateFaces (const float pool[], int strideInBytes, float minArea);

	DLL_DECLSPEC ZeroVector3 FaceNormal (ZeroEdge *face, const float array[], int strideInBytes) const;
	
	DLL_DECLSPEC void SpliteEdge (int newIndex, ZeroEdge *edge, ZeroEdge **edgein = NULL, ZeroEdge **edgeout = NULL);
	DLL_DECLSPEC void SpliteEdgeAndTriangulate (int newIndex, ZeroEdge *edge, ZeroEdge **edgein = NULL, ZeroEdge **edgeout = NULL);
	
	DLL_DECLSPEC void SpliteFace (int newIndex, ZeroEdge *face);

	DLL_DECLSPEC bool FlipEdge (ZeroEdge *edge);
	DLL_DECLSPEC ZeroEdge *FindEdge (int v0, int v1) const;
	
	DLL_DECLSPEC bool IsEdgeCollinear (ZeroEdge *edge, const float pool[], int strideInBytes) const;


	DLL_DECLSPEC void Quadrangulate (const float vertex[], int strideInBytes);
	DLL_DECLSPEC void Triangulate (const float vertex[], int strideInBytes);
	DLL_DECLSPEC void OptimalTriangulation (const float vertex[], int strideInBytes);
	DLL_DECLSPEC void TriangulateFace (ZeroEdge *face, const float vertex[], int strideInBytes);


	DLL_DECLSPEC void ConvexPartition (const float vertex[], int strideInBytes);

	
	DLL_DECLSPEC int WireMeshing (ZeroFace face[], const float vertex[], int strideInBytes, bool useOptimalTrangulation = true);
	DLL_DECLSPEC ZeroSphere CalculateSphere (const float vertex[], int strideInBytes, const ZeroMatrix *basis = NULL);
	
	// this is a obsollete function
	DLL_DECLSPEC void PackVertex (ZeroPolyhedraDescriptor &desc, void* const destArray, void* const unpackArray, int srcStrideInBytes);
	DLL_DECLSPEC void PackVertex (ZeroPolyhedraDescriptor &desc, float destArray[], int dstStrideInBytes, const float unpackArray[], int srcStrideInBytes);

	
	DLL_DECLSPEC void Merge (ZeroPolyhedraDescriptor& myDesc, float myPool[], int myStrideInBytes,
									 const ZeroPolyhedra& he, const float hisPool[], int hisStrideInBytes);

	DLL_DECLSPEC void DeleteOverlapingFaces (const float pool[], int strideInBytes, float distTol);
	DLL_DECLSPEC void CombineOpenFaces (const float pool[], int strideInBytes, float distTol);

	DLL_DECLSPEC void GetConnectedFaces (ZeroVisList<ZeroEdge*>& faceList, ZeroEdge *startFace, int markValue = -1) const;
	DLL_DECLSPEC void GetCoplanarFaces (ZeroVisList<ZeroEdge*>& faceList, ZeroEdge *startFace,
													const float pool[], int strideInBytes, float normalDeviation, int markValue = -1) const;
	
	DLL_DECLSPEC void SavexMesh (const char *Name, const float pool[], int strideInBytes) const;
	
	DLL_DECLSPEC void Optimize (const float pool[], int strideInBytes, float tol);
	DLL_DECLSPEC void CollapseDegenerateFaces (ZeroPolyhedraDescriptor &desc, const float pool[], int strideInBytes, float area);

	// this function assume the mesh is a legal one;
	// note: recomend a call to Triangulate before using this fuinctions
	// return index count
	DLL_DECLSPEC int TriangleList (unsigned outputBuffer[], int maxBufferSize) const;


	// this function assume the mesh is a legal one;
	// note1: recomend a call to Triangulate before using this fuinctions
	// note2: a index set to 0xffffffff indicate a run start
	// return index count
	DLL_DECLSPEC int TriangleStrips (unsigned outputBuffer[], int maxBufferSize, int vertexCacheSize = 256) const;
	DLL_DECLSPEC int TriangleList (unsigned outputBuffer[], int maxBufferSize, int vertexCacheSize = 10) const;

};

#endif
#endif
