/****************************************************************************
*
*  Visual C++ 6.0 created by: Julio Jerez
*
****************************************************************************/
#include "ZeroTypes.h"
#include "Msh.h"
#include "ZeroFile.h"
#include "ZeroStack.h"
#include "ZeroMatrix.h"
#include "ZeroObject.h"
#include "ZeroCamera.h"
#include "ZeroUnknown.h"
#include "ZeroGeoUtil.h"
#include "ZeroSegment.h"
#include "ZeroResource.h"
#include "ZeroGeometry.h"
#include "ZeroInterval.h"
#include "ZeroTransform.h"
#include "ZeroCollision.h"
#include "ZeroPolyhedra.h"
#include "ZeroMeshGeometry.h"
#include "ZeroMeshContext.h"

// allocator for objects
//ZeroAllocator *goAlloc;

ZERO_RTTI_CONTRUCTOR(ZeroObject);
/*
void* ZeroObject::operator new (size_t size, bool isSim)
{
	// we ignore sim in this game
	return ZeroUnknown::operator new (size, goAlloc);
}

void* ZeroObject::operator new (size_t size)
{
	return ZeroUnknown::operator new (size, goAlloc);
}

void  ZeroObject::operator delete (void *ptr)
{
	ZeroUnknown::operator delete (ptr);
}

void  ZeroObject::operator delete (void *ptr, bool isSim)
{
	ZeroUnknown::operator delete (ptr);
}
*/
#if (PLATFORM == PLATFORM_PS2) 
void ZeroObject::cleanUpGeometries()
{
	//free the memory !!!!
	ZeroObject* obj;
	for (obj = GetFirst(); obj; obj = obj->GetNext()) 
	{
		ZeroGeometry *geo = obj->GetGeometry();
		if (geo) {
			if (geo->GetSegments()) 
			{
				ZeroSegmentList::Iterator iter(*geo->GetSegments());
				for (iter.Begin(); iter; iter ++) {
					
					ZeroSegment *segment = *iter;
					if(segment && !(segment->IsDynamicVb()))
					{
						segment->InitBuffer();
						segment->FreePool();
					}
				}
			}
		}
	
	}
}
#endif

ZeroObject::~ZeroObject ()
{
	AttachGeometry (NULL);
	AttachTransform (NULL);
}



ZeroObject::ZeroObject (const ZeroObject &clone)
	: ZeroNode<ZeroObject>(clone)
	, mGeometry(NULL)
	, mTransform(NULL)
	, mRotation(clone.mRotation)
	, mPosition(clone.mPosition)
{
	if (clone.mGeometry) {
		mGeometry = (ZeroGeometry*)clone.mGeometry->CreateClone(clone.mGeometry->IsSim());
		if (mGeometry) {
			ZeroOwnRef(mGeometry, "ZeroObject");
		}
	}

	if (clone.mTransform) {
		mTransform = (ZeroTransform*)clone.mTransform->CreateClone (this, IsSim());
		if (mTransform) {
			ZeroOwnRef(mTransform, "ZeroObject");
		}
	}
}


void ZeroObject::CloneFixUp (const ZeroBaseNode &clone)
{
	const ZeroObject& cloneObj = (const ZeroObject&) clone;

	if (mGeometry) {
		mGeometry->CloneFixUp (*this, cloneObj);
	}

	if (mTransform) {
		mTransform->CloneFixUp (*cloneObj.GetTransform());
	}

	ZeroBaseNode::CloneFixUp (clone);
}


void ZeroObject::Scale (float scale)
{
	ZeroObject *node;
	
	SetObjectPosition(GetObjectPosition() * scale);

	ZeroVector4 size(GetDimensions());
	size.x *= scale;
	size.y *= scale;
	size.z *= scale;
	size.w *= scale;
	SetDimensions(size);

	if (GetGeometry()) {
		GetGeometry()->Scale (scale);
	}
	
	for (node = GetChild(); node; node = node->GetSibling()) {
		node->Scale(scale);
	}
}



ZeroObject* ZeroObject::Duplicate (bool isSim)
{
	ZeroObject *tmp = (ZeroObject *)CreateClone(isSim);
	tmp->CloneFixUp(*this);
	return tmp;
}

void ZeroObject::AddMesh (ZeroObject *mesh)
{
	ZeroObject *ptr;
	ZeroObject *child;
	
	SetObjectRotation(mesh->GetObjectRotation());
	SetObjectPosition(mesh->GetObjectPosition());
	SetDimensions(mesh->GetDimensions());

	for (ptr = mesh->GetChild(); ptr; ptr = ptr->GetSibling()) {
		child = (ZeroObject *)ptr->CreateClone(IsSim());
		child->Attach(this);
		child->Release();
		child->CloneFixUp (*ptr);
	}
}

