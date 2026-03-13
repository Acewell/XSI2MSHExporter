#ifndef __NEWMEMORY_H_
#define __NEWMEMORY_H_

#ifdef USE_NEW_MEMORY_MGR

// Allocates the specified block of RAM, w/ malloc. This should be the
// last use of malloc in the game.
void NewMemory_BootInit(unsigned int Heapsize);

// Allocates memory of the specified size. IsPermanent is a flag as to
// what kind of memory to allocate. Alignment is the specified # of
// bits to align to-- if this is 4, it'll allocate to a 4-byte
// (32-bit) boundary, if 6, 16-byte boundary, etc.
void *NewMemory_malloc(size_t size, bool IsPermanent=true, int Alignment=4);

// Frees the specified block. Complains if double-freed, etc.
void NewMemory_free(void *p);

// Sets a block as non-purgeable by the reset code
void NewMemory_SetAsNonPurgeable(void *p);

// Sets a flag as to whether permanent memory should be complained
// about if it's freed. Useful for determining what blocks are really
// scratch memory freed during load.
void NewMemory_ComplainOnPermFree(bool NewSetting);

// Returns the flag as to whether permanent memory should be complained
// about if it's freed.
bool NewMemory_GetComplainOnPermFree(void);

// ------------------------------------------------------------------
//
// Various logging, stats, and utility functions.

// Does a text-dump of all allocated blocks. If ShowAll is true, it'll
// list every block. If not, it'll just do a summary.
void NewMemory_ShowBlocks(bool ShowAll=true);

// Returns the total amount of used memory (including memmgr overhead)
unsigned int NewMemory_GetTotalUsed(void);

// Returns the total amount of unused memory.
unsigned int NewMemory_GetTotalUnused(void);

// Returns the total amount of overhead by the Memory Manager.
unsigned int NewMemory_GetTotalMemMgrUsed(void);

// Returns the total number of allocated memmgr blocks
unsigned int NewMemory_GetNumBlocks(void);

// Returns the largest block we could allocate right now. (Warning-
// this function is kinda expensive. Don't call that often!)
unsigned int NewMemory_GetLargestFree(void);

// let memory do its thing before a batch
void ZeroMemory_BeginBatch(const char *name);

// ... after
void ZeroMemory_EndBatch(void);

// prepare for future resets
void NewMemory_Preset(void);

// wipeout changes since preset
void NewMemory_Reset(void);

// see if a pointer will be lost on reset
bool ZeroMemory_WillReset(void *ptr);

// Returns the next AllocationID to be handed out. Useful for
// snapshotting memory, see the ShowMemorySince
unsigned int NewMemory_GetAllocId(void);

// Does a text-dump of all allocated blocks since the specified
// AllocID was handed out. Useful for seeing who's left memory around.
void NewMemory_ShowBlocksSince(unsigned int AllocId);

#else

// NewMemory is not on. Turn 

// Sets a flag as to whether permanent memory should be complained
// about if it's freed. Useful for determining what blocks are really
// scratch memory freed during load.
inline void NewMemory_ComplainOnPermFree(bool NewSetting) {}

// Returns the flag as to whether permanent memory should be complained
// about if it's freed.
inline bool NewMemory_GetComplainOnPermFree(void) {return false;}

// Sets a block as non-purgeable by the reset code
inline void NewMemory_SetAsNonPurgeable(void *p) {}

// ------------------------------------------------------------------
//
// Various logging, stats, and utility functions.

// Does a text-dump of all allocated blocks. If ShowAll is true, it'll
// list every block. If not, it'll just do a summary.
inline void NewMemory_ShowBlocks(bool ShowAll=true) {}

// Returns the total amount of used memory (including memmgr overhead)
inline unsigned int NewMemory_GetTotalUsed(void) { return 0;}

// Returns the total amount of unused memory.
inline unsigned int NewMemory_GetTotalUnused(void) {return 0;}

// Returns the total amount of overhead by the Memory Manager.
inline unsigned int NewMemory_GetTotalMemMgrUsed(void) {return 0;}

// Returns the total number of allocated memmgr blocks
inline unsigned int NewMemory_GetNumBlocks(void) {return 0;}

// Returns the largest block we could allocate right now. (Warning-
// this function is kinda expensive. Don't call that often!)
inline unsigned int NewMemory_GetLargestFree(void) {return 0;}

// before batch
inline void ZeroMemory_BeginBatch(const char *) {}

// ... after
inline void ZeroMemory_EndBatch(void) {}

// prepare for future resets
inline void NewMemory_Preset(void) {}

// wipeout changes since preset
inline void NewMemory_Reset(void) {}

// see if a pointer will be lost on reset
inline bool ZeroMemory_WillReset(void *ptr) { return false; }

// Returns the next AllocationID to be handed out. Useful for
// snapshotting memory, see the ShowMemorySince
inline unsigned int NewMemory_GetAllocId(void) {return 0;}

// Does a text-dump of all allocated blocks since the specified
// AllocID was handed out. Useful for seeing who's left memory around.
inline void NewMemory_ShowBlocksSince(unsigned int AllocId) {}


#endif // USE_NEW_MEMORY is off.


#endif // #ifndef __NEWMEMORY_H_
