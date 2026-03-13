/****************************************************************************
*
*  Visual C++ 6.0 created by: Julio Jerez
*
****************************************************************************/
#ifndef __ZeroObject__
#define __ZeroObject__

#include "ZeroTypes.h"
#include "ZeroRtti.h"
#include "ZeroNode.h"
#include "ZeroList.h"
#if (PLATFORM == PLATFORM_X86_WIN32) 
#include "ZeroFile.h"
#endif
#include "ZeroVector3.h"
#include "ZeroQuaternion.h"

#ifdef _DEBUG
#include "ZeroTransform.h"
#endif

class ZeroGeometry;
class ZeroTransform;
class ZeroCollision;
class ZeroUserData;
#if (PLATFORM == PLATFORM_X86_WIN32) 
class ZEROMESH_DRIVER;
#endif

class ZeroObjectData
{
	public:
	unsigned modelType;
	unsigned nameCRC;

	ZeroObjectData()
	{
		modelType = 0;
		nameCRC = 0;
	}
};


class ZeroObject: public ZeroNode<ZeroObject> 
{
	ZERO_RTTI(ZeroBaseNode);

	// attached geometry
	ZeroGeometry *mGeometry;

	// attached transform
	ZeroTransform *mTransform;

	// object matrix
	ZeroQuaternion mRotation;
	ZeroVector3 mPosition;

protected:

#if (PLATFORM == PLATFORM_X86_WIN32) 
	// print the object hierarchy
	DLL_DECLSPEC void PrintHierarchy (ZeroFile &file, char *indentation) const; 
#endif

	// render
	DLL_DECLSPEC virtual void Render (ZeroCamera *camera, const ZeroMatrix& worldMatrix, unsigned flags = 0) const;

	// destructor
	DLL_DECLSPEC ~ZeroObject ();

	// clone the object
	ZeroUnknown *CreateClone (bool isSim)
	{
		return new (isSim) ZeroObject (*this);
	}

	// perform post-clone fix-up
	virtual void CloneFixUp (const ZeroBaseNode &clone);

public:

	//
	// CONSTRUCTORS
	//

	// constructor
	ZeroObject()
		:ZeroNode<ZeroObject>(), mGeometry(NULL), mTransform(NULL), mRotation(1, 0, 0, 0), mPosition(0, 0, 0)
	{
	}
	
	// name constructor
	ZeroObject (const char *name)
		:ZeroNode<ZeroObject>(name), mGeometry(NULL), mTransform(NULL), mRotation(1, 0, 0, 0), mPosition(0, 0, 0)
	{
	}

	// data constructor
	ZeroObject (const ZeroObjectData& data)
		:ZeroNode<ZeroObject>(), mGeometry(NULL), mTransform(NULL), mRotation(1, 0, 0, 0), mPosition(0, 0, 0)
	{
	}

	// copy constructor
	DLL_DECLSPEC ZeroObject (const ZeroObject &clone);


#if (PLATFORM == PLATFORM_X86_WIN32)
	//
	// FILE METHODS
	//

	// save to a file
	DLL_DECLSPEC void Save (ZeroFile& file, ZeroSaveType type) const;

	// save to a filename
	void Save (const char *name, ZeroSaveType type)	const
	{
		ZeroFile file (name, "wb");
		Save (file, type);
	}


	//
	// GEOMETRY UTILITY METHODS
	//

	// get the total vertex and face count
	DLL_DECLSPEC void TotalVertexAndFaceCount(int& faceCount, int& vertexCount) const;

	// get all geometry in a flat array
	DLL_DECLSPEC virtual void FlatFaceArray (const ZeroMatrix &matrix,
											 float* const vertexPool,  int strideInBytes, int &vCount, 
											 int maxVertex, ZeroFace* const facePool, int &fCount, int maxFaces) const; 

	// calculate the global bounding sphere
	DLL_DECLSPEC void CalculateGlobalSphere (ZeroSphere &sphere, const ZeroMatrix *basis = NULL) const;

