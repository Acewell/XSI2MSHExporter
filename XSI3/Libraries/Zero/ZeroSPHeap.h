/****************************************************************************
*
*  Visual C++ 6.0 created by: Julio Jerez
*
****************************************************************************/

#ifndef __ZeroSPHeap__
#define __ZeroSPHeap__

#include "ZeroTypes.h"
#include "ZeroHeap.h"


template <class OBJECT, class KEY>
class ZeroSPHeap:public ZeroHeap<OBJECT,KEY>
{
	public:
	KEY maxVal;
	ZeroSPHeap(int maxSize, KEY maxValue)
#if(PLATFORM ==PLATFORM_PS2)
		:ZeroHeap<OBJECT,KEY>( maxSize,  maxValue, (void*) 0x70000000)
#else
		:ZeroHeap<OBJECT,KEY>( maxSize,  maxValue)
#endif
	{
		maxVal = maxValue;
	
	}
	void Restore()
	{
		pool[0].key = maxVal;
	}
};

#endif

