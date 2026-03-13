/****************************************************************************
*
*  Visual C++ 6.0 created by: Julio Jerez
*
****************************************************************************/
#ifndef __ZeroList__
#define __ZeroList__


#include "ZeroTypes.h"
#include "ZeroDebug.h"
#include "ZeroMemory.h"
#include "ZeroPredict.h"

const int LIST_POOL_ALLOCATION_SIZE = 32;


template<class T>
class ZeroList 
{
public:
	class ZeroListNode
	{
		friend class ZeroList<T>;
		
		unsigned allocator	: 8;
		unsigned alive		: 1;
		unsigned ref		: 24;
		
		ZeroListNode *next;
		ZeroListNode *prev;
		
		~ZeroListNode ()
		{
		}
		
	protected:
		void Unlink ()
		{
			if (prev) {
				prev->next = next;
			}
			if (next) {
				next->prev = prev;
			}
			prev = NULL;
			next = NULL;
		}
		
		void Remove ()
		{
			Kill();
			Unlink();
			Release();
		}
		
		void SetNext(ZeroListNode *node) 
		{
			next = node;
			if (next) {
				next->prev = this;
			}
		}
		
		void SetPrev(ZeroListNode *node) 
		{
			prev = node;
			if (prev) {
				prev->next = this;
			}
		}
		
	public:
		T info;
		
		ZeroListNode (ZeroListNode *prevArg = NULL, ZeroListNode *nextArg = NULL) 
			: alive(true), ref(1), info ()
		{
			SetNext(nextArg);
			SetPrev(prevArg);
		}
		
		ZeroListNode (const T &a_info, ZeroListNode *prevArg = NULL, ZeroListNode *nextArg = NULL) 
			: alive(true), ref(1), info (a_info)
		{
			SetNext(nextArg);
			SetPrev(prevArg);
		}
		
		ZeroListNode *AddRef()
		{
			++ref;
			return this;
		}
		
		int Release()
		{
			--ref;
			if (ref)
			{
				return ref;
			}
			delete this;
			return 0;
		}

		int GetRefCount() const
		{
			return ref;
		}

		void Kill()
		{
			alive = false;
		}
		
		void Unkill()
		{
			alive = true;
		}

		bool IsAlive() const
		{
			return alive;
		}
		
		void *operator new(size_t size, ZeroAllocator *a_allocator)
		{
			ZeroListNode *node;
			node = (ZeroListNode *)a_allocator->malloc (size);
			node->allocator = a_allocator->GetIndex();
			return node;
		}
		
		void operator delete(void *ptr)
		{
			ZeroListNode *node;
			node = (ZeroListNode *)ptr;
			ZeroAllocator *lallocator = ZeroAllocator::GetIndexAllocator(node->allocator);
			if (lallocator)
				lallocator->free(ptr);
		}
		
		void operator delete(void *ptr, ZeroAllocator *allocator)
		{
			allocator->free(ptr);
		}
		
		ZeroListNode *GetNext() const
		{
			return next;
		}
		
		ZeroListNode *GetPrev() const
		{
			return prev;
		}
	};
	
	
private:
	int count;
	ZeroListNode* last;
	ZeroListNode* first;
	ZeroAllocator* nodeAllocator;	// set by constroctor of list
	ZeroAllocator* listAllocator;	// set by operator new of list
	
	
#ifdef __ENABLE_SANITY_CHECK 
	bool SanityCheck ()
	{
		int j;
		ZeroListNode *node;
		
		j = 0;
		for (node = first; node; node = node->GetNext()) {
			j ++;
			if (node->GetPrev()) {
				if (node->GetPrev()->GetNext() != node) {
					return false; 
				}
			}
			if (node->GetNext()) {
				if (node->GetNext()->GetPrev() != node)	{
					return false;
				}
			}
		}
		if (count != j) {
			return false;
		}
		return true;
	}
#endif
	
	
protected:
	ZeroList (ZeroAllocator* nodeAllocatorArg)
	{
		count = 0;
		last = NULL;
		first = NULL;
		nodeAllocator = nodeAllocatorArg;
	}
	
public:
	
	// note the heap does not has virtual funtions, therefore it does not need a virtual destructor
	//virtual ~ZeroList () 
	~ZeroList () 
	{
		RemoveAll ();
	}
	
