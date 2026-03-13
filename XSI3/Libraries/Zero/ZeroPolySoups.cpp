/****************************************************************************
*
*  File Name  : Bitmap.C
*  Visual C++ 4.0 base by Julio Jerez
*
****************************************************************************/
#include "ZeroTypes.h"
#include "ZeroStack.h"
#include "ZeroMatrix.h"
#include "ZeroCamera.h"
#include "ZeroGeoUtil.h"
#include "ZeroPolySoups.h"
#include "ZeroPolyhedra.h"
#include "ZeroRenderState.h"
#include "ZeroRenderDescriptor.h" 


ZERO_RTTI_CONTRUCTOR(ZeroPolySoup);


struct ZeroPolySoupRenderVertex
{
	float x;
	float y;
	float z;
	ZeroColor color;
};

static ZeroRenderState renderStates[] =
{
	ZeroRenderState (ZERO_RENDERSTATE_FOGENABLE, FALSE),
	ZeroRenderState (ZERO_RENDERSTATE_COLORVERTEX, true),
	ZeroRenderState (ZERO_RENDERSTATE_LIGHTING, FALSE),
	ZeroRenderState (ZERO_RENDERSTATE_ALPHABLENDENABLE, FALSE),
	ZeroRenderState (ZERO_RENDERSTATE_END, 0),
};


struct ZeroPolySoupRenderContext
{
	int count;
	int maxCount;
	ZeroColor edgeColor;
	ZeroCamera *camera;
	ZeroMatrix worldMatrix;
	ZeroPolySoupRenderVertex *vertexPool;

	ZeroPolySoupRenderContext (
		const ZeroCamera* cam, 
		const ZeroMatrix& mat, 
		ZeroPolySoupRenderVertex* pool,
		int mCount,
		ZeroColor eColor)
	{
		count = 0;
		camera = (ZeroCamera*) cam;
		worldMatrix = mat;
		vertexPool = pool;
		maxCount	= mCount;
		edgeColor = eColor;
	}

	static ZeroIntersectStatus Render  (
		void *contextArg, 
		const int indexArray[], 
		int indexCount,
		const float polygon[], 
		int strideInBytes)
	{
		int i;
		int j;
		int stride;
		int index;
		ZeroPolySoupRenderContext* context;
	
		if (!indexCount) {
			return continueSearch;
		}
	
		stride = strideInBytes / sizeof (float);
		context = (ZeroPolySoupRenderContext*) contextArg;
		
		index = context->count; 
	
		ZeroPolySoupRenderVertex p0;
		j = indexArray[indexCount-1] * stride;
		p0.x = polygon[j + 0];
		p0.y = polygon[j + 1];
		p0.z = polygon[j + 2];
		p0.color = context->edgeColor;
	
		for (i = 0; i < indexCount; i ++) {
			j = indexArray[i] * stride;
			ZeroPolySoupRenderVertex p1;
	
			p1.x = polygon[j + 0];
			p1.y = polygon[j + 1];
			p1.z = polygon[j + 2];
			p1.color = context->edgeColor;
	
			context->vertexPool[index * 2 + 0] = p0;
			context->vertexPool[index * 2 + 1] = p1;
			p0 = p1;
	
			index ++;
			if (index >= context->maxCount) {
				ZeroMatrix tmpMat (context->worldMatrix);
				const ZeroMatrix *matrix; 
	
				matrix = &tmpMat;
				context->camera->SetWorldMatrix (1, &matrix);
	
				context->camera->SetRenderStates (renderStates);
				context->camera->SetTextureStages (0, NULL, NULL);
	
				// create the render descriptor
				ZeroRenderDescriptor desc (context->camera);
				desc.primitive = ZERO_PRIMITIVE_LINELIST;
				desc.format = ZERO_FORMAT_DIFFUSE;
				desc.flags = ZERO_RENDERFLAG_CLIP;
				desc.material = NULL;
				desc.vertexCount = context->count * 2;
				desc.indexCount = 0;
	
				// lock the render descriptor
				context->camera->Lock(desc, true);
	
				// fill in the render descriptor
				ZeroPolySoupRenderVertex *src = context->vertexPool;
				for (int i = 0; i < context->count; i++)
				{
					desc.vertexRecord.position.ptr[0] = src->x;
					desc.vertexRecord.position.ptr[1] = src->y;
					desc.vertexRecord.position.ptr[2] = src->z;
					desc.vertexRecord.position.ptr += desc.vertexRecord.position.stride;
					desc.vertexRecord.diffuse.color[0] = src->color;
					desc.vertexRecord.diffuse.color += desc.vertexRecord.diffuse.stride;
					src++;
					desc.vertexRecord.position.ptr[0] = src->x;
					desc.vertexRecord.position.ptr[1] = src->y;
					desc.vertexRecord.position.ptr[2] = src->z;
					desc.vertexRecord.position.ptr += desc.vertexRecord.position.stride;
					desc.vertexRecord.diffuse.color[0] = src->color;
					desc.vertexRecord.diffuse.color += desc.vertexRecord.diffuse.stride;
					src++;
				}
	
				// unlock the render descriptor
				context->camera->Unlock(desc);
	
				// render the descriptor
				context->camera->Render (desc);
	
				index	= 0;
			}
		}
	
		context->count	= index; 
	
		return continueSearch;
	}
};


