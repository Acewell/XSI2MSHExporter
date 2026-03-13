/****************************************************************************
*
*  Visual C++ 6.0 created by: Julio Jerez
*
****************************************************************************/
#ifndef __ZeroHeap__
#define __ZeroHeap__

#include "ZeroTypes.h"
#include "ZeroMemory.h"


template <class OBJECT, class KEY>
class ZeroHeap
{
protected:
	int count;
	int maxcount;
	bool allocated;
	
	struct RECORD {
		KEY key;
		OBJECT obj;
	};
	RECORD *pool;

public:
	ZeroHeap (int maxSize, KEY maxValue, const void *buffer)
	{
		allocated = false;
		pool = (RECORD *) buffer;
		maxcount = maxSize;
		pool[0].key = maxValue;
		Flush();
	}
	
	ZeroHeap (int maxSize, KEY maxValue)
	{
		allocated = true;
		pool = (RECORD *)ZeroMalloc ((maxSize + 1) * sizeof (RECORD), false);
		maxcount = maxSize;
		pool[0].key = maxValue;
		Flush();
	}
	
	
	// note the heap does not has virtual funtions, therefore it does not need a virtual destructor
//	virtual ~ZeroHeap ()
	~ZeroHeap ()
	{    
		if (allocated == true) {
			ZeroFree (pool);
		}
	}
	
	void* __cdecl operator new (size_t size)
	{
		return ZeroMalloc (size, false);
	}
	
	void  __cdecl operator delete (void *ptr)
	{
		ZeroFree (ptr);
	}
	
	void Flush () 
	{
		count = 0;
	}
	
	
	KEY MaxValue ()	const 
	{
		return pool[0].key;
	}
	KEY Value(int i = 0) const 
	{
		return pool[i + 1].key;
	}
	
	int GetCount () const 
	{ 
		return count;
	}

	int GetMaxSize () const 
	{
		return maxcount - 1;
	}

	
	OBJECT operator[] (int i) 
	{ 
		_ASSERTE (i<= count);
		return pool[i+1].obj;
	}
	
	void Pop ();
	void Push (OBJECT &obj, KEY key);
	
	void Pop (void Notify(OBJECT &obj, int Index));
	void Push (OBJECT &obj, KEY key, void Notify(OBJECT &obj, int Index));
	
	void Update (int index, KEY newKey);

	void Remove (OBJECT &obj);
	void Remove (int Index);
	void Remove (int Index, void Notify(OBJECT &obj, int Index));
	
	
	int Find (OBJECT &obj);
	int Find (KEY key);
			
};


template <class OBJECT, class KEY>
void ZeroHeap<OBJECT,KEY>::Pop ()
{
	int j;
	int k;
	RECORD Tmp;
	
	pool[1] = pool[count--];
	Tmp = pool[1];
	k = 1;
	while ( k <= (count>>1)) {
		j = k + k;
		if ((j < count) && (pool[j].key < pool[j+1].key)) {
			j ++;
		}
		if (Tmp.key >= pool[j].key) {
			break;
		}
		pool[k] = pool[j];
		k = j;
	}
	pool[k] = Tmp;
}

template <class OBJECT, class KEY>
void ZeroHeap<OBJECT,KEY>::Push (OBJECT &obj, KEY key)
{
	int i;
	RECORD Tmp;
	
	_ASSERTE (count < maxcount);
	_ASSERTE (key < MaxValue());
	
	count ++;
	pool [count].key = key;
	pool [count].obj = obj;
		
	i = count;
	Tmp = pool[count];
	for (;;) {
		if (pool[i>>1].key > Tmp.key ) {
			break;
		}
		pool[i] = pool[i>>1];
		i >>= 1;
	}
	pool[i] = Tmp;
}

template <class OBJECT, class KEY>
void ZeroHeap<OBJECT,KEY>::Pop (void Notify(OBJECT &obj, int Index))
{
	int j;
	int k;
	RECORD Tmp;
	
	Notify (pool[1].obj, - 1);
	pool[1] = pool[count--];
	Notify (pool[1].obj, 0);
	
	Tmp = pool[1];
	k = 1;
	while ( k <= (count>>1)) {
		j = k + k;
		if ((j < count) && (pool[j].key < pool[j+1].key)) {
			j ++;
		}
		if (Tmp.key >= pool[j].key) {
			break;
		}
		pool[k] = pool[j];
		Notify (pool[k].obj, k - 1);
		k = j;
	}
	pool[k] = Tmp;
	Notify (pool[k].obj, k - 1);
}

