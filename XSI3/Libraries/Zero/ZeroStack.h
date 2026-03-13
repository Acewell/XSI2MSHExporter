/****************************************************************************
*
*  Visual C++ 6.0 created by: Julio Jerez
*
****************************************************************************/
#ifndef __ZeroStack__
#define __ZeroStack__

#include "ZeroTypes.h"

DLL_DECLSPEC void __cdecl SetMaxStackSize (int Size, int Commit = 4096);

namespace InternalStack
{
	class StackPool
	{
		protected:
#ifdef USE_NEW_MEMORY_MGR
		void *Ptr; // Need to be able to deallocate this from temp ram - NM 7/10/02
#else
		const void *Ptr;
#endif
		DLL_DECLSPEC StackPool (int Size);
		DLL_DECLSPEC ~StackPool ();
	};
}

template<class T>
class ZeroStack: public InternalStack::StackPool
{
	int size;

	public:
	ZeroStack (int elements)
		:InternalStack::StackPool (elements * sizeof(T))
	{
		size = elements;
	}

	~ZeroStack ()
	{
	}

	unsigned SizeInBytes() const
	{
		return size * sizeof(T);
	}

	T& operator[] (int entry) 
	{
		T *mem;

		_ASSERTE (entry >= 0);
		_ASSERTE (entry < size);

		mem = (T*) Ptr;
		return mem[entry];
	}
};

#endif
