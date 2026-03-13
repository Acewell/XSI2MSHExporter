/****************************************************************************
*
*  Visual C++ 6.0 created by: Julio Jerez
*
****************************************************************************/
#include "ZeroTypes.h"
#include "ZeroGraph.h"

ZeroGraph::ZeroGraph (bool isSim, const ZeroGraph& clone)
	:ZeroSimTree <ZeroArch, GRAPH_KEY>(),
	 nodesArray ()
{
	int mark1;
	ZeroArch	*arch;
	ZeroUnknown* node;
	ZeroUnknown* state0;
	ZeroUnknown* state1;
	ZeroUnknown* arch01;
	ZeroUnknown* arch10;

	mark = 0;
	// temp trees cane visual
	ZeroVisTree<ZeroUnknown*, ZeroUnknown*> archList;
	ZeroVisTree<ZeroUnknown*, ZeroUnknown*> nodesList;

	Iterator iter (clone);
	for (iter.Begin(); iter; iter ++) {
		arch = &(*iter)->info;
		if (!nodesList.Find(arch->node))	{
			node = arch->node->CreateClone(isSim);
			nodesList.Insert(node, arch->node);
		}
		if (!archList.Find(arch->arch)) {
			node = arch->arch->CreateClone(isSim);
			archList.Insert(node, arch->arch);
		}
	}

	mark1 = clone.IncLRU();
	for (iter.Begin(); iter; iter ++) {
		arch = &(*iter)->info;
		if (arch->mark == mark1) {
			continue;
		}

		arch->mark = mark1;
		state0 = nodesList.Find(arch->node)->info;
		arch01 = archList.Find(arch->arch)->info;

		arch = arch->twin;
		arch->mark = mark1;
		state1 = nodesList.Find(arch->node)->info;
		arch10 = archList.Find(arch->arch)->info;

		AddLink (state0, arch01, state1, arch10);
	}

	ZeroSimTree<ZeroUnknown*, ZeroUnknown*>::Iterator archListIter (archList);
	for (archListIter.Begin(); archListIter; archListIter ++) {
		node = (*archListIter)->info;
		node->Release();
	}

	ZeroSimTree<ZeroUnknown*, ZeroUnknown*>::Iterator nodesListIter (nodesList);
	for (nodesListIter.Begin(); nodesListIter; nodesListIter ++) {
		node = (*nodesListIter)->info;
		node->Release();
	}
}


void ZeroGraph::AddLink (
	ZeroUnknown *nodeA_Arg, 
	ZeroUnknown *archA_Arg,
	ZeroUnknown *nodeB_Arg, 
	ZeroUnknown *archB_Arg)
{
	ZeroTreeNode *node_APtr;
	ZeroTreeNode *node_BPtr;
	ZeroArch *link;
	ZeroArch *linkA;
	ZeroArch *linkB;
	ZeroGraphNodes::ZeroTreeNode *entry_A;
	ZeroGraphNodes::ZeroTreeNode *entry_B;

#ifdef __ENABLE_SANITY_CHECK 
	_ASSERTE (nodeA_Arg);
	_ASSERTE (nodeB_Arg);
	entry_A = nodesArray.Find(nodeA_Arg);
	if (entry_A) {
		link = entry_A->info;
		do {
			_ASSERTE	(link->twin->node != nodeB_Arg);
			link = link->twin->next;
		} while (link != entry_A->info);
	}

	entry_B = nodesArray.Find(nodeB_Arg);
	if (entry_B) {
		link = entry_B->info;
		do {
			_ASSERTE (link->twin->node != nodeA_Arg);
			link = link->twin->next;
		} while (link != entry_B->info);
	}
#endif

	GRAPH_KEY Key_AB (nodeA_Arg, nodeB_Arg);
	_ASSERTE (!Find(Key_AB));
	ZeroArch TmpNode_A (nodeA_Arg, archA_Arg);
	node_APtr = Insert (TmpNode_A, Key_AB);
	linkA = &node_APtr->info;

	GRAPH_KEY Key_BA (nodeB_Arg, nodeA_Arg);
	_ASSERTE (!Find(Key_BA));
	ZeroArch TmpNode_B (nodeB_Arg, archB_Arg);
	node_BPtr = Insert (TmpNode_B, Key_BA);
	linkB = &node_BPtr->info;

	linkA->twin = linkB;
	linkB->twin = linkA;

	entry_A = nodesArray.Find(nodeA_Arg);
	if (!entry_A) {
		linkA->entry = nodesArray.Insert (linkA, nodeA_Arg);
		linkA->prev = linkB;
		linkB->next = linkA;
	} else {
		link = entry_A->info;

		link->prev->next = linkA;
		linkA->prev = link->prev;

		link->prev = linkB;
		linkB->next = link;
	}

	entry_B = nodesArray.Find(nodeB_Arg);
	if (!entry_B) {
		linkB->entry = nodesArray.Insert (linkB, nodeB_Arg);
		linkA->next = linkB;
		linkB->prev = linkA;
	} else {
		link = entry_B->info;

		link->prev->next = linkB;
		linkB->prev = link->prev;

		linkA->next = link;
		link->prev = linkA;
	}

#ifdef __ENABLE_SANITY_CHECK 
	_ASSERTE (SanityCheck ());
#endif
}

