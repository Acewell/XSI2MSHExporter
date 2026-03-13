
// New Memory Manager for Zero/PS2.
// By Nathan Mates
//
// Goals: to create a memory manager that handles temporary and
// permanent memory much better, with integrated memory-trashing
// detection features, and more.
//
// The gcc malloc() libs only allocate memory fairly blindly, and in a
// linear format. Thus, many holes can exist in memory as files are
// loaded, dumped, etc.

// ------------------------------------
// Defines that affect how the memory manager behaves. These have been
// pulled into this file to reduce rebuilds when changing them.

#include "ZeroTypes.h"
#include "NewMemory.h"
#include "ZeroMemory.h"

//#include "sntty.h"
#define USE_HOST 0
#define TRACK_HI_LO 0

#ifdef USE_NEW_MEMORY_MGR


#if (PLATFORM == PLATFORM_PS2)
#include "..\GraphicPS2\PS2Video.h"

#define GS_BGCOLOR      ((volatile u_long *)(0x120000e0))

  #ifdef _DEBUG
//    #define DEBUG_MEMORY
  #endif

  // Nathan's flag to report memory usage periodically
  #ifdef _DEBUG
//    #define REPORT_MEMORY
  #endif

#endif

#ifdef DEBUG_MEMORY
	// Compile flag to zap memory when allocated.
	#define CLEAR_ON_ALLOCATE

	// Compile flag to surround blocks with check values, and check them on freeing
	#define SURROUND_WITH_CHECK_VALUES

  // Compile flag to store the Allocation ID in a block
  #define STORE_ALLOC_ID

	// Compile flag to zap memory when releasing
	#define CLEAR_ON_RELEASE

  // Compile flag to do heap consistenty checks (which can be tweaked by 
  #define DO_HEAP_CONSISTENCY_CHECK

  // Compile flag to never relase memory (uses a LOT of memory!)
  //  #define NEVER_RELEASE_MEMORY
  // Flavors of the above...
  //  #define NEVER_RELEASE_PERM_MEMORY
  //  #define NEVER_RELEASE_TEMP_MEMORY
#else

  // #define STORE_ALLOC_ID

#endif

// Define flag to complain on someone freeing memory marked as part
// of the preset block

#define COMPLAIN_ON_PRESET_FREE

// Check values to fill various chunks of memory with. Note, for PS2
// compatability, only the lowest byte is actually used in memset,
// etc. Thus, all 4 bytes should be the same here. - NM 7/17/02

// Value to fill data before a block
const unsigned int BLOCK_PRE_CLEAR_VALUE = 0xADADADAD;
// Value to fill data middle of a just-allocated block
const unsigned int BLOCK_CLEAR_VALUE = 0xCDCDCDCD;
// Value to fill data middle of a just-allocated block
const unsigned int BLOCK_POST_CLEAR_VALUE = 0xFDFDFDFD;
// Value to fill data released block
const unsigned int BLOCK_RELEASED_CLEAR_VALUE = 0xDDDDDDDD;

// Padding values, if SURROUND_WITH_CHECK_VALUES is on
#ifdef SURROUND_WITH_CHECK_VALUES
    const unsigned int BLOCK_HEADER_LONGS =  4; // in unsigned ints, so # bytes is this *4
    const unsigned int BLOCK_TRAILER_BYTES = 4; // Actually bytes (as might not be long-aligned)
#else
    const unsigned int BLOCK_HEADER_LONGS =  0; // in unsigned ints, so # bytes is this *4
    const unsigned int BLOCK_TRAILER_BYTES = 0; // Actually bytes (as might not be long-aligned)
#endif


// ------------------------------------
// Internal structures, etc.

const unsigned int PERM_ALLOCATED_MAGIC  = 0x5A3F9219;
const unsigned int SOLID_ALLOCATED_MAGIC = 0x3011DE23; // like perm, but never freeable
const unsigned int TEMP_ALLOCATED_MAGIC  = 0x8F241399;
const unsigned int NONPURGE_ALLOCATED_MAGIC = 0x93445FF3;
const unsigned int FREE_MAGIC            = 0xFD37DA77;
const unsigned int BOUNDARY_MAGIC        = 0x738F238B; // Start/end block


struct NewMemBlock
{
	// Flag that something is allocated, or free
	unsigned int Magic;
	// Length of the Data[] array, i.e. how big this block is if
	// allocated, or how much free space it is if this is a free block
	unsigned int Len;

	// Doubly-linked list of pointers to next, previous blocks. Note:
	// these aren't necessarily calculateable from this+Len, as there
	// may be a small gap rounding the next block up to the next
	// multiple of 4/16.
	NewMemBlock *prev;
	NewMemBlock *next;

#ifdef STORE_ALLOC_ID
	unsigned int AllocID;
#endif

#ifdef SURROUND_WITH_CHECK_VALUES
  unsigned int PreCheckBlock[BLOCK_HEADER_LONGS];
#endif

	unsigned char Data[0]; // Variable-sized, see Len for its real size.

#ifdef SURROUND_WITH_CHECK_VALUES
	// Note: this is not the address you're looking for. The real
	// address of this is Len bytes past Data. However, this is here so
	// that the size of this struct (without any Data) is correct.
	//  unsigned char PostCheckBlock[BLOCK_TRAILER_BYTES];
	// DevStudio complains about somehting after a 0-sized array, so ignore this...
#endif
};

const size_t BLOCK_HEADER_SIZE = sizeof(NewMemBlock);

// ------------------------------------
// Some globals (to this file, anyhow)

static void *MyHeap=NULL; // Pointer to my block of RAM
static NewMemBlock *gMemBase; // Bottom of memory block, used for allocating perm ram
static NewMemBlock *gMemTop; // Top of memory blocks, used for allocating temp ram.


// How much space we'll let be for a (permanent) block of
// memory. Setting this too small can lead to slowdowns due to blocks
// that can't be allocated, too large will waste memory. These are
// tracked in parallel so that an allocation will start from the
// appropriate search point.
const int NUM_SIZES_TRACKED = 4;
const size_t ALLOC_SIZES[NUM_SIZES_TRACKED] = { 32, 128, 512, 2048}; // MUST BE SORTED ORDER!
size_t gMemoryHoleSizes[NUM_SIZES_TRACKED]; // Filled in to be above + header size. 

// How much space we'll let be for a (temp) block of memory. Setting
// this too small can lead to slowdowns due to blocks that can't be
// allocated, too large will waste memory. Second number includes
// overhead
const int MIN_MEMORY_HOLE = 48;
const int REAL_MIN_MEMORY_HOLE = MIN_MEMORY_HOLE + BLOCK_HEADER_LONGS * sizeof(unsigned int) + BLOCK_TRAILER_BYTES * sizeof(unsigned char);

// Some cached pointers to the first & last blocks of memory marked as
// free. Done to speed up allocations w/o having to walk a big
// list. (Note: this may not point to a free block, but it must point
// to an address below/above the first free one).
static NewMemBlock *gFirstFreeBySize[NUM_SIZES_TRACKED]; // First (low) block of ram that's free.
static NewMemBlock *gLastFree; // Highest block of ram that's free.
static NewMemBlock *gMarkBlock;	// Highest allocated block at Preset time

// Tracker data. These are only used with DEBUG_MEMORY, but this shouldn't hurt the binary...
static unsigned int NumMemBlocks = 0;
// Total heap size.
static unsigned int gHeapSize;
// Amount of memory flagged as in use.
static unsigned int gUsedMemory; 
// Amount of memory flagged as free.
static unsigned int gFreeMemory;
// Amount of memory lost to memmgr usage.
static unsigned int gMemoryOverhead;
// Unique identifier to add to every used memory block, if desired.
static unsigned int gNumBlocksAllocated=0;
// Unique identifier to add to every used memory block, if desired.
static unsigned int gNumBlocksFreed=0;
// Complain-on-free-perm memory flag. Should go to false only when
// dumping FE/game out of memory.
#ifdef WIN32
static bool ComplainOnFreePerm=false;
#else
static bool ComplainOnFreePerm=false;
#endif

#if (PLATFORM == PLATFORM_PS2)
// End of app, static data in app. Used to get a better idea as to
// memory footprint
extern unsigned long _end;
static unsigned int ApplicationEnd; // 
extern unsigned int StaticDrawListSize;
extern unsigned int MainDrawListSize;
extern unsigned int MainDrawListLimit; // How much was ever used at once.
#else
static unsigned long _end = 0;
#endif

// flag to constantly check memory, if the above flags are valid.
#ifdef _DEBUG
bool ConstantlyCheckMemory=false;
#else
bool ConstantlyCheckMemory=false;
#endif

static bool inBatch = false;
static void BatchAlloc(size_t size, bool perm, int align);
static void BatchFree(size_t size, bool perm);

