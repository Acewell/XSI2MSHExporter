/****************************************************************************
*
*  Visual C++ 6.0 created by: Julio Jerez
*
****************************************************************************/
#include "ZeroTypes.h"
#include "ZeroList.h"
#include "ZeroStack.h"
#include "ZeroDebug.h"
#include "ZeroCamera.h"
#include "ZeroGeoUtil.h"
#include "ZeroSegment.h"
#include "ZeroInterval.h"
#include "ZeroPolyhedra.h"
#include "ZeroRenderState.h"
#include "ZeroMeshGeometry.h"
#include "ZeroCameraSphere.h"
#include "ZeroRenderDescriptor.h"


ZERO_RTTI_CONTRUCTOR(ZeroMeshGeometry);							  

static const float DEFAULT_MAX_RENDER_DIST = 100000.0f;

static void DefualtRenderRenderFnt (ZeroCamera* camera, const ZeroRenderItem& context)
{
	const ZeroMatrix *worldMatrix;

	worldMatrix	= &context.worldMatrix;
	camera->SetWorldMatrix (1, &worldMatrix);
}


ZeroMeshGeometry::ZeroMeshGeometry(const char *name)
	:ZeroGeometry (name), sphere(), segments()
{
	renderFnct = DefualtRenderRenderFnt;
	setCustomData = NULL;
	sphereDirty	= true;
	mMaxDistance = DEFAULT_MAX_RENDER_DIST;
}

ZeroMeshGeometry::ZeroMeshGeometry(const ZeroGeometryData &data)
	:ZeroGeometry (data), sphere(), segments()
{
	renderFnct = DefualtRenderRenderFnt;
	setCustomData = NULL;
	sphereDirty	= true;
	if (data.dataGeometry) {
		ZeroSegmentList *segmentList = data.dataGeometry->GetSegments();
		if (segmentList) {
			ZeroSegmentList::Iterator iter(*segmentList);
			for (iter.Begin(); iter; iter ++) {
				AddSegment(*iter);
			}
		}
	}
	mMaxDistance = DEFAULT_MAX_RENDER_DIST;
}


ZeroMeshGeometry::ZeroMeshGeometry(const ZeroMeshGeometry &geometry)
	:ZeroGeometry (geometry), sphere(geometry.sphere), segments()
{
	renderFnct = geometry.renderFnct;
	setCustomData = geometry.setCustomData;
	sphereDirty	= geometry.sphereDirty;
	mMaxDistance = geometry.mMaxDistance;

	ZeroSegmentList::Iterator iter(geometry.segments);
	for (iter.Begin(); iter; iter ++) {
		AddSegment(*iter);
	}
}

ZeroMeshGeometry::~ZeroMeshGeometry()
{
	ZeroSegmentList::Iterator iter(segments);

	for (iter.Begin(); iter; iter ++) {
		(*iter)->Release();
	}
}


void ZeroMeshGeometry::AddSegment (ZeroSegment *segment)
{
	segments.Append (segment);
	segment->AddRef();
}


void ZeroMeshGeometry::RemoveSegment (ZeroSegment *segment)
{
	ZeroSegmentList::ZeroListNode *node;

	sphereDirty	= true;
	node = segments.Find (segment);
	if (node) {
		segments.Remove (node);
		segment->Release();
	}
}

#ifdef _DEBUG
static ZeroRenderState renderStatesList[] =
{
	ZeroRenderState (ZERO_RENDERSTATE_ALPHABLENDENABLE, FALSE),
	ZeroRenderState (ZERO_RENDERSTATE_LIGHTING, FALSE),
	ZeroRenderState (ZERO_RENDERSTATE_SRCBLEND, ZERO_BLEND_ONE),
	ZeroRenderState (ZERO_RENDERSTATE_DESTBLEND, ZERO_BLEND_ZERO),
	ZeroRenderState (ZERO_RENDERSTATE_END, 0),
};
#endif

