#ifndef __ZeroQueue__
#define __ZeroQueue__

#include "ZeroTypes.h"
#include "ZeroUnknown.h"
#include "ZeroMemory.h"
#include "ZeroPredict.h"
#include <new>


// queue template class
template<class T> class ZeroQueue
{
protected:
	// memory allocator
	ZeroAllocator *allocator;

	// storage array
	int size;
	T *array;

	// queue values
	int count;
	T *first;
	T *last;
	
public:
	// queue iterator
	class Iterator
	{
	protected:
		T *ptr;
		ZeroQueue<T> *queue;
		
	public:
		// constructor
		Iterator (const ZeroQueue<T> &Me)
		{
			queue = (ZeroQueue *)&Me;
			Begin();
		}
		
		// copy constructor
		Iterator (const Iterator &i)
		{
			ptr = i.ptr;
			queue = i.queue;
		}
		
		// destructor
		~Iterator ()
		{
		}
		
		// is the iterator done?
		operator int() const
		{
			return ptr != NULL;
		}
		
		// comparison
		bool operator == (const Iterator &iter) const
		{
			return (ptr == iter.ptr);
		}
		
		// go to the first item
		void Begin ()
		{
			ptr = queue->GetFirst();
		}
		
		// go to the last item
		void End ()
		{
			ptr = queue->GetLast();
		}

		// set the item
		void Set (T *element)
		{
			if (element >= queue->GetArray() &&
				element < queue->GetArray() + size)
			{
				ptr = element;
			}
		}
		
		// pre-increment
		Iterator operator++ ()
		{
			_ASSERTE (ptr);
			if (ptr == queue->GetLast())
			{
				ptr = NULL;
			}
			else
			{
				if (++ptr >= queue->GetArray() + queue->GetSize())
				{
					ptr = queue->GetArray();
				}
			}
			return *this;
		}
		
		// post-increment
		Iterator operator++ (int)
		{
			_ASSERTE (ptr);
			Iterator i (*this);
			if (ptr == queue->GetLast())
			{
				ptr = NULL;
			}
			else
			{
				if (++ptr >= queue->GetArray() + queue->GetSize())
				{
					ptr = queue->GetArray();
				}
			}
			return i;
		}
		
		// pre-decrement
		Iterator operator-- () 
		{
			_ASSERTE (ptr);
			if (ptr == queue->GetFirst())
			{
				ptr = NULL;
			}
			else
			{
				if (--ptr < queue->GetArray())
				{
					ptr = queue->GetArray() + queue->GetSize() - 1;
				}
			}
			return *this;
		}
		
		// post-decrement
		Iterator operator-- (int) 
		{
			_ASSERTE (ptr);
			Iterator i (*this);
			if (ptr == queue->GetFirst())
			{
				ptr = NULL;
			}
			else
			{
				if (--ptr < queue->GetArray())
				{
					ptr = queue->GetArray() + queue->GetSize() - 1;
				}
			}
			return i;
		}
		
		T &operator* () 
		{
			return *ptr;
		}
	};
	
	// constructor
	ZeroQueue (ZeroAllocator *allocator, int size)
		: allocator(allocator), size(size)
	{
		array = static_cast<T *>(allocator->malloc(size * sizeof(T)));
		count = 0;
		first = NULL;
		last = NULL;
	}
	
	// destructor
	virtual ~ZeroQueue () 
	{
		RemoveAll ();
		allocator->free(array);
	}
	
	// get the number of items
	int GetCount () const
	{
		return count;
	}
	
	// get the number of items
	operator int() const
	{
		return count;
	}
	
	// get the array pointer
	T *GetArray() const
	{
		return array;
	}

	// get the size of the array
	int GetSize() const
	{
		return size;
	}

	// get the first item
	T *GetFirst() const
	{
		return first;
	}
	
	// get the last item
	T *GetLast() const
	{
		return last;
	}

	// pop the first item
	void PopFirst()
	{
		if (count > 0)
		{
			first->~T();
			if (++first >= array + size)
			{
				first = array;
			}
			--count;
			if (count == 0)
			{
				first = NULL;
				last = NULL;
			}
		}
	}

	// pop the last item
	void PopLast()
	{
		if (count > 0)
		{
			last->~T();
			if (--last < array)
			{
				last = array + size - 1;
			}
			--count;
			if (count == 0)
			{
				first = NULL;
				last = NULL;
			}
		}
	}

	// append an element
	T *Append (const T &info)
	{
		if (count < size)
		{
			if (count)
			{
				if (++last >= array + size)
				{
					last = array;
				}
				::new (last) T(info);
			}
			else
			{
				::new (array) T(info);
				first = array;
				last = array;
			}
			++count;
			return last;
		}
		else
		{
			return NULL;
		}
	}
	
	// prepend an element
	T *Prepend (const T &info)
	{
		if (count < size)
		{
			if (count)
			{
				if (--first < array)
				{
					first = array + size - 1;
				}
				::new (first) T(info);
			}
			else
			{
				::new (array) T(info);
				first = array;
				last = array;
			}
			++count;
			return first;
		}
		else
		{
			return NULL;
		}
	}
	
	// find an element
	T *Find (const T &info) const
	{
		T *ptr = first;
		if (count)
		{
			while (1)
			{
				if (info == *ptr)
				{
					return ptr;
				}
				if (ptr == last)
				{
					return NULL;
				}
				if (++ptr >= array + size)
				{
					ptr = array;
				}
			}
		}
		return NULL;
	}
	
	// remove all elements
	void RemoveAll ()
	{
		if (count)
		{
			while (1)
			{
				first->~T();
				if (first == last)
				{
					break;
				}
				if (++first >= array + size)
				{
					first = array;
				}
			}
		}
		count = 0;
		first = NULL;
		last = NULL;
	}
};


template<class T> class ZeroVisQueue: public ZeroQueue<T>
{
public:
	ZeroVisQueue (int size)
		:ZeroQueue<T> (ZeroAllocator::GetVisAllocator(1, size * sizeof (T)), size)
	{
	}
};


template<class T> class ZeroSimQueue: public ZeroQueue<T>
{
public:
	ZeroSimQueue (int size)
		:ZeroQueue<T> (ZeroAllocator::GetSimAllocator(1, size * sizeof (T)), size)
	{
	}
};


#endif