ZeroPolySoupBuilder::ZeroPolySoupBuilder ()
	:m_faceArray(), m_indexArray(), m_vertexArray()
{
	m_faceCount = 0;
	m_indexCount = 0;
	m_vertexCount = 0;
}

void ZeroPolySoupBuilder::Begin()
{
	m_faceCount = 0;
	m_indexCount = 0;
	m_vertexCount = 0;
}

void ZeroPolySoupBuilder::AddMesh (
	const float vertex[], 
	int vertexCount, 
	int strideInBytes, 
	int faceCount, 
	const int faceIndexCount[], 
	const int faceID[], 
	const int faceIndices[], 
	const ZeroMatrix& worldMatrix)
{
	int i;
	int j;
	int index;
	int count;
	int indexCount;
	int* dstIndex;
	const int* srcIndex;

	m_vertexArray[m_vertexCount + vertexCount].x = 0.0f;
	worldMatrix.TransformVectors (&m_vertexArray[m_vertexCount].x, sizeof (ZeroVector3), vertex, strideInBytes , vertexCount);

	indexCount = 0;
	for (i = 0; i < faceCount; i ++) {
		indexCount += faceIndexCount[i] + 1;
	}

	m_faceArray[m_faceCount + faceCount] = 0;
	m_indexArray[m_indexCount + indexCount] = 0;

	srcIndex = &faceIndices[0];
	dstIndex = &m_indexArray[m_indexCount];
	for (i = 0; i < faceCount; i ++) {
		count = faceIndexCount[i];
		m_faceArray[m_faceCount	+ i] = count + 1;

		dstIndex[0] = faceID[i];
		dstIndex ++;
		for (j = 0; j < count; j ++) {
			index = m_vertexCount + srcIndex[0];
			dstIndex[0] = index;
			srcIndex ++;
			dstIndex ++;
		}
	}

	m_faceCount += faceCount;
	m_indexCount += indexCount;
	m_vertexCount += vertexCount;

}

void ZeroPolySoupBuilder::End()
{

	int i;
	int j;
	int k;
	int count;
	int index;
	int* indexArray;
	unsigned* indexMap;
	ZeroStack<unsigned> indexMapPool (m_indexCount);

	indexMap	= &indexMapPool[0];
	m_vertexCount = VertexListToIndexList (&m_vertexArray[0].x, sizeof (ZeroVector3), m_vertexCount, &indexMap[0], 0, 1.0e-3f);

	indexArray = &m_indexArray[0];
	for (i = 0; i < m_faceCount; i ++) {
		count = m_faceArray[i];
		indexArray ++;
		for (j = 0; j < count - 1; j ++) {
			index = indexArray[0];
			index = indexMap[index];
			indexArray[0] = index;
			indexArray ++; 
		}
	}


	// rotate polygon with bad normals
	k = 0;
	for (i = 0; i < m_faceCount; i ++) {
		int i0;
		int i1;
		int i2;
		float mag2;

		count = m_faceArray[i];
		for (j = 0; j < count - 1; j ++) {
			indexMap[j] = m_indexArray[k + j + 1];
		}
		for ( ;j < 2 * (count - 1); j ++) {
			indexMap[j] = indexMap[j - (count - 1)];
		}

		for (j = 0; j < count - 1; j ++) {
			i0 = indexMap[j + 0]; 
			i1 = indexMap[j + 1]; 
			i2 = indexMap[j + 2]; 

			ZeroVector4 p0 (&m_vertexArray[i0].x);
			ZeroVector4 p1 (&m_vertexArray[i1].x);
			ZeroVector4 p2 (&m_vertexArray[i2].x);

			ZeroVector4	p10 (p1 - p0);
			ZeroVector4	p20 (p2 - p0);

			p10 = p10.Scale (rsqrt (DotProduct (p10, p10)));
			p20 = p20.Scale (rsqrt (DotProduct (p20, p20)));
			ZeroVector4 n (CrossProduct (p10, p20));
			mag2 = DotProduct (n, n);
			if (mag2 > 1.0e-1f * 1.0e-1f) {
				if (j) {
					for (i0 = 0; i0 < count - 1; i0 ++) {
						m_indexArray[k + i0 + 1] = indexMap[i0 + j];
					}
				}
				break;
			}
		}
		k += count;
	}


}