#if (PLATFORM == PLATFORM_X86_WIN32)

void ZeroObject::PrintHierarchy (
	ZeroFile &file, 
	char *indentation) const
{
	
	const char* name = GetName();
	ZeroMatrix matrix (GetObjectMatrix ());
	
	file.Print ("%s%s\n", indentation, name);
	//	ZeroQuaternion q (matrix);
	//	file.Print ("%s  %f, %f, %f, %f, %f, %f, %f", indentation, matrix.posit.x, matrix.posit.y, matrix.posit.z, q.s, q.x, q.y, q.z);
	//	file.Print ("\n");
	
	ZeroBaseNode::PrintHierarchy (file, indentation);
}

void ZeroObject::TotalVertexAndFaceCount(
	int& faceCount, 
	int& vertexCount)	const
{	
	ZeroObject *obj;
	ZeroGeometry *geo;
	
	geo = GetGeometry();
	if (geo) {
		geo->TotalVertexAndFaceCount(faceCount, vertexCount);
	}
	
	for (obj = GetChild(); obj; obj = obj->GetSibling()) {
		obj->TotalVertexAndFaceCount(faceCount, vertexCount);
	} 
}

void ZeroObject::PackVertexFormat()
{
	ZeroObject *obj;
	ZeroGeometry *geo;
	
	geo = GetGeometry();
	if (geo) {
		geo->PackVertexFormat();
	}
	
	for (obj = GetChild(); obj; obj = obj->GetSibling()) {
		obj->PackVertexFormat();
	} 
}

void ZeroObject::Save (
	ZeroFile& file, 
	ZeroSaveType type) const
{
	ZeroObject *node;
	
	for (node = GetChild(); node; node = node->GetSibling()) {
		Save (file, type);
	}
}


void ZeroObject::CalculateGlobalSphere (
	ZeroSphere &sphere, 
	const ZeroMatrix *basis) const
{
	int fCount;
	int vCount;
	int maxFace;
	int maxVertex;
	
	maxFace = 0;
	maxVertex = 0;
	
	TotalVertexAndFaceCount(maxFace, maxVertex);
	
	ZeroStack<ZeroFace> face(maxFace + 100);
	ZeroStack<ZeroVector3> pool(maxVertex + 100);
	
	sphere = identitySphere;
	ZeroMatrix invMatrix (GetWorldMatrix().Inverse());
	
	vCount = 0;
	fCount = 0;
	FlatFaceArray (invMatrix, 
						&pool[0].x, sizeof (ZeroVector3), vCount, maxVertex,
						&face[0], fCount, maxFace);
	
	if (!vCount) {
		return; 
	}
	
#ifdef TOOL_DEVELOPMENT 
	ZeroPolyhedra *Hull;
	Hull = NULL;
	if (!basis) {
		Hull = CreateConvexHull (&pool[0].x, sizeof (ZeroVector3), vCount);
	}
	
	if (basis || !Hull) {
		sphere.SetDimensions (&pool[0].x, sizeof (ZeroVector3), vCount, basis);
	} else {
		sphere = Hull->CalculateSphere(&pool[0].x, sizeof (ZeroVector3), basis);
		delete Hull;
	}
#else
	sphere.SetDimensions (&pool[0].x, sizeof (ZeroVector3), vCount, basis);
#endif
}

void ZeroObject::FlatFaceArray (
	const ZeroMatrix &spaceMatrix, 
	float *vertex, 
	const int strideInBytes,
	int &vCount,
	const int maxVertex, 
	ZeroFace* const facePool, 
	int &iCount,
	const int maxIndices) const
{
	ZeroObject *obj;
	ZeroGeometry *geometry;
	
	ZeroMatrix matrix (GetObjectMatrix() * spaceMatrix);
	
	geometry = GetGeometry();
	if (geometry) {
		geometry->FlatFaceArray (matrix, vertex, strideInBytes, vCount, maxVertex, facePool, iCount, maxIndices);
	}
	
	for (obj = GetChild(); obj; obj = obj->GetSibling()) {
		obj->FlatFaceArray (matrix, vertex, strideInBytes, vCount, maxVertex, facePool, iCount, maxIndices);
	}
}