#if 0
void ZeroMeshGeometry::DebugRender (
	ZeroCamera *camera, 
	const ZeroMatrix& worldMatrix, 
	ZeroMatrix*	matrixStack,
	unsigned count,
	ZeroColor color) const
{
#ifdef _DEBUG
	struct COLOR_VERTEX
	{
		ZeroVector3 vv;
		ZeroColor diffuse;
		COLOR_VERTEX(const ZeroVector3 &vv, const ZeroColor diffuse)
			: vv(vv), diffuse(diffuse)
		{
		}
	};

	// generate the geometry
	const ZeroSphere &S = sphere;
	COLOR_VERTEX point[] = 
	{
		COLOR_VERTEX(S.posit - S.right * S.size.x - S.up * S.size.y - S.front * S.size.z, color),
		COLOR_VERTEX(S.posit + S.right * S.size.x - S.up * S.size.y - S.front * S.size.z, color),
		COLOR_VERTEX(S.posit - S.right * S.size.x + S.up * S.size.y - S.front * S.size.z, color),
		COLOR_VERTEX(S.posit + S.right * S.size.x + S.up * S.size.y - S.front * S.size.z, color),
		COLOR_VERTEX(S.posit - S.right * S.size.x - S.up * S.size.y + S.front * S.size.z, color),
		COLOR_VERTEX(S.posit + S.right * S.size.x - S.up * S.size.y + S.front * S.size.z, color),
		COLOR_VERTEX(S.posit - S.right * S.size.x + S.up * S.size.y + S.front * S.size.z, color),
		COLOR_VERTEX(S.posit + S.right * S.size.x + S.up * S.size.y + S.front * S.size.z, color),
	};
	static unsigned short index[] = 
	{
		0, 1, 2, 3, 4, 5, 6, 7,
		0, 2, 1, 3, 4, 6, 5, 7,
		0, 4, 1, 5, 2, 6, 3, 7,
	};

	// create a render descriptor
	ZeroRenderDescriptor desc(camera);
	desc.primitive = ZERO_PRIMITIVE_LINELIST;
	desc.format = ZERO_FORMAT_DIFFUSE;
	desc.flags = ZERO_RENDERFLAG_CLIP;
	desc.material = NULL;
	desc.vertexCount = sizeof(point) / sizeof(point[0]); 
	desc.indexCount = sizeof(index) / sizeof(index[0]);

	// lock the render descriptor
	camera->Lock(desc, true);

	// fill in the render descriptor
	COLOR_VERTEX *ppoint = point;
	for (int i = 0; i < sizeof(point) / sizeof(point[0]); i++)
	{
		desc.vertexRecord.position.ptr[0] = ppoint->vv.x;
		desc.vertexRecord.position.ptr[1] = ppoint->vv.y;
		desc.vertexRecord.position.ptr[2] = ppoint->vv.z;
		desc.vertexRecord.position.ptr += desc.vertexRecord.position.stride;
		desc.vertexRecord.diffuse.color[0] = point->diffuse;
		desc.vertexRecord.diffuse.color += desc.vertexRecord.diffuse.stride;
	}
	memcpy(desc.indexArray, index, sizeof(index));

	// unlock the render descriptor
	camera->Unlock(desc);

	// set the render state
	camera->SetRenderStates(renderStatesList);
	camera->SetTextureStages(0, NULL, NULL);

	// set the camera matrix
	const ZeroMatrix *matrix = &worldMatrix;
	camera->SetWorldMatrix(count, &matrix);
	
	// render the wireframe
	camera->Render(desc);
#endif
}

