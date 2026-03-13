/****************************************************************************
*
*  Visual C++ 6.0 created by: Julio Jerez
*
****************************************************************************/
#ifndef __ZeroNode__
#define __ZeroNode__

#include "ZeroTypes.h"
#include "ZeroRtti.h"
#include "ZeroUnknown.h"
#include "ZeroMatrix.h"
#include "ZeroSphere.h"

class ZeroFile;

class ZeroBaseNode: public ZeroUnknown
{
	ZeroBaseNode *parent;
	ZeroBaseNode *child;
	ZeroBaseNode *sibling;

	ZERO_RTTI(ZeroUnknown);

	void Clear()
	{
		child = NULL;
		parent = NULL;
		sibling = NULL;
	}

	protected:

	ZeroBaseNode ()
		:ZeroUnknown ()
	{
		Clear ();
	}

	ZeroBaseNode (const char *name)
		:ZeroUnknown (name)
	{
		Clear ();
	}

	DLL_DECLSPEC ~ZeroBaseNode () ;
	DLL_DECLSPEC ZeroBaseNode (const ZeroBaseNode &clone);
	DLL_DECLSPEC virtual void CloneFixUp (const ZeroBaseNode &clone);
	DLL_DECLSPEC virtual void PrintHierarchy (ZeroFile &file, char *indentation) const; 
	DLL_DECLSPEC void EnumerateByTypeID(unsigned typeId, const ZeroBaseNode** bufferOut, int& count) const;


	public:
	ZeroBaseNode *GetChild () const
	{
		return child;
	}

	ZeroBaseNode *GetSibling () const
	{
		return sibling;
	}

	ZeroBaseNode *GetParent () const
	{
		return parent;
	}

	DLL_DECLSPEC virtual void Attach (ZeroBaseNode *parent);
	DLL_DECLSPEC virtual void Detach ();
	
	DLL_DECLSPEC ZeroBaseNode *GetRoot () const;
	DLL_DECLSPEC ZeroBaseNode *GetFirst() const;
	DLL_DECLSPEC ZeroBaseNode *GetLast() const;
	DLL_DECLSPEC ZeroBaseNode *GetNext() const;
	DLL_DECLSPEC ZeroBaseNode *GetPrev() const;

	DLL_DECLSPEC void ReplaceNode(ZeroBaseNode *newNode);

	DLL_DECLSPEC ZeroBaseNode *Find (unsigned nameCRC) const; 
	ZeroBaseNode *Find (const char *name) const
	{
		return Find (CalcLowerCRC (name)); 
	} 

	int EnumerateByTypeID(unsigned typeId, const ZeroBaseNode** bufferOut) const
	{
		int count;
		ZeroBaseNode *root;
		root = GetRoot();

		count = 0;
		root->EnumerateByTypeID(typeId, bufferOut, count);
		return count;
	}


	DLL_DECLSPEC void DebugPrint (const char *fileName) const; 
	DLL_DECLSPEC bool SanityCheck();

};


template<class T>
class ZeroNode: public ZeroBaseNode
{
	protected:
	ZeroUnknown *CreateClone (bool isSim)
	{
		return new (isSim) T (*(T*)this);
	}
	
	~ZeroNode () 
	{
	}



	public:
	ZeroNode ()
		:ZeroBaseNode ()
	{
	}

	ZeroNode (const char *name)
		:ZeroBaseNode (name)
	{
	}

	ZeroNode (const T &clone)
		:ZeroBaseNode (clone)
	{
	}

	void CloneFixUp (const ZeroBaseNode &clone)
	{
		ZeroBaseNode::CloneFixUp (clone);
	}


	T *GetChild () const
	{
		return (T*) ZeroBaseNode::GetChild();
	}

	T *GetSibling () const
	{
		return (T*) ZeroBaseNode::GetSibling ();
	}

	T *GetParent () const
	{
		return (T*) ZeroBaseNode::GetParent ();
	}

	
	T *GetRoot () const
	{
		return (T*) ZeroBaseNode::GetRoot ();
	}


	T *GetFirst() const
	{
		return (T*) ZeroBaseNode::GetFirst ();
	}

	T *GetLast() const
	{
		return (T*) ZeroBaseNode::GetLast ();
	}


	T *GetNext() const
	{
		return (T*) ZeroBaseNode::GetNext ();
	}

	T *GetPrev() const
	{
		return (T*) ZeroBaseNode::GetPrev ();
	}


	T *Find (unsigned nameCRC) const 
	{
		return (T*) ZeroBaseNode::Find (nameCRC);
	}

	void ReplaceNode(T *newNode)
	{
		ZeroBaseNode::ReplaceNode (newNode);
	}


	T *Find (const char *name) const
	{
		return (T*) ZeroBaseNode::Find (name);
	} 

	int EnumerateByTypeID(unsigned typeId, const T** bufferOut) const
	{
		return ZeroBaseNode::EnumerateByTypeID(typeId, (const ZeroBaseNode **)bufferOut);
	}


};

#endif
