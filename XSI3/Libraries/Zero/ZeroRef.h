/****************************************************************************
*
*  Visual C++ 6.0 created by: Julio Jerez
*
****************************************************************************/
#ifndef __ZeroRef__
#define __ZeroRef__

#include "ZeroTypes.h"
#include "ZeroRTTI.h"
#include "ZeroMemory.h"
#include "ZeroPredict.h"
#include "ZeroDebug.h"

#ifdef _DEBUG
#define TRACK_REFERENCES
#endif

#ifdef TRACK_REFERENCES
#include "ZeroList.h"
#endif

const int POOL_ALLOCATION_SIZE = 16;

class ZeroRef
{
	friend class ZeroAllocator;

	static ZeroRttiInfo rtti;

	unsigned allocator	: 8; // this member is set by the memory manager
	unsigned alive		: 1; 
	unsigned ref		: 23;

#ifdef TRACK_REFERENCES
	class Tracker : public ZeroList<ZeroRef *>
	{
	public:
		static Tracker &GetTracker();

	public:
		Tracker();
		~Tracker();

		void SetIdentifier(int id);
		void IncIdentifier();
		void Print();

		void RemoveResets(void);

	public:
		int identifier;
	};
	friend class Tracker;

	Tracker::ZeroListNode *node;
	int identifier;
#endif

protected:
	virtual ~ZeroRef ();

public:
	DLL_DECLSPEC ZeroRef ();

	DLL_DECLSPEC ZeroRef *AddRef();
	DLL_DECLSPEC int Release ();

	int GetRefCount() const
	{
		return ref;
	}

	virtual bool IsTypeID(unsigned idcrc) const
	{
		return rtti.IsTypeID(idcrc);
	}

	virtual unsigned GetTypeID() const
	{
		return rtti.GetTypeID();
	}

	static unsigned GetRttiTypeID()
	{
		return rtti.GetTypeID();
	}

	virtual const char *GetTypeName() const
	{
		return rtti.GetTypeName();
	}

	static const char *GetRttiTypeName()
	{
		return rtti.GetTypeName();
	}

	bool IsTypeIDByName(const char *className) const
	{
		return IsTypeID (CalcLowerCRC(className, strlen (className), 0));
	}

	ZeroAllocator *GetAllocator() const
	{
		return ZeroAllocator::GetIndexAllocator(allocator);
	}

	bool IsSim () const
	{
		return GetAllocator()->IsSim();
	}

	bool IsAlive () const
	{
		return alive;
	}

	virtual void Kill () 
	{
		alive = false;
	}

	virtual void Unkill ()
	{
		alive = true;
	}

	DLL_DECLSPEC void* operator new (size_t size); 
	DLL_DECLSPEC void* operator new (size_t size, bool isSim); 
	DLL_DECLSPEC void* operator new (size_t size, ZeroAllocator* allocator); 


	DLL_DECLSPEC void  operator delete (void *ptr); 
	DLL_DECLSPEC void  operator delete (void *ptr, bool isSim); 
	DLL_DECLSPEC void  operator delete (void *ptr, ZeroAllocator* allocator); 

	static void Reset(void);

#ifdef TRACK_REFERENCES
	static void SetTrackIdentifier(unsigned id);
	static void IncTrackIdentifier();
	static void PrintTracked();
#endif
};

#define ZeroAddRef(item, label) (item)->AddRef()
#define ZeroRelease(item, label) (item)->Release()
#define ZeroOwnRef(item, label)

#endif
