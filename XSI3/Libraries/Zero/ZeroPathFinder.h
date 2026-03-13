//********************************************************************
//	By Julio Jerez
// VC: 6.0
//********************************************************************
#ifndef __ZeroPathFinder__
#define __ZeroPathFinder__


#include "ZeroTypes.h"
#include "ZeroTree.h"
#include "ZeroHeap.h"
#include "ZeroStack.h"



template<class NODEID, class COST> class ZeroPathFinder; 

template<class NODEID, class COST>
class	ZeroPathNode
{
	friend class ZeroPathFinder<NODEID, COST>; 


	NODEID id;
	COST nodeCost;
	COST heuristicCost;
	ZeroPathNode* next;

	ZeroPathNode ()
	{
	}

	public:
	const ZeroPathNode* GetNext() const
	{
		return next;
	}


	COST GetNodeCost() const
	{
		return nodeCost;
	}

	NODEID GetId () const
	{
		return id;
	}
};


template<class NODEID, class COST>
class ZeroPathCloseList: public ZeroSimTree<ZeroPathNode<NODEID, COST>, NODEID>
{
	protected:
	ZeroPathCloseList ()
		:ZeroSimTree<ZeroPathNode<NODEID, COST>, NODEID>()
	{
	}
};


template<class NODEID, class COST>
class ZeroPathOpenHeap: public ZeroHeap<ZeroPathCloseList<NODEID, COST>::ZeroTreeNode*, COST>
{
	protected:
	ZeroPathOpenHeap(COST maxCost, int maxElements)
		:ZeroHeap<ZeroPathCloseList<NODEID, COST>::ZeroTreeNode*, COST>(maxElements, maxCost)
	{
	}
};