	// collapse all geometry into one node
	DLL_DECLSPEC void CollapseGeometries();

	// flip the coordenate system
	DLL_DECLSPEC void FlipCoordenateSystem();

	// pack the vertex format
	DLL_DECLSPEC virtual void PackVertexFormat();
#endif


	//
	// USER DATA LIST METHODS	
	//

	// purge all user data
	virtual void PurgeUserData ()
	{
		_ASSERT(0);
	}

	// attach a user data item
	virtual void AttachUserData (ZeroUserData *userDataArg)
	{
		_ASSERT(0);
	}

	// remove a user data item
	virtual void RemoveUserData (unsigned crc)
	{
		_ASSERT(0);
	}
	void RemoveUserData (const char *name)
	{
		RemoveUserData (CalcLowerCRC (name));
	}

	// get a user data item
	virtual ZeroUserData* GetUserData (unsigned crc) const
	{
		return NULL;
	}
	ZeroUserData* GetUserData (const char *name) const
	{
		return GetUserData (CalcLowerCRC (name));
	}


	//
	// GEOMETRY METHODS
	//

	// attach a geometry object
	void AttachGeometry (ZeroGeometry *aGeometry)
	{
		ZeroAttachObject (mGeometry, aGeometry, "ZeroObject");
	}

	// get the attached geometry
	ZeroGeometry* GetGeometry (void) const
	{
		return mGeometry;
	}


	//
	// TRANSFORM METHODS
	//

	// attach a transform object
	void AttachTransform (ZeroTransform *aTransform)
	{
#ifdef _DEBUG
		_ASSERTE(aTransform == NULL || aTransform->IsAlive());
#endif
		ZeroAttachObject (mTransform, aTransform, "ZeroObject");
	}

	// get the attached transform
	ZeroTransform* GetTransform (void) const
	{
		return mTransform;
	}


	//
	// COLLISION METHODS
	//

	// attach a collision object
	virtual void AttachCollision (ZeroCollision *aCollision)
	{
		_ASSERT(0);
	}

	// get the collision object
	virtual ZeroCollision* GetCollision (void) const
	{
		return NULL;
	}


	//
	// MESH METHODS
	//

	// duplicate the hierarchy
	DLL_DECLSPEC ZeroObject* Duplicate (bool isSim);

	// attach a hierarchy
	DLL_DECLSPEC void AddMesh (ZeroObject *aMesh);

	// scale the hierarchy
	DLL_DECLSPEC virtual void Scale (float aScale);

	// render the hierarchy
	DLL_DECLSPEC virtual void RenderAll (ZeroCamera *aCamera, const ZeroMatrix &aMatrix, unsigned aFlags = 0) const;


	//
	// PHYSICS METHODS
	//

	// get the object rotation
	const ZeroQuaternion &GetObjectRotation (void) const
	{
		return mRotation;
	}

	// get the object position
	const ZeroVector3 &GetObjectPosition (void) const
	{
		return mPosition;
	}

	// get the object matrix
	const ZeroMatrix GetObjectMatrix (void) const
	{
		return ZeroMatrix(mRotation, mPosition);
	}

	// get the object sphere
	const ZeroSphere GetObjectSphere (void) const
	{
		return ZeroSphere(GetObjectMatrix(), GetDimensions());
	}

	// set the bounding box dimensions
	virtual void SetDimensions (const ZeroVector4 &aSize)
	{
		_ASSERTE(MagnitudeSq(aSize) < EPSILON);
	}

	// get the bounding box dimensions
	virtual const ZeroVector4 &GetDimensions (void) const
	{
		return identitySphere.size;
	}

	// set the object rotation
	void SetObjectRotation (const ZeroQuaternion &aRotation)
	{
		mRotation = aRotation;
	}

	// set the object position
	void SetObjectPosition (const ZeroVector3 &aPosition)
	{
		mPosition = aPosition;
	}