template <class OBJECT, class KEY>
void ZeroHeap<OBJECT,KEY>::Push (OBJECT &obj, KEY key, void Notify(OBJECT &obj, int Index))
{
	int i;
	RECORD Tmp;
	
	count ++;
	_ASSERTE (count < maxcount);
	_ASSERTE (key < MaxValue());

	pool [count].key = key;
	pool [count].obj = obj;
	Notify (pool[count].obj, count - 1);
	
	i = count;
	Tmp = pool[count];
	for (;;) {
		if (pool[i>>1].key > Tmp.key) {
			break;
		}
		pool[i] = pool[i>>1];
		Notify (pool[i].obj, i - 1);
		i >>= 1;
	}
	pool[i] = Tmp;
	Notify (pool[i].obj, i - 1);
}



template <class OBJECT, class KEY>
int ZeroHeap<OBJECT,KEY>::Find (OBJECT &obj)
{
	int i;
	// For now let perform a linear search
	// this is efficient if the size of the heap is small
	// ex: count < 32
	// this will be change to a binary search in the heap shoud the 
	// the size of the heap get larger than 32
	//_ASSERTE (count <= 32);
	for (i = 0; i < count; i ++) {
		if (pool[i+1].obj == obj) {
			return i;
		}
	}
	return - 1;
}


template <class OBJECT, class KEY>
int ZeroHeap<OBJECT,KEY>::Find (KEY key)
{
    int i;
    // ex: count < 32
    // this will be change to a binary search in the heap shoud the 
    // the size of the heap get larger than 32
    _ASSERTE (count <= 32);
    for (i = 0; i < count; i ++)	{
		if (pool[i+1].key == key) {
			return i;
		}
	}
    return - 1;
}

template <class OBJECT, class KEY>
void ZeroHeap<OBJECT,KEY>::Update (int index, KEY newKey)
{
	_ASSERTE (index < count);
	_ASSERTE (index >= 0);

	int i;
	RECORD Tmp;			
	i = index+1;
	pool[i].key = newKey;
	Tmp = pool[i];
	for (;;) {
		if (pool[i>>1].key > Tmp.key ) {
			break;
		}
		pool[i] = pool[i>>1];
		i >>= 1;
	}
	pool[i] = Tmp;
}

template <class OBJECT, class KEY>
void ZeroHeap<OBJECT,KEY>::Remove (int Index)
{
	int j;
	int k;
	RECORD Tmp;
	
	_ASSERTE (Index < count);
	_ASSERTE (Index >= 0);
	
	k = Index + 1;
	pool[k] = pool[count--];
	Tmp = pool[k];
	while ( k <= (count>>1)) {
		j = k + k;
		if ((j < count) && (pool[j].key < pool[j+1].key)) {
			j ++;
		}
		if (Tmp.key >= pool[j].key) {
			break;
		}
		pool[k] = pool[j];
		k = j;
	}
	pool[k] = Tmp;
}

template <class OBJECT, class KEY>
void ZeroHeap<OBJECT,KEY>::Remove (OBJECT &obj)
{
	int index;
	index = Find (obj);
	if (index != -1) {
		Remove (index);
	}
}


template <class OBJECT, class KEY>
void ZeroHeap<OBJECT,KEY>::Remove (int Index, void Notify(OBJECT &obj, int Index))
{
	int j;
	int k;
	RECORD Tmp;
	
	_ASSERTE (Index < count);
	_ASSERTE (Index >= 0);
	
	k = Index + 1;
	Notify (pool[k].obj, - 1);
	if (k == count) {
		count--;
		return;
	}
	
	pool[k] = pool[count--];
	Notify (pool[k].obj, k - 1);
	
	Tmp = pool[k];
	while (k <= (count>>1)) {
		j = k + k;
		if ((j < count) && (pool[j].key < pool[j+1].key)) {
			j ++;
		}
		if (Tmp.key >= pool[j].key) {
			break;
		}
		pool[k] = pool[j];
		Notify (pool[k].obj, k - 1);
		k = j;
	}
	pool[k] = Tmp;
	Notify (pool[k].obj, k - 1);
}

#endif
