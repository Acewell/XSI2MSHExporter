/****************************************************************************
*
*  Visual C++ 6.0 created by: Julio Jerez
*
****************************************************************************/
#include "ZeroTypes.h"
#include "ZeroList.h"
#include "ZeroTree.h"
#include "ZeroDebug.h"
#include "ZeroMemory.h"
#include "NewMemory.h"

#define KEEP_CALLSTACK 0
#define STACK_DEPTH 5

#if (PLATFORM==PLATFORM_PS2)
// End of app, static data in app. Used to get a better idea as to
// memory footprint
extern unsigned long _end;
#endif

#define ENABLE_POOL_ALLOCATION 1
#ifdef _DEBUG
	#define ZERO_TRACK_POOL_MEMORY

	#if (PLATFORM==PLATFORM_GAMECUBE)
		#define DEBUG_MEMORY
  #elif (PLATFORM==PLATFORM_PS2)
//		#define DEBUG_MEMORY
	#endif

#endif

#ifdef DEBUG_MEMORY
	// Compile flag to zap memory when allocated.
	#define CLEAR_ON_ALLOCATE

	// Compile flag to track all allocations and verify things against
	// double-deletion or invalid deletes.
	#define TRACK_ALL_ALLOCATIONS

	// Compile flag to make memory blocks larger than they really are.
	#define SURROUND_MEMORY_BLOCKS

	// These are only possible when SURROUND_MEMORY_BLOCKS is on
	#ifdef SURROUND_MEMORY_BLOCKS

	// Compile flag to surround blocks with check values, and check them on freeing
	#define SURROUND_WITH_CHECK_VALUES

	#ifdef SURROUND_WITH_CHECK_VALUES

	  // Compile flag to zap memory when releasing
	  #define CLEAR_ON_RELEASE

	  // Compile flag to check the heap
	  //#define HEAP_CONSISTENCY_CHECK
	#endif

	#endif

	// flag to constantly check memory, if the above flags are valid.
	static bool ConstantlyCheckMemory=false;

	// Compile flag to check after allocation. Normally off, as we assume
	// that works.
	// #define CHECK_AFTER_ALLOCATE

	// Allocate 16 bytes (GameCube = 32) before, 4 after for size, trashing detection.
	// It possibly could be 8 bytes before, but I think that might
	// un-align data on the PS2 in case pointers are handed out on a
	// 16-byte boundary.
	#if (PLATFORM==PLATFORM_PS2)
		const unsigned int BLOCK_HEADER_BYTES = 16;
	#elif (PLATFORM==PLATFORM_GAMECUBE)
		const unsigned int BLOCK_HEADER_BYTES = 32;
	#endif	
	const unsigned int BLOCK_TRAILER_BYTES = 4;

	// Value to fill data before a block
	const unsigned int BLOCK_PRE_CLEAR_VALUE = 0xADADADAD;
	// Value to fill data middle of a just-allocated block
	const unsigned int BLOCK_CLEAR_VALUE = 0xCDCDCDCD;
	// Value to fill data middle of a just-allocated block
	const unsigned int BLOCK_POST_CLEAR_VALUE = 0xFDFDFDFD;
	// Value to fill data released block
	const unsigned int BLOCK_RELEASED_CLEAR_VALUE = 0xDDDDDDDD;
	
	#ifdef TRACK_ALL_ALLOCATIONS
	  // How many allocations we can track at once (warning- uses memory!)
	  const int MAX_ALLOCATIONS_TRACKED=65536;

	  static int AllocationCount=0;
	  class AllocationTracker
	  {
	  	void *CurAllocations[MAX_ALLOCATIONS_TRACKED];
	  	public:
	  	AllocationTracker::AllocationTracker()
	  	{
	  	}
	  	virtual ~AllocationTracker()
	  	{
		  	bool leaks = false;
			for(int i=0;i<AllocationCount;i++) 
			{
				leaks = true;
				ZeroTrace(( "Leak at address = %p\n", CurAllocations[i] ));	  	
			}
			if (!leaks)
			{
				ZeroTrace(("No leaks in leak tracker!\n"));
			}
	  	}	  	
	  	void*& operator[](int i) { return CurAllocations[i]; }
	  };
	  static void*& GetCurAllocations(int i)
	  {
		  static AllocationTracker tracker;
	  	  return tracker[i];
	  }
	  static int BytesAllocated=0;
	#endif //#ifdef TRACK_ALL_ALLOCATIONS	
#endif

static unsigned allocationCalls = 0;
static unsigned allocatedMemory = 0;
static unsigned triggerAddress = 0x00000000;

#if 1//(PLATFORM == PLATFORM_PS2)
unsigned SurfaceBanksUsed=0;
static int TrackOverhead=0;
#endif

unsigned GetAllocatedMemory(void)
{
#ifdef TRACK_ALL_ALLOCATIONS
	return BytesAllocated;
#else
	return 0;
#endif
}

static unsigned int sFreeMem = 0;
#ifdef SURROUND_MEMORY_BLOCKS
	static unsigned int totalalloced = 0;
#endif

unsigned int GetFreeMem()
{
	#ifdef SURROUND_MEMORY_BLOCKS
		return sFreeMem - totalalloced;
	#else
		return sFreeMem - allocatedMemory;
	#endif
} 

// Checks one block for consistency, with p pointing to the true start
// of a block (i.e. (long)ptr[0] is the size, [1] the start of the guard stuff, etc
static inline void CheckOneBlock(void *p)
{
	#ifdef SURROUND_WITH_CHECK_VALUES
		unsigned int *CheckPtr=(unsigned int *)p;
		unsigned int size=CheckPtr[0]; // WARNING: may not be valid
		_ASSERTE(CheckPtr[1]==BLOCK_PRE_CLEAR_VALUE);
		_ASSERTE(CheckPtr[2]==BLOCK_PRE_CLEAR_VALUE);
		_ASSERTE(CheckPtr[3]==BLOCK_PRE_CLEAR_VALUE);

		// Advance to the end of the block we allocated. This must be a
		// char*, as we may have been told to allocate a non-multiple of 4
		// bytes.
		unsigned char *EndPtr=(((unsigned char*)p) + size - BLOCK_TRAILER_BYTES);
		_ASSERTE(EndPtr[0]==(unsigned char)BLOCK_POST_CLEAR_VALUE);
		_ASSERTE(EndPtr[1]==(unsigned char)BLOCK_POST_CLEAR_VALUE);
		_ASSERTE(EndPtr[2]==(unsigned char)BLOCK_POST_CLEAR_VALUE);
		_ASSERTE(EndPtr[3]==(unsigned char)BLOCK_POST_CLEAR_VALUE);
	#endif
}

static inline void HeapConsistencyCheck()
{
	#ifdef HEAP_CONSISTENCY_CHECK
		#ifdef TRACK_ALL_ALLOCATIONS

			if(!ConstantlyCheckMemory)
				return;

			int i;

			for(i=0;i<AllocationCount;i++) 
				CheckOneBlock(GetCurAllocations(i));

		#endif
	#endif
}

