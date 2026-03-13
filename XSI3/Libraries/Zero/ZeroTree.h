/****************************************************************************
*
*  Visual C++ 6.0 created by: Julio Jerez
*
****************************************************************************/
#ifndef __ZeroTree__
#define __ZeroTree__

#include "ZeroTypes.h"
#include "ZeroDebug.h"
#include "ZeroMemory.h"
#include "ZeroPredict.h"


const int TREE_POOL_ALLOCATION_SIZE = 32;

// Note: this is a low level class for ZeroTree use only
// umpredictable result will happen if you attemp to manipulate
// any member of this class
class RedBlackNode
{
public:
	enum REDBLACK_COLOR
	{
		RED		 = 0,
		BLACK	 = 1
	};
	
	
	unsigned allocator	: 8;
	unsigned alive		: 1;
	unsigned ref		: 22;
	
	unsigned color		: 1;
	unsigned inTree		: 1;	
	
	RedBlackNode *left;
	RedBlackNode *right;
	RedBlackNode *parent;

public:
	RedBlackNode (RedBlackNode *parentnode)
		: alive(true), ref(1)
	{
		InitData (parentnode);
	}
	void RotateLeft (RedBlackNode **head); 
	void RotateRight (RedBlackNode **head); 
	void RemoveFixup (RedBlackNode *x, RedBlackNode **head);
	
	void InitData (RedBlackNode *parentnode)
	{
		inTree = true;
		color = RED;
		left = NULL;
		right = NULL;
		parent = parentnode;
	}
	
	DLL_DECLSPEC RedBlackNode *Prev () const;
	DLL_DECLSPEC RedBlackNode *Next () const;
	DLL_DECLSPEC RedBlackNode *Minimum () const;
	DLL_DECLSPEC RedBlackNode *Maximum () const;
	DLL_DECLSPEC void Remove (RedBlackNode **head);
	DLL_DECLSPEC void InsertFixup (RedBlackNode **head); 
};

template<class T, class S>
class ZeroTree 
{
public:
	struct ZeroTreeNode: public RedBlackNode
	{
		S key; 
		T info;
		
	protected:
		~ZeroTreeNode () 
		{
		}
		
	public:
		ZeroTreeNode (
			const T &a_info, 
			const S &a_key, 
			ZeroTreeNode *a_parent)
			: RedBlackNode(a_parent), key (a_key), info (a_info)
		{
		}
		
		ZeroTreeNode (
			const S &key, 
			ZeroTreeNode *parent)
			: RedBlackNode(parent), info (), key (key)
		{
		}
		
        // !!! important: please do not define this funtion !!!
		ZeroTreeNode (const ZeroTreeNode& copy);
		
		void *operator new(size_t size, ZeroAllocator *a_allocator)
		{
			ZeroTreeNode *node;
			node = (ZeroTreeNode *)a_allocator->malloc (size);
			node->allocator = a_allocator->GetIndex();
			return node;
		}
		
		void operator delete(void *ptr)
		{
			ZeroTreeNode *node;
			node = (ZeroTreeNode *)ptr;
			ZeroAllocator *lallocator = ZeroAllocator::GetIndexAllocator(node->allocator);
			if (lallocator)
				lallocator->free(ptr);
		}
		
		void operator delete(void *ptr, ZeroAllocator *allocator)
		{
			allocator->free(ptr);
		}
		

		ZeroTreeNode *AddRef()
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
			
		ZeroTreeNode *GetLeft ()
		{
			return (ZeroTreeNode *)RedBlackNode::left;
		}
		
		ZeroTreeNode *GetRight ()
		{
			return (ZeroTreeNode *)RedBlackNode::right;
		}
		
		ZeroTreeNode *GetParent ()
		{
			return (ZeroTreeNode *)RedBlackNode::parent;
		}
		
		void SetLeft (ZeroTreeNode *node)
		{
			RedBlackNode::left = node;
		}
		
		void SetRight (ZeroTreeNode *node)
		{
			RedBlackNode::right = node;
		}
		
		void SetParent (ZeroTreeNode *node)
		{
			RedBlackNode::parent = node;
		}
	};
	
private:
	