void ZeroMeshGeometry::DebugRender (
	ZeroCamera *camera, 
	const ZeroMatrix& worldMatrix, 
	ZeroColor color) const
{
#ifdef _DEBUG
	struct COLOR_VERTEX
	{
		ZeroVector3 vv;
		ZeroColor diffuse;
		COLOR_VERTEX(const ZeroVector3 &vv, const ZeroColor diffuse)
			: vv(vv), diffuse(diffuse)
		{
		}
	};

	// generate the geometry
	const ZeroSphere &S = sphere;
	COLOR_VERTEX point[] = 
	{
		COLOR_VERTEX(S.posit - S.right * S.size.x - S.up * S.size.y - S.front * S.size.z, color),
		COLOR_VERTEX(S.posit + S.right * S.size.x - S.up * S.size.y - S.front * S.size.z, color),
		COLOR_VERTEX(S.posit - S.right * S.size.x + S.up * S.size.y - S.front * S.size.z, color),
		COLOR_VERTEX(S.posit + S.right * S.size.x + S.up * S.size.y - S.front * S.size.z, color),
		COLOR_VERTEX(S.posit - S.right * S.size.x - S.up * S.size.y + S.front * S.size.z, color),
		COLOR_VERTEX(S.posit + S.right * S.size.x - S.up * S.size.y + S.front * S.size.z, color),
		COLOR_VERTEX(S.posit - S.right * S.size.x + S.up * S.size.y + S.front * S.size.z, color),
		COLOR_VERTEX(S.posit + S.right * S.size.x + S.up * S.size.y + S.front * S.size.z, color),
	};
	static unsigned short index[] = 
	{
		0, 1, 2, 3, 4, 5, 6, 7,
		0, 2, 1, 3, 4, 6, 5, 7,
		0, 4, 1, 5, 2, 6, 3, 7,
	};

	// create a render descriptor
	ZeroRenderDescriptor desc(camera);
	desc.primitive = ZERO_PRIMITIVE_LINELIST;
	desc.format = ZERO_FORMAT_DIFFUSE;
	desc.flags = ZERO_RENDERFLAG_CLIP;
	desc.material = NULL;
	desc.vertexCount = sizeof(point) / sizeof(point[0]); 
	desc.indexCount = sizeof(index) / sizeof(index[0]);

	// lock the render descriptor
	camera->Lock(desc, true);

	// fill in the render descriptor
	COLOR_VERTEX *ppoint = point;
	for (int i = 0; i < sizeof(point) / sizeof(point[0]); i++)
	{
		desc.vertexRecord.position.ptr[0] = ppoint->vv.x;
		desc.vertexRecord.position.ptr[1] = ppoint->vv.y;
		desc.vertexRecord.position.ptr[2] = ppoint->vv.z;
		desc.vertexRecord.position.ptr += desc.vertexRecord.position.stride;
		desc.vertexRecord.diffuse.color[0] = point->diffuse;
		desc.vertexRecord.diffuse.color += desc.vertexRecord.diffuse.stride;
	}
	memcpy(desc.indexArray, index, sizeof(index));

	// unlock the render descriptor
	camera->Unlock(desc);

	// set the render state
	camera->SetRenderStates(renderStatesList);
	camera->SetTextureStages(0, NULL, NULL);

	// set the camera matrix
	const ZeroMatrix *matrix = &worldMatrix;
	camera->SetWorldMatrix(1, &matrix);
	
	// render the wireframe
	camera->Render(desc);
#endif
}

#endif

void ZeroMeshGeometry::SetSphere()
{

	int i;
	int stride;
	int vCount;
	int strideInBytes;
	int segVertexCount;
	float *vertex;
	ZeroSegment *segment;


	vCount = 0;
	ZeroSegmentList::Iterator iter(segments);
	for (iter.Begin(); iter; iter ++) {
		segment = *iter;
		vCount += segment->GetVertexCount();
	}

	if (vCount == 0) {
		return;
	}


	ZeroVector3 *pool;
	ZeroStack<ZeroVector3> vertexPool(vCount); 

	vCount = 0;
	pool = &vertexPool[0];
	for (iter.Begin(); iter; iter ++) {
		segment = *iter;

		const ZeroRenderDescriptor &desc = *segment->AppLock();

		vertex = desc.vertexRecord.position.ptr;
		stride = desc.vertexRecord.position.stride;

		segVertexCount = segment->GetVertexCount();
		for (i = 0; i < segVertexCount; i ++) {
			pool[vCount].x = vertex[0];
			pool[vCount].y = vertex[1];
			pool[vCount].z = vertex[2];
			vertex += stride;
			vCount ++;
		}
		segment->AppUnlock();
	}
	strideInBytes = sizeof (ZeroVector3);

	ZeroSphere	sph;

#ifdef TOOL_DEVELOPMENT 
	ZeroPolyhedra *hull;
	hull = NULL;
	if (vCount > 16) {
		hull = CreateConvexHull (&pool[0].x, strideInBytes, vCount);
	}


	if (hull) {
		sph = hull->CalculateSphere(&pool[0].x, strideInBytes);
		delete hull;
	} else {
		sph.SetDimensions (&pool[0].x, strideInBytes, vCount);
	}
#else
	sph.SetDimensions (&pool[0].x, strideInBytes, vCount);
#endif

	SetSphere(sph);
}

void ZeroMeshGeometry::TotalVertexAndFaceCount(
	int& faceCount, 
	int& vertexCount)	const
{
	ZeroSegment *segment;

	ZeroSegmentList::Iterator iter(segments);
	for (iter.Begin(); iter; iter ++) {
		segment = *iter;
		faceCount += segment->GetFaceCount();
		vertexCount += segment->GetVertexCount();
	}
}