#if (PLATFORM == PLATFORM_GAMECUBE)

	void *heapArenaLo, *heapArenaHi;
	u32 heapAllocHi;
	
	unsigned int GetHighestAllocatedMem()
	{
		return (unsigned int)heapAllocHi - (unsigned int)heapArenaLo;	
	}	
	unsigned int GetRemainingFreeMem()
	{
		return (unsigned int)heapArenaHi - (unsigned int)heapAllocHi;	
	}	

	// *****************************************************************************
	// *****************************************************************************
	// GAMECUBE
	// *****************************************************************************
	// *****************************************************************************
	// we need to grab the vertex buffers first
	extern void gcDriverInitMemory();
	OSHeapHandle sHeap = 0;
	u32 heapReportNext;

	inline void *OneAndOnlyMalloc(unsigned int size)
	{
		static int IsHeapInitialised = 0;
		int adjustedSize = size;
		
		if (!IsHeapInitialised)
		{
			heapArenaLo = OSGetArenaLo();
			heapArenaHi = OSGetArenaHi();
					
			heapArenaLo = OSInitAlloc(heapArenaLo, heapArenaHi, 2);
			OSSetArenaLo(heapArenaLo);

			gcDriverInitMemory();
			
			// the call to driver gcDriverInitMemory changes the arena	
			heapArenaLo = OSGetArenaLo();
			heapArenaHi = OSGetArenaHi();

			heapAllocHi = 0;
			heapReportNext = 0x80000000 + 1024*1024*8;
			
			heapArenaLo = (void *)OSRoundUp32B(heapArenaLo);
			heapArenaHi = (void *)OSRoundDown32B(heapArenaHi);
			sFreeMem = (unsigned int)heapArenaHi - (unsigned int)heapArenaLo;
			sHeap = OSCreateHeap(heapArenaLo, heapArenaHi);
			OSSetCurrentHeap(sHeap);
			OSSetArenaLo(heapArenaLo);
					
			int memUsage = 48 * 1024 * 1024 - sFreeMem;
			OSReport("Code and static memory usage: %d\n", memUsage);

			IsHeapInitialised = 1;
		}

		void *ptr = NULL;

		HeapConsistencyCheck();

		#ifdef SURROUND_MEMORY_BLOCKS
			// Allocate 32 bytes before, 4 after for size, trashing detection.
			// It possibly could be 8 bytes before, but I think that might
			// un-align data on the PS2 in case pointers are handed out on a
			// 16-byte boundary. 
			adjustedSize += (BLOCK_HEADER_BYTES+BLOCK_TRAILER_BYTES);

			totalalloced += size;
		//	if (size > 1024)
		//		OSReport("allocated %d bytes at %X, total alloced = %d\n", size, ptr, totalalloced);
		#endif

		ptr = OSAlloc(adjustedSize);
		
		#ifdef DEBUG_MEMORY
			if( (unsigned int)ptr == triggerAddress )
			{
				// put breakpoint here
				int iii;
				iii=1;
			}
		#endif

		#ifndef _FINAL
			u32 top = (u32)ptr + adjustedSize;
			if (top > heapAllocHi)
			{
				heapAllocHi = top;
				if (heapAllocHi > heapReportNext)
				{
					unsigned int mem = (unsigned int)top - (unsigned int)heapArenaLo;
					unsigned int memTotal = (unsigned int)heapArenaHi - (unsigned int)heapArenaLo;
					OSReport("Heap used %d of %d\n", mem, memTotal);
					heapReportNext += 1024*1024/10;
					if ( heapAllocHi > heapReportNext )
						heapReportNext = heapAllocHi + 1024*1024/10;
				}
			}
		#endif

		#ifdef TRACK_ALL_ALLOCATIONS
			// Ensure we don't overflow our buffers.
			_ASSERTE(AllocationCount < MAX_ALLOCATIONS_TRACKED);
			GetCurAllocations(AllocationCount) = ptr;
			AllocationCount++;
			BytesAllocated += adjustedSize;
		#endif

		#ifdef CLEAR_ON_ALLOCATE
			// Clear data we just allocated.
			memset(ptr,BLOCK_CLEAR_VALUE,adjustedSize);
		#endif

		#ifdef SURROUND_WITH_CHECK_VALUES
			// Build up header: 4 bytes with our size, then 12 b
			unsigned int *ClearPtr;
			ClearPtr=(unsigned int *)ptr;
			ClearPtr[0]=adjustedSize; // Size we allocated, kept around to check during free
			ClearPtr[1]=BLOCK_PRE_CLEAR_VALUE; // Pre-block trashing detection
			ClearPtr[2]=BLOCK_PRE_CLEAR_VALUE;
			ClearPtr[3]=BLOCK_PRE_CLEAR_VALUE;
			ClearPtr[4]=BLOCK_PRE_CLEAR_VALUE; 
			ClearPtr[5]=BLOCK_PRE_CLEAR_VALUE;
			ClearPtr[6]=BLOCK_PRE_CLEAR_VALUE;
			ClearPtr[7]=BLOCK_PRE_CLEAR_VALUE; // Pre-block trashing detection

			// Advance to the end of the block we allocated. This must be a
			// char*, as we may have been told to allocate a non-multiple of 4
			// bytes.
			unsigned char *EndPtr=(((unsigned char*)ptr) + adjustedSize - BLOCK_TRAILER_BYTES);
			EndPtr[0]=(unsigned char)BLOCK_POST_CLEAR_VALUE;
			EndPtr[1]=(unsigned char)BLOCK_POST_CLEAR_VALUE;
			EndPtr[2]=(unsigned char)BLOCK_POST_CLEAR_VALUE;
			EndPtr[3]=(unsigned char)BLOCK_POST_CLEAR_VALUE;

			// Now, advance the pointer we allocated to skip over our header
			// before passing it back to the rest of the game.
		#endif

		#ifdef SURROUND_MEMORY_BLOCKS
			// Hand back a different pointer to the game than the one we allocated.
			ptr=(void*) (((unsigned char*)ptr) + BLOCK_HEADER_BYTES);
		#endif

		#ifdef CHECK_AFTER_ALLOCATE
			HeapConsistencyCheck();
		#endif
		
		if(ptr == NULL)
			OSReport("FAILED TO ALLOCATE!");
			
		return ptr;
	}
	inline void OneAndOnlyFree(void *ptr)
	{
		HeapConsistencyCheck();

		#ifdef SURROUND_MEMORY_BLOCKS
			// Back up pointer we passed to the rest of the game to get the real
			// pointer to our data and the pointer used by free.
			ptr=(void*) (((unsigned char*)ptr) - BLOCK_HEADER_BYTES);
			unsigned int *CheckPtr=(unsigned int *)ptr;
			unsigned int adjustedSize=CheckPtr[0]; // WARNING: may not be valid
			int freedSize = adjustedSize - (BLOCK_HEADER_BYTES+BLOCK_TRAILER_BYTES);
			totalalloced -= freedSize;
		//	if (freedSize > 1024)
		//		OSReport("free'd %d bytes at %X, total alloced = %d\n", freedSize, ptr, totalalloced);
		#endif

		#ifdef TRACK_ALL_ALLOCATIONS
			BytesAllocated -= adjustedSize;

			int i,ListPos=-1;
			bool FoundIt=false;
			for(i=0;i<AllocationCount;i++) {
				if(GetCurAllocations(i) == ptr) {
					FoundIt=true;
					ListPos=i;
					break; // get out of this loop.
				}
			}

			_ASSERTE(FoundIt);
			if(FoundIt) {
				for(i=ListPos;i<(AllocationCount-1);i++)
					GetCurAllocations(i)=GetCurAllocations(i+1);
				AllocationCount--;
			}
		#endif

		#ifdef SURROUND_WITH_CHECK_VALUES
			CheckOneBlock(ptr);
		#endif

		#ifdef CLEAR_ON_RELEASE
			// Clear data we're about to free
			memset(ptr,BLOCK_RELEASED_CLEAR_VALUE,adjustedSize);
		#endif

		OSFree(ptr);

		HeapConsistencyCheck();
	}