#if 0	// temporarily removed by ken
void ZeroObject::FlipCoordenateSystem ()
{
	int i;
	int stride;
	int faceCount;
	int vertexCount;
	float *vertexPtr;
	const char *name;
	ZeroObject *ptr;
	ZeroFace *face;
	ZeroSegment *segment;
	ZeroGeometry *geometry;
	
	ZeroMatrix matrix (identityMatrix);
	matrix.right.x *= -1; 
	
	for (ptr = GetFirst(); ptr; ptr = ptr->GetNext()) {
		name = ptr->GetName();
		
		ZeroMatrix baseMatrix (ptr->GetObjectMatrix());
		
		baseMatrix.posit.x *= -1; 
		
		ZeroQuaternion poseRot(baseMatrix);
		
		poseRot.y *= -1.0f;
		poseRot.z *= -1.0f;
		
		baseMatrix = ZeroMatrix (poseRot, baseMatrix.posit);
		
		ptr->SetObjectMatrix (baseMatrix);
		
		geometry = ptr->GetGeometry();
		if (!geometry) {
			continue;
		}
		
		ZeroList<ZeroSegment *>::Iterator iter(*geometry->GetSegments());
		for (iter.Begin(); iter; iter ++) {
			segment = *iter;
			
			vertexCount = segment->GetVertexCount();
			
			vertexPtr = segment->GetVertexRecord()->position.ptr;
			stride = segment->GetVertexRecord()->position.stride * sizeof (float);
			matrix.TransformVectors (vertexPtr, stride, vertexPtr, stride, vertexCount);
			
			vertexPtr = segment->GetVertexRecord()->normal.ptr;
			stride = segment->GetVertexRecord()->normal.stride * sizeof (float);
			matrix.TransformVectors (vertexPtr, stride, vertexPtr, stride, vertexCount);
			
			face = segment->GetFaceTable();
			faceCount = segment->GetFaceCount();
			for (i = 0; i < faceCount; i ++) {
				int j;
				j = face[i].index[1];
				face[i].index[1] = face[i].index[2];
				face[i].index[2] = j;
			}
		}
	}
}
#endif

void ZeroObject::CollapseGeometries()
{
	ZeroObject *obj;
	
	ZeroGeometry *rootGeo = new (false) ZeroMeshGeometry (GetName());
	for (obj = GetFirst(); obj; obj = obj->GetNext()) {
		if (obj != this) {
			ZeroGeometry *geo = obj->GetGeometry();
			if (geo) {
				if (geo->GetSegments()) {
					ZeroMatrix transformMatrix (obj->GetWorldMatrix (this));
					ZeroMatrix rotationMatrix (transformMatrix);
					rotationMatrix.posit = ZeroVector3 (0, 0, 0);
					ZeroSegmentList::Iterator iter(*geo->GetSegments());
					for (iter.Begin(); iter; iter ++) {
						ZeroSegment *segment = *iter;
						const ZeroRenderDescriptor &desc = *segment->AppLock();
						
						float *ptr = desc.vertexRecord.position.ptr;
						int stride = desc.vertexRecord.position.stride * sizeof (float);

					#if (PLATFORM != PLATFORM_PS2)
						transformMatrix.TransformVectors (ptr, stride, ptr, stride, segment->GetVertexCount());
					#else
						transformMatrix.TransformVectors (ptr, stride, ptr, stride, segment->GetVertexCount());
					#endif
						
						ptr = desc.vertexRecord.normal.ptr;
						stride = desc.vertexRecord.normal.stride * sizeof (float);
#if (PLATFORM != PLATFORM_PS2)
						rotationMatrix.TransformVectors (ptr, stride, ptr, stride, segment->GetVertexCount());
#else
						//transformMatrix.TransformVectors (ptr, stride, ptr, stride, segment->GetVertexCount());
#endif
						
						segment->AppUnlock();

						rootGeo->AddSegment (segment);
					}
				}
			}
		}
	}
	
	rootGeo->SetSphere();
	AttachGeometry(rootGeo);
	rootGeo->Release();
	
	for (obj = GetChild(); obj; obj = GetChild()) {
		obj->Detach();
	}
}

#endif

ZeroMatrix ZeroObject::GetWorldMatrix (const ZeroObject *root) const
{
	if (this == root)
		return identityMatrix;
	ZeroMatrix matrix (GetObjectMatrix ());
	for (ZeroObject *obj = GetParent(); (obj != root) && (obj != NULL); obj = obj->GetParent()) {
		ZeroMatrixMultiply(matrix, matrix, obj->GetObjectMatrix ());
	}
	return matrix;
}