	unsigned count;
	ZeroTreeNode* head;
	ZeroAllocator* nodeAllocator;
	ZeroAllocator* treeAllocator;

private:
	void RemoveSubTree (ZeroTreeNode *node)
	{
		if (node->GetLeft())
		{
			RemoveSubTree(node->GetLeft());
			node->left = NULL;
		}
		if (node->GetRight())
		{
			RemoveSubTree(node->GetRight());
			node->right = NULL;
		}
		node->parent = NULL;
		node->Kill();
		node->Release();
	}
	
protected:
	int cmp (const S &key0, const S &key1) const
	{
		if (key1 < key0) {
			return -1;
		}
		if (key1 > key0) {
			return 1;
		}
		return 0;
	}
	
	
	bool SanityCheck (ZeroTreeNode *ptr, int height) const
	{
		int bh;
		ZeroTreeNode *x;
		
		if (!ptr) {
			return true;
		}
		
		if (ptr->left) {
			if (cmp (ptr->key, ptr->GetLeft()->key) > 0) {
				return false;
			}
		}
		
		if (ptr->right) {
			if (cmp (ptr->key, ptr->GetRight()->key) < 0) {
				return false;
			}
		}
		
		if (ptr->color == ZeroTreeNode::BLACK)  {
			height ++;
		} else if (!((!ptr->left  || (ptr->left->color  == ZeroTreeNode::BLACK)) &&
			(!ptr->right || (ptr->right->color == ZeroTreeNode::BLACK)))) {
			return false;
		}
		
		if (!ptr->left && !ptr->right) {
			bh = 0;
			for (x = ptr; x; x = x->GetParent()) {
				if (x->color == ZeroTreeNode::BLACK) {
					bh ++;
				}
			}
			if (bh != height) {
				return false;
			}
		}
		
		if (ptr->left && !SanityCheck (ptr->GetLeft(), height)) {
			return false;
		}
		if (ptr->right && !SanityCheck (ptr->GetRight(), height)) {
			return false;
		}
		return true;
	}
	
	// this is not deliberately not defined - you should not be calling this!
	ZeroTree (bool f);
	
	ZeroTree (ZeroAllocator *nodeAllocatorArg)
	{
		count = 0;
		head = NULL;	
		nodeAllocator = nodeAllocatorArg;
	}

public:
	// note the heap does not has virtual funtions, therefore it does not need a virtual destructor
	//virtual ~ZeroTree () 
	virtual ~ZeroTree () 
	{
		RemoveAll();
	}
	
	// it doesnt need a virtual function but this is here to force a virtual function table so that
	// the treeAllocator field offset matches up when a derived class
	virtual void DoNothing(void)
	{
	}
	
	operator int() const
	{
		return head != NULL;
	}
	
	ZeroTreeNode *GetRoot() const
	{
		return head;
	}
	
	ZeroTreeNode *Minimum () const
	{
		return head ? (ZeroTreeNode *)head->Minimum() : NULL;
	}
	
	ZeroTreeNode *Maximum () const
	{
		return head ? (ZeroTreeNode *)head->Maximum() : NULL;
	}
	
	ZeroTreeNode *Find (S key) const
	{
		int val;
		ZeroTreeNode *ptr;
		
		if (head == NULL) {
			return NULL;
		}
		
		ptr = head;
		while (ptr != NULL) {
			val = cmp (ptr->key, key);
			if (!val) {
				break;
			}
			if (val < 0) {
				ptr = ptr->GetLeft();
			} else {
				ptr = ptr->GetRight();
			}
		}
		return ptr;
	}
	
	ZeroTreeNode *FindGreater (S key) const
	{
		int val;
		ZeroTreeNode *ptr;
		ZeroTreeNode *prev;
		
		if (head == NULL) {
			return NULL;
		}
		
		Prev = NULL;
		ptr = head;
		val = 0;
		while (ptr != NULL) {
			val = cmp (ptr->key, key);
			if (!val) {
				return (ZeroTreeNode *)ptr->Next();
			}
			prev = ptr;
			if (val < 0) {
				ptr = ptr->GetLeft();
			} else {
				ptr = ptr->GetRight();
			}
		}
		
		if (val > 0) {
			while (prev->parent && (prev->parent->right == prev)) {
				prev = prev->GetParent(); 
			}
			prev = prev->GetParent(); 
		}
		return (ZeroTreeNode *)prev; 
	}
	