// Utility function. Verifies the type of the block is valid
static inline bool ValidBlockType(NewMemBlock *p)
{
	return (p->Magic == PERM_ALLOCATED_MAGIC) || (p->Magic == TEMP_ALLOCATED_MAGIC) ||
		(p->Magic == FREE_MAGIC) || (p->Magic == BOUNDARY_MAGIC) || 
		(p->Magic == SOLID_ALLOCATED_MAGIC) || (p->Magic == NONPURGE_ALLOCATED_MAGIC);
}

// Utility function. Verifies the type of the block is valid
static inline bool BlockHasGuardBytes(NewMemBlock *p)
{
	return (p->Magic == PERM_ALLOCATED_MAGIC) || (p->Magic == TEMP_ALLOCATED_MAGIC) ||
		(p->Magic == SOLID_ALLOCATED_MAGIC) || (p->Magic == NONPURGE_ALLOCATED_MAGIC);
}

// Checks one block for consistency, with p pointing to the true start
// of a block (i.e. (long)ptr[0] is the size, [1] the start of the
// guard stuff, etc
static NewMemBlock *LastCheckedBlock; 
static NewMemBlock *gWatchBlock=(NewMemBlock *)(0x682d330); 
static inline void CheckOneBlock(NewMemBlock *p)
{
#ifdef SURROUND_WITH_CHECK_VALUES
	LastCheckedBlock=p;
	int i;

	_ASSERTE(ValidBlockType(p));

	// Sanity check-- make sure that the length of this block doesn't
	// intrude on the next
	if(p->next != NULL) {
		unsigned int NextStart=(unsigned int)(p->next);
		unsigned int CalcEnd=(unsigned int)(p->Data + p->Len);
		_ASSERTE(CalcEnd <= NextStart);
	}

	if(!BlockHasGuardBytes(p)) {
		// todo: WRITEME! [check all of this block? Not sure...]
		return;
	}
	
	for(i=0;i<BLOCK_HEADER_LONGS;i++) {
#ifdef _DEBUG
		_ASSERTE(p->PreCheckBlock[i] == BLOCK_PRE_CLEAR_VALUE);
#else
#ifndef CDROM
		if(p->PreCheckBlock[i] != BLOCK_PRE_CLEAR_VALUE)
			printf("ERROR: mem trash at %08X. Is %08X, should be %08X\n",
						 p->PreCheckBlock+i,p->PreCheckBlock[i],BLOCK_PRE_CLEAR_VALUE);
#endif
#endif
	}
	
	unsigned char *RealEndPtr=p->Data + p->Len;
	const unsigned char PostCheckValue=(const unsigned char)BLOCK_POST_CLEAR_VALUE;
	for(i=0;i<BLOCK_TRAILER_BYTES;i++) {
#ifdef _DEBUG
		_ASSERTE(RealEndPtr[i] == PostCheckValue);
#else
#ifndef CDROM
		if(RealEndPtr[i] != PostCheckValue)
			printf("ERROR: mem trash at %08X. Is %02X, should be %02X\n",
						 RealEndPtr+i,RealEndPtr[i],PostCheckValue);
#endif
#endif
	}
#endif
}

#if TRACK_HI_LO

static unsigned char *permHi = (unsigned char *)0;
static unsigned char *tempLo = (unsigned char *)0x1fffffff;

void TrackPerm(NewMemBlock *blk)
{
	if (blk->Data > permHi + 256*1024)
	{
		permHi = blk->Data;
		printf("permHi(0x%x) tempLo(0x%x) diff(%d)\n", permHi, tempLo, tempLo - permHi);
	}
}

void TrackTemp(NewMemBlock *blk)
{
	if (blk->Data < tempLo - 256*1024)
	{
		tempLo = blk->Data;
		printf("tempLo(0x%x) permHi(0x%x)\n", tempLo, permHi, tempLo - permHi);
	}
}

#else

inline void TrackPerm(NewMemBlock *)
{
}

inline void TrackTemp(NewMemBlock *)
{
}

#endif


#ifdef CDROM
static inline
#endif
void NewMemory_HeapConsistencyCheck()
{
#ifdef DO_HEAP_CONSISTENCY_CHECK
	if(!ConstantlyCheckMemory)
		return;

	// Scan the linked list, forwards and backwards to ensure the count is sane
	// and all 
	unsigned int BlockCount;
	NewMemBlock *cur=gMemBase;
	BlockCount=0;
	bool FoundFirstFree[NUM_SIZES_TRACKED];
	memset(FoundFirstFree,0,sizeof(FoundFirstFree));
	int i;

#ifdef _DEBUG
	// Keep a mini-chain of the last few block ptrs in varbs, so if the
	// chain is corrupted, we have a little more idea as to where it
	// went sour.
	NewMemBlock *Last=NULL;
	NewMemBlock *Last2=NULL;
	NewMemBlock *Last3=NULL;
#endif

	while(cur) {
#ifdef _DEBUG
		Last3=Last2;
		Last2=Last;
		Last=cur;
#endif
		_ASSERTE(ValidBlockType(cur));

		for(i=0;i<NUM_SIZES_TRACKED;i++) {
			if((!FoundFirstFree[i]) && (cur->Magic == FREE_MAGIC) && (cur->Len >= gMemoryHoleSizes[i])) {
				_ASSERTE(cur == gFirstFreeBySize[i]);
				FoundFirstFree[i]=true;
			}
		}

		// Sanity check-- make sure that the length of this block doesn't
		// intrude on the next
		if(cur->next != NULL) {
			unsigned int NextStart=(unsigned int)(cur->next);
			unsigned int CalcEnd=(unsigned int)(cur->Data + cur->Len);
			_ASSERTE(CalcEnd <= NextStart);
		}

		BlockCount++;
		// Ensure we're not in an infinite loop!
		_ASSERTE(BlockCount < (NumMemBlocks+4));
		cur=cur->next;
	}
	_ASSERTE(BlockCount == NumMemBlocks); // count must match *exactly*

	// Same thing, from the other end.
	cur=gMemTop;
	BlockCount=0;
	FoundFirstFree[0]=false;
	while(cur) {
		_ASSERTE(ValidBlockType(cur));

		if((!FoundFirstFree[0]) && (cur->Magic == FREE_MAGIC) && (cur->Len >= REAL_MIN_MEMORY_HOLE)) {
			_ASSERTE(cur == gLastFree);
			FoundFirstFree[0]=true;
		}

		BlockCount++;
		// Ensure we're not in an infinite loop!
		_ASSERTE(BlockCount < (NumMemBlocks+4));
		cur=cur->prev;
	}
	_ASSERTE(BlockCount == NumMemBlocks); // count must match *exactly*

	// Now that we know the onelinks are ok, check each block.
	cur=gMemBase;
	while(cur) {
		CheckOneBlock(cur);
		cur=cur->next;
	}
#endif
}

// Checks one block by a pointer passed in.
void NewMemory_CheckBlock(void *p)
{
	// Sanity checks, firstoff.
	if(p==NULL)
		return;
	unsigned char *pData=(unsigned char*)p;
	// Back up to real header
	NewMemBlock *pBlock=(NewMemBlock *)(pData-BLOCK_HEADER_SIZE);

	NewMemory_CheckBlock(pBlock);
}

// Allocates the specified block of RAM, w/ malloc. This should be the
// last use of malloc in the game.
void NewMemory_BootInit(unsigned int Heapsize)
{
	int i;

#ifdef WIN32
	// Need to be initialized by first malloc (which is part of global
	// constructor startup, premain. Avoid double-inits.
	if(gHeapSize)
		return;
#endif

#if (PLATFORM == PLATFORM_PS2)
	ApplicationEnd = (unsigned int)(&_end);
#endif

	MyHeap=malloc(Heapsize);

#if 1//def DEBUG_MEMORY
	// HACK -- to avoid a crash on the PS2, initialize the heap at
	// startup. This is a 1-time cost, so it shouldn't be *that* bad.
	memset(MyHeap,BLOCK_RELEASED_CLEAR_VALUE,Heapsize);
#endif

	// Last block is a special 0-size block. Figure out where the end is.
	unsigned char *LastHeader=(unsigned char*) ((((unsigned int)MyHeap) + Heapsize - BLOCK_HEADER_SIZE - 8) & 0xFFFFFFFC);

	// Create special boundary blocks at start/end
	gMemTop=(NewMemBlock*)LastHeader;
	gMemTop->Magic=BOUNDARY_MAGIC;
	gMemTop->Len = 0;

	gMemBase=(NewMemBlock*)MyHeap;
	gMemBase->Magic=BOUNDARY_MAGIC;
	gMemBase->Len = 0;

	NewMemBlock *FreeBlock=(NewMemBlock*)((((unsigned int)(gMemBase->Data)) + 3) &0xFFFFFFFC);
	FreeBlock->Magic=FREE_MAGIC;
	FreeBlock->Len=(unsigned int) (((unsigned char *)gMemTop) - FreeBlock->Data);

	// Link up this list.
	gMemBase->prev=NULL;
	gMemBase->next=FreeBlock;
	FreeBlock->prev=gMemBase;
	FreeBlock->next=gMemTop;
	gMemTop->prev=FreeBlock;
	gMemTop->next=NULL;

	// Note some stats here...
	NumMemBlocks=3; // We've got 3 blocks in the list total-- 2 boundary, 1 free
	gUsedMemory = gMemoryOverhead = 3 * BLOCK_HEADER_SIZE;
	gFreeMemory = FreeBlock->Len;
	gHeapSize=Heapsize;

	gLastFree=FreeBlock;

	for(i=0;i<NUM_SIZES_TRACKED;i++) {
		// Ensure the table is in sorted order...
		if(i>0) {
			_ASSERTE(ALLOC_SIZES[i] > ALLOC_SIZES[i-1]);
		}

		// Note how much space is really needed in each chunk.
		gMemoryHoleSizes[i] = ALLOC_SIZES[i] + BLOCK_HEADER_SIZE + BLOCK_TRAILER_BYTES * sizeof(unsigned char);
		// Assume the heap is sufficient to hold all blocks by size
		gFirstFreeBySize[i]=FreeBlock;
	}

	NewMemory_HeapConsistencyCheck();
}