ZeroVector3 ZeroObject::GetWorldDirection(const ZeroVector3 &dir, const ZeroObject *root) const
{
	if (this == root)
		return dir;
	ZeroVector3 d (GetObjectMatrix().RotateVector(dir));
	for (ZeroObject *obj = GetParent(); (obj != root) && (obj != NULL); obj = obj->GetParent()) {
		d = obj->GetObjectMatrix().RotateVector(d);
	}
	return d;
}

ZeroVector3 ZeroObject::GetWorldPosition (
	const ZeroVector3 &pos, 
	const ZeroObject *root) const
{
	if (this == root)
		return pos;
	ZeroVector3 p (GetObjectMatrix().TransformVector(pos));
	for (ZeroObject *obj = GetParent(); (obj != root) && (obj != NULL); obj = obj->GetParent()) {
		p = obj->GetObjectMatrix().TransformVector(p);
	}
	return p;
}

ZeroVector3 ZeroObject::GetWorldPosition (const ZeroObject *root) const
{
	if (this == root)
		return ZeroVector3(0.0f, 0.0f, 0.0f);
	ZeroVector3 p(GetObjectPosition());
	for (ZeroObject *obj = GetParent(); (obj != root) && (obj != NULL); obj = obj->GetParent()) {
		p = obj->GetObjectMatrix().TransformVector(p);
	}
	return p;
}


ZeroMatrix ZeroObject::GetLocalMatrix (
	const ZeroMatrix &matrixInOtherObjectSpace,
	const ZeroObject *otherObject) const
{
	ZeroMatrix mat (matrixInOtherObjectSpace);
	if (otherObject) {
		ZeroMatrixMultiply(mat, mat, otherObject->GetWorldMatrix (NULL));
	}
	ZeroMatrixMultiply(mat, mat, GetWorldMatrix (NULL).Inverse());
	return mat;
}



ZeroVector3 ZeroObject::GetLocalDirection (
	const ZeroVector3 &dirInOtherObjectSpace, 
	const ZeroObject *otherObject) const
{
	int i;
	int count;
	const ZeroObject *obj;
	const ZeroObject *array[128];

	ZeroVector3 p (dirInOtherObjectSpace);
	if (otherObject) {
		p = otherObject->GetWorldDirection (dirInOtherObjectSpace, NULL);
	}

	count = 0;
	obj = this;
	do {
		array[count] = obj;
		count ++;
		obj = obj->GetParent();
	} while (obj);

	for (i = count; i; i --) {
		obj = array[i - 1];
		p = obj->GetObjectMatrix().UnrotateVector(p);
	}

	return p;
}


ZeroVector3 ZeroObject::GetLocalPosition (
	const ZeroVector3 &pos, 
	const ZeroObject *otherObject) const
{
	int i;
	int count;
	const ZeroObject *obj;
	const ZeroObject *array[128];


	ZeroVector3 p (pos);
	if (otherObject) {
		p = otherObject->GetWorldPosition (pos, NULL);
	}

	count = 0;
	obj = this;
	do {
		array[count] = obj;
		count ++;
		obj = obj->GetParent();
	} while (obj);

	for (i = count; i; i --) {
		obj = array[i - 1];
		p = obj->GetObjectMatrix().UntransformVector(p);
	}

	return p;
}

ZeroVector3 ZeroObject::GetCollisionWorldPosition () const
{
	// returns the world position of an object's collision sphere
	if ( GetCollision() )
		return GetWorldPosition(GetCollision()->GetSphere().posit);
	else
		return GetWorldPosition();
}

ZeroVector3 ZeroObject::GetWorldVelocity (const ZeroVector3 &pos, const ZeroObject *root) const
{
	const ZeroObject *obj;

	ZeroVector3 p(pos);
	ZeroVector3 v(0, 0, 0);

	for (obj = this; (obj != root) && (obj != NULL); obj = obj->GetParent()) {
		const ZeroTransform *transform = obj->GetTransform();
		const ZeroMatrix matrix (obj->GetObjectMatrix());
		if (transform)
		{
			v += CrossProduct(transform->GetOmega(), p);
		}
		v = matrix.RotateVector(v);
		if (transform)
		{
			v += transform->GetVelocity();
		}
		p = matrix.TransformVector(p);
	}

	return v;
}


