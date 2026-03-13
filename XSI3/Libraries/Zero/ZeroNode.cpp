/****************************************************************************
*
*  Visual C++ 6.0 created by: Julio Jerez
*
****************************************************************************/
#include "ZeroTypes.h"
#include "ZeroNode.h"
#include "ZeroFile.h"



ZERO_RTTI_CONTRUCTOR(ZeroBaseNode);							  


ZeroBaseNode::ZeroBaseNode (const ZeroBaseNode &clone)
	:ZeroUnknown (clone)
{
	ZeroBaseNode *obj;
	ZeroBaseNode *newObj;

	// julio
	// ken please do not change the code without asking me first
	Clear ();

	for (obj = clone.child; obj; obj = obj->sibling) {
		newObj = (ZeroBaseNode *)obj->CreateClone (IsSim());
		newObj->Attach(this);
		newObj->Release();
	}
}

void ZeroBaseNode::CloneFixUp (const ZeroBaseNode &clone)
{
	ZeroBaseNode *obj;
	ZeroBaseNode *cloneChild;

	_ASSERTE (GetNameCRC() == clone.GetNameCRC());

	cloneChild = clone.GetChild();
	for (obj = child; obj; obj = obj->sibling) {
		obj->CloneFixUp (*cloneChild);
		cloneChild = cloneChild->GetSibling();
	}
}


ZeroBaseNode::~ZeroBaseNode () 
{
	ZeroBaseNode *ptr;
	ZeroBaseNode *tmp;

	if (child) {
		for (ptr = child; ptr && ptr->Release(); ptr = tmp) {
			ptr->Kill();
			tmp = ptr->sibling;
			ptr->parent = NULL;
			ptr->sibling = NULL;
		}
	}
	
	for (ptr = sibling; ptr && ptr->Release(); ptr = tmp) {
		ptr->Kill();
		tmp = ptr->sibling;
		ptr->parent = NULL;
		ptr->sibling = NULL;
	}
}


void ZeroBaseNode::Attach (ZeroBaseNode *parentArg)
{
	ZeroBaseNode *tmp;

	_ASSERTE(!parent);
	_ASSERTE(parentArg);

	if (!parentArg) {
		return;
	}

	parent = parentArg;

	// if the parent already has a child...
	if (parentArg->child) {
		for (tmp = parent->child; tmp->sibling; tmp = tmp->sibling);
		tmp->sibling = this;
	} else {
		parent->child = this;
	}

	// add a reference
	AddRef();
}

void ZeroBaseNode::Detach ()
{
	// exit if this node has no parent
	if (!parent)
	{
		return;
	}

	// if this is the first child...
	if (parent->child == this)
	{
		// set this node's sibling as the first child
		parent->child = sibling;
	}
	else
	{
		// remove this node from the sibling list
		ZeroBaseNode *tmp;
		for (tmp = parent->child; tmp->sibling != this; tmp = tmp->sibling);
		tmp->sibling = sibling;
	}

	// clear the parent and sibling pointers
	parent = NULL;
	sibling = NULL;

	// remove a reference
	Release();
}



ZeroBaseNode* ZeroBaseNode::GetRoot ()	const
{
	const ZeroBaseNode *root;
	for (root = this; root->parent; root = root->parent);
	return (ZeroBaseNode*)root;
}


ZeroBaseNode* ZeroBaseNode::GetFirst() const
{
	ZeroBaseNode *ptr;

	for (ptr = (ZeroBaseNode *)this; ptr->child; ptr = ptr->child);
	return ptr;
}

ZeroBaseNode* ZeroBaseNode::GetNext() const
{
	ZeroBaseNode *x;
	ZeroBaseNode *ptr;

	if (sibling) {
		return sibling->GetFirst();
	}

	x = (ZeroBaseNode *)this;
	for (ptr = parent; ptr && (x == ptr->sibling); ptr = ptr->parent) {
		x = ptr;
	}
	return ptr;
}



ZeroBaseNode* ZeroBaseNode::GetLast() const
{
	ZeroBaseNode *ptr;
		
	for (ptr = (ZeroBaseNode *)this; ptr->sibling; ptr = ptr->sibling);
	return ptr;
}


ZeroBaseNode* ZeroBaseNode::GetPrev() const
{
	ZeroBaseNode *x;
	ZeroBaseNode *ptr;

	if (child) {
		return child->GetNext();
	}

	x = (ZeroBaseNode *)this;
	for (ptr = parent; ptr && (x == ptr->child); ptr = ptr->child) {
		x = ptr;
	}
	return ptr;
}



ZeroBaseNode* ZeroBaseNode::Find (unsigned nameCRC) const 
{
	ZeroBaseNode *ptr;

	for (ptr = GetFirst(); ptr; ptr = ptr->GetNext()) {
		if (nameCRC == ptr->GetNameCRC()) {
			break;
		}
	}
	return ptr;
}


void ZeroBaseNode::ReplaceNode(ZeroBaseNode *newNode)
{
	ZeroBaseNode *node;

	_ASSERTE (!newNode->GetChild());
	_ASSERTE (!newNode->GetParent());
	_ASSERTE (!newNode->GetSibling());


	for (node = GetChild(); node; node = GetChild()) {
		node->AddRef();
		node->Detach();
		node->Attach (newNode);
		node->Release();
	}

	node = GetParent();
	if (node) {
		if (sibling) {
			newNode->sibling = sibling;
			sibling = NULL;
		}
		newNode->Attach(node);
		Detach();
	}
}


void ZeroBaseNode::EnumerateByTypeID (
	unsigned typeId, 
	const ZeroBaseNode** bufferOut, 
	int& count) const
{
	ZeroBaseNode *node;

	bufferOut[count] = NULL;
	if (IsTypeID(typeId)) {
		bufferOut[count] = this;
	}

	count ++;
	for (node = GetChild(); node; node = node->GetSibling()) {
		node->EnumerateByTypeID(typeId, bufferOut, count);
	}
}




bool ZeroBaseNode::SanityCheck()
{
	return true;
}

void ZeroBaseNode::PrintHierarchy (
	ZeroFile &file, 
	char *indent) const
{
	ZeroBaseNode *node;
	char newIndent[512];

	strcpy (newIndent, indent);
	strcat (newIndent, "  ");

	for (node = child; node; node = node->sibling) {
		node->PrintHierarchy (file, newIndent);
	} 
}


void ZeroBaseNode::DebugPrint (const char *fileName) const
{
	char indent[512];

	indent[0] = '\0';
	ZeroFile file (fileName, "w");
	PrintHierarchy (file, indent); 
}