// Allocates permanent ram. Split from temp for less branching during
// runs. Returns a pointer to a block, or NULL if out of memory (BAD!)
static NewMemBlock *NewMemory_mallocPerm(size_t size, int Alignment)
{
	// Calculate masks for alignment-- this is how much must be added to
	// a pointer to safely guarantee alignment after the mask is
	// applied.
	int AlignSlop=(1<<Alignment)-1;
	int AlignMask=~AlignSlop;

	// Where the data payload of a new block should be placed (aligned
	// to specified place)
	unsigned char *DataStart=NULL;

	// Minimum size of a block to consider, accounting for overhead
	size_t SizeWithPadding = size + BLOCK_HEADER_LONGS * sizeof(unsigned int) + BLOCK_TRAILER_BYTES * sizeof(unsigned char);

	// Look from the first block we know is free of any size, going
	// earlier if possible.
	NewMemBlock *cur=gFirstFreeBySize[NUM_SIZES_TRACKED-1];
	int i;
	for(i=0;i<NUM_SIZES_TRACKED;i++) {
		if((gMemoryHoleSizes[i] >= SizeWithPadding) && (gFirstFreeBySize[i])) {
			cur = gFirstFreeBySize[i];
			break;
		}
	}

#ifdef DEBUG_MEMORY
	// Keep a mini-chain of the last few block ptrs in varbs, so if the
	// chain is corrupted, we have a little more idea as to where it
	// went sour.
	_ASSERTE(cur->Magic == FREE_MAGIC);
	int MinSize= REAL_MIN_MEMORY_HOLE;
	if(MinSize > gMemoryHoleSizes[0])
		MinSize = gMemoryHoleSizes[0];
	_ASSERTE(cur->Len >= MinSize);
	NewMemBlock *Last=NULL;
	NewMemBlock *Last2=NULL;
	NewMemBlock *Last3=NULL;
#endif

	// Note the surrounding blocks. May *not* be NULL -- we must have
	// the boundary blocks at the start/end of things, so a free block
	// must be surrounded by other blocks.
	NewMemBlock *Prev=NULL;
	NewMemBlock *Next=NULL;

	do {
#ifdef DEBUG_MEMORY
		Last3=Last2;
		Last2=Last;
		Last=cur;
		_ASSERTE(ValidBlockType(cur));
#endif

		if((cur->Magic == FREE_MAGIC) && (cur->Len >= SizeWithPadding)) {
			// See if we'll have enough space to put this block. If so, use it.

			// Figure out the next possible starting address within this block.
			unsigned char *FreeStart=cur->Data; // Where data could first start...
			DataStart=(unsigned char*) ((((unsigned int)FreeStart) + AlignSlop) & AlignMask);
			
			// Note how much space would be needed by headers, etc if we
			// carve this up.  If it'd fit, do so.
			unsigned int NextBlock=(unsigned int)(cur->Data + cur->Len);
			unsigned int CalcEnd=(unsigned int)(DataStart + size + BLOCK_TRAILER_BYTES);
			if(CalcEnd <= NextBlock) {
				// Note the surrounding blocks. May *not* be NULL -- we must have
				// the boundary blocks at the start/end of things, so a free block
				// must be surrounded by other blocks.
				Prev=cur->prev;
				Next=cur->next;
				break; // Fit! get out of while loop!
			}

			// Oops, not enough space in this block. Move on.
		}

		// This block is wrong type, or out of space. Look at next.
		cur=cur->next;
	} while(cur != NULL);

	if(cur == NULL) {
#if (PLATFORM == PLATFORM_PS2)
		*GS_BGCOLOR = 0x0000080; // bright red border
#endif
#if !defined(CDROM) || USE_HOST
		printf("COULD NOT ALLOCATE PERM MEMORY, size 0x%08X (%d). Mini mem dump:\n",size,size);
		NewMemory_ShowBlocks(false);
		int *StopNow = NULL;
		*StopNow++ = 17;
		*StopNow++ = 18;
		*StopNow++ = 19;
		*StopNow++ = 12;
#endif
		_ASSERTE(0); // OUT OF MEMORY!
		return NULL; // flag that...
	}

#ifdef DEBUG_MEMORY
	_ASSERTE(Prev != NULL);
	_ASSERTE(Next != NULL);
#endif
	NewMemBlock *NewBlock=NULL; // What we'll be using, and allocating into

	// Actually allocate it, out of either the start or the end of the block.
	// Make a new header for this block, backing up from where we know
	// we can place data.
	NewBlock=(NewMemBlock*)(DataStart-BLOCK_HEADER_SIZE);
	NewBlock->Magic=PERM_ALLOCATED_MAGIC;
	NewBlock->Len = size;
	
	unsigned char *NewEnd=NewBlock->Data+size+BLOCK_TRAILER_BYTES;
	unsigned char *pNext=(unsigned char *)Next;
		
	// Now, see, if there's enough space to make a block of at least
	// MIN_MEMORY_HOLE bytes afterwards.
	if((NewBlock->Data + size + BLOCK_HEADER_SIZE + BLOCK_TRAILER_BYTES + REAL_MIN_MEMORY_HOLE) < pNext) {
		// Yes, make a new block...
		NewMemBlock *NewFree=(NewMemBlock*)(((unsigned int)NewEnd+3) & 0xFFFFFFFC);
		NewFree->Magic=FREE_MAGIC;
		NewFree->Len=(pNext - NewFree->Data);
		_ASSERTE(NewFree->Len >= REAL_MIN_MEMORY_HOLE);
			
		// Ok, link up Prev <-> NewBlock <-> newfree <-> Next. [Note: cur is cut out
		// of the loop now.]
		Prev->next= NewBlock;
		NewBlock->prev = Prev;
		NewBlock->next = NewFree;
		NewFree->prev = NewBlock;
		NewFree->next = Next;
		Next->prev = NewFree;
		// Sanity check to keep track of blocks. We added 1 here.
		++NumMemBlocks;
		gMemoryOverhead += BLOCK_HEADER_SIZE + BLOCK_TRAILER_BYTES;
		// Update tracker-- if we chopped up the first free block, then
		// update where to look for free blocks later.
		for(i=0;i<NUM_SIZES_TRACKED;i++) {
			if(cur == gFirstFreeBySize[i])
				gFirstFreeBySize[i]=NewFree;
		}
		if(cur == gLastFree)
			gLastFree = NewFree;
	}
	else {
		// Not enough space left to make a new free block.
		// link up Prev <-> NewBlock <-> Next  (cur is replaced by NewBlock)
		Prev->next= NewBlock;
		NewBlock->prev = Prev;
		NewBlock->next = Next;
		Next->prev = NewBlock;

		// Update tracker-- if we chopped up the first free block, then
		// update where to look for free blocks later.
		for(i=0;i<NUM_SIZES_TRACKED;i++) {
			if(cur == gFirstFreeBySize[i])
				gFirstFreeBySize[i]=Next;
		}

		if(cur == gLastFree) {
			gLastFree = Prev;
			while((gLastFree->Magic != FREE_MAGIC) || (gLastFree->Len < REAL_MIN_MEMORY_HOLE))
				gLastFree = gLastFree->prev;		
		}

		// Sanity check to keep track of blocks. We didn't add any
		// blocks, just turned cur into NewBlock.
		//			NumMemBlocks+=0;
	}

	TrackPerm(NewBlock);

	return NewBlock;
}


