/****************************************************************************
*
*  Visual C++ 6.0 created by: Julio Jerez
*
****************************************************************************/
#include "ZeroTypes.h"
#include "ZeroStack.h"
#include "ZeroDebug.h"
#include "ZeroCamera.h"
#include "ZeroMemory.h"
#include "ZeroSegment.h"
#include "ZeroSurface.h"
#include "ZeroPolyhedra.h"
#include "ZeroMaterial.h"


ZERO_RTTI_CONTRUCTOR(ZeroSegment);


#ifdef TOOL_DEVELOPMENT 

#if (PLATFORM == PLATFORM_PS2)

void ZeroSegment::TriangleStrips ()
{
	ZeroPolyhedra polyMsh;
	
	ZeroStack<ZeroFace> faces(GetFaceCount());
	int faceCount = GetFaceCount();

	int ii;
	unsigned count;
	unsigned outCount=0;
	unsigned numAccumulatedFaces=0;
	// create a Zero polyhedra from the segment
	polyMsh.BeginFace();
	for (ii = 0; ii < faceCount; ii ++) {
		polyMsh.AddFace (faces[ii].index[0], faces[ii].index[1], faces[ii].index[2]); 
	}
	polyMsh.EndFace();

	// generate triangle strip indices
	ZeroStack<unsigned> indexPool(faceCount * 4);
	int indexCount = polyMsh.TriangleStrips(&indexPool[0], faceCount * 4);
	
	
	// connect the strip
	ZeroStack<unsigned> outIndices(indexCount*2);
	// the first strip
	memcpy(&outIndices[0],&indexPool[1],indexPool[0]*sizeof(unsigned));
	// duplicate the last index of the first strip
	outIndices[indexPool[0]]=outIndices[indexPool[0]-1];
	outCount=indexPool[0]+1;
	numAccumulatedFaces=(indexPool[0])-2;
	
	//ii=indexPool[0]+1; 
	for (ii =indexPool[0]+1 ; ii < indexCount;)
	{
		//get the count
		count = indexPool[ii];
		//duplicate the first index
		outIndices[outCount++]=indexPool[ii+1];
		// if the number of accumulated tris is odd, we duplicatre the first one twice
		if(numAccumulatedFaces%2){
			outIndices[outCount++]=indexPool[ii+1];
		//	numAccumulatedFaces++;
		}
		//copy the indices
		memcpy(&outIndices[outCount],&indexPool[ii+1],count*sizeof(unsigned)); 
		//increment the output buffer position
		outCount+=count;
		numAccumulatedFaces=(count)-2;
		//duplicate the last index
		outIndices[outCount]=outIndices[outCount-1];
		outCount++;
		// increment the counter
		ii+=count+1;
	}
	
	
	/*	mDesc.primitive=ZERO_PRIMITIVE_TRIANGLESTRIP;

	if (mIndexData)
		delete [] mIndexData;
	mIndexCount = outCount;
	mIndexData = ZeroNew (unsigned short[mIndexCount]);
	for (ii = 0; ii < mIndexCount; ii++)
		mIndexData[ii] = (unsigned short)outIndices[ii];
	*/
}

#endif

