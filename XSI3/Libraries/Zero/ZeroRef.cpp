/****************************************************************************
*
*  Visual C++ 6.0 created by: Julio Jerez
*
****************************************************************************/
#include "ZeroTypes.h"
#include "ZeroRef.h"
#include "ZeroDebug.h"
#include "ZeroMemory.h"
#include "ZeroPredict.h"

#ifdef _DEBUG
#define UNKNOWN_CHECK_POINTER
#if (PLATFORM == PLATFORM_X86_WIN32)
#define LOG_REFERENCES
#endif
#endif


ZeroRttiInfo ZeroRef::rtti ("ZeroRef");


#ifdef UNKNOWN_CHECK_POINTER
static const unsigned pointerCheck = 0x00000000;
#endif


#ifdef LOG_REFERENCES
#include "ZeroFile.h"
#include "ZeroCallStack.h"
static const unsigned triggerRef = 0x00000000;
#endif


#ifdef TRACK_REFERENCES
#include "ZeroUnknown.h"

ZeroRef::Tracker::Tracker() : ZeroList<ZeroRef *>(ZeroAllocator::GetVisAllocator(16, sizeof(ZeroListNode)))
{
	identifier = 0;
}
ZeroRef::Tracker::~Tracker()
{
}
ZeroRef::Tracker &ZeroRef::Tracker::GetTracker()
{
	static Tracker tracker;
	return tracker;
}
void ZeroRef::Tracker::SetIdentifier(int id)
{
	identifier = id;
}
void ZeroRef::Tracker::IncIdentifier()
{
	identifier++;
}
void ZeroRef::Tracker::Print()
{
	ZeroTrace(("ZeroRef with identifier %d:\n", identifier));
	Iterator i(*this);
	for(i.Begin(); i; i++)
	{
		ZeroRef *ref = *i;
		if (ref->identifier == identifier)
		{
			if (ref->IsTypeID(ZeroUnknown::GetRttiTypeID()))
			{
				ZeroTrace(("%p (%d ref) (type \"%s\") (name \"%s\")\n",
					ref, ref->GetRefCount(), ref->GetTypeName(), static_cast<ZeroUnknown *>(ref)->GetName()));
			}
			else
			{
				ZeroTrace(("%p (%d ref) (type \"%s\")\n",
					ref, ref->GetRefCount(), ref->GetTypeName()));
			}
		}
	}
}

void ZeroRef::Tracker::RemoveResets(void)
{
	ZeroListNode *node, *next;
	for (node = GetFirst(); node; node = next)
	{
		next = node->GetNext();
		if (ZeroMemory_WillReset(node))
			Remove(node);
	}
}

void ZeroRef::SetTrackIdentifier(unsigned id)
{
	Tracker::GetTracker().SetIdentifier(id);
}
void ZeroRef::IncTrackIdentifier()
{
	Tracker::GetTracker().IncIdentifier();
}
void ZeroRef::PrintTracked()
{
	Tracker::GetTracker().Print();
}

#endif


void ZeroRef::Reset(void)
{
#ifdef TRACK_REFERENCES
	Tracker::GetTracker().RemoveResets();
	Tracker::GetTracker().Print();
#endif
}

ZeroRef::~ZeroRef ()
{
#ifdef TRACK_REFERENCES
	Tracker::GetTracker().Remove(node);
#endif

	alive = false;
}

ZeroRef::ZeroRef ()
: alive(true)
, ref(1)
{
#ifdef UNKNOWN_CHECK_POINTER
	_ASSERTE ((unsigned) this != pointerCheck);
#endif

#ifdef TRACK_REFERENCES
	node = Tracker::GetTracker().Append(this);
	identifier = Tracker::GetTracker().identifier;
#endif

#ifdef LOG_REFERENCES
	if ((unsigned)this == triggerRef)
	{
		char buf[256];
		ZeroCallStack::Trace(buf, 256, 2, 2);
		ZeroFile refLog("reflog.txt", "wt");
		refLog.Print("Reference tracking for ZeroRef %p\n\n", this);
		refLog.Print("ZeroRef(->%d):\n%s\n", ref, buf);
	}
#endif
}

ZeroRef *ZeroRef::AddRef () 
{
	ref ++;
	_ASSERTE (ref < 0xfffff);

#ifdef LOG_REFERENCES
	if ((unsigned)this == triggerRef)
	{
		char buf[256];
		ZeroCallStack::Trace(buf, 256, 2, 2);
		ZeroFile refLog("reflog.txt", "at");
		refLog.Print("AddRef(->%d):\n%s\n", ref, buf);
	}
#endif

	return this;
}

int ZeroRef::Release ()
{
	ref --;
	_ASSERTE (ref < 0xfffff);

#ifdef LOG_REFERENCES
	if ((unsigned)this == triggerRef)
	{
		char buf[256];
		ZeroCallStack::Trace(buf, 256, 2, 2);
		ZeroFile refLog("reflog.txt", "at");
		refLog.Print("Release(->%d):\n%s\n", ref, buf);
	}
#endif

	if (ref)
	{
		return ref;
	}
	delete this;
	return 0;
}


void* ZeroRef::operator new (size_t size)
{
	ZeroAssert ((0u, "use placement version of new")); 
	return NULL;
}


void* ZeroRef::operator new (size_t size, bool isSim) 
{
	ZeroRef	*ptr;
	ZeroAllocator* allocator;

	allocator = NULL;
	if (isSim) {
		allocator = ZeroAllocator::GetSimAllocator(POOL_ALLOCATION_SIZE, size);
	} else {
		allocator = ZeroAllocator::GetVisAllocator(POOL_ALLOCATION_SIZE, size);
	}

	ptr = (ZeroRef *)allocator->malloc (size);
	ptr->allocator = allocator->GetIndex();
	return ptr;
}

void* ZeroRef::operator new (size_t size, ZeroAllocator* allocator)
{
	ZeroRef	*ptr;
	ptr = (ZeroRef *)allocator->malloc (size);
	ptr->allocator = allocator->GetIndex();
	return ptr;
}



void ZeroRef::operator delete (void *ptr)
{
	ZeroRef	*obj;
	obj = (ZeroRef *)ptr;
	if (obj->GetAllocator())
		obj->GetAllocator()->free(ptr);
}

void ZeroRef::operator delete (void *ptr, bool isSim)
{
	ZeroRef	*obj;
	obj = (ZeroRef *)ptr;
	if (obj->GetAllocator())
		obj->GetAllocator()->free(ptr);
}


void ZeroRef::operator delete (void *ptr, ZeroAllocator* allocator)
{
	ZeroRef	*obj;
	obj = (ZeroRef *)ptr;
	if (obj->GetAllocator())
		obj->GetAllocator()->free(ptr);
}
