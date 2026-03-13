/****************************************************************************
*
*  Visual C++ 6.0 created by: Julio Jerez
*
****************************************************************************/
#include "ZeroTypes.h"
#include "ZeroUnknown.h"
#include "ZeroDebug.h"
#include "ZeroMemory.h"
#include "ZeroPredict.h"



ZERO_RTTI_CONTRUCTOR(ZeroUnknown);


void AttachObject (ZeroUnknown **oldObj, ZeroUnknown *newObj)
{
	if (*oldObj != newObj) {
		if (*oldObj) {
		   (*oldObj)->Release();
		}
		*oldObj = newObj;
		if (newObj) 
		{
			ZeroAssert ((newObj->GetRefCount() > 0, "Attaching a deleted object")); 
			newObj->AddRef();
		}
	}
}

void* ZeroVisUnknown::operator new (size_t size)
{
	return ZeroUnknown::operator new (size, ZeroAllocator::GetVisAllocator(16, size));
}

void ZeroVisUnknown::operator delete (void *ptr)
{
	ZeroUnknown::operator delete (ptr);
}

void* ZeroVisUnknown::operator new (size_t size, bool isSim)
{
	ZeroAssert ((0u, "do not use placement new operator with ZeroVisUnknown"));
	return ZeroUnknown::operator new (size, ZeroAllocator::GetVisAllocator(16, size));
}

void ZeroVisUnknown::operator delete (void *ptr, bool isSim)
{
	ZeroAssert ((0u, "do not use placement delete operator with ZeroVisUnknown"));
	ZeroUnknown::operator delete (ptr);
}

void* ZeroSimUnknown::operator new (size_t size)
{
	return ZeroUnknown::operator new (size, ZeroAllocator::GetSimAllocator(16, size));
}


void ZeroSimUnknown::operator delete (void *ptr)
{
	ZeroUnknown::operator delete (ptr);
}


void* ZeroSimUnknown::operator new (size_t size, bool isSim)
{
	ZeroAssert ((0u, "do not use placement new operator with ZeroSimUnknown"));
	return ZeroUnknown::operator new (size, ZeroAllocator::GetSimAllocator(16, size));
}

void ZeroSimUnknown::operator delete (void *ptr, bool isSim)
{
	ZeroAssert ((0u, "do not use placement delete operator with ZeroSimUnknown"));
	ZeroUnknown::operator delete (ptr);
}