/*
void ZeroSegment::AddBackFaceToSeg()
{
	unsigned short *fIndexData = ZeroNew (unsigned short[mIndexCount * 2]);
	memcpy(fIndexData, mIndexData, mIndexCount * sizeof(unsigned short));
	for (int i = 0 ; i < mIndexCount; i += 3)
	{
		fIndexData[mIndexCount + i] = mIndexData[i + 2];
		fIndexData[mIndexCount + i + 1] = mIndexData[i + 1];
		fIndexData[mIndexCount + i + 1] = mIndexData[i];
	}
	mIndexCount *= 2;
	delete [] mIndexData;
	mIndexData = fIndexData;
}


void  ZeroSegment::ComputeNormals()
{
	if(!(GetVertexFormat() & ZERO_FORMAT_NORMAL)) 
		return;
	int i;
	ZeroStack<ZeroFace> Zface(GetFaceCount());
	int faceCount = GetFaceTable(&Zface[0]);
	float *Vertex  = GetVertexRecord().position.ptr;
	float *Normals = GetVertexRecord().normal.ptr;
	int stride = GetVertexRecord().position.stride;
	ZeroVector3 *P0, *P1, *P2 ; 
	ZeroVector3 *N0, *N1, *N2 ; 
	int			ind0,ind1,ind2;
	ZeroVector3	FaceNormal;
	float length;
	for(i = 0; i < stride * GetVertexCount(); i+=stride)
	{
		Normals[i+0] = 0.f;
		Normals[i+1] = 0.f;
		Normals[i+2] = 0.f;	
	}
	for(i = 0 ; i< faceCount; i++)
	{
		ind0 = Zface[i].index[0]*stride;
		ind1 = Zface[i].index[1]*stride;
		ind2 = Zface[i].index[2]*stride;

		P0 = (ZeroVector3*)(&Vertex[ind0]);
		P1 = (ZeroVector3*)(&Vertex[ind1]);
		P2 = (ZeroVector3*)(&Vertex[ind2]);

		N0 = (ZeroVector3*)(&Normals[ind0]);
		N1 = (ZeroVector3*)(&Normals[ind1]);
		N2 = (ZeroVector3*)(&Normals[ind2]);

		FaceNormal = CrossProduct((*P1) - (*P0),(*P2) - (*P0));
		*N0 += FaceNormal;
		*N1 += FaceNormal;
		*N2 += FaceNormal;
	}
	for(i = 0; i < stride * GetVertexCount(); i+=stride)
	{
		length = (Normals[i] * Normals[i]) + (Normals[i+1] * Normals[i+1]) + (Normals[i+2] * Normals[i+2]); 
		if (length < 0.001f) {
			length = 0.001f;
		}
		length = rsqrt (length);
		Normals[i+0] *= length;
		Normals[i+1] *= length;
		Normals[i+2] *= length;	
	}
}



void ZeroSegment::UpdateVertexRecord (
	ZeroCamera *camera, 
	const ZeroVertexRecord& sourceData, 
	const int* indirectVertexList, 
	int vertexIndexCount,
	const unsigned short *indexList, 
	int indexCount)
{
	int i;
	int index;
	int srcVertexStride;
	int dstVertexStride;
	float *dstVertexPtr;
	const float *srcVertexPtr;
	int srcNormalStride;
	int dstNormalStride;
	float *dstNormalPtr;
	const float *srcNormalPtr;
	int srcUV0Stride;
	int dstUV0Stride;
	float *dstUV0Ptr;
	const float *srcUV0Ptr;


	ZeroRenderDescriptor& desc = Lock (camera);


	_ASSERTE (desc.format == sourceData.format);


	srcVertexStride = sourceData.position.stride;
	srcNormalStride = sourceData.normal.stride;
	srcUV0Stride = sourceData.uv[0].stride;

	dstVertexStride = desc.vertexRecord.position.stride;
	dstNormalStride = desc.vertexRecord.normal.stride;
	dstUV0Stride = desc.vertexRecord.uv[0].stride;

	srcVertexPtr = sourceData.position.ptr;
	srcNormalPtr = sourceData.normal.ptr;
	srcUV0Ptr = sourceData.uv[0].ptr;

	dstVertexPtr = desc.vertexRecord.position.ptr;
	dstNormalPtr = desc.vertexRecord.normal.ptr;
	dstUV0Ptr = desc.vertexRecord.uv[0].ptr;

	for (i = 0; i < vertexIndexCount; i ++) {
		int indexByStride;
		index = indirectVertexList[i];

		indexByStride = index * srcVertexStride;
		dstVertexPtr[0] = srcVertexPtr[indexByStride];
		dstVertexPtr[1] = srcVertexPtr[indexByStride + 1];
		dstVertexPtr[2] = srcVertexPtr[indexByStride + 2];
		

		dstVertexPtr += dstVertexStride;


		if (desc.format & ZERO_FORMAT_NORMAL) {
			indexByStride = index * srcNormalStride;

			dstNormalPtr[0] = srcNormalPtr[indexByStride];
			dstNormalPtr[1] = srcNormalPtr[indexByStride + 1];
			dstNormalPtr[2] = srcNormalPtr[indexByStride + 2];

			dstNormalPtr += dstNormalStride;
		}


		if (desc.format &  ZERO_FORMAT_TEX1) {	
			indexByStride = index * srcUV0Stride;

			dstUV0Ptr[0] = srcUV0Ptr[indexByStride];
			dstUV0Ptr[1] = srcUV0Ptr[indexByStride + 1];

			dstUV0Ptr += dstUV0Stride;

		}
	}

	memcpy (desc.indexArray, indexList,	sizeof (short) * indexCount * 3);

	desc.camera->Unlock(desc);
}
*/


int ZeroSegment::CreatePolyhedra (
	ZeroPolyhedra& polyhedrum,
	float const* pool, 
	int strideInBytes,
	const ZeroMatrix& worldMatrix)
{
	int i;
	int j;
	int i0;
	int i1;
	int i2;
	int faceCount;
	int vCount;
	int vertexStride;
	unsigned *facePoolPtr;
	ZeroEdge *face;
	float* vertex;
	ZeroVector3 *vertexPoolPtr;

	faceCount = GetFaceCount();

	ZeroStack<ZeroFace> faceTable(faceCount);
	ZeroStack<unsigned> facePool(faceCount * 3); 
	ZeroStack<ZeroVector3> vertexPool(faceCount * 3); 



	const ZeroRenderDescriptor& desc = *AppLock();


	GetFaceTable(&faceTable[0], 0);

	facePoolPtr = &facePool[0];
	vertexPoolPtr = &vertexPool[0];

	vertex = desc.vertexRecord.position.ptr;
	vertexStride = desc.vertexRecord.position.stride;

	for (i = 0; i < faceCount; i ++) {
		for (j = 0; j < 3; j ++) {
			i0 = faceTable[i].index[j] * vertexStride;
			vertexPoolPtr[i * 3 + j].x = vertex[i0 + 0];
			vertexPoolPtr[i * 3 + j].y = vertex[i0 + 1];
			vertexPoolPtr[i * 3 + j].z = vertex[i0 + 2];
		}
	}

	vCount = VertexListToIndexList (&vertexPoolPtr[0].x, sizeof(ZeroVector3), faceCount * 3, facePoolPtr, sizeof(ZeroVector3), 0);

	worldMatrix.TransformVectors (pool, strideInBytes, vertexPoolPtr, sizeof (ZeroVector3), vCount);

	polyhedrum.BeginFace ();
	for (i = 0; i < faceCount; i ++) {
		i0 = facePoolPtr[i * 3 + 0];
		i1 = facePoolPtr[i * 3 + 1];
		i2 = facePoolPtr[i * 3 + 2];
		face = polyhedrum.AddFace (i0, i1, i2);
#ifdef _DEBUG
		if (!face) {
			ZeroTrace (("segment %d unable to merge face %d %d %d\n", i, i0, i1, i2));
		}
#endif
	}
	polyhedrum.EndFace();


	AppUnlock();

	return vCount;
}

#endif //TOOL_DEVELOPMENT 