	class Iterator
	{
		ZeroListNode *ptr;
		ZeroList *list;
		
	public:
		Iterator ()
		{
			ptr = NULL;
			list = NULL;
		}
		Iterator (const ZeroList<T> &l)
		{
			list = (ZeroList *)&l;
			Begin();
		}
		
		Iterator (const Iterator &i)
		{
			ptr = i.ptr;
			list = i.list;
		}
		
		~Iterator ()
		{
		}
		
		operator int() const
		{
			return ptr != NULL;
		}
		
		bool operator == (const Iterator &iter) const
		{
			return (ptr == iter.ptr);
		}
		
		void Begin ()
		{
			ptr = list->GetFirst();
		}
		
		void End ()
		{
			ptr = list->GetLast();
		}
		
		void SetList (const ZeroList *l)
		{
			list = (ZeroList *)l;
			Begin();
		}
		
		void Set (ZeroListNode *node)
		{
			ptr = node;
		}
		
		Iterator operator++ ()
		{
			_ASSERTE (ptr);
			ptr = ptr->GetNext();
			return *this;
		}
		
		Iterator operator++ (int)
		{
			_ASSERTE (ptr);
			Iterator i (*this);
			ptr = ptr->GetNext();
			return i;
		}
		
		Iterator operator-- () 
		{
			_ASSERTE (ptr);
			ptr = ptr->GetPrev();
			return *this;
		}
		
		Iterator operator-- (int) 
		{
			_ASSERTE (ptr);
			Iterator i (*this);
			ptr = ptr->GetPrev();
			return i;
		}
		
		T &operator* () 
		{
			return ptr->info;
		}
		
		ZeroListNode *GetNode()
		{
			return ptr;
		}
	};
	
	
	int GetCount () const
	{
		return count;
	}
	
	operator int() const
	{
		return count;
	}
	
	// declare, but do not define the assignment operator (this will cause the compiler to alert 
	// users to assignment errors)
	ZeroList& operator=(const ZeroList& list) const;
	
	ZeroListNode *GetFirst() const
	{
		return first;
	}
	
	ZeroListNode *GetLast() const
	{
		return last;
	}
	
	ZeroListNode *GetNodeFromInfo (T &info) const
	{
		unsigned offset;
		ZeroListNode *ptr;
		
		ptr = (ZeroListNode *) &info;
		
		offset = (unsigned) &ptr->info - (unsigned)ptr;
		return (ZeroListNode *) (((unsigned) &info) - offset);
	}
	
	
	void Append (ZeroListNode *node)
	{
		_ASSERTE (node->next == NULL);
		_ASSERTE (node->prev == NULL);
		count++;
		node->AddRef();
		node->Unkill();
		node->SetPrev(last);
		last = node;
		if (first == NULL) {
			first = last;
		}
#ifdef __ENABLE_SANITY_CHECK 
		_ASSERTE (SanityCheck ());
#endif
	}
	
	ZeroListNode *Append (const T &info)
	{
		count++;
		last = new (nodeAllocator) ZeroListNode(info, last, NULL);
		if (first == NULL) {
			first = last;
		}
#ifdef __ENABLE_SANITY_CHECK 
		_ASSERTE (SanityCheck ());
#endif
		return last;
	}
	
	ZeroListNode *Append ()
	{
		count++;
		last = new (nodeAllocator) ZeroListNode(last, NULL);
		if (first == NULL) {
			first = last;
		}
#ifdef __ENABLE_SANITY_CHECK 
		_ASSERTE (SanityCheck ());
#endif
		return last;
	}
	
	
	ZeroListNode *InsertAfter(const T &info, ZeroListNode *node) 
	{
		_ASSERTE (node != NULL);
		count++;
		ZeroListNode *newNode = new (nodeAllocator) ZeroListNode(info, node, node->next);
		if (last == node)
			last = newNode;
		return newNode;
	}
	
	ZeroListNode *InsertAfter(ZeroListNode *node) 
	{
		_ASSERTE (node != NULL);
		count++;
		ZeroListNode *newNode = new (nodeAllocator) ZeroListNode(node, node->next);
		if (last == node)
			last = newNode;
		return newNode;
	}
	