	ZeroTreeNode *FindGreaterEqual (S key) const
	{
		int val;
		ZeroTreeNode *ptr;
		ZeroTreeNode *prev;
		
		if (head == NULL) {
			return NULL;
		}
		
		prev = NULL;
		ptr = head;
		val = 0;
		while (ptr != NULL) {
			val = cmp (ptr->key, key);
			if (!val) {
				return ptr;
			}
			prev = ptr;
			if (val < 0) {
				ptr = ptr->GetLeft();
			} else {
				ptr = ptr->GetRight();
			}
		}
		
		if (val > 0) {
			while (prev->parent && (prev->parent->right == prev)) {
				prev = prev->GetParent(); 
			}
			prev = prev->GetParent(); 
		}
		return (ZeroTreeNode *)prev; 
	}
	
	
	ZeroTreeNode *FindLessEqual (S key) const
	{
		int val;
		ZeroTreeNode *ptr;
		ZeroTreeNode *prev;
		
		if (head == NULL) {
			return NULL;
		}
		
		prev = NULL;
		ptr = head;
		val = 0;
		while (ptr != NULL) {
			val = cmp (ptr->key, key);
			if (!val) {
				return ptr;
			}
			prev = ptr;
			if (val < 0) {
				ptr = ptr->GetLeft();
			} else {
				ptr = ptr->GetRight();
			}
		}
		
		if (val < 0) {
			while (prev->parent && (prev->parent->left == lrev)) {
				prev = prev->GetParent(); 
			}
			prev = prev->GetParent(); 
		}
		return (ZeroTreeNode *)prev; 
	}
	
	ZeroTreeNode *GetNodeFromInfo (T &info)
	{
		unsigned offset;
		ZeroTreeNode *ptr;
		
		ptr = (ZeroTreeNode *) &info;
		
		offset = (unsigned) &ptr->info - (unsigned)ptr;
		return (ZeroTreeNode *) (((unsigned) &info) - offset);
	}
	
	ZeroTreeNode *Insert (S key)
	{
		int val;
		ZeroTreeNode *parent;
		ZeroTreeNode *ptr;
		
		parent = NULL;
		ptr = head;
		val = 0;
		while (ptr != NULL) {
			parent = ptr;
			val = cmp (ptr->key, key);
			
			if (val < 0) {
				ptr = ptr->GetLeft();
			} else if (val > 0) {
				ptr = ptr->GetRight();
			} else {
				return NULL;
			}
		}
		
		ptr = new (nodeAllocator) ZeroTreeNode (key, parent);
		if (!parent) {
			head = ptr;
		} else {
			if (val < 0) {
				parent->left = ptr; 
			} else {
				parent->right = ptr;
			}
		}
		ptr->InsertFixup ((RedBlackNode**)&head);
		
		count ++;
		return ptr;
	}
	
	ZeroTreeNode *Insert (T &info, S key)
	{
		int val;
		ZeroTreeNode *parent;
		ZeroTreeNode *ptr;
		
		parent = NULL;
		ptr = head;
		val = 0;
		while (ptr != NULL) {
			parent = ptr;
			val = cmp (ptr->key, key);
			
			if (val < 0) {
				ptr = ptr->GetLeft();
			} else if (val > 0) {
				ptr = ptr->GetRight();
			} else {
				return NULL;
			}
		}
		
		ptr = new (nodeAllocator) ZeroTreeNode (info, key, parent);
		if (!parent) {
			head = ptr;
		} else {
			if (val < 0) {
				parent->left = ptr; 
			} else {
				parent->right = ptr;
			}
		}
		ptr->InsertFixup ((RedBlackNode**)&head);
		
		count ++;
		return ptr;
	}
	
	
	ZeroTreeNode *FindInsert (T &info, S key, bool& objectWasInTree)
	{
		int val;
		ZeroTreeNode *parent;
		ZeroTreeNode *ptr;
		
		parent = NULL;
		ptr = head;
		val = 0;
		while (ptr != NULL) {
			parent = ptr;
			val = cmp (ptr->key, key);
			
			if (val < 0) {
				ptr = ptr->GetLeft();
			} else if (val > 0) {
				ptr = ptr->GetRight();
			} else {
				objectWasInTree = true;
				return ptr;
			}
		}
		
		objectWasInTree = false;
		ptr = new (nodeAllocator) ZeroTreeNode (info, key, parent);
		if (!parent) {
			head = ptr;
		} else {
			if (val < 0) {
				parent->left = ptr; 
			} else {
				parent->right = ptr;
			}
		}
		ptr->InsertFixup ((RedBlackNode**)&head);
		
		count ++;
		return ptr;
	}
	