// Allocates temp ram. Split from permanent for less branching during
// runs. Returns a pointer to a block, or NULL if out of memory (BAD!)
static NewMemBlock *NewMemory_mallocTemp(size_t size, int Alignment)
{
	// Calculate masks for alignment-- this is how much must be added to
	// a pointer to safely guarantee alignment after the mask is
	// applied.
	int AlignSlop=(1<<Alignment)-1;
	int AlignMask=~AlignSlop;

	// Look from the first block we know is free.
	NewMemBlock *cur=gLastFree;

	// Where the data payload of a new block should be placed (aligned
	// to specified place)
	unsigned char *DataStart=NULL;

	// Minimum size of a block to consider, accounting for overhead
	size_t SizeWithPadding = size + BLOCK_HEADER_LONGS * sizeof(unsigned int) + BLOCK_TRAILER_BYTES * sizeof(unsigned char);

#ifdef DEBUG_MEMORY
	// Keep a mini-chain of the last few block ptrs in varbs, so if the
	// chain is corrupted, we have a little more idea as to where it went sour.
	NewMemBlock *Last=NULL;
	NewMemBlock *Last2=NULL;
	NewMemBlock *Last3=NULL;
#endif

	// Note the surrounding blocks. May *not* be NULL -- we must have
	// the boundary blocks at the start/end of things, so a free block
	// must be surrounded by other blocks.
	NewMemBlock *Prev=NULL;
	NewMemBlock *Next=NULL;

	do {
#ifdef DEBUG_MEMORY
		Last3=Last2;
		Last2=Last;
		Last=cur;
		_ASSERTE(ValidBlockType(cur));
#endif

		if((cur->Magic == FREE_MAGIC) && (cur->Len >= SizeWithPadding)) {
			// See if we'll have enough space to put this block. If so, use it.

			// Calculate the last (highest) address we could put a chunk of
			// the specified size to fit just before 'Next'. If that's
			// greater than the current (free) block's data start, we can
			// fit here.
			unsigned int tDataStart=(((unsigned int)cur->next) - size - BLOCK_TRAILER_BYTES) & AlignMask;
			DataStart=(unsigned char*)tDataStart;
			if(DataStart >= cur->Data) {
				// Note the surrounding blocks. May *not* be NULL -- we must have
				// the boundary blocks at the start/end of things, so a free block
				// must be surrounded by other blocks.
				Prev=cur->prev;
				Next=cur->next;

				break; // Fit! get out of while loop!
			}

			// Oops, not enough space in this block. Move on.
		}

		// This block is wrong type, or out of space. Look at previous.
		cur=cur->prev;
	} while(cur != NULL);

	if(cur == NULL) {
#if (PLATFORM == PLATFORM_PS2)
		*GS_BGCOLOR = 0x0000080; // bright red border
#endif
#if !defined(CDROM) || USE_HOST
		printf("COULD NOT ALLOCATE TEMP MEMORY, size 0x%08X (%d). Mini mem dump:\n",size,size);
		NewMemory_ShowBlocks(false);
#endif
		_ASSERTE(0); // OUT OF MEMORY!
		return NULL; // flag that...
	} 

#ifdef DEBUG_MEMORY
	_ASSERTE(Prev != NULL);
	_ASSERTE(Next != NULL);
#endif

	NewMemBlock *NewBlock=NULL; // What we'll be using, and allocating into

	// Actually allocate it, out of either the start or the end of the block.
	// Temp ram. Allocate at the end of things.

	// Make a new header for this block, backing up from where we know
	// we can place data.
	NewBlock=(NewMemBlock*)(DataStart-BLOCK_HEADER_SIZE);
	NewBlock->Magic=TEMP_ALLOCATED_MAGIC;
	NewBlock->Len = size;
		
	// Now, see, if there's enough space to make a block of at least 8
	// bytes before this block.
	unsigned char* pNewBlock=(unsigned char*)NewBlock;
	if((cur->Data + BLOCK_HEADER_SIZE + BLOCK_TRAILER_BYTES + 8) < pNewBlock) {
		// There's space to shorten Cur, and place NewBlock after it. Do so.
			
		// Shorten Cur...
		cur->Len = pNewBlock - cur->Data;
		// Link up cur <-> NewBlock <-> Next (no need to touch Prev)
		cur->next = NewBlock;
		NewBlock->prev = cur;
		NewBlock->next = Next;
		Next->prev = NewBlock;
		// Sanity check to keep track of blocks. We added 1 here.
		++NumMemBlocks;
		gMemoryOverhead += BLOCK_HEADER_SIZE + BLOCK_TRAILER_BYTES;

		// Nothing needs to be done to gLastFree here, as we know cur
		// has space in it. 
	}
	else {
		// Not enough space to chop up cur. Replace it with NewBlock, 
		// link up Prev <-> NewBlock <-> Next  
		// Not enough space left to make a new free block.
		// link up Prev <-> NewBlock <-> Next  (cur is replaced by NewBlock)
		Prev->next= NewBlock;
		NewBlock->prev = Prev;
		NewBlock->next = Next;
		Next->prev = NewBlock;

		if(cur == gLastFree) {
			gLastFree=Prev;
			// Find first free block under this one.
			while((gLastFree->Magic != FREE_MAGIC) || (gLastFree->Len < REAL_MIN_MEMORY_HOLE))
				gLastFree = gLastFree->prev;
		}

		// Sanity check to keep track of blocks. We didn't add any
		// blocks, just turned cur into NewBlock.
		//			NumMemBlocks+=0;
	}

	// Now is there enough space, 

	TrackTemp(NewBlock);
	
	return NewBlock;
}

// Utility function to adjust the first free blocks 
static inline void NewMemory_AdjustFirstFrees(void)
{
	int i;
	for(i=0;i<NUM_SIZES_TRACKED;i++) {
		// Sizes must be in order! If this pointer is less, then the smaller
		// size had to be run forward. Snap this forward for speed.
		if((i>0) && (gFirstFreeBySize[i] < gFirstFreeBySize[i-1]))
			gFirstFreeBySize[i] = gFirstFreeBySize[i-1];

		// Make sure sizes of things are appropriate
		while((gFirstFreeBySize[i]) && 
					((gFirstFreeBySize[i]->Magic != FREE_MAGIC) || (gFirstFreeBySize[i]->Len < gMemoryHoleSizes[i])))
			gFirstFreeBySize[i] = gFirstFreeBySize[i]->next;
	}

	while((gLastFree->Magic != FREE_MAGIC) || (gLastFree->Len < REAL_MIN_MEMORY_HOLE))
		gLastFree = gLastFree->prev;
}

// static int trackAlloc = -1;

// Allocates memory of the specified size. IsPermanent is a flag as to
// what kind of memory to allocate. Alignment is the specified # of
// bits to align to-- if this is 4, it'll allocate to a 4-byte
// (32-bit) boundary, if 6, 16-byte boundary, etc.
void *NewMemory_malloc(size_t size, bool IsPermanent, int Alignment)
{
	// Effective C++, item 8 (page 34) says to treat 0-byte blocks as a
	// request for 1 byte - NM 7/12/02
	if(size == 0)
		size=1;

#ifndef CDROM
	if(size > (1024*1024))
		printf("Large Alloc (0x%08X (%d), %d, %d)\n",size, size, IsPermanent, Alignment);
#endif

#ifdef WIN32
	// Need to be initialized by first malloc (which is part of global
	// constructor startup, premain. 64MB ought to be enough on the PC
	// for a game fitting in 32-40MB for consoles...
	if(!gHeapSize)
		NewMemory_BootInit(1024*1024*96);
#else

	_ASSERTE(MyHeap != NULL); // Make sure we got allocated correctly.
#endif

	if (inBatch)
		BatchAlloc(size, IsPermanent, Alignment);

	// Scan memory, if compiled to do so.
	NewMemory_HeapConsistencyCheck();

	NewMemBlock *NewBlock;
	if(IsPermanent)
		NewBlock = NewMemory_mallocPerm(size, Alignment);
	else
		NewBlock = NewMemory_mallocTemp(size, Alignment);

	// Adjust trackers...
	NewMemory_AdjustFirstFrees();

	// All ready to hand memory back. If necessary, set up things.
#ifdef CLEAR_ON_ALLOCATE
	
	memset((void*)NewBlock->Data,BLOCK_CLEAR_VALUE,size);
#endif

#ifdef STORE_ALLOC_ID
	NewBlock->AllocID=gNumBlocksAllocated;
#endif

//	if (gNumBlocksAllocated == trackAlloc)
//		ZeroTrace(("break here\n"));

#ifdef SURROUND_WITH_CHECK_VALUES
	if(BLOCK_HEADER_LONGS > 0)
		memset(NewBlock->PreCheckBlock,BLOCK_PRE_CLEAR_VALUE,sizeof(NewBlock->PreCheckBlock));

	// Get the real end of the block. We can use sizeof() on the
	// postcheckblock, but we can't use its address.
	if(BLOCK_TRAILER_BYTES > 0) {
		unsigned char *RealEnd=NewBlock->Data + NewBlock->Len;
		memset(RealEnd,BLOCK_POST_CLEAR_VALUE,sizeof(unsigned char)* BLOCK_TRAILER_BYTES);
	}
#endif

	// Keep track of some of these stats.
	const size_t Footprint=(size + BLOCK_TRAILER_BYTES + BLOCK_HEADER_SIZE);
	gUsedMemory += Footprint;
	gFreeMemory -= Footprint;
	++gNumBlocksAllocated;
	if(0) { // abs(gNumBlocksAllocated-264) < 3) {
		ZeroTrace(("gNumBlocksAllocated now %d IsPerm=%d. size=%d NewBlock=%08X\n",gNumBlocksAllocated,IsPermanent,size,NewBlock));
	}

#ifndef CDROM
#ifdef REPORT_MEMORY
	if((gNumBlocksAllocated % 1000) == 0) {
#ifdef DEBUG_MEMORY
		bool OriginalCheck=ConstantlyCheckMemory;
		ConstantlyCheckMemory=true;
		NewMemory_HeapConsistencyCheck();
		ConstantlyCheckMemory=OriginalCheck;
#endif
		ZeroTrace(("# allocations = %d. Memory summary:\n",gNumBlocksAllocated));
		NewMemory_ShowBlocks(false);
	}
#endif
#endif

	NewMemory_HeapConsistencyCheck();

	return (void*)NewBlock->Data;
}