	ZeroListNode *InsertAfter(ZeroListNode *newNode, ZeroListNode *node) 
	{
		_ASSERTE (node != NULL);
		count++;
		newNode->AddRef();
		newNode->Unkill();
		newNode->SetNext(node->next);
		newNode->SetPrev(node);
		if (last == node)
			last = newNode;
#ifdef __ENABLE_SANITY_CHECK 
		_ASSERTE (SanityCheck ());
#endif
		return newNode;
	}
	
	void Prepend (ZeroListNode *node)
	{
		_ASSERTE (node->next == NULL);
		_ASSERTE (node->prev == NULL);
		count++;
		node->AddRef();
		node->Unkill();
		node->SetPrev(first);
		first = node;
		if (last == NULL) {
			last = first;
		}
#ifdef __ENABLE_SANITY_CHECK 
		_ASSERTE (SanityCheck ());
#endif
	}
	
	ZeroListNode *Prepend (const T &info)
	{
		count++;
		first = new (nodeAllocator) ZeroListNode(info, NULL, first);
		if (last == NULL) {
			last = first;
		}
#ifdef __ENABLE_SANITY_CHECK 
		_ASSERTE (SanityCheck ());
#endif
		return first;
		
	}
	
	ZeroListNode *Prepend ()
	{
		count++;
		first = new (nodeAllocator) ZeroListNode(NULL, first);
		if (last == NULL) {
			last = first;
		}
#ifdef __ENABLE_SANITY_CHECK 
		_ASSERTE (SanityCheck ());
#endif
		return first;
	}
	
	void RotateToEnd (ZeroListNode *node)
	{
		if (node != last) {
			if (last != first) {
				if (node == first) {
					first = first->GetNext();
				}
				node->Unlink();
				last->SetNext(node);
				last = node; 
			}
		}
		
#ifdef __ENABLE_SANITY_CHECK 
		_ASSERTE (SanityCheck ());
#endif
	}
	
	void RotateToBegin (ZeroListNode *node)
	{
		if (node != first) {
			if (last != first) {
				if (node == last) {
					last = last->GetPrev();
				}
				node->Unlink();
				first->SetPrev(node);
				first = node; 
			}
		}
		
#ifdef __ENABLE_SANITY_CHECK 
		_ASSERTE (SanityCheck ());
#endif
	}
	
	
	ZeroListNode *Find (const T &info) const
	{
		ZeroListNode *node;
		for (node = first; node; node = node->GetNext()) {
			if (info == node->info) {
				break;
			}
		}
		return node;
	}
	
	void Remove (ZeroListNode *node)
	{
		_ASSERTE (node);
		
		count --;
		_ASSERTE (count >= 0);
		
		if (node == first) {
			first = first->GetNext();
		}
		if (node == last) {
			last = last->GetPrev();
		}
		node->Remove();
		
#ifdef __ENABLE_SANITY_CHECK 
		_ASSERTE (SanityCheck ());
#endif
	}
	
	bool Remove (const T &info)
	{
		ZeroListNode *node;
		
		node = Find (info);
		if (node) {
			Remove (node);
		}
		return (node != NULL);
	}
	
	void RemoveAll ()
	{
		ZeroListNode *node;
		for (node = first; node; node = first) {
			count --;
			first = node->GetNext();
			node->Remove();
		}
		_ASSERTE (count == 0);
		last = NULL;
		first = NULL;
	}
	
	void RemoveResets(void)
	{
		ZeroListNode *node, *next;
		for (node = first; node; node = next) {
			next = node->GetNext();
			if (ZeroMemory_WillReset(node))
				Remove(node);
		}
	}

	void* operator new (size_t size)
	{
		_ASSERTE (0);
		ZeroList *me;
		ZeroAllocator *allocator;
		
		allocator = ZeroAllocator::GetGenericAllocator();
		me	= (ZeroList *)allocator->malloc (size); 
		me->listAllocator = allocator;
		return me;
	}
	
	void* operator new (size_t size, ZeroAllocator *allocator)
	{
		ZeroList *me;
		
		me	= (ZeroList *)allocator->malloc (size); 
		me->listAllocator = allocator;
		return me;
	}
	