void ZeroObject::Render (
	ZeroCamera *camera, 
	const ZeroMatrix& worldMatrix,
	unsigned flags) const
{
	ZeroGeometry *geometry = GetGeometry();
	if (geometry) 
	{
//		ZeroGeometry* renderGeo = geometry->GetCurrentLOD();
//		if(renderGeo)
//			renderGeo->Render(this, camera, worldMatrix, 1, flags);
//
//		renderGeo = geometry->GetFadeLOD();
//		if(renderGeo)
//			renderGeo->Render(this, camera, worldMatrix, 1, flags);

		geometry->Render(this, camera, worldMatrix, flags);
	}

}

#define RENDER_ALL_RECURSIVE

void ZeroObject::RenderAll (
	ZeroCamera *camera, 
	const ZeroMatrix& worldMatrix,
	unsigned flags) const
{
#ifdef RENDER_ALL_RECURSIVE
	ZeroMatrix matrix;
	
	for (ZeroObject *obj = GetChild(); obj; obj = obj->GetSibling()) 
	{
		ZeroMatrixMultiply(matrix, obj->GetObjectMatrix(), worldMatrix);
		obj->RenderAll (camera, matrix, flags);
	}

	Render (camera, worldMatrix, flags);
#else
	int i;
	const ZeroObject *object;
	const ZeroObject *objectStack[64];
	static ZeroMatrix matrixStack[64];

	// push this onto the stack
	i = 0;
	object = this;
	objectStack[0] = object;
	matrixStack[0] = worldMatrix;

	// while the stack is not empty...
	do
	{
		// while there is a child...
		while (object->GetChild())
		{
			// push the child onto the stack
			++i;
			object = object->GetChild();
			objectStack[i] = object;
			ZeroMatrixMultiply (matrixStack[i], object->GetObjectMatrix(), matrixStack[i-1]);
		};

		// while the stack is not empty...
		do
		{
			// render the top entry
			object->Render (camera, matrixStack[i], flags);

			// if the entry has a sibling...
			if (object->GetSibling())
			{
				// replace it with its sibling
				object = object->GetSibling();
				objectStack[i] = object;
				ZeroMatrixMultiply (matrixStack[i], object->GetObjectMatrix(), matrixStack[i - 1]);
				break;
			}

			// pop the entry off the stack
			--i;
			object = objectStack[i];
		}
		while (i > 0);
	}
	while (i > 0);
#endif
}


void ZeroObject::DebugRender (
	ZeroCamera *camera, 
	const ZeroMatrix& worldMatrix,
	ZeroColor color) const
{
	_ASSERTE(0);
/*
	ZeroObject *obj;
	
	if (GetGeometry()) {
		//		GetGeometry()->GetSphere().DebugRender(camera, worldMatrix, color);
		GetGeometry()->DebugRender (camera, worldMatrix, color);
	}
	
	for (obj = GetChild(); obj; obj = obj->GetSibling()) {
		obj->DebugRender (camera, obj->GetObjectMatrix() * worldMatrix, color);
	}
*/
}

void ZeroObject::DebugRender (
	ZeroCamera *camera, 
	const ZeroMatrix& worldMatrix,
	ZeroMatrix* matrixStack,
	unsigned count,
	ZeroColor color) const
{
	_ASSERTE(0);
/*
	ZeroObject *obj;
	
	if (GetGeometry()) {
		GetGeometry()->GetSphere().DebugRender(camera, worldMatrix, color);
		GetGeometry()->DebugRender (camera, worldMatrix,matrixStack,count, color);
	}
	
	for (obj = GetChild(); obj; obj = obj->GetSibling()) {
		obj->DebugRender (camera, obj->GetObjectMatrix() * worldMatrix, color);
	}
*/	
}


const bool ZeroObject::IsProcedural() const
{
	return false;
}


int ZeroObject::HasGeometry()
{
	ZeroObject *obj;

	for (obj = GetFirst(); obj; obj = obj->GetNext()) {
		if (GetGeometry()) {
			return 1;
		}
	}
	
	return 0;
}

void ZeroObject::GetGeometryExtents(ZeroVector3 &min, ZeroVector3 &max, const ZeroMatrix& worldMatrix)
{
	ZeroObject *obj;
	
	if (GetGeometry())
	{
		GetGeometry()->GetExtents (min, max, worldMatrix);
	}
	
	for (obj = GetChild(); obj; obj = obj->GetSibling()) 
	{
		ZeroMatrix matrix (obj->GetObjectMatrix() * worldMatrix);
		obj->GetGeometryExtents (min, max, matrix);
	}
}

namespace InternalObject
{
	void Startup(void)
	{
//		goAlloc = ZeroAllocator::GetGenericAllocator();
	}
}
