/****************************************************************************
*
*  Visual C++ 6.0 created by: Julio Jerez
*
****************************************************************************/
#ifndef __ZeroMemory__
#define __ZeroMemory__

#include "ZeroTypes.h"
#include "ZeroPredict.h"

// Grab header only if we'll be using its functions; this'll help
// notice problems early and often if things go wrong.
#include "NewMemory.h"

class ZeroUnknown;

DLL_DECLSPEC unsigned __cdecl ZeroAllocatedCalls();
DLL_DECLSPEC unsigned __cdecl ZeroAllocatedBytes();

// Default alignments for various platforms, no matter which memory
// manager is in use.
#if ( PLATFORM == PLATFORM_PS2)
  const int PLATFORM_ALIGNSHIFT = 4;
#else
  const int PLATFORM_ALIGNSHIFT = 2;
#endif

#ifdef USE_NEW_MEMORY_MGR
  #ifdef ZERO_TRACK_MEMORY_ALLOCATION
    #error CANT_DEFINE_BOTH_MEMORY_STRATEGIES
  #endif

  // Make these inline so there's not 3-4 function calls just to get
  // to one function. 
  inline void * ZeroMalloc(unsigned size, bool aligned, bool IsPermanent = true, int alignShift = PLATFORM_ALIGNSHIFT) { return NewMemory_malloc(size, IsPermanent, alignShift); }
  inline void ZeroFree(void *ptr) { NewMemory_free(ptr); }

  inline void* operator new(unsigned size) { return NewMemory_malloc(size, true, PLATFORM_ALIGNSHIFT); }
  inline void* operator new(unsigned size, bool IsPermanent) { return NewMemory_malloc(size, IsPermanent, PLATFORM_ALIGNSHIFT); }
  inline void* __cdecl operator new[] (size_t size)	{return NewMemory_malloc(size, true, PLATFORM_ALIGNSHIFT); }
  inline void* __cdecl operator new[] (size_t size,bool IsPermanent)	{return NewMemory_malloc(size, IsPermanent, PLATFORM_ALIGNSHIFT); }

  inline void operator delete(void *ptr) { NewMemory_free(ptr); }
  inline void operator delete[] (void *ptr) { NewMemory_free(ptr); }

  // Macros to (optionally-- see version w/o NewMemory on) pass thru
  // the IsPermanent flag.
  #define ZERONEW(constructor,IsPermanent) new(IsPermanent) constructor
  #define ZEROMALLOC(size,aligned,IsPermanent) ZeroMalloc(size,aligned,IsPermanent)
  #define NEW_TEMP new(false)
  #define NEW_PERM new(true)
 // Standard format, assumes permanent memory. Use ZERONEW if you want the choice
  #define ZeroNew(constructor) new constructor

	// prepare for future resets
	void ZeroMemory_Preset(void);

	// wipeout changes since preset
	void ZeroMemory_Reset(void);

	// see if something allocated by the VisAllocator
	bool VisAllocator_WillReset(void *ptr);

#else
  // Not using NewMemory. Define some stuff.

  DLL_DECLSPEC void __cdecl ZeroFree (void *ptr);

  #if ( PLATFORM == PLATFORM_PS2)
 	  DLL_DECLSPEC void* __cdecl ZeroMalloc (unsigned size, bool aligned, int alignShift = PLATFORM_ALIGNSHIFT);

  	#define ZeroNew(constructor) new constructor
  #else

	  #ifdef ZERO_TRACK_MEMORY_ALLOCATION
		  void* __cdecl operator new (size_t size, unsigned lineNumber, const char *fileName);
		  void __cdecl operator delete (void *ptr, unsigned lineNumber, const char *fileName);

		  DLL_DECLSPEC void* __cdecl ZeroMallocTrackLeaks (unsigned size, bool aligned, const char *fileName, unsigned lineNumber);
		  #define ZeroMalloc(size, aligned) ZeroMallocTrackLeaks (size, aligned, __FILE__, __LINE__);

		  #define ZeroNew(constructor) new (__LINE__, __FILE__) constructor 
	  #else

   	  DLL_DECLSPEC void* __cdecl ZeroMalloc (unsigned size, bool aligned, int alignShift = PLATFORM_ALIGNSHIFT);

		  #define ZeroNew(constructor) new constructor
	  #endif
  #endif

  // Macro versions of some functions that ignore the permanent flag
  #define ZERONEW(constructor,IsPermanent) ZeroNew(constructor)
  #define ZEROMALLOC(size,aligned,IsPermanent) ZeroMalloc(size,aligned)
  #define NEW_TEMP new
  #define NEW_PERM new

	// prepare for future resets
	inline void ZeroMemory_Preset(void) {}

	// wipeout changes since preset
	inline void ZeroMemory_Reset(void) {}

	inline bool VisAllocator_WillReset(void *ptr) { return false; }

#endif

// display the remaining free memory
unsigned int GetFreeMem();

// display remaining allocations in the global memory tracker (assuming this is being called at 
// shut down, since they remain they must be leaks)
void DisplayMemoryLeaks();

// empty the pool allocator
void ReleasePoolAllocator();

// purge the pool allocator
void PurgePoolMemory();

// set the memory block identifier
void SetMemoryIdentifier(unsigned id);
void IncMemoryIdentifier();

// display allocations with the current identifier
void PrintMemoryBlocks();

class ZeroAllocator 
{
protected: 
	bool mIsSim;
	unsigned mIndex;

	static ZeroAllocator *mArray[256];

public:
	DLL_DECLSPEC ZeroAllocator ();
	DLL_DECLSPEC ZeroAllocator (const ZeroAllocator &aOriginal);

	DLL_DECLSPEC virtual ~ZeroAllocator ();

	bool IsSim () const
	{
		return mIsSim;
	}

	unsigned GetIndex () const
	{
		return mIndex;
	}

	virtual void *malloc (unsigned int aSize)
	{
		return ZeroMalloc (aSize, false);
	}

	virtual void free (void *aPtr)
	{
		ZeroFree (aPtr);
	}

	virtual void Purge()
	{
	}

	static ZeroAllocator *GetIndexAllocator(unsigned aIndex)
	{
		return mArray[aIndex];
	}

	DLL_DECLSPEC static ZeroAllocator *GetSimAllocator(int heapSizeInElements, int elemntSizeInBytes);
	DLL_DECLSPEC static ZeroAllocator *GetVisAllocator(int heapSizeInElements, int elemntSizeInBytes);
	DLL_DECLSPEC static ZeroAllocator *GetTempAllocator(int heapSizeInElements, int elemntSizeInBytes);
	DLL_DECLSPEC static ZeroAllocator *GetGenericAllocator();
	DLL_DECLSPEC static ZeroAllocator *GetVisPermAllocator();
	DLL_DECLSPEC static void PurgeAll();
};

#endif
