#ifndef __ZeroEntity__
#define __ZeroEntity__

#include "ZeroObject.h"
#include "ZeroUserData.h"

class ZeroMeshContext;

class ZeroEntity : public ZeroObject
{
	ZERO_RTTI(ZeroObject);

	// box size
	ZeroVector4 mSize;

	// attached collision
	ZeroCollision *mCollision;

	// user data list
	ZeroUserDataList mUserDataList;

protected:

	// destructor
	DLL_DECLSPEC ~ZeroEntity ();

	// clone the object
	ZeroUnknown *CreateClone (bool isSim)
	{
		return new (isSim) ZeroEntity (*this);
	}

	// perform post-clone fix-up
	virtual void CloneFixUp (const ZeroBaseNode &clone);

public:

	//
	// CONSTRUCTORS
	//

	// constructor
	ZeroEntity()
		: ZeroObject(), mSize(0, 0, 0), mCollision(NULL), mUserDataList()
	{
	}
	
	// name constructor
	ZeroEntity (const char *name)
		: ZeroObject(name), mSize(0, 0, 0), mCollision(NULL), mUserDataList()
	{
	}

	// data constructor
	ZeroEntity (const ZeroObjectData& data)
		: ZeroObject(), mSize(0, 0, 0), mCollision(NULL), mUserDataList()
	{
	}


	// copy constructor
	DLL_DECLSPEC ZeroEntity (const ZeroEntity &clone);

	// load mesh from a file
	DLL_DECLSPEC ZeroEntity (const char *file, ZeroMeshContext *context);


#if (PLATFORM == PLATFORM_X86_WIN32)
	//
	// FILE METHODS
	//

	// replace a mesh format driver
	static DLL_DECLSPEC void UpgradeFileFormat (const ZEROMESH_DRIVER & newFormat); 

	// save mesh data
	DLL_DECLSPEC bool SaveMesh (const char *name);
#endif

	//
	// USER DATA METHODS
	//

	// purge all user data
	virtual void PurgeUserData ()
	{
		mUserDataList.Purge();
	}

	// attach a user data item
	virtual void AttachUserData (ZeroUserData *userDataArg)
	{
		mUserDataList.AttachData (userDataArg);
	}

	// remove a user data item
	virtual void RemoveUserData (unsigned crc)
	{
		mUserDataList.RemoveUserData (crc);
	}

	// get a user data item
	virtual ZeroUserData* GetUserData (unsigned crc) const
	{
		return mUserDataList.FindUserData (crc);
	}

	// get the user data list
	const ZeroUserDataList& GetUserList () const
	{
		return mUserDataList;
	}


	//
	// COLLISION METHODS
	//

	// attach a collision object
	virtual void AttachCollision (ZeroCollision *collisionArg)
	{
		ZeroAttachObject (mCollision, collisionArg, "ZeroEntity");
	}

	// get the attached collision
	virtual ZeroCollision* GetCollision (void) const
	{
		return mCollision;
	}

	
	//
	// PHYSICS METHODS
	//

	// set the bounding box dimensions
	virtual void SetDimensions (const ZeroVector4 &aSize)
	{
		mSize = aSize;
	}

	// get the bounding box dimensions
	virtual const ZeroVector4 &GetDimensions (void) const
	{
		return mSize;
	}
};

#endif
