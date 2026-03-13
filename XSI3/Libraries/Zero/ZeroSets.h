/****************************************************************************
*
*  Visual C++ 6.0 created by: Julio Jerez
*
****************************************************************************/
#ifndef __ZeroSets__
#define __ZeroSets__
#include "ZeroTypes.h"
#include "ZeroMemory.h"


template<class T>
class ZeroSet
{
	mutable int maxSize;
	mutable int granulatity;
	mutable T *array;

	void Resize (int size) const
	{
		int i;
		T *newArray;

		if (size >= maxSize) {
			size = size + granulatity - (size + granulatity) % granulatity;
			newArray	= ZeroNew ( T [size]);
			if (array) {
				for (i = 0; i < maxSize; i ++) {
					newArray[i]	= array[i];
				}
				delete[] array;
			}
			array = newArray;
			maxSize = size;
		} else if (size < maxSize) {
			size = size + granulatity - (size + granulatity) % granulatity;
			newArray	= ZeroNew (T [size]);
			if (array) {
				for (i = 0; i < size; i ++) {
					newArray[i]	= array[i];
				}
				delete[] array;
			}
			array = newArray;
			maxSize = size;
		}
	}


	public:
	ZeroSet (int granulatitySize)
	{
		maxSize	= 0;
		granulatity	= granulatitySize;
		array	= NULL;
	}

	ZeroSet( const ZeroSet& set )
	{
		int i;
		// expect to be empty
		_ASSERTE( !set.array );
		array = NULL;
		maxSize = 0;
		granulatity = set.granulatity;
		if (set.array) {
			for (i = 0; i < set.GetSize(); i++ ) {
				(*this)[i] = set[i];
			}
		}
	}

	~ZeroSet ()
	{
		if (array) {
			delete[] array;
		}
	}

	int GetSize ()	const
	{
		return maxSize;
	}

	T &operator[] (int i) 
	{ 
		_ASSERTE (i >= 0);
		if (i >= maxSize) {
			Resize (i);
		}
		return array[i];
	}

	const T &operator[] (int i) const
	{ 
		if (i >= maxSize) {
			Resize (i);
		}
		return array[i];
	}

};

#endif