#elif 1
	// *****************************************************************************
	// *****************************************************************************
	// PC/PS2
	// *****************************************************************************
	// *****************************************************************************
	inline void *OneAndOnlyMalloc(unsigned int size)
	{
#ifdef USE_NEW_MEMORY_MGR
		return NewMemory_malloc(size);
#else
		void *ptr;

		HeapConsistencyCheck();

		#ifdef SURROUND_MEMORY_BLOCKS
			// Allocate 16 bytes before, 4 after for size, trashing detection.
			// It possibly could be 8 bytes before, but I think that might
			// un-align data on the PS2 in case pointers are handed out on a
			// 16-byte boundary. 
			size+=(BLOCK_HEADER_BYTES+BLOCK_TRAILER_BYTES);
		#endif

		ptr = malloc(size);

		// If out of memory, we are in *so* much trouble. Flag it as so
		// for release/CD builds as well. - NM 6/26/02
#if (PLATFORM == PLATFORM_PS2)
		if(ptr == NULL)
			*GS_BGCOLOR = 0x0000080; // bright red border
#endif

		#ifdef TRACK_ALL_ALLOCATIONS
			// Ensure we don't overflow our buffers.
			_ASSERTE(AllocationCount < MAX_ALLOCATIONS_TRACKED);
			GetCurAllocations(AllocationCount) = ptr;
			AllocationCount++;
			BytesAllocated += size;
			
			TrackOverhead +=(BLOCK_HEADER_BYTES+BLOCK_TRAILER_BYTES);

			if(size == 786580) { // Surface bank cache
				SurfaceBanksUsed += size;
			}
			else if ((size== 6400084) || (size == 1440084)) {
				// DrawList allocs. ignore.
			}
			else if (size== 524308) {
				// VisPerm allocator
			}
			else if (size== 138818) {
				// zwp headers
			}
			else {
				if((size == 88888888) || (size == 9999999))
					printf("WATCH ME!\n");

				// Not categorized.
				if(size>32788)
					printf("+  this=%6d AllocTtl=%7d, (stat=%7d realuse=%7d). surf=%7d\n",
								 size,BytesAllocated,(unsigned int)(&_end),
								 BytesAllocated + ((unsigned int)(&_end)),SurfaceBanksUsed);
				else if((AllocationCount & 0xFFF) == 0) {
					printf(">  this=%6d AllocTtl=%7d, (stat=%7d realuse=%7d). surf=%7d. Alloc-Surf=%7d\n",
								 size,BytesAllocated,(unsigned int)(&_end),
								 BytesAllocated + ((unsigned int)(&_end)),SurfaceBanksUsed,
								 BytesAllocated-SurfaceBanksUsed);
				}
			}

			
		#endif

		#ifdef CLEAR_ON_ALLOCATE
			// Clear data we just allocated.
			memset(ptr,BLOCK_CLEAR_VALUE,size);
		#endif

		#ifdef SURROUND_WITH_CHECK_VALUES
			// Build up header: 4 bytes with our size, then 12 b
			unsigned int *ClearPtr;
			ClearPtr=(unsigned int *)ptr;
			ClearPtr[0]=size; // Size we allocated, kept around to check during free
			ClearPtr[1]=BLOCK_PRE_CLEAR_VALUE; // Pre-block trashing detection
			ClearPtr[2]=BLOCK_PRE_CLEAR_VALUE;
			ClearPtr[3]=BLOCK_PRE_CLEAR_VALUE;

			// Advance to the end of the block we allocated. This must be a
			// char*, as we may have been told to allocate a non-multiple of 4
			// bytes.
			unsigned char *EndPtr=(((unsigned char*)ptr) + size - BLOCK_TRAILER_BYTES);
			EndPtr[0]=(unsigned char)BLOCK_POST_CLEAR_VALUE;
			EndPtr[1]=(unsigned char)BLOCK_POST_CLEAR_VALUE;
			EndPtr[2]=(unsigned char)BLOCK_POST_CLEAR_VALUE;
			EndPtr[3]=(unsigned char)BLOCK_POST_CLEAR_VALUE;
		#endif

		#if (PLATFORM == PLATFORM_PS2)
			#ifdef DEBUG_MEMORY
				unsigned int ptr2=(unsigned int)ptr;
				if(ptr2 == 0x22a52c0)
					printf("Suspicious allocation. Breakpoint me!\n");
			#endif
		#endif

		#ifdef SURROUND_MEMORY_BLOCKS
			// Now, advance the pointer we allocated to skip over our header
			// before passing it back to the rest of the game.
			ptr=(void*) (((unsigned char*)ptr) + BLOCK_HEADER_BYTES);
		#endif

		#ifdef CHECK_AFTER_ALLOCATE
			HeapConsistencyCheck();
		#endif
		
		return ptr;
#endif
	}

	inline void OneAndOnlyFree(void *ptr)
	{
#ifdef USE_NEW_MEMORY_MGR
		NewMemory_free(ptr);
#else
		HeapConsistencyCheck();

		#ifdef SURROUND_MEMORY_BLOCKS
			// Back up pointer we passed to the rest of the game to get the real
			// pointer to our data and the pointer used by free.
			ptr=(void*) (((unsigned char*)ptr) - BLOCK_HEADER_BYTES);
			unsigned int *CheckPtr=(unsigned int *)ptr;
			unsigned int size=CheckPtr[0]; // WARNING: may not be valid
		#endif

		#ifdef TRACK_ALL_ALLOCATIONS
			BytesAllocated -= size;

			int i,ListPos=-1;
			bool FoundIt=false;
			for(i=0;i<AllocationCount;i++) {
				if(GetCurAllocations(i) == ptr) {
					FoundIt=true;
					ListPos=i;
					break; // get out of this loop.
				}
			}

			_ASSERTE(FoundIt);
			if(FoundIt) {
				for(i=ListPos;i<(AllocationCount-1);i++)
					GetCurAllocations(i)=GetCurAllocations(i+1);
				AllocationCount--;
			}

			TrackOverhead -=(BLOCK_HEADER_BYTES+BLOCK_TRAILER_BYTES);

			if(size == 786580) { // Surface bank cache
			}
			else if ((size== 6400084) || (size == 1440084)) {
				// DrawList allocs. ignore.
			}
			else if (size== 524308) {
				// VisPerm allocator
			}
			else if (size== 138818) {
				// zwp headers
			}
			else {
				// Not categorized.
				if(size>32788)
					printf("-  this=%6d AllocTtl=%7d, (stat=%7d realuse=%7d). surf=%7d\n",
								 size,BytesAllocated,(unsigned int)(&_end),
								 BytesAllocated + ((unsigned int)(&_end)),SurfaceBanksUsed);
				else if((AllocationCount & 0xFFF) == 0)
					printf("<  this=%6d AllocTtl=%7d, (stat=%7d realuse=%7d). surf=%7d MemOvhd=%6d\n",
								 size,BytesAllocated,(unsigned int)(&_end),
								 BytesAllocated + ((unsigned int)(&_end)),SurfaceBanksUsed,TrackOverhead);

			}
	

#if 0
			if(size > (1024*1024)) {
				printf("Large deallocation of %d bytes... examine?\n",size);
			}
#endif

		#endif

		#ifdef SURROUND_WITH_CHECK_VALUES
			CheckOneBlock(ptr);
		#endif

		#ifdef CLEAR_ON_RELEASE
			// Clear data we're about to free
			memset(ptr,BLOCK_RELEASED_CLEAR_VALUE,size);
		#endif

		#if 0//(PLATFORM == PLATFORM_PS2)
			unsigned long ptr2=(unsigned long)ptr;
			if(ptr2 == 0x2256bc0)
				printf("Suspicious deletion. Breakpoint me!\n");
		#endif

		free(ptr);

		#ifdef CHECK_AFTER_ALLOCATE
			HeapConsistencyCheck();
		#endif
#endif
	}
#else
/*
	extern "C" {
		void *dbgmalloc(unsigned int size);
		void dbgfree(void *ptr);
	}

	inline void *OneAndOnlyMalloc(unsigned int size)
	{
		void *ptr;
		ptr = dbgmalloc(size);
		return ptr;
	}
	inline void OneAndOnlyFree(void *ptr)
	{
		dbgfree(ptr);
	}
*/	
#endif


