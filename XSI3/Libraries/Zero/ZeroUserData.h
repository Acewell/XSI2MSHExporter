/****************************************************************************
*
*  Visual C++ 6.0 created by: Julio Jerez
*
****************************************************************************/
#ifndef __ZeroUserData__
#define __ZeroUserData__

#include "ZeroTypes.h"
#include "ZeroRtti.h"
#include "ZeroList.h"
#include "ZeroUnknown.h"


class ZeroObject;

class ZeroUserData: public ZeroUnknown
{
	ZeroObject *owner;

	ZERO_RTTI(ZeroUnknown);

	protected:
	ZeroUserData (const ZeroUserData& clone, ZeroObject *ownerArg)
		:ZeroUnknown (clone)
	{
		owner = ownerArg;
	}

	public:
	ZeroUserData (const char *userTypeName, ZeroObject *ownerArg)
		:ZeroUnknown (userTypeName)
	{
		owner = ownerArg;
	}	

	ZeroUserData (unsigned ID, ZeroObject *ownerArg)
		:ZeroUnknown (ID)
	{
		owner = ownerArg;
	}	

	
	virtual ZeroUserData *CreateClone(ZeroObject *ownerArg)
	{
		_ASSERTE (0);
		return NULL;
	}

	virtual void CloneFixUp (const ZeroUserData &clone)
	{
	}

	// create a clone
	virtual ZeroUserData *CreateClone(ZeroObject *ownerArg, bool isSim)
	{
		return new (isSim) ZeroUserData (*this, ownerArg);
	}

	ZeroObject *GetOwner()
	{
		return owner;
	}

	const ZeroObject *GetOwner() const
	{
		return owner;
	}

	bool operator== (const ZeroUnknown &Target) const
	{
		return (Target.GetNameCRC() == GetNameCRC());
	}

	virtual void *Save (void *data)
	{
		return data;
	}

	virtual void *Load (void *data)
	{
		return data;
	}

	virtual void Update (float dt)
	{
	}
};



class ZeroUserDataList: public ZeroVisList<ZeroUserData *>
{
	friend class ZeroEntity;

	protected:
	DLL_DECLSPEC ~ZeroUserDataList ();

	public:
	ZeroUserDataList ()
		:ZeroVisList<ZeroUserData *>()
	{
	}

	DLL_DECLSPEC void AttachData (ZeroUserData *userDataArg);

	DLL_DECLSPEC void Purge (); 

	DLL_DECLSPEC ZeroUserData *FindUserData (unsigned CRC) const; 

	DLL_DECLSPEC void RemoveUserData (unsigned CRC);

	DLL_DECLSPEC void *Save (void *data);

	DLL_DECLSPEC void *Load (void *data);

	DLL_DECLSPEC void Update (float dt);
};

#endif