// Sets a block as non-purgeable by the reset code
void NewMemory_SetAsNonPurgeable(void *p)
{
	// Sanity checks, firstoff.
	if(p==NULL)
		return;

	unsigned char *pData=(unsigned char*)p;
	// Back up to real header
	NewMemBlock *pBlock=(NewMemBlock *)(pData-BLOCK_HEADER_SIZE);
	if((pBlock->Magic != PERM_ALLOCATED_MAGIC) && (pBlock->Magic != TEMP_ALLOCATED_MAGIC)
		 && (pBlock->Magic != NONPURGE_ALLOCATED_MAGIC)) {
		_ASSERTE(0); // double-free, or bad pointer passed in.
		return; // Don't damage memory any more. It's probably trashed already :(
	}

	pBlock->Magic = NONPURGE_ALLOCATED_MAGIC;

}

// Frees the specified block. Complains if double-freed, etc.
void NewMemory_free(void *p)
{
	// Sanity checks, firstoff.
	if(p==NULL)
		return;

#ifdef NEVER_RELEASE_MEMORY
	// Never release any memory... kinda dangerous here!
	++gNumBlocksFreed;
	if(0) { //abs(gNumBlocksFreed - 29898) < 10) {
		ZeroTrace(("gNumBlocksFreed now %d, p=%08X\n",gNumBlocksFreed,p));
		bool OriginalCheck=ConstantlyCheckMemory;
		ConstantlyCheckMemory=true;
		NewMemory_HeapConsistencyCheck();
		ConstantlyCheckMemory=OriginalCheck;
	}

	NewMemory_HeapConsistencyCheck();
	return;
#endif


	NewMemory_HeapConsistencyCheck();

	unsigned char *pData=(unsigned char*)p;
	// Back up to real header
	NewMemBlock *pBlock=(NewMemBlock *)(pData-BLOCK_HEADER_SIZE);
	if((pBlock->Magic != PERM_ALLOCATED_MAGIC) && (pBlock->Magic != TEMP_ALLOCATED_MAGIC)
		 && (pBlock->Magic != NONPURGE_ALLOCATED_MAGIC)) {
		_ASSERTE(0); // double-free, or bad pointer passed in.
		return; // Don't damage memory any more. It's probably trashed already :(
	}

	if (inBatch)
		BatchFree(pBlock->Len, pBlock->Magic == PERM_ALLOCATED_MAGIC);

#ifdef NEVER_RELEASE_PERM_MEMORY
	// Never release any memory... kinda dangerous here!
	if(pBlock->Magic == PERM_ALLOCATED_MAGIC) {
		++gNumBlocksFreed;
		if(0) { //abs(gNumBlocksFreed - 29898) < 10) {
			ZeroTrace(("gNumBlocksFreed now %d, p=%08X\n",gNumBlocksFreed,p));
			bool OriginalCheck=ConstantlyCheckMemory;
			ConstantlyCheckMemory=true;
			NewMemory_HeapConsistencyCheck();
			ConstantlyCheckMemory=OriginalCheck;
		}

		NewMemory_HeapConsistencyCheck();
		return;
	}
#endif

	// Check this block (heapchecks as a whole may be off, but we should
	// check this one block, as that's real fast.)
	CheckOneBlock(pBlock);

	if((pBlock->Magic == PERM_ALLOCATED_MAGIC) && (ComplainOnFreePerm)) {
		const bool DONT_FREE_PERM_MEMORY_ALL_THE_TIME = false;
		_ASSERTE(DONT_FREE_PERM_MEMORY_ALL_THE_TIME); 
	}

#ifdef CLEAR_ON_RELEASE
#ifdef SURROUND_WITH_CHECK_VALUES
	// Zap the check bytes also...
	memset(pBlock->PreCheckBlock,BLOCK_RELEASED_CLEAR_VALUE,pBlock->Len+(BLOCK_HEADER_LONGS*sizeof(unsigned int))+(BLOCK_TRAILER_BYTES*sizeof(unsigned char)));
#else
	memset(pBlock->Data,BLOCK_RELEASED_CLEAR_VALUE,pBlock->Len);
#endif
#endif

	NewMemBlock *Prev=pBlock->prev;
	NewMemBlock *Next=pBlock->next;

	// Note the types of our neighbors' blocks...
	bool PrevIsFree=(Prev->Magic == FREE_MAGIC);
	bool NextIsFree=(Next->Magic == FREE_MAGIC);

	// We now have 4 possible cases as to what memory can be here, as
	// we're guaranteed to be somewhere in the middle of the linked list
	// of memory (putting a boundary block at the end means it'll never
	// count as 'free')
	//
	// The cases are: (Alloc could be boundary)
	// Alloc-this-Alloc - set this to free, nothing else to do.
	// Free-this-Alloc - merge with previous block
	// Alloc-this-Free - merge next with this
	// Free-this-Free - merge this and next with prev

	// Update stats...
	const size_t Footprint=(pBlock->Len + BLOCK_TRAILER_BYTES + BLOCK_HEADER_SIZE);
	gUsedMemory -= Footprint;
	gFreeMemory += Footprint;
	int i;

	if((!PrevIsFree) && (!NextIsFree)) {
		// Recalculate length of this block (which might have extra space for headers, etc.
		pBlock->Len = ((unsigned char*)pBlock->next) - pBlock->Data;

		// A bit of a hack here... if this is a small block at the end of
		// a larger permanent block, subsume it into the larger one.
		if((pBlock->Len <= 16) && (Prev->Magic == PERM_ALLOCATED_MAGIC)) {
			// Figure out total length available here.
			// -- Don't do this-- it breaks the post-clear value detection. NM 9/16/02
			//			Prev->Len = ((unsigned char*)Next) - Prev->Data;
		
			// Rearrange links, cutting this block out of the loop
			Prev->next = Next;
			Next->prev = Prev;
			// Sanity check to keep track of blocks. We deleted one here.
			--NumMemBlocks;
			gMemoryOverhead -= BLOCK_HEADER_SIZE + BLOCK_TRAILER_BYTES;
		}
		else {
			// Flag this as a free block.
			pBlock->Magic = FREE_MAGIC;

			// Update first/last trackers, only if block is large enough.
			for(i=0;i<NUM_SIZES_TRACKED;i++) {
				if((pBlock->Len >= gMemoryHoleSizes[i]) && (gFirstFreeBySize[i] > pBlock))
					gFirstFreeBySize[i] = pBlock;
			}
			
			if(pBlock->Len >= REAL_MIN_MEMORY_HOLE) {
				if(gLastFree < pBlock)
					gLastFree = pBlock;
			}
		}

#ifndef CDROM
		gWatchBlock = pBlock;
#endif

	}
	else if((PrevIsFree) && (!NextIsFree)) {
		// Free-this-alloc. Merge w/ previous block.

		// Figure out total length available here.
		Prev->Len = ((unsigned char*)Next) - Prev->Data;
		
		// Rearrange links, cutting this block out of the loop
		Prev->next = Next;
		Next->prev = Prev;
		// Sanity check to keep track of blocks. We deleted one here.
		--NumMemBlocks;
		gMemoryOverhead -= BLOCK_HEADER_SIZE + BLOCK_TRAILER_BYTES;

		// Update first/last trackers, only if block is large enough.
		for(i=0;i<NUM_SIZES_TRACKED;i++) {
			if((Prev->Len >= gMemoryHoleSizes[i]) && (gFirstFreeBySize[i] > Prev))
				gFirstFreeBySize[i] = Prev;
		}

		// Rearrange trackers, only if block is large enough.
		if(Prev->Len >= REAL_MIN_MEMORY_HOLE) {
			if(gLastFree == pBlock)
				gLastFree=Prev;
			else if(gLastFree < Prev)
				gLastFree = Prev;
		}

#ifndef CDROM
		gWatchBlock = Prev;
#endif

	}
	else if((!PrevIsFree) && (NextIsFree)) {
		// Alloc-this-Free - merge next with this
		NewMemBlock *NextNext=Next->next; // block after this.
		// Calculate true length of what's left
		pBlock->Len = ((unsigned char*)NextNext) - pBlock->Data;
		// Rearrange links
		pBlock->next=NextNext;
		NextNext->prev=pBlock;
		pBlock->Magic = FREE_MAGIC;
		// Sanity check to keep track of blocks. We deleted one here.
		--NumMemBlocks;
		gMemoryOverhead -= BLOCK_HEADER_SIZE + BLOCK_TRAILER_BYTES;

		// Always must tweak this-- if pointers point to the block we're
		// zapping, must tweak them.
		// Update first/last trackers, only if block is large enough.
		for(i=0;i<NUM_SIZES_TRACKED;i++) {
			if(gFirstFreeBySize[i] == Next)
				gFirstFreeBySize[i] = pBlock;
		}
		if(gLastFree == Next)
			gLastFree = pBlock;

		// Rearrange trackers, only if block is large enough.
		for(i=0;i<NUM_SIZES_TRACKED;i++) {
			if((pBlock->Len >= gMemoryHoleSizes[i]) && (gFirstFreeBySize[i] > pBlock))
				gFirstFreeBySize[i] = pBlock;
		}

		if(pBlock->Len >= REAL_MIN_MEMORY_HOLE) {
			if(gLastFree < pBlock) {
				gLastFree = pBlock;
			}
		}

#ifndef CDROM
		gWatchBlock = pBlock;
#endif

	}
	else if((PrevIsFree) && (NextIsFree)) {
		// Free-this-Free - merge this and next with prev
		NewMemBlock *Prev=pBlock->prev;
		NewMemBlock *NextNext=pBlock->next->next; // block after this.
		// Calculate true length of what's left
		Prev->Len = ((unsigned char*)NextNext) - Prev->Data;
		// Rearrange links
		Prev->next=NextNext;
		NextNext->prev=Prev;
		// Sanity check to keep track of blocks. We deleted two here.
		NumMemBlocks-=2;
		gMemoryOverhead -= 2*(BLOCK_HEADER_SIZE + BLOCK_TRAILER_BYTES);

		// Always must tweak this-- if pointers point to the block we're
		// zapping, must tweak them.
		for(i=0;i<NUM_SIZES_TRACKED;i++) {
			if(gFirstFreeBySize[i] == Next)
				gFirstFreeBySize[i] = Prev;
		}
		if(gLastFree == Next)
			gLastFree = Prev;

		// Rearrange trackers, only if block is large enough.
		for(i=0;i<NUM_SIZES_TRACKED;i++) {
			if((Prev->Len >= gMemoryHoleSizes[i]) && (gFirstFreeBySize[i] > Prev))
				gFirstFreeBySize[i] = Prev;
		}

		if(Prev->Len >= REAL_MIN_MEMORY_HOLE) {
			if(gLastFree < Prev) {
				gLastFree = Prev;
			}
		}

#ifndef CDROM
		gWatchBlock = Prev;
#endif
	}
	else {
		// Only 4 possiblilites for 2 bools...
		_ASSERTE(0);
	}

	// Make sure everything is happy.
	NewMemory_AdjustFirstFrees();

#ifndef CDROM
	unsigned int Ptr=(unsigned int)gWatchBlock;
	if((gWatchBlock->Len == 1424) && (Ptr > 0x121A000) && (Ptr < 0x121C000)) {
		printf("Len = %d. Ptr = %08X\n",gWatchBlock->Len,Ptr);
		printf("Breakpoint me!\n");
	}
#endif


	++gNumBlocksFreed;
	if(0) { //abs(gNumBlocksFreed - 19) < 10) {
		ZeroTrace(("gNumBlocksFreed now %d, p=%08X PFree=%d NFree=%d\n",gNumBlocksFreed,p,PrevIsFree,NextIsFree));
		bool OriginalCheck=ConstantlyCheckMemory;
		ConstantlyCheckMemory=true;
		NewMemory_HeapConsistencyCheck();
		ConstantlyCheckMemory=OriginalCheck;
	}

	NewMemory_HeapConsistencyCheck();
}