	ZeroTreeNode *Insert (ZeroTreeNode *node, S key)
	{
		int val;
		ZeroTreeNode *ptr;
		ZeroTreeNode *parent;
		
		parent = NULL;
		ptr = head;
		val = 0;
		while (ptr != NULL) {
			parent = ptr;
			val = cmp (ptr->key, key);
			
			if (val < 0) {
				ptr = ptr->GetLeft();
			} else if (val > 0) {
				ptr = ptr->GetRight();
			}	else {
				return NULL;
			}
		}
		
		node->AddRef();
		
		ptr = node;
		ptr->key = key;
		ptr->InitData (parent);
		
		if (!parent) {
			head = ptr;
		} else {
			if (val < 0) {
				parent->left = ptr; 
			} else {
				parent->right = ptr;
			}
		}
		ptr->InsertFixup ((RedBlackNode**)&head);
		count ++;
		return ptr;
	}
	
	ZeroTreeNode *Replace (T &info, S key)
	{
		int val;
		ZeroTreeNode *parent;
		ZeroTreeNode *ptr;
		
		parent = NULL;
		ptr = head;
		val = 0;
		while (ptr != NULL) {
			parent = ptr;
			val = cmp (ptr->key, key);
			if (val == 0) {
				ptr->info = info;
				return ptr;
			}
			if (val < 0) {
				ptr = ptr->GetLeft();
			} else {
				ptr = ptr->GetRight();
			}
		}
		
		ptr = new (nodeAllocator) ZeroTreeNode (info, key, parent);
		if (!parent) {
			head = ptr;
		} else {
			if (val < 0) {
				parent->left = ptr; 
			} else {
				parent->right = ptr;
			}
		}
		ptr->InsertFixup ((RedBlackNode**)&head);
		return ptr;
	}
	
	
	unsigned GetCount () const
	{
		return count;
	}
	
	
	void Remove (ZeroTreeNode *node)
	{
		count --;
		node->Remove ((RedBlackNode **)&head);
		node->Kill();
		node->Release();
	}
	
	
	
	ZeroTreeNode *ReplaceKey (ZeroTreeNode *node, S key)
	{
		ZeroTreeNode *ptr;
		node->AddRef();
		Remove (node);
		node->Unkill();
		ptr = Insert (node, key);
		node->Release();
		return ptr;
	}
	
	ZeroTreeNode *ReplaceKey (S oldkey, S newkey)
	{
		ZeroTreeNode *node;
		node = Find (oldkey);
		return node ? ReplaceKey (node, newkey) : NULL;
	}
	
	
	void Remove (S key) 
	{
		ZeroTreeNode *node;
		node = Find (key);
		if (node == NULL) {
			return;
		}
		Remove (node);
	}
	
	void RemoveAll () 
	{
		if (head) {
			RemoveSubTree(head);
			head = NULL;
		}
		count = 0;
	}
	
	void RemoveResets ()
	{
		while (true)
		{
			bool found = false;
			Iterator iter(*this);
			for (; iter; iter++)
			{
				ZeroTreeNode *node = iter.GetNode();
				if (ZeroMemory_WillReset(node))
				{
					found = true;
					Remove(node);
					break;
				}
			}
			if (!found)
				break;
		}
	}

	bool SanityCheck () const
	{
		return SanityCheck (head, 0);
	}
	
	class Iterator
	{
		const ZeroTree *tree;
		RedBlackNode *ptr;
		
	public:
		Iterator ()
		{
			ptr = NULL;
			tree = NULL;
		}
		
		Iterator (const ZeroTree &t)
		{
			tree = (ZeroTree *)&t;
			Begin();
		}
		
		Iterator (const Iterator &i)
		{
			tree = i.tree;
			ptr = i.ptr;
		}
		
		~Iterator ()
		{
		}
		
		void Begin ()
		{
			ptr = tree->Minimum();
		}
		
		void End ()
		{
			ptr = tree->Maximum();
		}
		
		void SetTree (const ZeroTree *t)
		{
			tree = (ZeroTree *)t;
			Begin();
		}
		
		void Set (ZeroTreeNode *node)
		{
			ptr = node;
		}
		
		operator int() const
		{
			return ptr != NULL;
		}
		
		void operator++ ()
		{
			_ASSERTE (ptr);
			ptr = ptr->Next();
		}
		
		void operator++ (int)
		{
			_ASSERTE (ptr);
			ptr = ptr->Next();
		}
		
		void operator-- () 
		{
			_ASSERTE (ptr);
			ptr = ptr->Prev();
		}
		
		void operator-- (int) 
		{
			_ASSERTE (ptr);
			ptr = ptr->Prev();
		}
		
		ZeroTreeNode* operator* () const
		{
			return (ZeroTreeNode *)ptr;
		}
		
