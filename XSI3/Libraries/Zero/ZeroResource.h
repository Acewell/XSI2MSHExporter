/****************************************************************************
*
*  Visual C++ 6.0 created by: Julio Jerez
*
****************************************************************************/
#ifndef __ZeroResource__
#define __ZeroResource__

#include "ZeroTypes.h"
#include "ZeroCrc.h"
#include "ZeroTree.h"


template<class LOADER_DRIVER>
class ResourceLoader: public ZeroTree<LOADER_DRIVER, unsigned>
{

	public:
	ResourceLoader()
		:ZeroTree<LOADER_DRIVER, unsigned> (ZeroAllocator::GetGenericAllocator())
	{
	}

	~ResourceLoader()
	{
	}


	bool AddResourceLoader (const LOADER_DRIVER &driver)
	{
		unsigned Code;
		char TmpExt[256];

		strcpy (TmpExt, driver.ext);
		strlwr (TmpExt);

		Code = CalcStrID (TmpExt);

		LOADER_DRIVER tmpDriver (driver);
		if (Insert (tmpDriver, Code)) {
			return true;
		}
		return false;
	}

	LOADER_DRIVER *FindLoader (const char *Ext)
	{
		unsigned Code;
		ZeroTreeNode *Node;
		char TmpExt[256];

		strcpy (TmpExt, Ext);
		strlwr (TmpExt);

		Code = CalcStrID (TmpExt);

		Node = Find(Code);
		if (Node) {
			return &Node->info;
		}
		return NULL;
	}

	bool UpdateResourceLoader (const LOADER_DRIVER &Driver)
	{
		LOADER_DRIVER *OldDriver;

		OldDriver = FindLoader (Driver.ext);

		if (OldDriver) {
			*OldDriver = Driver;
			return true;
		} else {
			return AddResourceLoader (Driver);
		}
	}
};


#endif