#ifdef ZERO_TRACK_MEMORY_ALLOCATION
// *****************************************************************************
// *****************************************************************************
// PC Leak Tracker
// *****************************************************************************
// *****************************************************************************
//static void CollectCallStack(unsigned int *callstack, int skip);
//static void DumpCallStack(unsigned int *callstack);
static const int FILE_NAME_SIZE = 52;

struct ZeroLeaksTrackerEntry
{
	void *ptr;
	unsigned int size;
	unsigned int id			: 8;
	unsigned int lineNumber : 24;
	char fileName[FILE_NAME_SIZE];
	
#if KEEP_CALLSTACK
	unsigned int callstack[STACK_DEPTH];
#endif

	ZeroLeaksTrackerEntry (unsigned size_, void* ptr_, const char* fileName_, unsigned lineNumber_, unsigned id_)
	{
		ptr = ptr_;
		size = size_;
		id = id_;
		lineNumber = lineNumber_;
		strncpy (fileName, fileName_, FILE_NAME_SIZE - 1);
#if KEEP_CALLSTACK
		CollectCallStack(callstack, 1);
#endif
	}
};

class ZeroMemoryLeakTracker: ZeroTree<ZeroLeaksTrackerEntry, unsigned>
{
	struct ZeroLeakTrackerAllocator: ZeroAllocator 
	{

		void *malloc (unsigned int size)
		{
			return OneAndOnlyMalloc (size);
		}
		void free (void *ptr)
		{
			OneAndOnlyFree (ptr);
		}
	};

	static ZeroLeakTrackerAllocator* GetZeroLeakTrackerAllocator()
	{
		static ZeroLeakTrackerAllocator sLeakTrackerAllocator;
		return &sLeakTrackerAllocator;
	}

	unsigned id;

	public:
	ZeroMemoryLeakTracker ()
		:ZeroTree<ZeroLeaksTrackerEntry, unsigned> (GetZeroLeakTrackerAllocator()), id(0)
	{
	}

	~ZeroMemoryLeakTracker ()
	{
		/*
		bool flag = false;
		if (flag)
		{
			unsigned int callstack[STACK_DEPTH];
			CollectCallStack(callstack, 0);
			DumpCallStack(callstack);
		}
		*/
		ZeroAssert ((!GetCount(), "!!! %d memory leaks left behind !!!", GetCount()));
		Iterator iter (*this);
		for (iter.Begin(); iter; iter ++)
		{
//			ZeroLeaksTrackerEntry& entry = (*iter)->info;
#ifdef WIN32
			char buf[256];
			sprintf(buf, "%s(%d) : address %p\n",
				(*iter)->info.fileName,
				(*iter)->info.lineNumber,
				(*iter)->info.ptr);
			OutputDebugString(buf);
#endif
#if KEEP_CALLSTACK
			//DumpCallStack((*iter)->info.callstack);
#endif
		}
	}

	static ZeroMemoryLeakTracker& GetLeakTracker()
	{
		static ZeroMemoryLeakTracker leakTrackerMap;
		return leakTrackerMap;
	}

	void SetMemoryIdentifier (unsigned idArg)
	{
		id = idArg;
	}

	void IncMemoryIdentifier ()
	{
		id++;
	}

	void PrintMemoryBlocks ()
	{
#ifdef WIN32
		char buf[256];
		sprintf(buf, "Memory Blocks with id %d:\n", id);
		OutputDebugString(buf);
#else
		ZeroTrace("Memory Blocks with id %d:\n", id);
#endif
		Iterator iter (*this);
		for (iter.Begin(); iter; iter ++)
		{
			if ((*iter)->info.id == id)
			{
#ifdef WIN32
				sprintf(buf, "%s(%d) : address %p\n",
					(*iter)->info.fileName,
					(*iter)->info.lineNumber,
					(*iter)->info.ptr);
				OutputDebugString(buf);
#else
				ZeroTrace("%s(%d) : address %p\n",
					(*iter)->info.fileName,
					(*iter)->info.lineNumber,
					(*iter)->info.ptr);
#endif
			}
		}
	}

	void RegisterMemoryBlock (void *ptr, void* retPointer, unsigned int size, const char *fileName, unsigned lineNumber)
	{
		ZeroAssert (((unsigned)retPointer != triggerAddress, "address %p triggered", triggerAddress));

		ZeroLeaksTrackerEntry entry (size, retPointer, fileName, lineNumber, id);

		allocatedMemory += (size + sizeof (ZeroTreeNode));
		Insert (entry, (unsigned) ptr);
	}

	void UnregisterMemoryBlock (void *ptr)
	{
		ZeroTreeNode* node;
		node = Find ((unsigned)ptr);
		ZeroAssert ((node, "freeing a memory block that was never allocated"));

		if (node) {
			allocatedMemory -= (node->info.size + sizeof (ZeroTreeNode));
			ZeroAssert ((allocatedMemory >= 0, "freing more memory than was allocated"));
			Remove(node);
		}
	}
};

// set the memory block identifier
void SetMemoryIdentifier(unsigned id)
{
	ZeroMemoryLeakTracker::GetLeakTracker().SetMemoryIdentifier(id);
}

// increment the memory block identifier
void IncMemoryIdentifier()
{
	ZeroMemoryLeakTracker::GetLeakTracker().IncMemoryIdentifier();
}

// display allocations with the current identifier
void PrintMemoryBlocks()
{
	ZeroMemoryLeakTracker::GetLeakTracker().PrintMemoryBlocks();
}

#else

// set the memory block identifier
void SetMemoryIdentifier(unsigned id)
{
}

// increment the memory block identifier
void IncMemoryIdentifier()
{
}

// display allocations with a specified identifier
void PrintMemoryBlocks()
{
}

#endif

#ifndef USE_NEW_MEMORY_MGR

#ifdef ZERO_TRACK_MEMORY_ALLOCATION
static const int alignShift = 2;
void* __cdecl ZeroMallocTrackLeaks (unsigned size, bool aligned, const char *fileName, unsigned lineNumber)
#else 
void* __cdecl ZeroMalloc (unsigned size, bool aligned, int alignShift)
#endif
{
	unsigned val;
	void *ptr;
	void *retPtr;
	void **originalPtr;

#ifdef ZERO_TRACK_MEMORY_ALLOCATION
	ZeroMemoryLeakTracker::GetLeakTracker();
#endif

	int alignMask = 1<<alignShift;

	retPtr = NULL; 
	if (size) {
#if ((PLATFORM == PLATFORM_GAMECUBE) || (PLATFORM == PLATFORM_PS2))
		// note that all values returned from OSAlloc are 32 byte aligned.
		// some functions such as DVDRead require this 32 byte alignment
		retPtr = OneAndOnlyMalloc(size);

#if (PLATFORM == PLATFORM_PS2)
#ifdef DEBUG
		// Verify that everything is aligned to 16 bytes
		unsigned int CheckPtr=(unsigned int) retPtr;
		_ASSERTE((CheckPtr & 0x0F) == 0);
		_ASSERTE(alignShift <= 4);
#endif
#endif

#else

		val = 0;
		ptr = NULL;
		size += sizeof (void*);
		if (aligned) {
			size += alignMask;
			ptr = OneAndOnlyMalloc (size);
			ZeroAssert(( ptr, "Out of memory!" ));
			val = (unsigned)ptr;
			val = (val + sizeof (void*) + alignMask - 1) & (unsigned) -alignMask;
		}  else {
			ptr = OneAndOnlyMalloc (size);
			ZeroAssert(( ptr, "Out of memory!" ));
			val = (unsigned)ptr;
			val += sizeof (void*);
		}

		retPtr = (void*) val;
		originalPtr = (void**)retPtr;
		originalPtr --;
		originalPtr[0]	= ptr;
#endif

#ifdef ZERO_TRACK_MEMORY_ALLOCATION
		ZeroMemoryLeakTracker::GetLeakTracker().RegisterMemoryBlock(ptr, retPtr, size, fileName, lineNumber);
#endif

	}

	return retPtr;
}
#endif