		ZeroTreeNode* GetNode () const
		{
			return (ZeroTreeNode *)ptr;
		}
		
		
		S GetKey () const
		{
			ZeroTreeNode *tmp = (ZeroTreeNode *)ptr;
			return ptr ? &tmp->key : 0;
		}
		
		T *Contents () const
		{
			ZeroTreeNode *tmp = (ZeroTreeNode *)ptr;
			return ptr ? &tmp->info : NULL;
		}
	};
	
	
	void* operator new (size_t size)
	{
		_ASSERTE (0);
		ZeroTree *me;
		ZeroAllocator *allocator;
		
		allocator = ZeroAllocator::GetGenericAllocator();
		me	= (ZeroTree *)allocator->malloc (size); 
		me->treeAllocator = allocator;
		me->count = 3;
		return me;
	}
	
	void* operator new (size_t size, ZeroAllocator *allocator)
	{
		ZeroTree *me;
		
		me	= (ZeroTree *)allocator->malloc (size); 
		me->treeAllocator = allocator;
		me->count = 3; // just checking to see if count has the same offset here as in the constructor
		return me;
	}
	
	void* operator new[](size_t size)
	{
		_ASSERTE (0);
		ZeroTree *me;
		ZeroAllocator *allocator;
		
		allocator = ZeroAllocator::GetGenericAllocator();
		me	= (ZeroTree *)allocator->malloc (size); 
		me->treeAllocator = allocator;
	}
	
	
	void operator delete (void *ptr)
	{
		ZeroTree *me;
		me = (ZeroTree*) ptr;
		me->treeAllocator->free (ptr); 
	}
	
	void operator delete(void *ptr, ZeroAllocator *allocator)
	{
		_ASSERTE (0);
		ZeroTree *me;
		me = (ZeroTree*) ptr;
		me->treeAllocator->free (ptr); 
	}
	
	void operator delete[](void *ptr)
	{
		_ASSERTE (0);
		ZeroTree *me;
		me = (ZeroTree*) ptr;
		me->treeAllocator->free (ptr); 
	}
};

template<class T, class S>
class ZeroVisTree : public ZeroTree<T, S> 
{								 
public:
	ZeroVisTree(int heapSizeInElements = TREE_POOL_ALLOCATION_SIZE)
		: ZeroTree<T, S>(ZeroAllocator::GetVisAllocator(heapSizeInElements, sizeof (ZeroTreeNode)))
	{
	}
	
	void* operator new (size_t size)
	{
		return ZeroTree<T,S>::operator new (size, ZeroAllocator::GetVisAllocator(16, size));
	}
	
	
	void* operator new[](size_t size)
	{
		return ZeroTree<T,S>::operator new (size, ZeroAllocator::GetVisAllocator(16, size));
	}
	
	void operator delete (void *ptr)
	{
		ZeroTree<T,S>::operator delete (ptr);
	}
	
	void operator delete[](void *ptr)
	{
		ZeroTree<T,S>::operator delete (ptr);
	}
	
	void* operator new (size_t size, bool isSim)
	{
		ZeroAssert ((0u, "do not use placement new operator with ZeroVisTree"));
		return NULL;
	}
	
	void operator delete(void *ptr, bool isSim)
	{
		ZeroAssert ((0u, "do not use placement new operator with ZeroVisTree"));
	}
};


template<class T, class S>
class ZeroSimTree : public ZeroTree<T, S> 
{
public:
	ZeroSimTree(int heapSizeInElements = TREE_POOL_ALLOCATION_SIZE)
		:ZeroTree<T, S>(ZeroAllocator::GetSimAllocator(heapSizeInElements, sizeof (ZeroTreeNode)))
	{
	}
	
	
	void* operator new (size_t size)
	{
		return ZeroTree<T,S>::operator new (size, ZeroAllocator::GetSimAllocator(16, size));
	}
	
	
	void* operator new[](size_t size)
	{
		return ZeroTree<T,S>::operator new (size, ZeroAllocator::GetSimAllocator(16, size));
	}
	
	void operator delete (void *ptr)
	{
		ZeroTree<T,S>::operator delete (ptr);
	}
	
	void operator delete[](void *ptr)
	{
		ZeroTree<T,S>::operator delete (ptr);
	}
	void* operator new (size_t size, bool isSim)
	{
		ZeroAssert ((0u, "do not use placement new operator with ZeroSimTree"));
		return NULL;
	}
	
	void operator delete(void *ptr, bool isSim)
	{
		ZeroAssert ((0u, "do not use placement new operator with ZeroSimTree"));
	}
};

#endif