bool gLastShowAll;

// Does a text-dump of all allocated blocks
void NewMemory_ShowBlocks(bool ShowAll)
{
#if defined(CDROM) && !USE_HOST
	return;
#endif

#if !defined(CDROM) || USE_HOST
	NewMemory_HeapConsistencyCheck();

#ifdef WIN32
	FILE *fpout=NULL;
	if(ShowAll)
		fpout=fopen("C:\\MemDump.txt","w");
#endif

	static const unsigned int BlockMagics[] = {
		PERM_ALLOCATED_MAGIC, SOLID_ALLOCATED_MAGIC, TEMP_ALLOCATED_MAGIC, FREE_MAGIC, BOUNDARY_MAGIC,
	};
	static const char *BlockTypeNames[] = {
		"Perm", "Sold", "Temp", "Free", "End", "????"
	};
	const int NumBlockTypes = sizeof(BlockMagics) / sizeof(BlockMagics[0]);
	int i,Type;

	gLastShowAll = ShowAll;

	if(ShowAll) {
#ifdef WIN32
		if(fpout)
			fprintf(fpout,"%5s %8s %8s %8s(%6s)\n","Type","BlkHdr","DataAdr","Len","Len");
#else
		printf("%5s %8s %8s %8s(%6s)\n","Type","BlkHdr","DataAdr","Len","Len");
#endif
	}

	// Track some stats as we go along...
	unsigned int TotalUsed=0;
	unsigned int PermUsed=0;
	unsigned int TempUsed=0;
	unsigned int TotalFree=0;
	unsigned int LargestFree=0;
	unsigned int NumFree=0;
	unsigned int NumUsed=0;

	bool SeenLargeBlock=false;

	NewMemBlock *cur=gMemBase;
	unsigned int LastMagic=0;
	while(cur) {
		Type=NumBlockTypes+1;
		for(i=0;i<NumBlockTypes;i++)
			if(cur->Magic == BlockMagics[i])
				Type = i;
		
		if(cur->Magic == FREE_MAGIC) {
			++NumFree;
			TotalFree += cur->Len;
			if(LargestFree < cur->Len)
				LargestFree = cur->Len;
			if(cur->Len > (1024*1024))
				SeenLargeBlock = true;
		}
		else {
			++NumUsed;
			TotalUsed+= cur->Len + BLOCK_HEADER_SIZE + BLOCK_TRAILER_BYTES;
			if(cur->Magic == PERM_ALLOCATED_MAGIC) {
				PermUsed+= cur->Len + BLOCK_HEADER_SIZE + BLOCK_TRAILER_BYTES;
			}
			else if(cur->Magic == TEMP_ALLOCATED_MAGIC) {
				TempUsed+= cur->Len + BLOCK_HEADER_SIZE + BLOCK_TRAILER_BYTES;
			}
		}

		if((ShowAll) && (LastMagic != cur->Magic)) {
			LastMagic = cur->Magic;
#ifdef WIN32
		if(fpout)
			fprintf(fpout,"%5s %08X %08X(%6d)\n",BlockTypeNames[Type],cur,cur->Data,cur->Len,cur->Len);
#else
			printf("%5s %08X %08X(%7d). F=%d\n",BlockTypeNames[Type],cur,cur->Len,cur->Len,TotalFree);
#endif
		}
		cur=cur->next;
	}
	// Fragmentation is amount of free blocks that aren't part of
	// largest unused, divided by total heapsize.

#ifdef WIN32
	if(fpout) {
		fprintf(fpout,"-- end of memory blocks. %d used, %d free.\n"
						 " * Alloc      = %08X (%8d) bytes (cached %08X(%8d)) bytes\n"
						 "   -> Perm     = %08X(%8d) bytes\n"
						 "   -> Temp     = %08X(%8d) bytes\n"
						 " * Free       = %08X (%8d) bytes (cached %08X(%8d)) bytes. Largest = %08X(%8d)\n"
						 " * Overhead   = %08X (%8d) bytes\n"
						 " * Fragmented = %08X (%8d) bytes , %% fragmented = %3.4f\n",
						 NumUsed,NumFree,
						 TotalUsed,TotalUsed,gUsedMemory,gUsedMemory,
						 PermUsed,PermUsed,
						 TempUsed,TempUsed,
						 TotalFree,TotalFree,gFreeMemory,gFreeMemory,LargestFree,LargestFree,
						 gMemoryOverhead,gMemoryOverhead,
						 TotalFree-LargestFree, TotalFree-LargestFree,100.0f*((float)(TotalFree-LargestFree)) / ((float)gHeapSize)
						 );
		fclose(fpout);
	}
#endif

#if (PLATFORM == PLATFORM_PS2)
#if !defined(CDROM) || USE_HOST
	printf("-- end of memory blocks. %d used, %d free.\n"
				 " * Alloc      = %08X (%8d) bytes (cached %08X(%8d)) bytes\n"
				 "   -> Perm     = %08X(%8d) bytes\n"
				 "   -> Temp     = %08X(%8d) bytes\n"
				 " * Free       = %08X (%8d) bytes (cached %08X(%8d)) bytes. Largest = %08X(%8d)\n"
				 " * Overhead   = %08X (%8d) bytes\n"
				 " * Fragmented = %08X (%8d) bytes , %% fragmented = %3.4f\n"
				 " * Application= %08X (%8d).\n"
				 " + Static Drawlist = %8d/%8d. Main Drawlist = %8d/%8d\n",
				 NumUsed,NumFree,
				 TotalUsed,TotalUsed,gUsedMemory,gUsedMemory,
				 PermUsed,PermUsed,
				 TempUsed,TempUsed,
				 TotalFree,TotalFree,gFreeMemory,gFreeMemory,LargestFree,LargestFree,
				 gMemoryOverhead,gMemoryOverhead,
				 TotalFree-LargestFree, TotalFree-LargestFree,100.0f*((float)(TotalFree-LargestFree)) / ((float)gHeapSize),
				 ApplicationEnd,ApplicationEnd,
				 (((unsigned int)ps2Video::StaticDrawList.pWrite) & 0xFFFFFFF) - ((unsigned int)ps2Video::staticDrawListBuf_Ptr),StaticDrawListSize,MainDrawListLimit,MainDrawListSize
				 );
#endif

#else
	ZeroTrace(("-- end of memory blocks. %d used, %d free.\n"
						 " * Alloc      = %08X (%8d) bytes (cached %08X(%8d)) bytes\n"
						 "   -> Perm     = %08X(%8d) bytes\n"
						 "   -> Temp     = %08X(%8d) bytes\n"
						 " * Free       = %08X (%8d) bytes (cached %08X(%8d)) bytes. Largest = %08X(%8d)\n"
						 " * Overhead   = %08X (%8d) bytes\n"
						 " * Fragmented = %08X (%8d) bytes , %% fragmented = %3.4f\n",
						 NumUsed,NumFree,
						 TotalUsed,TotalUsed,gUsedMemory,gUsedMemory,
						 PermUsed,PermUsed,
						 TempUsed,TempUsed,
						 TotalFree,TotalFree,gFreeMemory,gFreeMemory,LargestFree,LargestFree,
						 gMemoryOverhead,gMemoryOverhead,
						 TotalFree-LargestFree, TotalFree-LargestFree,100.0f*((float)(TotalFree-LargestFree)) / ((float)gHeapSize)
						 ));
#endif
#endif
}