void ZeroMeshGeometry::FlatFaceArray (
	const ZeroMatrix &matrix, 
	float * const vertexPool, 
	int strideInBytes, 
	int &vCount, 
	int maxVertex, 
	ZeroFace* const facePool, 
	int &fCount,	
	int maxFaces) const
{
	int trgStride;
	int srcStride;
	int vertexBase;
	int segFaceCount;
	int segVertexCount;
	float* pool; 
	float* vertex;
	ZeroFace* face; 
	ZeroSegment *segment;

	trgStride = strideInBytes / sizeof (float);

	vertexBase = vCount;

	pool = vertexPool + vCount * trgStride;
	face = facePool + fCount;

	ZeroSegmentList::Iterator iter(segments);
	for (iter.Begin(); iter; iter ++) {
		segment = *iter;
		segVertexCount = segment->GetVertexCount();
		if ((vCount + segVertexCount) > maxVertex) {
			return;
		}

		segFaceCount = segment->GetFaceCount();
		if ((fCount + segFaceCount) > maxFaces) {
			return;
		}

		const ZeroRenderDescriptor& desc = *segment->AppLock();

		srcStride = desc.vertexRecord.position.stride;

		if(srcStride == 2) {
			segment->AppUnlock();
			return;
		}


		vertex = desc.vertexRecord.position.ptr;
		
		matrix.TransformVectors (pool, strideInBytes, vertex, srcStride * sizeof(float), segVertexCount);
		pool += segVertexCount * trgStride;

		segment->GetFaceTable(face, vertexBase);
		face += segFaceCount;
		vertexBase += segVertexCount;

		fCount += segFaceCount;
		vCount += segVertexCount;

		segment->AppUnlock();
	}
}


void ZeroMeshGeometry::Scale(float scale)
{

	ZeroSegment *segment;

	ZeroSegmentList::Iterator iter(segments);
	for (iter.Begin(); iter; iter ++) {
		segment = *iter;
		segment->Scale (scale);
	}

	sphere.posit = sphere.posit.Scale (scale);
	sphere.Scale (scale, scale, scale); 

}


void ZeroMeshGeometry::PackVertexFormat()
{
	_ASSERTE (0);
/*
	ZeroSegment *segment;

	ZeroGeometry::PackVertexFormat();

	ZeroSegmentList::Iterator iter(segments);
	for (iter.Begin(); iter; iter ++) {
		segment = *iter;
		segment->PackVertexFormat();
	}
*/
}


void ZeroMeshGeometry::GetExtents(ZeroVector3 &min, ZeroVector3 &max, const ZeroMatrix& worldMatrix)
{
	// generate the geometry
	const ZeroSphere &S = sphere;
	ZeroVector3 point[] = 
	{
		ZeroVector3(S.posit - S.right * S.size.x - S.up * S.size.y - S.front * S.size.z),
		ZeroVector3(S.posit + S.right * S.size.x - S.up * S.size.y - S.front * S.size.z),
		ZeroVector3(S.posit - S.right * S.size.x + S.up * S.size.y - S.front * S.size.z),
		ZeroVector3(S.posit + S.right * S.size.x + S.up * S.size.y - S.front * S.size.z),
		ZeroVector3(S.posit - S.right * S.size.x - S.up * S.size.y + S.front * S.size.z),
		ZeroVector3(S.posit + S.right * S.size.x - S.up * S.size.y + S.front * S.size.z),
		ZeroVector3(S.posit - S.right * S.size.x + S.up * S.size.y + S.front * S.size.z),
		ZeroVector3(S.posit + S.right * S.size.x + S.up * S.size.y + S.front * S.size.z),
	};

	for (int i = 0; i < 8; i++)
	{
		ZeroVector3 tmp;
		tmp = worldMatrix.TransformVector(point[i]);

		if (tmp.x < min.x)
			min.x = tmp.x;
		if (tmp.y < min.y)
			min.y = tmp.y;
		if (tmp.z < min.z)
			min.z = tmp.z;

		if (tmp.x > max.x)
			max.x = tmp.x;
		if (tmp.y > max.y)
			max.y = tmp.y;
		if (tmp.z > max.z)
			max.z = tmp.z;
	}
}