void ZeroGraph::RemoveNode (ZeroUnknown *node)
{
	ZeroArch *ptr;
	ZeroArch *next;
	ZeroArch *twin;
	ZeroArch *link;
	ZeroGraphNodes::ZeroTreeNode *entry;

	entry = nodesArray.Find (node);
	if (!entry) {
		return;
	}

	link = entry->info;
	_ASSERTE (link->node == node);

	ptr = link;
	do {
		next = ptr->twin->next;

		twin = ptr->twin;

		_ASSERTE (twin);
		_ASSERTE (twin->entry || (twin->twin->next != twin));

		if (twin->entry) {
			if (twin->twin->next == twin) {
				nodesArray.Remove (twin->node);
			} else {
			   twin->twin->next->entry = nodesArray.Replace (twin->twin->next, twin->node);
			}
		}

		ptr->next->prev = twin->prev;
		twin->prev->next = ptr->next;
		
		GRAPH_KEY Key0(node, twin->node);
		Remove (Key0);

		GRAPH_KEY Key1(twin->node, node);
		Remove (Key1);

		ptr = next;
	} while (ptr != link);

	nodesArray.Remove (entry);

#ifdef __ENABLE_SANITY_CHECK 
	_ASSERTE (SanityCheck ());
#endif
}


void ZeroGraph::RemoveAllNode ()
{
	ZeroUnknown *node;
	ZeroGraphNodes::Iterator iter (nodesArray);
	ZeroGraphNodes::ZeroTreeNode *treenode;

	for (iter.Begin(); iter; ) {
		treenode = *iter;
		iter++;
		node = treenode->info->node;
		treenode->AddRef();
		RemoveNode (node);
		if (!treenode->Release()) {
			iter.Begin();
		}
	}
}


#ifdef __ENABLE_SANITY_CHECK 
bool ZeroGraph::SanityCheck ()
{
	int count;
	ZeroUnknown *node; 
	ZeroArch *arch;
	ZeroArch *link;
	ZeroGraphNodes::Iterator nodeIter(nodesArray);

	for (nodeIter.Begin(); nodeIter; nodeIter++) {
		arch = *nodeIter.Contents();
		link = arch;
		node = link->node; 

		count = 0;
		do {
			count++;
			_ASSERTE (count < 1000000);

			_ASSERTE (link->node == node);
			_ASSERTE (link == link->twin->twin);  
			_ASSERTE (link == link->next->prev);  

			link = link->twin->next;
		} while (link != arch);

		count = 0;
		do {
			count ++;
			_ASSERTE (count < 1000000);
			link = link->prev->twin;
		} while (link != arch);
	}

	if (!nodesArray.SanityCheck ()) {
		return false;
	}

	return ZeroSimTree <ZeroArch, GRAPH_KEY>::SanityCheck ();
}
#endif