#ifndef USE_NEW_MEMORY_MGR

void __cdecl ZeroFree (void *ptr)
{
	void **originalPtr;
	if (ptr) {
#if ((PLATFORM != PLATFORM_GAMECUBE) && (PLATFORM != PLATFORM_PS2))
		originalPtr = (void**)ptr;
		originalPtr --;
		ptr = originalPtr[0];
#endif

#ifdef ZERO_TRACK_MEMORY_ALLOCATION
		ZeroMemoryLeakTracker::GetLeakTracker().UnregisterMemoryBlock (ptr);
#endif

		OneAndOnlyFree (ptr);
	}
}
#endif

unsigned __cdecl ZeroAllocatedCalls()
{
	return allocationCalls;
}

unsigned __cdecl ZeroAllocatedBytes()
{
#ifdef SURROUND_MEMORY_BLOCKS
	return totalalloced;
#else
	return allocatedMemory;
#endif
}

static const int MAXIMUM_ALLOCATORS = 256;
ZeroAllocator *ZeroAllocator::mArray[MAXIMUM_ALLOCATORS];

ZeroAllocator::ZeroAllocator ()
{
	mIsSim = false;

	for (mIndex = 0; mIndex < MAXIMUM_ALLOCATORS; mIndex++)
	{
		if (mArray[mIndex] == NULL)
		{
			mArray[mIndex] = this;
			break;
		}
	}

#ifdef ZERO_TRACK_MEMORY_ALLOCATION
	ZeroMemoryLeakTracker::GetLeakTracker();
#endif
}

ZeroAllocator::ZeroAllocator (const ZeroAllocator &aOriginal)
{
	mIsSim = aOriginal.mIsSim;
	mIndex = aOriginal.mIndex;
	if (mIndex < MAXIMUM_ALLOCATORS)
	{
		mArray[mIndex] = this;
	}
}

ZeroAllocator::~ZeroAllocator ()
{
	if (mArray[mIndex] == this)
	{
		mArray[mIndex] = NULL;
	}
}


// *****************************************************************************
// *****************************************************************************
// Pool Allocator
// *****************************************************************************
// *****************************************************************************
#ifdef _DEBUG
static char *checkPtr = (char *)0x20493DC;
#endif

class ZeroPoolBaseAllocator: public ZeroAllocator
{
	struct PurgeList
	{
		PurgeList* next;

		PurgeList (PurgeList *first)
		{
			next = first;
		}
	};

	struct Pool
	{
		int count;
		char *buffer;
		Pool (char *ptr)
		{
			count = 0;
			buffer = ptr;
		}
	};

	struct ZeroAllocatorPool: public ZeroList<Pool>
	{
		int elementSizeInBytes;
		int heapSizeInElements;
	
		ZeroAllocatorPool(int heapSizeInElementsArg, int elemntSizeInBytesArg)
			:ZeroList<Pool>(ZeroAllocator::GetVisPermAllocator())
		{
#if !ENABLE_POOL_ALLOCATION
			// make it easier to track leaks by effectively disabling pooling
			heapSizeInElementsArg = 1;
#endif
			elementSizeInBytes = elemntSizeInBytesArg;
			heapSizeInElements = heapSizeInElementsArg;
			if (elementSizeInBytes > 1024 * 4) {
				heapSizeInElements = 1;
			}
			_ASSERTE (heapSizeInElements > 0);
			_ASSERTE (elementSizeInBytes > 0);

		}

		ZeroAllocatorPool(const ZeroAllocatorPool& clone)
			:ZeroList<Pool>(ZeroAllocator::GetVisPermAllocator())
		{
			elementSizeInBytes = clone.elementSizeInBytes;
			heapSizeInElements = clone.heapSizeInElements;
			_ASSERTE (heapSizeInElements > 0);
			_ASSERTE (elementSizeInBytes > 0);

			// AddNewPool ();
		}

		~ZeroAllocatorPool()
		{
			FreeAll();
		}

		void FreeAll()
		{
			Iterator iter (*this);
			for (iter.Begin(); iter; iter ++) {
				ZeroFree ((*iter).buffer);
			}	
			
			// need to also remove all nodes from list, otherwise if this function was
			// called again, it would incorrectly try to free memory again
			ZeroList<Pool>::RemoveAll();
		}

		ZeroListNode* AddNewPool ()
		{
			char *ptr;

			size_t BytesToAlloc = elementSizeInBytes * heapSizeInElements + sizeof (Pool);
			bool AllocatePermanent = true;
#if 0
			// Hackety-hack-hack. Lots of blocks of this size are showing up
			// in permanent memory after a load, then getting zapped on
			// FreePoolMemory. So, shove them over into temp ram instead. -
			// NM 9/7/02
			if(BytesToAlloc == 1416) {
				AllocatePermanent = false;
			}
#endif

			ptr = (char*)ZEROMALLOC (BytesToAlloc, 1,AllocatePermanent); 
			Pool pool (ptr);
			Append (pool);
			return GetLast();
		}
	
		void *malloc (unsigned int size)
		{
			char *ptr;
			ZeroListNode *node;

			node = GetLast();
			if (!node || (node->info.count >= heapSizeInElements)) {
				node = AddNewPool();
			}

			ptr = &node->info.buffer[node->info.count * elementSizeInBytes];
			node->info.count += 1;

			return ptr;
		}

		void VerifyEmpty(PurgeList *&purge)
		{
			// for each pool
			Iterator iter(*this);
			while (iter) {
				// see if all elements are on the purge list
				ZeroListNode *node = iter.GetNode();
				bool allPurged = true;
				char *ptr = node->info.buffer;
				// ZeroTrace(("checking block at %d\n", ptr));
				for (int i = 0; i < node->info.count; i++) {
					PurgeList *p = (PurgeList *)purge;
					while (p != NULL)
					{
						if ((char *)p == ptr)
							break;
						p = p->next;
					}
					if (p == NULL) {
						ZeroTrace(("address %p is not on the purge list\n", ptr));
						allPurged = false;
						break;
					}
					ptr += elementSizeInBytes;
				}
				++iter;
			}
		}

		void Purge(PurgeList *&purge)
		{
			// for each pool
			Iterator iter(*this);
			while (iter) {
				// see if all elements are on the purge list
				ZeroListNode *node = iter.GetNode();
				bool allPurged = true;
				char *ptr = node->info.buffer;
				// ZeroTrace(("checking block at %d\n", ptr));
#ifdef _DEBUG
				if (ptr == checkPtr)
					ZeroBreak();
#endif
				for (int i = 0; i < node->info.count; i++) {
					PurgeList *p = (PurgeList *)purge;
					while (p != NULL)
					{
						if ((char *)p == ptr)
							break;
						p = p->next;
					}
					if (p == NULL) {
						// ZeroTrace(("address %d is not on the purge list\n", ptr));
						allPurged = false;
						break;
					}
					ptr += elementSizeInBytes;
				}
				++iter;
				// if yes
				if (allPurged) {
					// ZeroTrace(("block at %d is all purged\n", node->info.buffer));
					// remove elements from purge list
					char *start = node->info.buffer;
					char *end = start + heapSizeInElements * elementSizeInBytes;
					PurgeList **prev = &purge;
					PurgeList *p = purge;
					int i = 0;
					while (p != NULL)
					{
						char *addr = (char *)p;
						if ((addr >= start) && (addr < end))
						{
							i++;
							// attach prev to next
							*prev = p->next;
						}
						else
						{
							// set up pointer to relink
							prev = &p->next;
						}
						p = p->next;
					}
					// debugging
					ZeroAssert((i == node->info.count, "purgelist corrupted"));
					// remove and release pool
					ZeroFree(node->info.buffer);
					Remove(node);
				}
			}
		}
		