	// set the object matrix
	void SetObjectMatrix (const ZeroMatrix &aMatrix)
	{
		mRotation = ZeroQuaternion(aMatrix);
		mPosition = aMatrix.posit;
	}
	void SetObjectMatrix (const ZeroQuaternion &aRotation, const ZeroVector3 &aPosition)
	{
		mRotation = aRotation;
		mPosition = aPosition;
	}
	
	// set the object sphere
	void SetObjectSphere (const ZeroSphere &aSphere)
	{
		SetObjectMatrix(aSphere);
		SetDimensions(aSphere.size);
	}

	// calculate world velocity
	DLL_DECLSPEC ZeroVector3 GetWorldVelocity (const ZeroVector3 &pos = ZeroVector3(0, 0, 0), const ZeroObject *root = NULL) const;

	// calculate the world sphere
	ZeroSphere GetWorldSphere (const ZeroObject *root = NULL) const
	{
		return ZeroSphere (GetWorldMatrix (root), GetDimensions ());
	}

	virtual ZeroSphere GetVisTestSphere (const ZeroObject *root = NULL) const
	{
		return ZeroSphere (GetWorldMatrix (root), GetDimensions ());
	}
	
	virtual ZeroVector3 GetVisTestPos () const
	{
		return GetWorldPosition();
	}



	// calculate the world matrix
	DLL_DECLSPEC ZeroMatrix GetWorldMatrix (const ZeroObject *root = NULL) const;

	// calculate the world direction of a vector
	DLL_DECLSPEC ZeroVector3 GetWorldDirection (const ZeroVector3 &dir = ZeroVector3(0, 0, 1), const ZeroObject *root = NULL) const;

	// calculate the world position of a point
	DLL_DECLSPEC ZeroVector3 GetWorldPosition (const ZeroVector3 &pos, const ZeroObject *root = NULL) const;

	// calculate the world position
	DLL_DECLSPEC ZeroVector3 GetWorldPosition (const ZeroObject *root = NULL) const;

	// calculate the local direction of a vector
	DLL_DECLSPEC ZeroVector3 GetLocalDirection (const ZeroVector3 &dirInOtherObjectSpace, const ZeroObject *otherObject = NULL) const;

	// calculate the local position of a point
	DLL_DECLSPEC ZeroVector3 GetLocalPosition (const ZeroVector3 &posInOtherObjectSpace, const ZeroObject *otherObject = NULL) const;

	// calculate the local matrix
	DLL_DECLSPEC ZeroMatrix GetLocalMatrix (const ZeroMatrix &matrixInOtherObjectSpace, const ZeroObject *otherObject = NULL) const;

	// get the collision world position
	DLL_DECLSPEC ZeroVector3 GetCollisionWorldPosition () const;

	// is the object procedural?
	DLL_DECLSPEC virtual const bool IsProcedural() const;

	//**************************************************************************
	//
	// mombojumbo crap
	//
	//**************************************************************************

	// debugging render
	DLL_DECLSPEC virtual void DebugRender (ZeroCamera *camera, const ZeroMatrix& absoluteWorldMatrix, ZeroColor color) const;
	DLL_DECLSPEC virtual void DebugRender (ZeroCamera *camera, const ZeroMatrix& absoluteWorldMatrix, ZeroMatrix* matrixStack, unsigned count, ZeroColor color) const;

	// does the object have geometry?
	int  HasGeometry();

	// get the extents of the geometry
	void GetGeometryExtents(ZeroVector3 &min, ZeroVector3 &max, const ZeroMatrix& worldMatrix);

#if (PLATFORM == PLATFORM_PS2) 
	// clean up geometry data
	void cleanUpGeometries();
#endif

//	DLL_DECLSPEC void* operator new (size_t size, bool isSim); 
//	DLL_DECLSPEC void* operator new (size_t size);
//	DLL_DECLSPEC void  operator delete (void *ptr); 
//	DLL_DECLSPEC void  operator delete (void *ptr, bool isSim); 

};

#endif
