/****************************************************************************
*
*  Visual C++ 6.0 created by: Julio Jerez
*
****************************************************************************/
#ifndef __ZeroUnknown__
#define __ZeroUnknown__

#include "ZeroRef.h"
#include "ZeroCrc.h"


class ZeroUnknown : public ZeroRef
{
	ZERO_RTTI(ZeroRef);

	unsigned id;
#ifdef _DEBUG
	const char *idStr;
#endif

protected:
	virtual ~ZeroUnknown ()
	{
	}

public:
	ZeroUnknown ()
		: ZeroRef(), id(0)
#ifdef _DEBUG
		, idStr("")
#endif
	{
	}

	ZeroUnknown (const char *name)
		: ZeroRef(), id(0)
#ifdef _DEBUG
		, idStr("")
#endif
	{
		SetName (name);
	}

	ZeroUnknown (unsigned crc)
		: ZeroRef(), id(crc)
#ifdef _DEBUG
		, idStr(InvertStrID(crc))
#endif
	{
	}

	ZeroUnknown(const ZeroUnknown &clone)
		: ZeroRef(), id(clone.id)
#ifdef _DEBUG
		, idStr(clone.idStr)
#endif
	{

	}

	virtual ZeroUnknown *CreateClone(bool isSim)
	{
		return NULL;
	}


	void SetNameCRC (unsigned crc)
	{
		id = crc;
#ifdef _DEBUG
		idStr = InvertStrID(id);
#endif
	}

	virtual void SetName (const char *name)
	{
		id = 0;
		if (name) {
			id = CalcStrID (name);
#ifdef _DEBUG
			idStr = InvertStrID(id);
#endif
		}
	}

	unsigned GetNameCRC () const
	{
		return id;
	}

	const char* GetName() const
	{
		return InvertStrID (id);
	}
};


DLL_DECLSPEC void AttachObject (ZeroUnknown **oldObj, ZeroUnknown *newObj);
#define ZeroAttachObject(o, n, l) AttachObject((ZeroUnknown **)&(o), (ZeroUnknown *)(n))


class ZeroVisUnknown: public ZeroUnknown
{
public:
	ZeroVisUnknown ()
		: ZeroUnknown ()
	{
	}

	ZeroVisUnknown (const char *name)
		: ZeroUnknown (name)
	{
	}

	ZeroVisUnknown (unsigned crc)
		: ZeroUnknown(crc)
	{
	}

	ZeroVisUnknown(const ZeroUnknown &clone)
		: ZeroUnknown(clone)
	{
	}

	DLL_DECLSPEC void* operator new (size_t size);
	DLL_DECLSPEC void* operator new (size_t size, bool isSim);
	DLL_DECLSPEC void operator delete (void *ptr);
	DLL_DECLSPEC void operator delete (void *ptr, bool isSim);
};

class ZeroSimUnknown: public ZeroUnknown
{
public:
	ZeroSimUnknown ()
		: ZeroUnknown ()
	{
	}

	ZeroSimUnknown (const char *name)
		: ZeroUnknown (name)
	{
	}

	ZeroSimUnknown (unsigned crc)
		: ZeroUnknown(crc)
	{
	}

	ZeroSimUnknown(const ZeroUnknown &clone)
		: ZeroUnknown(clone)
	{
	}

	DLL_DECLSPEC void* operator new (size_t size);
	DLL_DECLSPEC void* operator new (size_t size, bool isSim);
	DLL_DECLSPEC void operator delete (void *ptr);
	DLL_DECLSPEC void operator delete (void *ptr, bool isSim);
};

#endif