		int DisplayStats()
		{
			int bytes = GetCount() * heapSizeInElements * elementSizeInBytes;
			ZeroTrace(("Pool Size %d allocated %d bytes\n", elementSizeInBytes, bytes));			
			return bytes;
		}

		void RemoveResets(PurgeList *&purge)
		{
			Iterator iter(*this);
			while (iter)
			{
				ZeroListNode *node = iter.GetNode();
				iter++;
				char *start = node->info.buffer;
				if (ZeroMemory_WillReset(start))
				{
					// remove elements from purge list
					char *end = start + heapSizeInElements * elementSizeInBytes;
					PurgeList **prev = &purge;
					PurgeList *p = purge;
					int i = 0;
					while (p != NULL)
					{
						char *addr = (char *)p;
						if ((addr >= start) && (addr < end))
						{
							i++;
							// attach prev to next
							*prev = p->next;
						}
						else
						{
							// set up pointer to relink
							prev = &p->next;
						}
						p = p->next;
					}
					Remove(node);
				}
			}
		}
	};


	PurgeList *purge;
	ZeroAllocatorPool	poolManager;
	public:

	ZeroPoolBaseAllocator (int heapSizeInElements, int elemntSizeInBytes, bool isSimArg)
		:ZeroAllocator(), poolManager(heapSizeInElements, elemntSizeInBytes)
	{
		mIsSim	= isSimArg; 
		purge	= NULL;
	}

	void FreeAll()
	{
		poolManager.FreeAll();
	}

	void *malloc (unsigned int size)
	{
		void *obj;

		_ASSERTE ((int)size == poolManager.elementSizeInBytes);

//		if (poolManager.elementSizeInBytes == 292)
//			ZeroBreak();

		obj = NULL;
		if (purge) {
			PurgeList *ptr;
			ptr = purge;
#ifdef _DEBUG
			unsigned int RealAddr=(unsigned int) ptr;
			if(RealAddr == 0x682d340)
				printf("NOTE: suspicious dealloc! Trap me!\n");
#endif

			purge	= purge->next;
			obj = ptr;
		} else {
			obj = poolManager.malloc(size);
		}

#ifdef ZERO_TRACK_POOL_MEMORY
		// put trigger address here
		if ( (unsigned)obj == triggerAddress )
		{
			// put breakpoint here
			int iii;
			iii=1;
		}
#endif

		return obj;
	}

	void free (void *ptr)
	{
	 	PurgeList *obj;

//		if (poolManager.elementSizeInBytes == 292)
//			ZeroBreak();
#ifdef _DEBUG
		unsigned int RealAddr=(unsigned int) ptr;
		if(RealAddr == 0x682d340)
			printf("NOTE: suspicious dealloc! Trap me!\n");
#endif

		obj = (PurgeList *) ptr;
		obj->next = purge;
		purge	= obj;
	}

	void Purge()
	{
		poolManager.Purge(purge);
	}

#ifdef ZERO_TRACK_POOL_MEMORY
	void VerifyEmpty()
	{
		poolManager.VerifyEmpty(purge);
	}
#endif

	int DisplayStats()
	{
		// displays the memory allocated for this allocated node
		return poolManager.DisplayStats();
	}

	void RemovePresets(int &total)
	{
		while (purge)
		{
			total += poolManager.elementSizeInBytes;
			purge = purge->next;
		}
		ZeroList<ZeroPoolBaseAllocator::Pool>::ZeroListNode *node = poolManager.GetLast();
		if (node)
		{
			int diff = poolManager.heapSizeInElements - node->info.count;
			node->info.count = poolManager.heapSizeInElements;
			total += poolManager.elementSizeInBytes * diff;
		}
	}

	void RemoveResets(void)
	{
		poolManager.RemoveResets(purge);
	}

};



class ZeroPoolBaseAllocatorTree: public ZeroTree<ZeroPoolBaseAllocator, int>
{
	bool isSim;
	public:
	ZeroPoolBaseAllocatorTree (bool isSimArg)
		:ZeroTree<ZeroPoolBaseAllocator, int>(ZeroAllocator::GetVisPermAllocator())
	{
		isSim = isSimArg;
	}

	~ZeroPoolBaseAllocatorTree()
	{
		RemoveAll();
	}

	void RemoveAll()
	{	
		Iterator iter(*this);
#ifdef ZERO_TRACK_POOL_MEMORY
		// for now, output stats prior to destruction
		DisplayStats();
		
		// walk the tree to check for non-freed memory (leaks!)
		for ( iter.Begin(); iter; iter++ )
			(*iter)->info.VerifyEmpty();
#endif

		for ( iter.Begin(); iter; iter++ )
			(*iter)->info.FreeAll();

		// call base class
		ZeroTree<ZeroPoolBaseAllocator, int>::RemoveAll();
	}

	void RemovePresets(int &total)
	{
		Iterator iter(*this);
		for (; iter; iter++)
		{
			ZeroTreeNode *node = iter.GetNode();
			node->info.RemovePresets(total);
		}
	}

	void RemoveResets(void)
	{
		// remove the reset nodes
		bool found = true;
		while (found)
		{
			found = false;
			Iterator iter(*this);
			for (; iter; iter++)
			{
				ZeroTreeNode *node = iter.GetNode();
				if (VisAllocator_WillReset(node))
				{
					found = true;
					Remove(node);
					break;
				}
			}
		}
		// let the non reset nodes reset if needed
		{
			Iterator iter(*this);
			for (; iter; iter++)
			{
				ZeroTreeNode *node = iter.GetNode();
				node->info.RemoveResets();
			}
		}
	}

	ZeroAllocator* GetAllocator (int heapSizeInElements, int elemntSizeInBytes)
	{
		ZeroTreeNode *node;

		node = Find (elemntSizeInBytes);
		if (!node) {
			ZeroPoolBaseAllocator sPoolBaseAllocator(heapSizeInElements, elemntSizeInBytes, isSim);
			node = Insert (sPoolBaseAllocator, elemntSizeInBytes);
		}
		return &node->info;
	}

	void Purge()
	{
		Iterator iter(*this);
		while (iter) {
			ZeroTreeNode *node = *iter;
			node->info.Purge();
			iter++;
		}
	}
	
	void DisplayStats()
	{
		// displays, for each tree entry, the memory allocated
		int bytes = 0;
		Iterator iter(*this);
		for ( iter.Begin(); iter; iter++ )
			bytes += (*iter)->info.DisplayStats();
		
		// display total pool allocated memory	
		float kb = bytes * 0.001f;
		ZeroTrace(("Total pool allocated memory = %f Kb\n", kb ));								
	}
};

static ZeroPoolBaseAllocatorTree* sAllocMap = NULL;
void ReleasePoolAllocator()
{
	_ASSERTE(sAllocMap);
	sAllocMap->RemoveAll();
}

void PurgePoolMemory()
{
	_ASSERTE(sAllocMap);
	sAllocMap->Purge();
}

static ZeroAllocator* GetCommonAllocator(int heapSizeInElements, int elemntSizeInBytes)
{
	static ZeroPoolBaseAllocatorTree sAlloc(true);
	sAllocMap = &sAlloc;
	return sAlloc.GetAllocator (heapSizeInElements, elemntSizeInBytes);
}

// this allocator create object in batches
// and is use for non simulation objects
ZeroAllocator *ZeroAllocator::GetVisAllocator(int heapSizeInElements, int elemntSizeInBytes)
{
	// there is no vis allocator anymore!
	return GetCommonAllocator(heapSizeInElements, elemntSizeInBytes);
}

