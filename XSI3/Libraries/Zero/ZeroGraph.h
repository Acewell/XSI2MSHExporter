/****************************************************************************
*
*  Visual C++ 6.0 created by: Julio Jerez
*
****************************************************************************/
#ifndef __ZeroGraph__
#define __ZeroGraph__

#include "ZeroTypes.h"
#include "ZeroUnknown.h"
#include "ZeroTree.h"

struct ZeroArch;


class GRAPH_KEY
{
  	ZeroUnknown *node0;
  	ZeroUnknown *node1;

	public:
	GRAPH_KEY (ZeroUnknown *node_A_Arg, ZeroUnknown *node_B_Arg)
	{
		node0 = node_A_Arg;
		node1 = node_B_Arg;
	}

	bool operator> (const GRAPH_KEY &key) const
	{
		if (node0 > key.node0) {
			return true;
		}
		if (node0 == key.node0) {
			if (node1 > key.node1) {
				return true;
			}
		}
		return false;
	}

	bool operator< (const GRAPH_KEY &key) const
	{
		if (node0 < key.node0) {
			return true;
		}
		if (node0 == key.node0) {
			if (node1 < key.node1) {
				return true;
			}
		}
		return false;
	}
};


struct ZeroGraphNodes: public ZeroSimTree <ZeroArch *, ZeroUnknown *>
{
	public:
	ZeroGraphNodes()
		: ZeroSimTree <ZeroArch *, ZeroUnknown *>()
	{
	}

	~ZeroGraphNodes() {}
};

struct ZeroArch
{
	int mark;
	ZeroUnknown *arch;
	ZeroUnknown *node;
	ZeroArch *twin;
	ZeroArch *next;
	ZeroArch *prev;
	ZeroGraphNodes::ZeroTreeNode *entry;

	ZeroArch (
		ZeroUnknown *nodeArg, 
		ZeroUnknown *archArg = NULL)
	{
		mark = 0;
		twin = NULL;
		next = NULL;
		prev = NULL;
		node = nodeArg;
		arch = archArg;
		entry	= NULL;

		if (node) {
			node->AddRef();
		}
		if (arch) {
			arch->AddRef();
		}
	}

	ZeroArch (const ZeroArch &link) 
	{
		mark = link.mark;
		twin = link.twin;
		next = link.next;
		prev = link.prev;
		node = link.node;
		arch = link.arch;
		entry = link.entry;

		if (node) {
			node->AddRef();
		}
		if (arch) {
			arch->AddRef();
		}
	}
	

	~ZeroArch ()
	{
		if (node) {
			node->Release();
		}
		if (arch) {
			arch->Release();
		}
	}
};


class ZeroGraph: public ZeroSimTree <ZeroArch, GRAPH_KEY>
{
	mutable int mark;
	ZeroGraphNodes nodesArray;

	public:
	ZeroGraph (bool isSim)
		:ZeroSimTree <ZeroArch, GRAPH_KEY>(),
		 nodesArray()
	{
		mark = 0;
	}

	DLL_DECLSPEC ZeroGraph (bool isSim, const ZeroGraph& clone);

	~ZeroGraph ()
	{
	}

	int IncLRU () const
	{
		return ++mark;
	}

	DLL_DECLSPEC void AddLink (ZeroUnknown *nodeA, ZeroUnknown *archA,
							   ZeroUnknown *nodeB, ZeroUnknown *archB);

	void AddLink (ZeroUnknown *nodeA, ZeroUnknown *nodeB)
	{
		AddLink (nodeA, NULL, nodeB, NULL);
	}

	DLL_DECLSPEC void RemoveNode (ZeroUnknown *node);

	DLL_DECLSPEC void RemoveAllNode ();

	ZeroArch *FindNode (ZeroUnknown *nodeKey)
	{
		ZeroGraphNodes::ZeroTreeNode *node;
		node = nodesArray.Find(nodeKey);
		return node ? node->info : NULL;
	}

	ZeroArch *FindNodeByID (unsigned id)
	{
		ZeroGraphNodes::ZeroTreeNode *nodePtr;
		ZeroGraphNodes::Iterator iter(nodesArray);
		for (iter.Begin(); iter; iter++) {
			nodePtr = *iter;
			if (nodePtr->info->node->GetNameCRC() == id) {
				return nodePtr->info;
			}
		}
		return NULL;
	}


#ifdef __ENABLE_SANITY_CHECK 
	bool SanityCheck ();
#endif

};

#endif