template<class NODEID, class COST>
class ZeroPathFinder: 
	public ZeroPathCloseList<NODEID, COST>,
	public ZeroPathOpenHeap<NODEID, COST>
{
	int heapMaxCount;
	int maxChildren;
	int nodesExploredOverall;
	NODEID goalNode;
	ZeroPathNode<NODEID, COST>* path;

	public:
	ZeroPathFinder(COST maxCost, int maxElementsInOpenList, int nodeMaxChildren)
		:ZeroPathCloseList<NODEID, COST>(),
		 ZeroPathOpenHeap<NODEID, COST>(maxCost, maxElementsInOpenList)
	{
		maxChildren	= nodeMaxChildren;
		heapMaxCount = maxElementsInOpenList;
	}

	virtual ~ZeroPathFinder()
	{
	}

	void  __cdecl operator delete (void *ptr)
	{
		ZeroPathCloseList<NODEID,COST>::operator delete (ptr);
	}


	virtual const ZeroPathNode<NODEID, COST>* CalculatePath (NODEID source, NODEID goal)
	{
		int i;
		int count;
		bool state;
		COST maxCost;
		ZeroTreeNode* node;
		ZeroPathNode<NODEID, COST> cell;
		ZeroPathNode<NODEID, COST>* next;
		ZeroPathNode<NODEID, COST>* prev;
		ZeroPathNode<NODEID, COST>* link;
		ZeroStack<NODEID> idPool(maxChildren);
		NODEID* idArray;

		idArray = &idPool[0];
		ZeroPathOpenHeap<NODEID, COST>& heap = *this;
		ZeroPathCloseList<NODEID, COST>& close = *this;

		heap.Flush();
		close.RemoveAll();

		cell.id = source;
		cell.next = NULL;
		cell.nodeCost = COST (0);
		cell.heuristicCost = GetNodeHeuristicCost(cell.id);

		node = close.Insert (cell, source);
		
		maxCost = heap.MaxValue(); 
		heap.Push (node, maxCost - (cell.nodeCost + cell.heuristicCost));
		while (heap.GetCount()) {
			node = heap[0];
			ZeroPathNode<NODEID, COST>& parent = node->info;
			if (parent.id == goal) {
				link = &parent;
				next = NULL;
				do {
					prev = link->next;
					link->next = next;
					next = link;
					link = prev;
				} while (link);
				return next;
			}

			heap.Pop();
			count = EnumerateChildren(node->key, idArray);
			cell.next = &parent;
			for (i = 0; i < count; i ++) {
				cell.id = idArray[i];
				COST newCost (GetNodeCost(cell) + parent.nodeCost);

				node = close.FindInsert (cell, cell.id, state);
				if (state) {
					if (node->info.nodeCost <= newCost) {
						continue;
					}
					else {
						node->info = cell;
					}
				}

				ZeroPathNode<NODEID, COST>& newCell = node->info;
				newCell.nodeCost = newCost;
				newCell.heuristicCost = GetNodeHeuristicCost(cell.id);
				if (heap.GetCount() >= heapMaxCount) {
					heap.Remove (heapMaxCount);
				}
				heap.Push (node, maxCost - (newCell.nodeCost + newCell.heuristicCost));
			}
		}
		return NULL;
	}

	virtual void PathRequest (NODEID source, NODEID goal)
	{
		ZeroTreeNode* node;
		ZeroPathNode<NODEID, COST> cell;

		ZeroPathOpenHeap<NODEID, COST>& heap = *this;
		ZeroPathCloseList<NODEID, COST>& close = *this;

		heap.Flush();
		close.RemoveAll();

		nodesExploredOverall = 0;
		goalNode = goal;
		path = NULL;

		cell.id = source;
		cell.next = NULL;
		cell.nodeCost = COST (0);
		cell.heuristicCost = GetNodeHeuristicCost(cell.id);

		node = close.Insert (cell, source);
		
		COST maxCost = heap.MaxValue(); 
		heap.Push (node, maxCost - (cell.nodeCost + cell.heuristicCost));
	}

	virtual bool Update (int searchNodes)
	{
		int i;
		int count;
		bool state;
		COST maxCost;
		ZeroTreeNode* node;
		ZeroPathNode<NODEID, COST> cell;
		ZeroPathNode<NODEID, COST>* next;
		ZeroPathNode<NODEID, COST>* prev;
		ZeroPathNode<NODEID, COST>* link;
		ZeroStack<NODEID> idPool(maxChildren);
		NODEID* idArray;
		int nodesExploredThisUpdate = 0;

		idArray = &idPool[0];
		ZeroPathOpenHeap<NODEID, COST>& heap = *this;
		ZeroPathCloseList<NODEID, COST>& close = *this;

		maxCost = heap.MaxValue(); 
		while (heap.GetCount()) {

			// check if sufficient nodes have been searched this update
			if ( nodesExploredThisUpdate++ > searchNodes )
				return false;

			nodesExploredOverall++;
			node = heap[0];
			ZeroPathNode<NODEID, COST>& parent = node->info;
			if (AtGoal (parent.id, goalNode)) {
				link = &parent;
				next = NULL;
				do {
					prev = link->next;
					link->next = next;
					next = link;
					link = prev;
				} while (link);
				path = next;
				return true;
			}

			heap.Pop();
			count = EnumerateChildren(node->key, idArray);
			cell.next = &parent;
			for (i = 0; i < count; i ++) {
				cell.id = idArray[i];
				COST newCost (GetNodeCost(cell) + parent.nodeCost);

//				if (close.Find(cell.id)) {
//					if (parent.nodeCost < newCost) {
//						continue;
//					}
//				}
//				cell.nodeCost = newCost;
//				cell.heuristicCost = GetNodeHeuristicCost(cell.id);
//				node = close.Insert (cell, cell.id);

				node = close.FindInsert (cell, cell.id, state);
				if (state) {
					if (node->info.nodeCost <= newCost) {
						continue;
					}
					else {
						node->info = cell;
					}
				}

				ZeroPathNode<NODEID, COST>& newCell = node->info;
				newCell.nodeCost = newCost;
				newCell.heuristicCost = GetNodeHeuristicCost(cell.id);
				if (heap.GetCount() >= heapMaxCount) {
					heap.Remove (heapMaxCount);
				}
				heap.Push (node, maxCost - (newCell.nodeCost + newCell.heuristicCost));
			}
		}

		path = NULL;
		return true;
	}

	virtual COST GetNodeCost(const ZeroPathNode<NODEID, COST>& node) const
	{
		return COST (1);
	}

	virtual COST GetNodeHeuristicCost(NODEID id) const
	{
		return COST (1);
	}

	virtual int EnumerateChildren(NODEID parent, NODEID array[]) const
	{
		return 0;
	}

	virtual bool AtGoal(NODEID test, NODEID goal) { return false;}

	int GetNodesExplored() const
	{
		return nodesExploredOverall;
	}

	const ZeroPathNode<NODEID, COST>* GetPath() const
	{
		return path;
	}
};

#endif