/*
DLL_DECLSPEC void ZeroMeshGeometry::ApplyTransform(const ZeroMatrix& aMatrix)
{
	ZeroSegmentList::Iterator iter(segments);
	for (iter.Begin(); iter; iter ++) 
	{
		(*iter)->ApplyTransform(aMatrix);
	}
}
*/





void ZeroMeshGeometry::Render (
	const ZeroObject *owner, 
	ZeroCamera *camera, 
	const ZeroMatrix &worldMatrix, 
	unsigned flags) const
{
	int clipFlag = 0;	// This was being passed down uninitialized!
	unsigned priority;
	ZeroSegment *segment;

	if (segments.GetCount() == 0) {
		return;
	}


//	if(mMaxDistance < sRenderDistance)
//		return;

	float size = sphere.size.w;
	if(size < 3.0f && sRenderDistance > 80 + size * 70.0f)
		return;

//SEN: i need the clipFlag for the ps2
//TODO: this test is now done twice, take one out !!
	 // Test geometry sphere
//	INTERVAL0_IN(" Cul");
	clipFlag = TrivialRejection (camera, worldMatrix, flags);
//	INTERVAL0_OUT(" Cul");
	if (clipFlag < 0) {
		return;
	}
	if (clipFlag == 0) {
		flags &= ~ZERO_RENDERFLAG_CLIP;
	}

//	if (flags & ZERO_RENDERFLAG_CLIP)
//		clipFlag = 1;

	// if rendering immediate...
	if (flags & ZERO_RENDERFLAG_IMMEDIATE)
	{

		// create a fake render item
		ZeroRenderItem renderItem;
		renderItem.owner = owner;
		renderItem.fnt = renderFnct;
		renderItem.worldMatrix = worldMatrix;
		renderItem.mColor = camera->mRenderColor;
		renderItem.mTerrainColor = camera->mTerrainColor;		

		
		ZeroCameraSphere sphere;

		// for each segment...
		ZeroSegmentList::Iterator iter(segments);
		for (iter.Begin(); iter; iter ++)
		{
			// get the segment
			segment = *iter;

			// if the segment is not hidden...
			if (!segment->GetHiddenFlag())
			{
				int clipSegment = clipFlag;
				int flagsSegment = flags;
				if ((clipSegment != 0) && (segments.GetCount() > 1))
				{
					sphere = segment->GetSphere();
					clipSegment = sphere.VisibilityTest(camera, worldMatrix);
					if (clipSegment < 0)
						continue;
					if (clipSegment == 0)
						flagsSegment &= ~ZERO_RENDERFLAG_CLIP;
				}
				renderItem.appParameter = clipSegment;
				renderItem.flags = flagsSegment;

				renderItem.segment = segment;
				if (setCustomData) 
				{
					setCustomData (camera, renderItem);
				}
				camera->RenderItem(renderItem, 0);
				
			}
		}
	}
	else
	{
		/*		const ZeroMatrix *mat;
		mat	= &worldMatrix;
		camera->SetWorldMatrix (1, &mat);
		
		ZeroRenderItem dummyItem;*/

		ZeroCameraSphere sphere;

		ZeroSegmentList::Iterator iter(segments);
		for (iter.Begin(); iter; iter ++) {
			segment = *iter;

			if (!segment->GetHiddenFlag()) 
			{
//				segment->Render(camera, dummyItem);
				int clipSegment = clipFlag;
				int flagsSegment = flags;
				if ((clipSegment != 0) && (segments.GetCount() > 1))
				{
					sphere = segment->GetSphere();
					clipSegment = sphere.VisibilityTest(camera, worldMatrix);
					if (clipSegment < 0)
						continue;
					if (clipSegment == 0)
						flagsSegment &= ~ZERO_RENDERFLAG_CLIP;
				}

				priority	= segment->GetRenderPriority (camera, worldMatrix);
				ZeroRenderItem& renderItem = camera->GetRenderItem(priority);
				
				renderItem.owner = owner;
				renderItem.fnt	= renderFnct;
				renderItem.segment = segment;
				renderItem.worldMatrix = worldMatrix;
				renderItem.appParameter = clipSegment;
				renderItem.mColor = camera->mRenderColor;
				renderItem.mTerrainColor = camera->mTerrainColor;		
				renderItem.flags = flagsSegment;

				if (setCustomData) {
					setCustomData (camera, renderItem);
				}
			}
		}
	}
}  	