// Sets a flag as to whether permanent memory should be complained
// about if it's freed. Useful for determining what blocks are really
// scratch memory freed during load.
void NewMemory_ComplainOnPermFree(bool NewSetting)
{
	ComplainOnFreePerm=NewSetting;
}

// Returns the flag as to whether permanent memory should be complained
// about if it's freed.
bool NewMemory_GetComplainOnPermFree(void)
{
	return ComplainOnFreePerm;
}

// Returns the total amount of used memory (including memmgr overhead)
unsigned int NewMemory_GetTotalUsed(void)
{
	return gUsedMemory;
}

// Returns the total amount of unused memory.
unsigned int NewMemory_GetTotalUnused(void)
{
	return gFreeMemory;
}

// Returns the total amount of overhead by the Memory Manager.
unsigned int NewMemory_GetTotalMemMgrUsed(void)
{
	return gMemoryOverhead;
}

// Returns the total number of allocated memmgr blocks
unsigned int NewMemory_GetNumBlocks(void)
{
	return NumMemBlocks;
}

// Returns the largest block we could allocate right now. (Warning-
// this function is kinda expensive. Don't call that often!)
unsigned int NewMemory_GetLargestFree(void)
{
	// Track some stats as we go along...
	unsigned int LargestFree=0;

	NewMemBlock *cur=gMemBase->next; // skip boundary block...
	while(cur) {
		if(cur->Magic == FREE_MAGIC) {
			if(LargestFree < cur->Len)
				LargestFree = cur->Len;
		}
		cur=cur->next;
	}
	return LargestFree;
}

struct AllocInfo {
	AllocInfo(size_t sizeArg, bool permArg, int alignArg)
	{
		size = sizeArg;
		perm = permArg;
		align = alignArg;
		next = NULL;
		max = cur = total = 0;
	}
	AllocInfo *next;
	size_t size;
	bool perm;
	int align;
	int cur;
	int max;
	int total;
};

static AllocInfo *allocList;

static void DumpAlloc(void);

void ZeroMemory_BeginBatch(const char *name)
{
	inBatch = true;
}

void ZeroMemory_EndBatch(void)
{
	inBatch = false;
	DumpAlloc();
}

static void BatchAlloc(size_t size, bool perm, int align)
{
#if DO_BATCH
	static bool inProgress = false;
	if (inProgress)
		return;
	inProgress = true;

	// try to find size, perm combo
	AllocInfo *info = allocList;
	while (info)
	{
		if ((info->size == size) && (info->perm == perm))
			break;
		info = info->next;
	}
	// if !found
	if (!info)
	{
		// make record
		info = new AllocInfo(size, perm, align);
		info->next = allocList;
		allocList = info;
	}
	// note if perm !match
	if (info->align != align)
		ZeroTrace(("align !match\n"));
	// inc total
	++info->total;
	// inc cur
	++info->cur;
	// update max
	if (info->cur > info->max)
		info->max = info->cur;

	inProgress = false;
#endif
}

static void BatchFree(size_t size, bool perm)
{
#if 0
	// try to find size, perm
	AllocInfo *info = allocList;
	while (info)
	{
		if ((info->size == size) && (info->perm == perm))
			break;
		info = info->next;
	}
	// note !found
	if (!info)
	{
		ZeroTrace(("free !match\n"));
		return;
	}
	// dec cur
	--info->cur;
#endif
}

static void DumpAlloc(void)
{
#if DO_BATCH
	ZeroStreamFile *f = ZeroStreamFile::Open("mem.bin", false);
	AllocInfo *info = allocList;
	while (info)
	{
		char line[80];
		sprintf(line, "%d %d %d %d %d\n", info->size, info->perm ? 1 : 0, info->align, info->max, info->total);
		f->Stream(line, strlen(line));
		info = info->next;
	}
	delete f;
#endif
}


// Returns the next AllocationID to be handed out. Useful for
// snapshotting memory, see the ShowMemorySince
unsigned int NewMemory_GetAllocId(void)
{
	return gNumBlocksAllocated;
}