	void* operator new[](size_t size)
	{
		_ASSERTE (0);
		ZeroList *me;
		ZeroAllocator *allocator;
		
		allocator = ZeroAllocator::GetGenericAllocator();
		me	= (ZeroList *)allocator->malloc (size); 
		me->listAllocator = allocator;
		return me;
	}
	
	
	void operator delete (void *ptr)
	{
		ZeroList *me;
		me = (ZeroList*) ptr;
		me->listAllocator->free (ptr); 
	}
	
	void operator delete(void *ptr, ZeroAllocator *allocator)
	{
		_ASSERTE (0);
		
		ZeroList *me;
		me = (ZeroList*) ptr;
		me->listAllocator->free (ptr); 
	}
	
	void operator delete[](void *ptr)
	{
		_ASSERTE (0);
		
		ZeroList *me;
		me = (ZeroList*) ptr;
		me->listAllocator->free (ptr); 
	}
};


template<class T>
class ZeroVisList : public ZeroList<T>
{
	public:
		ZeroVisList(int heapSizeInElements = LIST_POOL_ALLOCATION_SIZE)
			:ZeroList<T> (ZeroAllocator::GetVisAllocator(heapSizeInElements, sizeof (ZeroListNode)))
		{
		}
		
		void* operator new (size_t size)
		{
			return ZeroList<T>::operator new (size, ZeroAllocator::GetSimAllocator(16, size));
		}
		
		void* operator new[](size_t size)
		{
			return ZeroList<T>::operator new (size, ZeroAllocator::GetSimAllocator(16, size));
		}
		
		
		void operator delete (void *ptr)
		{
			ZeroList<T>::operator delete (ptr);
		}
		
		void operator delete[](void *ptr)
		{
			ZeroList<T>::operator delete (ptr);
		}
		
		void* operator new (size_t size, bool isSim)
		{
			ZeroAssert ((0u, "do not use placement new operator with ZeroSimList"));
			return NULL;
		}
		
		void operator delete(void *ptr, bool isSim)
		{
			ZeroAssert ((0u, "do not use placement new operator with ZeroSimList"));
		}
};


template<class T>
class ZeroSimList : public ZeroList<T>
{
	public:
		ZeroSimList(int heapSizeInElements = LIST_POOL_ALLOCATION_SIZE)
			:ZeroList<T> (ZeroAllocator::GetSimAllocator(heapSizeInElements, sizeof (ZeroListNode)))
		{
		}
		
		
		void* operator new (size_t size)
		{
			return ZeroList<T>::operator new (size, ZeroAllocator::GetSimAllocator(16, size));
		}
		
		void* operator new[](size_t size)
		{
			return ZeroList<T>::operator new (size, ZeroAllocator::GetSimAllocator(16, size));
		}
		
		
		void operator delete (void *ptr)
		{
			ZeroList<T>::operator delete (ptr);
		}
		
		void operator delete[](void *ptr)
		{
			ZeroList<T>::operator delete (ptr);
		}
		
		
		void* operator new (size_t size, bool isSim)
		{
			ZeroAssert ((0u, "do not use placement new operator with ZeroVisList"));
			return NULL;
		}
		
		void operator delete(void *ptr, bool isSim)
		{
			ZeroAssert ((0u, "do not use placement new operator with ZeroSimList"));
		}
};

template<class T>
class ZeroTempList : public ZeroList<T>
{
public:
	ZeroTempList(int heapSizeInElements = LIST_POOL_ALLOCATION_SIZE)
		:ZeroList<T> (ZeroAllocator::GetTempAllocator(heapSizeInElements, sizeof (ZeroListNode)))
	{
	}
	
	void* operator new (size_t size)
	{
		return ZeroList<T>::operator new (size, ZeroAllocator::GetTempAllocator(16, size));
	}
	
	void* operator new[](size_t size)
	{
		return ZeroList<T>::operator new (size, ZeroAllocator::GetTempAllocator(16, size));
	}
	
	void operator delete (void *ptr)
	{
		ZeroList<T>::operator delete (ptr);
	}
	
	void operator delete[](void *ptr)
	{
		ZeroList<T>::operator delete (ptr);
	}
	
	void* operator new (size_t size, bool isSim)
	{
		ZeroAssert ((0u, "do not use placement new operator with ZeroTempList"));
		return NULL;
	}
	
	void operator delete(void *ptr, bool isSim)
	{
		ZeroAssert ((0u, "do not use placement new operator with ZeroTempList"));
	}
};

#endif
