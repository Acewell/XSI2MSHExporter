/****************************************************************************
*
*  Visual C++ 6.0 created by: Julio Jerez
*
****************************************************************************/
#ifndef __ZeroVirtualMem__
#define __ZeroVirtualMem__

#include "ZeroTypes.h"
#include "ZeroUnknown.h"


class ZeroVirtualMemory: public ZeroUnknown
{
	int maxSize;
	int pageSize;
	void *baseAddress;
	ZeroVirtualMemory *next;
	static ZeroVirtualMemory *root;

	protected:
	DLL_DECLSPEC ~ZeroVirtualMemory ();
	
	public:
	DLL_DECLSPEC ZeroVirtualMemory (int commitedSize, int maxSize);
	DLL_DECLSPEC int TotalMemoryUsed ();

	static DLL_DECLSPEC int __cdecl ExecptionHandler (void *exceptPtr);

	void *GetBuffer() 
	{
	   return baseAddress;
	}
};

#endif