ZeroPolySoup::ZeroPolySoup ()
{
}


ZeroPolySoup::~ZeroPolySoup ()
{
}


void ZeroPolySoup::DebugRender(const ZeroCamera* camera, const ZeroMatrix& worldMatrix, ZeroColor color) const
{
	ZeroVector3 p0;
	ZeroVector3 p1;

	ZeroStack<ZeroPolySoupRenderVertex> vertexPool (1024 * 3);


	color = ZeroColor (255, 0, 0, 128);
	camera->GetCameraBox (p0, p1);
	ZeroVector3 p2 = worldMatrix.UntransformVector(p0);
	ZeroVector3 p3 = worldMatrix.UntransformVector(p1);

	p0 = ZeroVector3(min(p2.x, p3.x) - 50, min(p2.y, p3.y) - 50, min(p2.z, p3.z) - 50);
	p1 = ZeroVector3(max(p2.x, p3.x) + 50, max(p2.y, p3.y) + 50, max(p2.z, p3.z) + 50);

	ZeroPolySoupRenderContext context (camera, worldMatrix, &vertexPool[0], 1024, color);
	ForAllSectors (p0, p1, ZeroPolySoupRenderContext::Render, &context);

	if (context.count)	{
		ZeroMatrix tmpMat (context.worldMatrix);
		const ZeroMatrix *matrix; 

		matrix = &tmpMat;
		context.camera->SetWorldMatrix (1, &matrix);

		context.camera->SetRenderStates (renderStates);
		context.camera->SetTextureStages (0, NULL, NULL);

		// create the render descriptor
		ZeroRenderDescriptor desc (context.camera);
		desc.primitive = ZERO_PRIMITIVE_LINELIST;
		desc.format = ZERO_FORMAT_DIFFUSE;
		desc.flags = ZERO_RENDERFLAG_CLIP;
		desc.material = NULL;
		desc.vertexCount = context.count * 2;
		desc.indexCount = 0;

		// lock the render descriptor
		context.camera->Lock(desc, true);

		// fill in the render descriptor
		ZeroPolySoupRenderVertex *src = context.vertexPool;
		for (int i = 0; i < context.count; i++) {
			desc.vertexRecord.position.ptr[0] = src->x;
			desc.vertexRecord.position.ptr[1] = src->y;
			desc.vertexRecord.position.ptr[2] = src->z;
			desc.vertexRecord.position.ptr += desc.vertexRecord.position.stride;
			desc.vertexRecord.diffuse.color[0] = src->color;
			desc.vertexRecord.diffuse.color += desc.vertexRecord.position.stride;
			src++;
			desc.vertexRecord.position.ptr[0] = src->x;
			desc.vertexRecord.position.ptr[1] = src->y;
			desc.vertexRecord.position.ptr[2] = src->z;
			desc.vertexRecord.position.ptr += desc.vertexRecord.position.stride;
			desc.vertexRecord.diffuse.color[0] = src->color;
			desc.vertexRecord.diffuse.color += desc.vertexRecord.position.stride;
			src++;
		}

		// unlock the render descriptor
		context.camera->Unlock(desc);

		// render the descriptor
		context.camera->Render (desc);
	}
}

float ZeroPolySoup::RayHit (
	const ZeroVector3& p0, 
	const ZeroVector3& p1, 
	ZeroVector3& normal) const
{
	ZeroPolySoupRayHit	ray (p0, p1);
	ForAllSectorsRayHit (p0, p1, 	ZeroPolySoupRayHit::RayHit, &ray);
	normal = ray.normal;
	return ray.t;
}


float ZeroPolySoup::RayTest (const ZeroVector3& p0, const ZeroVector3& p1) const
{
  ZeroPolySoupRayHit ray (p0, p1);
  ForAllSectorsRayHit (p0, p1, ZeroPolySoupRayHit::RayTest, &ray);
  return ray.t;
}






