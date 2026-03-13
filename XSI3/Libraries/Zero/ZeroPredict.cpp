#include "ZeroTypes.h"
#include "ZeroPredict.h"
#include "ZeroMemory.h"
#include "ZeroDebug.h"

inline void Copy(char *&p, char *data, int size, int dir)
{
	char *src, *dst;
	if (dir == 0)
	{
		src = data;
		dst = p;
	}
	else
	{
		src = p;
		dst = data;
	}
	memcpy(dst, src, size);
	p += size;
}

template <class T>
class ChunkList {
public:
	enum {
		chunkShift = 9,
		entriesPerChunk = 1 << chunkShift,
		entryMask = entriesPerChunk - 1
	};

	struct Chunk {
		T entries[512];
		Chunk *next;
	};

	int front;
	int pool;
	int chunkCount;
	Chunk *chunkPtrs[512];

	void Init(void)
	{
		chunkCount = 0;
		front = pool = -1;
		AddFree();
	}

	void Clean(void)
	{
		for (int i = 0; i < chunkCount; i++)
			ZeroFree(chunkPtrs[i]);
	}

	void AddFree(void)
	{
		Chunk *chunk = (Chunk *)ZeroMalloc(sizeof(Chunk), false);
		chunkPtrs[chunkCount] = chunk;
		int base = chunkCount << chunkShift;
		int i = entriesPerChunk;
		T (&entries)[512] = chunk->entries;
		while (i--)
		{
			entries[i].next = pool;
			pool = base + i;
		}
		++chunkCount;
	}

	void Free(int index)
	{
		T &entry = At(index);
		// remove entry from in use list
		if (entry.next >= 0)
		{
			T &next = At(entry.next);
			next.prev = entry.prev;
		}
		if (entry.prev >= 0)
		{
			T &prev = At(entry.prev);
			prev.next = entry.next;
		}
		else
		{
			front = entry.next;
		}
		// put entry on free pool
		entry.next = pool;
		pool = index;
	}

	T &At(int index)
	{
		int chunk = index >> chunkShift;
		index -= (chunk << chunkShift);
		return chunkPtrs[chunk]->entries[index];
	}

	int GetIndex(void)
	{
		if (pool < 0)
			AddFree();
		// remove from free pool
		int index = pool;
		T &entry = At(pool);
		pool = entry.next;
		// insert on in use list
		entry.next = front;
		entry.prev = -1;
		if (front >= 0)
		{
			T &next = At(front);
			next.prev = index;
		}
		front = index;
		// return index
		return index;
	}

	void Save(void)
	{
	}

	void Restore(void)
	{
	}
};

struct StateDesc {
	char *ptr;
	int size;
	int next;
	int prev;
};

// static char stateBuffer[1024*1024];
static ChunkList<StateDesc> stateList;
static int undoAllocs;
static int undoIndices[512];

//extern void *trackPtr;

static bool didInit = false;
static int stateHighMark = 0;

static void CopyState(int dir)
{
#if 0
	int total = 0;
	char *p = stateBuffer;
	if (dir == 0)
		stateList.Save();
	else
		stateList.Restore();
//	Copy(p, (char *)states, sizeof(states), dir);
	int i = stateList.front;
	while (i >= 0)
	{
		StateDesc &state = stateList.At(i);
		Copy(p, state.ptr, state.size, dir);
		total += state.size;
		i = state.next;
	}
	if (total > stateHighMark)
	{
		stateHighMark = total;
		ZeroTrace (("New Total %d\n", stateHighMark));
	}
#endif
}

void ZeroPredict::Init(void)
{
	stateList.Init();
}

void ZeroPredict::Clean(void)
{
	stateList.Clean();
}

void *ZeroPredict::AllocState(unsigned int size)
{
	// allocate memory
	char *mem = (char *)ZeroMalloc(size + sizeof(int), false);
	// track memory
	int index = stateList.GetIndex();
	StateDesc &state = stateList.At(index);
	*(int *)mem = index;
	mem += sizeof(int);
	state.ptr = mem;
	state.size = size;
	// if we are predicting
	if (inProgress)
	{
		// mark this alloc as needing undoing
		undoIndices[undoAllocs++] = index;
	}
	return mem;
}

void ZeroPredict::FreeState(void *ptr)
{
	_ASSERT(0);
/*
	if (ptr == trackPtr)
		ZeroBreak();
	if (inProgress)
		return;
	char *mem = (char *)ptr;
	mem -= sizeof(int);
	int index = *(int *)mem;
	stateList.Free(index);
	ZeroFree(mem);
*/
}

void ZeroPredict::MarkState(void *ptr, unsigned int size)
{
	_ASSERT(0);
}

void ZeroPredict::UnmarkState(void *ptr)
{
	_ASSERT(0);
}

bool ZeroPredict::inProgress = false;

void ZeroPredict::Begin(void)
{
	CopyState(0);
	inProgress = true;
	undoAllocs = 0;
}

void ZeroPredict::End(void)
{
	while (--undoAllocs >= 0)
	{
		int index = undoIndices[undoAllocs];
		StateDesc &state = stateList.At(index);
		char *mem = state.ptr;
		mem -= sizeof(int);
		int check;
		check = *(int *)mem;
		_ASSERT(check == index);
		stateList.Free(index);
		ZeroFree(mem);
	}
	inProgress = false;
	CopyState(1);
}
