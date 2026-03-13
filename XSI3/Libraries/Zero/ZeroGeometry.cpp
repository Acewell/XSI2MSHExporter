/****************************************************************************
*
*  Visual C++ 6.0 created by: Julio Jerez
*
****************************************************************************/
#include "ZeroTypes.h"
#include "ZeroGeometry.h"
#include "ZeroSegment.h"


ZERO_RTTI_CONTRUCTOR(ZeroGeometry);							  

float ZeroGeometry::sRenderDistance = -1.0f;


int ZeroGeometry::maxSegmentFaceSize = 1024;
int ZeroGeometry::maxSegmentVertexSize = 1024;

	
void __cdecl ZeroGeometry::SetSegmentVertexAndFaceSize (int vertexSize, int faceSize)
{
	maxSegmentFaceSize = vertexSize;
	maxSegmentVertexSize = faceSize;
}

void __cdecl ZeroGeometry::GetSegmentVertexAndFaceSize (int& vertexSize, int& faceSize)
{
	vertexSize = maxSegmentFaceSize;
	faceSize = maxSegmentVertexSize;
}


//Apply a transform to the data
void ZeroGeometry::ApplyTransform(const ZeroMatrix& aMatrix)
{
	//No segments
	if(GetSegments() == NULL)
		return;

	ZeroSegmentList::Iterator it(*GetSegments());
	for(it.Begin(); it; it++)
	{
		ZeroSegment* seg = *it;
		if(seg)
		{
			const ZeroRenderDescriptor* desc = seg->AppLock();
			float* vertexPtr = desc->vertexRecord.position.ptr;
			int vertexStride = desc->vertexRecord.position.stride;

			float* normalPtr = desc->vertexRecord.normal.ptr;
			int normalStride = desc->vertexRecord.normal.stride;
			for(int t=0; t < desc->vertexCount; t++)
			{
				ZeroVector3 pos(aMatrix.TransformVector(ZeroVector3(vertexPtr[0],vertexPtr[1],vertexPtr[2])));
				vertexPtr[0] = pos.x;
				vertexPtr[1] = pos.y;
				vertexPtr[2] = pos.z;
				vertexPtr += vertexStride;

				ZeroVector3 normal(aMatrix.TransformVector(ZeroVector3(normalPtr[0],normalPtr[1],normalPtr[2])));
				normalPtr[0] = normal.x;
				normalPtr[1] = normal.y;
				normalPtr[2] = normal.z;
				normalPtr += normalStride;
			}
			seg->AppUnlock();
		}
	}
}


void ZeroGeometry::EnumerateByType	(
	ZeroObject *me, 
	int& index, 
	const ZeroObject** bufferOut, 
	unsigned typeId) const
{
	ZeroObject *obj;
	ZeroGeometry *geometry;

	bufferOut[index] = NULL;
	geometry = me->GetGeometry();
	if (geometry) {
		if (geometry->IsTypeID(typeId)) {
			bufferOut[index] = me;
		}
	}

	index ++;
	for (obj = me->GetChild(); obj; obj = obj->GetSibling()) {
		EnumerateByType(obj, index, bufferOut, typeId);
	}
}