// this allocator create object in batches
// and is use for simulation objects
ZeroAllocator* ZeroAllocator::GetSimAllocator(int heapSizeInElements, int elemntSizeInBytes)
{
	// there is no sim allocator anymore!
	return GetCommonAllocator(heapSizeInElements, elemntSizeInBytes);
}

// this allocator is use for normal system memroty allocation
ZeroAllocator *ZeroAllocator::GetGenericAllocator()
{
	static ZeroAllocator sGenericAllocator;
	return &sGenericAllocator;
}

#ifndef USE_NEW_MEMORY_MGR
ZeroAllocator *ZeroAllocator::GetTempAllocator(int heapSizeInElements, int elemntSizeInBytes)
{
	return GetCommonAllocator(heapSizeInElements, elemntSizeInBytes);
}
#else
class ZeroTempAllocator : public ZeroAllocator {
public:
	virtual void *malloc (unsigned int size)
	{
		return NewMemory_malloc(size, false, PLATFORM_ALIGNSHIFT);
	}

	virtual void free(void *ptr)
	{
		NewMemory_free(ptr);
	}
};

ZeroAllocator *ZeroAllocator::GetTempAllocator(int heapSizeInElements, int elemntSizInBytes)
{
	static ZeroTempAllocator sTempAllocator;
	return &sTempAllocator;
}
#endif

class ZeroVisPermAllocator : public ZeroAllocator {
public:
	ZeroVisPermAllocator(unsigned int startSize)
	{
		heapSize = startSize;
		heapStart = (char *)ZeroMalloc(heapSize, false);
		heapEnd = heapStart + heapSize;
		heapPtr = heapStart;
	}

	~ZeroVisPermAllocator()
	{
		ZeroTrace(("VisPermMem: %d\n", heapPtr - heapStart));
		ZeroFree(heapStart);
	}

	virtual void *malloc (unsigned int size)
	{
		if (heapPtr + size >= heapEnd)
		{
			ZeroAssert((0U, "make the initial heap size bigger!"));
			// the code below is to keep from crashing but we should never need it
			heapSize = 32*1024;
			if (size > heapSize)
				heapSize = size + 1024;
			heapStart = (char *)ZeroMalloc(heapSize, false);
			heapEnd = heapStart + heapSize;
			heapPtr = heapStart;
			// end heap extension
		}
		char *ptr = heapPtr;
		heapPtr += size;
		return ptr;
	}

	virtual void free(void *ptr)
	{
	}

	void Preset(void)
	{
		heapPtrMark = heapPtr;
	}

	void Reset(void)
	{
		memset(heapPtrMark, 0x55, heapPtr - heapPtrMark);
		heapPtr = heapPtrMark;
	}

	unsigned int heapSize;
	char *heapStart;
	char *heapEnd;
	char *heapPtr;

	// reset stuff
	char *heapPtrMark;
};

ZeroAllocator *ZeroAllocator::GetVisPermAllocator()
{
#if ENABLE_POOL_ALLOCATION

#if (PLATFORM == PLATFORM_PS2)
	// We need a bit more space here on PS2 than GC (and can't recover
	// from out of space error) - NM 6/20/02
	static ZeroVisPermAllocator sVisPermAllocator(1024*(640+32)); // BillG says this should be enough for anyone!
#else
	static ZeroVisPermAllocator sVisPermAllocator(1024*512);
#endif

#else
	static ZeroVisPermAllocator sVisPermAllocator(1024*1024*4);
#endif
	return &sVisPermAllocator;
}

void ZeroAllocator::PurgeAll()
{
	// JN: removed this
	_ASSERTE(false);
}


#ifdef USE_NEW_MEMORY_MGR

void ZeroMemory_Preset(void)
{
	int total = 0;
	sAllocMap->RemovePresets(total);
	ZeroTrace(("pool waste: %d\n", total));

	ZeroVisPermAllocator *visPermAllocator = (ZeroVisPermAllocator *)ZeroAllocator::GetVisPermAllocator();
	visPermAllocator->Preset();
}

void ZeroMemory_Reset(void)
{
	sAllocMap->RemoveResets();
	ZeroVisPermAllocator *visPermAllocator = (ZeroVisPermAllocator *)ZeroAllocator::GetVisPermAllocator();
	visPermAllocator->Reset();
}

bool VisAllocator_WillReset(void *ptr)
{
	char *p = (char *)ptr;
	ZeroVisPermAllocator *visPermAllocator = (ZeroVisPermAllocator *)ZeroAllocator::GetVisPermAllocator();
	_ASSERTE(p >= visPermAllocator->heapStart);
	_ASSERTE(p < visPermAllocator->heapEnd);
	return (p >= visPermAllocator->heapPtrMark);
}

#endif

#ifdef ZERO_TRACK_MEMORY_ALLOCATION
	void* __cdecl operator new (size_t size)
	{
	//	_ASSERTE (0);
		return ZeroMalloc (size, true);
	}

	void* __cdecl operator new[] (size_t size)
	{
	//	_ASSERTE (0);
		return ZeroMalloc (size, true);
	}

	void __cdecl operator delete (void *ptr)
	{
		ZeroFree (ptr);
	}

	void __cdecl operator delete[] (void *ptr)
	{
		ZeroFree (ptr);
	}


	void* __cdecl operator new (size_t size, unsigned lineNumber, const char *fileName)
	{
		return ZeroMallocTrackLeaks (size, true, fileName, lineNumber);
	}

	void* __cdecl operator new[] (size_t size, unsigned lineNumber, const char *fileName)
	{
		return ZeroMallocTrackLeaks (size, true, fileName, lineNumber);
	}

	void __cdecl operator delete (void *ptr, unsigned lineNumber, const char *fileName)
	{
		ZeroFree (ptr);
	}

	void __cdecl operator delete[] (void *ptr, unsigned lineNumber, const char *fileName)
	{
		ZeroFree (ptr);
	}
	
#else //#ifdef ZERO_TRACK_MEMORY_ALLOCATION
 
	// *****************************************************************************
	// *****************************************************************************
	// GAMECUBE
	// *****************************************************************************
	// *****************************************************************************
#ifndef USE_NEW_MEMORY_MGR
	void* __cdecl operator new (size_t size)
	{
		return ZeroMalloc (size, true);
	}

	void* __cdecl operator new[] (size_t size)
	{
		return ZeroMalloc (size, true);
	}

	void __cdecl operator delete (void *ptr)
	{
		ZeroFree (ptr);
	}

	void __cdecl operator delete[] (void *ptr)
	{
		ZeroFree (ptr);
	}
#endif // #ifndef USE_NEW_MEMORY_MGR

#endif //#ifdef ZERO_TRACK_MEMORY_ALLOCATION