// Does a text-dump of all allocated blocks since the specified
// AllocID was handed out. Useful for seeing who's left memory around.
void NewMemory_ShowBlocksSince(unsigned int AllocId)
{
#ifdef STORE_ALLOC_ID
#ifndef CDROM
	NewMemory_HeapConsistencyCheck();

	static const unsigned int BlockMagics[] = {
		PERM_ALLOCATED_MAGIC, SOLID_ALLOCATED_MAGIC, TEMP_ALLOCATED_MAGIC, FREE_MAGIC, BOUNDARY_MAGIC,
	};
	static const char *BlockTypeNames[] = {
		"Perm", "Sold", "Temp", "Free", "End", "????"
	};
	const int NumBlockTypes = sizeof(BlockMagics) / sizeof(BlockMagics[0]);
	int i,Type;

	if(true) {
		ZeroTrace(("%5s %8s %8s %8s(%6s)\n","Type","BlkHdr","DataAdr","Len","Len"));
	}

	// Track some stats as we go along...
	unsigned int TotalUsed=0;
	unsigned int PermUsed=0;
	unsigned int TempUsed=0;
	unsigned int TotalFree=0;
	unsigned int LargestFree=0;
	unsigned int NumFree=0;
	unsigned int NumUsed=0;

	NewMemBlock *cur=gMemBase;
	while(cur) {
		Type=NumBlockTypes+1;
		for(i=0;i<NumBlockTypes;i++)
			if(cur->Magic == BlockMagics[i])
				Type = i;
		
		if(cur->Magic == FREE_MAGIC) {
			++NumFree;
			TotalFree += cur->Len;
			if(LargestFree < cur->Len)
				LargestFree = cur->Len;
		}
		else {
			++NumUsed;
			TotalUsed+= cur->Len + BLOCK_HEADER_SIZE + BLOCK_TRAILER_BYTES;
			if(cur->Magic == PERM_ALLOCATED_MAGIC) {
				PermUsed+= cur->Len + BLOCK_HEADER_SIZE + BLOCK_TRAILER_BYTES;
			}
			else if(cur->Magic == TEMP_ALLOCATED_MAGIC) {
				TempUsed+= cur->Len + BLOCK_HEADER_SIZE + BLOCK_TRAILER_BYTES;
			}
		}

		if((AllocId < cur->AllocID) && (cur->Magic != FREE_MAGIC)){
			ZeroTrace(("%5s %08X %08X(%6d)",BlockTypeNames[Type],cur,cur->Data,cur->Len,cur->Len));
		}
		cur=cur->next;
	}
	// Fragmentation is amount of free blocks that aren't part of
	// largest unused, divided by total heapsize.


#if (PLATFORM == PLATFORM_PS2)
#ifndef CDROM
	printf("-- end of memory blocks. %d used, %d free.\n"
				 " * Alloc      = %08X (%8d) bytes (cached %08X(%8d)) bytes\n"
				 "   -> Perm     = %08X(%8d) bytes\n"
				 "   -> Temp     = %08X(%8d) bytes\n"
				 " * Free       = %08X (%8d) bytes (cached %08X(%8d)) bytes. Largest = %08X(%8d)\n"
				 " * Overhead   = %08X (%8d) bytes\n"
				 " * Fragmented = %08X (%8d) bytes , %% fragmented = %3.4f\n"
				 " * Application= %08X (%8d).\n"
				 " + Static Drawlist = %8d/%8d. Main Drawlist = %8d/%8d\n",
				 NumUsed,NumFree,
				 TotalUsed,TotalUsed,gUsedMemory,gUsedMemory,
				 PermUsed,PermUsed,
				 TempUsed,TempUsed,
				 TotalFree,TotalFree,gFreeMemory,gFreeMemory,LargestFree,LargestFree,
				 gMemoryOverhead,gMemoryOverhead,
				 TotalFree-LargestFree, TotalFree-LargestFree,100.0f*((float)(TotalFree-LargestFree)) / ((float)gHeapSize),
				 ApplicationEnd,ApplicationEnd,
				 (((unsigned int)ps2Video::StaticDrawList.pWrite) & 0xFFFFFF) - ((unsigned int)ps2Video::staticDrawListBuf_Ptr),StaticDrawListSize,MainDrawListLimit,MainDrawListSize
				 );
#endif

#else
	ZeroTrace(("-- end of memory blocks. %d used, %d free.\n"
						 " * Alloc      = %08X (%8d) bytes (cached %08X(%8d)) bytes\n"
						 "   -> Perm     = %08X(%8d) bytes\n"
						 "   -> Temp     = %08X(%8d) bytes\n"
						 " * Free       = %08X (%8d) bytes (cached %08X(%8d)) bytes. Largest = %08X(%8d)\n"
						 " * Overhead   = %08X (%8d) bytes\n"
						 " * Fragmented = %08X (%8d) bytes , %% fragmented = %3.4f\n",
						 NumUsed,NumFree,
						 TotalUsed,TotalUsed,gUsedMemory,gUsedMemory,
						 PermUsed,PermUsed,
						 TempUsed,TempUsed,
						 TotalFree,TotalFree,gFreeMemory,gFreeMemory,LargestFree,LargestFree,
						 gMemoryOverhead,gMemoryOverhead,
						 TotalFree-LargestFree, TotalFree-LargestFree,100.0f*((float)(TotalFree-LargestFree)) / ((float)gHeapSize)
						 ));
#endif
#endif
#endif
}

void NewMemory_Preset(void)
{
	NewMemBlock *cur, *prev;
	// find top allocated
	cur = gMemTop;
	while (cur != gMemBase)
	{
		if (cur->Magic == PERM_ALLOCATED_MAGIC)
			break;
		cur = cur->prev;
	}
	gMarkBlock = cur;
#if defined(STORE_ALLOC_ID) && 0
	cur = gMarkBlock;
	while (cur)
	{
		ZeroTrace(("frag block: id(%d) size(%d)\n", cur->AllocID, cur->Len));
		if (cur->Magic == FREE_MAGIC)
			break;
		cur = cur->prev;
	}
#endif
	// mark free blocks lower as allocated 
	unsigned int total = 0;
	cur = gMarkBlock;
	while (cur)
	{
#ifdef COMPLAIN_ON_PRESET_FREE
		if (cur->Magic == PERM_ALLOCATED_MAGIC) {
			cur->Magic = SOLID_ALLOCATED_MAGIC;
		}
#endif
		if (cur->Magic == FREE_MAGIC)
		{
#ifdef COMPLAIN_ON_PRESET_FREE
			cur->Magic = SOLID_ALLOCATED_MAGIC;
#else
			cur->Magic = PERM_ALLOCATED_MAGIC;
#endif

			total += cur->Len;
#ifdef SURROUND_WITH_CHECK_VALUES
			memset(cur->PreCheckBlock,BLOCK_PRE_CLEAR_VALUE,sizeof(cur->PreCheckBlock));
			if(BLOCK_TRAILER_BYTES > 0) {
				cur->Len -= BLOCK_TRAILER_BYTES;
				unsigned char *RealEnd=cur->Data + cur->Len;
				memset(RealEnd,BLOCK_POST_CLEAR_VALUE,sizeof(unsigned char)* BLOCK_TRAILER_BYTES);
			}
#endif

#ifdef STORE_ALLOC_ID
			ZeroTrace(("waste block: addr(%x) id(%d) size(%d)\n", cur, cur->AllocID, cur->Len));
#endif
		}
		cur = cur->prev;
	}
	// todo - figure out how to use this memory
	ZeroTrace(("waste: %d\n", total));
#ifdef _DEBUG
	cur = gMemBase;
	prev = NULL;
	while (cur)
	{
		switch (cur->Magic) {
		case FREE_MAGIC:
		case PERM_ALLOCATED_MAGIC:
		case SOLID_ALLOCATED_MAGIC:
		case BOUNDARY_MAGIC:
			break;
		case TEMP_ALLOCATED_MAGIC:
			ZeroTrace(("TEMP_MEM still allocated at Preset\n"));
			break;
		default:
			ZeroBreak();
			break;
		}
		prev = cur;
		cur = cur->next;
	}
	// ZeroBreak();
#endif
	// push free pointers past mark block
	NewMemBlock *freeBlock = gMarkBlock->next;
	for(int i = 0; i < NUM_SIZES_TRACKED; i++)
		gFirstFreeBySize[i] = freeBlock;

	ZeroMemory_Preset();
}

void NewMemory_Reset(void)
{
	ZeroMemory_Reset();

	NewMemBlock *cur = gMemBase;

#ifdef _DEBUG
	// see if there are any temp allocations
	while (cur)
	{
		if (cur->Magic == TEMP_ALLOCATED_MAGIC)
		{
			ZeroTrace(("TEMP_MEM still allocated at Reset\n"));
			// ZeroBreak();
		}
		cur = cur->next;
	}

	if (gMemTop->prev != gLastFree)
	{
		ZeroTrace(("top block is not free\n"));
		// ZeroBreak();
	}
#endif

	// free all blocks after gMarkBlock
	while (true)
	{
		cur = gMarkBlock->next;
		while (cur)
		{
			if (cur->Magic == PERM_ALLOCATED_MAGIC)
			{
				memset(cur->Data, 0x33, cur->Len);
				void *ptr = cur->Data;
				NewMemory_free(ptr);
				break;
			}
#if 0
			if (cur->Magic == TEMP_ALLOCATED_MAGIC)
			{
				memset(cur->Data, 0x33, cur->Len);
				void *ptr = cur->Data;
				NewMemory_free(ptr);
				break;
			}
#endif
			cur = cur->next;
		}
		if (!cur)
			break;
	}

	ZeroTrace(("Reset\n"));
}

bool ZeroMemory_WillReset(void *ptr)
{
#ifdef _DEBUG
	NewMemBlock *p = (NewMemBlock *)ptr;
	p--;
	_ASSERTE(p > gMemBase);
	_ASSERTE(p < gMemTop);
#endif
	unsigned char *mark = gMarkBlock->Data + gMarkBlock->Len;
	if ((unsigned char *)ptr >= mark)
		return true;
	else
		return false;
}

#endif // USE_NEW_MEMORY_MGR
