/****************************************************************************
*
*  Visual C++ 4.0 base by Julio Jerez
*
****************************************************************************/
#ifndef  __ZeroCache__
#define  __ZeroCache__

#include "ZeroTypes.h"
#include "ZeroCrc.h"
#include "ZeroTree.h"
#include "ZeroRef.h"

template <class OBJECT>
class ZeroBaseCache: public ZeroVisTree <OBJECT*, unsigned>
{
	protected:
	const char *refLabel;
	char lastError[256];
	
	public:
	ZeroBaseCache (const char *a_refLabel = "")
		:ZeroVisTree <OBJECT *, unsigned>(), refLabel(a_refLabel)
	{
	}	

	virtual ~ZeroBaseCache ()
	{
		Flush ();
	}

	virtual OBJECT *UseItem (const char *name)
	{
		int crc;
		OBJECT *item;
		ZeroTreeNode *node;
	
		crc = CalcStrCRC (name);
		node = Find(crc);
		if (node) {
			item = (OBJECT*)node->info;
			ZeroAddRef(item, refLabel);
			return item;
		}
		
#ifdef _WIN32
		try {
#endif
			item = new (false) OBJECT (name);
			Insert (item, crc);
			ZeroAddRef(item, refLabel);
			lastError[0] = 0;
#ifdef _WIN32
		} catch (char *Err) {
			item = NULL;
			strncpy (lastError, Err, sizeof (lastError) - 1);
		}
#endif

		return item;
	}

	virtual void AddItem (int crc, OBJECT *item)
	{
		ZeroTreeNode *node;
	
		node = Find(crc);
		if (node) {
			return;
		}

		ZeroAddRef(item, refLabel);
		Insert (item, crc);
	}

	virtual void AddItem (const char *name, OBJECT *item)
	{
		AddItem (CalcStrCRC (name), item);
	}


	virtual OBJECT *FindItem (int crc) const
	{
		ZeroTreeNode *node;
	
		node = Find(crc);
		return node ? (OBJECT*)node->info : NULL;
	}

	virtual OBJECT *FindItem (const char *name) const
	{
		return FindItem (CalcStrCRC (name));
	}

	virtual void EvictItem (int crc)
	{
		ZeroTreeNode *node;

		node = Find(crc);
		if (node) {
			Remove(node);
			ZeroRelease(node->info, refLabel);
		}
	}

	virtual void EvictItem (const char *name) 
	{
		EvictItem (CalcStrCRC (name));
	}

	virtual void Flush ()
	{
		ZeroTreeNode *node;

		Iterator Iter(*this);
		for (Iter.Begin(); Iter; ) {
			node = *Iter;
			Iter++;
			Remove(node);
			ZeroRelease(node->info, refLabel);
		}
	}

	const char *LastError()	const
	{
		return lastError;
	}
};



template <class OBJECT, class CONTEXT>
class ZeroCache: public ZeroBaseCache <OBJECT>
{
	
	public:
	ZeroCache (const char *a_refLabel = "")
		:ZeroBaseCache <OBJECT>(a_refLabel)
	{
	}

	virtual OBJECT *UseItem (const char *name, CONTEXT *Context)
	{
		int crc;
		OBJECT *item;
		ZeroTreeNode *node;
	
		crc = CalcStrCRC (name);
		node = Find(crc);
		if (node) {
			ZeroAddRef(node->info, refLabel);
			return (OBJECT*)node->info;
		}
		
#ifdef _WIN32
		try {
#endif
			item = new (false) OBJECT (name, Context);
			Insert (item, crc);
			ZeroAddRef(item, refLabel);
			lastError[0] = 0;
#ifdef _WIN32
		} catch (char *Err) {
			item = NULL;
			strncpy (lastError, Err, sizeof (lastError) - 1);
		}
#endif

		return item;
	}

};


#endif