// *****************************************************************************
// *****************************************************************************
// PC/XBOX
// *****************************************************************************
// *****************************************************************************
/*
#if ((PLATFORM == PLATFORM_XBOX) || !((PLATFORM == PLATFORM_X86_WIN32) && defined(_DEBUG)))
void DumpCallStack(unsigned int *callstack)
{
}

void CollectCallStack(unsigned int *callstack, int skip)
{
}
#else
// call stack tracking from dark reign

#include <imagehlp.h>
#include <tlhelp32.h>
#include <stdio.h>
#include <shellapi.h>

  ///////////////////////////////////////////////////////////////////////////////
  //
  // NameSpace CallStack
  //
  namespace CallStack
  {
	  typedef unsigned int U32;
	  typedef int S32;
	  typedef int Bool;

#define STDCALL __stdcall
#define LOG_WARN(a)
#define LOG_ERR(a) ZeroTrace (a);

#define DEBUG_EXCEPTION_EXCEPTION 0xE0000001
#define DEBUG_EXCEPTION_ERROR     0xE0000002
#define DEBUG_CALLSTACK_LIMIT     0x80000000

    //
    // Prototypes
    //
    S32 STDCALL Filter(LPEXCEPTION_POINTERS ep);
    void Dump(CONTEXT *context);
    void Registers(CONTEXT *context);
    void Collect(U32 ebp, U32 *buff, U32 num, U32 skip);
    void Display(U32 *buf);
    void Expand(U32 addr, char **function, char **object, char **library, char **module, U32 *displacement);

    //
    // Init: Initialize call stack system
    //
    // Load the symbol file and read the symbol offset table into memory
    //
    void Init()
    {

      // Take a snapshot of memory and figure out what modules are loaded
      typedef HANDLE (WINAPI *CREATESNAPSHOT)(DWORD, DWORD); 
      typedef Bool   (WINAPI *MODULEWALK)(HANDLE, MODULEENTRY32 *); 

      HMODULE       kernel;
      HANDLE        snapShot;

      CREATESNAPSHOT create32SnapShot;
      MODULEWALK     module32First;
      MODULEWALK     module32Next;

      kernel = GetModuleHandle("KERNEL32.DLL");
      create32SnapShot = (CREATESNAPSHOT) GetProcAddress(kernel, "CreateToolhelp32Snapshot");
      module32First    = (MODULEWALK)     GetProcAddress(kernel, "Module32First");
      module32Next     = (MODULEWALK)     GetProcAddress(kernel, "Module32Next");

      if (!create32SnapShot || !module32First || !module32Next)
      {
        LOG_WARN(("Could not find functions in KERNEL32.DLL"))
      }
      else
      {
        snapShot = create32SnapShot(TH32CS_SNAPMODULE, 0);
        if (snapShot == (HANDLE) -1)
        {
          LOG_WARN(("Could not perform image snapshot"))
        }
        else
        {
        }
        CloseHandle(snapShot);
      }

      FreeLibrary(kernel);
    }


    //
    // Done: Shutdown call stack system
    //
    void Done()
    {
    }


    //
    // Dump
    //
    // Dumps the CallStatck from where we were called from
    //
    void Dump()
    {
      LOG_ERR(("CallStack:"))

      U32 callstack[STACK_DEPTH];
      U32 basePointer;

      __asm mov basePointer, ebp;

      // Do the stack
      Collect(basePointer, callstack, STACK_DEPTH, 0);
      Display(callstack);

    }


    //
    // Dump
    //
    // Dumps the CallStack of "context"
    //
    void Dump(CONTEXT *context)
    {
      LOG_ERR(("CallStack:\n"))

      U32 callstack[STACK_DEPTH];

      // Do EIP
      callstack[0] = context->Eip;
      callstack[1] = 0;
      Display(callstack);

      // Do the stack
      Collect(context->Ebp, callstack, STACK_DEPTH, 0);
      Display(callstack);
    }


    //
    // Registers
    //
    // Dumps the Registers of "context"
    //
    void Registers(CONTEXT *context)
    {
      LOG_ERR(("Registers:"))
      LOG_ERR(("EAX: %08Xh   CS: %08Xh  DS: %08Xh", (U32) context->Eax, (U32) context->SegCs, (U32) context->SegDs))
      LOG_ERR(("EBX: %08Xh  EIP: %08Xh  ES: %08Xh", (U32) context->Ebx, (U32) context->Eip  , (U32) context->SegEs))
      LOG_ERR(("ECX: %08Xh   SS: %08Xh  FS: %08Xh", (U32) context->Ecx, (U32) context->SegSs, (U32) context->SegFs))
      LOG_ERR(("EDX: %08Xh  EBP: %08Xh  GS: %08Xh", (U32) context->Edx, (U32) context->Ebp  , (U32) context->SegGs))
      LOG_ERR(("ESI: %08Xh  ESP: %08Xh           ", (U32) context->Esi, (U32) context->Esp))
      LOG_ERR(("EDI: %08Xh                       ", (U32) context->Edi))

      LOG_ERR((" CF: %08xh PF:%d AF:%d ZF:%d SF:%d OF:%d", 
        (U32) context->EFlags,
        (int) context->EFlags & 0x0001 ? 1 : 0,
        (int) context->EFlags & 0x0004 ? 1 : 0,
        (int) context->EFlags & 0x0010 ? 1 : 0,
        (int) context->EFlags & 0x0040 ? 1 : 0,
        (int) context->EFlags & 0x0080 ? 1 : 0,
        (int) context->EFlags & 0x0100 ? 1 : 0,
        (int) context->EFlags & 0x1000 ? 1 : 0 ))
    }


    //
    // Collect
    //
    // Using ebp supplied, collect num items on the callstack and place them into 
    // the buffer pointed to by buf.  Skip the first "skip" iterations of the
    // callstack. 
    //
    void Collect(U32 ebp, U32 *buff, U32 num, U32 skip)
    {
      U32 basePointer;
      U32 frameLimit;
      U32 framePointer;
      U32 caller = 0;

      basePointer = ebp;
      num--;

      // keep retracing until the bottom of stack is reached or the buffer size is exceeded
      while (num) 
      {
        // find address of caller
        __try
        {
          caller = *((unsigned *) (basePointer + 4));
        }
        __except (Filter(GetExceptionInformation()))
        {
          num = 0;
        }

        if (caller > DEBUG_CALLSTACK_LIMIT)
        {
          num = 0;
        }

        // Add this stack frame to the list
        if (num)
        {
          if (skip)
          {
            // Skip over this one
            skip--;
          }
          else
          {
            // Save the caller into the buffer
            *(buff++) = caller;

            // decrement buffer size
            num--;
          }

          // start of stack frame data
          framePointer = basePointer + 8;
    
          // beginning of next stack frame
          __try
          {
            frameLimit = *((U32 *) basePointer);
          }
          __except (Filter(GetExceptionInformation()))
          {
            break;
          }

          // go to the next link in the chain
          basePointer = frameLimit;
        }
      }

      // Terminate the buffer
      *buff++ = 0x00;
    }


    //
    // DisplayCallStack
    //
    // Takes a NULL terminated array of stack points and dumps them
    // to the logging system...
    //
    // It also attempts to resolve the names of these functions
    //
    void Display(U32 *buf)
    {
      char *module;
      char *function;
      char *object;
      char *library;
      U32  displacement;

      while (*buf)
      {
        // Each time it returns print out the information found
        Expand(*buf, &function, &object, &library, &module, &displacement);

        // Print out the results
        LOG_ERR(("%08X +%04X %s %s %s %s\n", *buf, displacement, module, library, object, function))
        buf++;
      }
    }


    //
    // Expand
    //
    void Expand(U32 addr, char **function, char **object, char **library, char **module, U32 *displacement)
    {
      static char unknown[] = "[Unknown]";
        *function = unknown;
        *object = unknown;
        *library = unknown;
        *displacement = addr;
    }


    //
    // Filter
    //
    S32 STDCALL Filter(LPEXCEPTION_POINTERS ep)
    {
      switch (ep->ExceptionRecord->ExceptionCode)
      {
        case EXCEPTION_ACCESS_VIOLATION:
          return (EXCEPTION_EXECUTE_HANDLER);
          break;

        default:
          return (EXCEPTION_CONTINUE_SEARCH);
          break;
      }
    }
  }

  void DumpCallStack(unsigned int *callstack)
  {
	  CallStack::Display(callstack);
  }

  void CollectCallStack(unsigned int *callstack, int skip)
  {
      unsigned int basePointer;

      __asm mov basePointer, ebp;

      // Do the stack
	  CallStack::Collect(basePointer, callstack, STACK_DEPTH, skip);
  }

#endif
*/
