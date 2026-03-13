/****************************************************************************
*
*  Visual C++ 6.0 created by: Julio Jerez
*
****************************************************************************/
#include "ZeroTypes.h"
#include "ZeroTree.h"

RedBlackNode *RedBlackNode::Minimum () const
{
	RedBlackNode *ptr;
	for (ptr = (RedBlackNode *)this; ptr->left; ptr = ptr->left);
	return ptr;
}

RedBlackNode *RedBlackNode::Maximum () const
{
	RedBlackNode *ptr;
	for (ptr = (RedBlackNode *)this; ptr->right; ptr = ptr->right);
	return ptr;
}


RedBlackNode *RedBlackNode::Prev () const
{
	RedBlackNode *x;
	RedBlackNode *ptr;

	if (left) {
		return left->Maximum ();
	}

	x = (RedBlackNode *)this;
	for (ptr = parent; ptr && x == ptr->left; ptr = ptr->parent) {
		x = ptr;
	}
	return ptr;

}

RedBlackNode *RedBlackNode::Next () const
{
	RedBlackNode *x;
	RedBlackNode *ptr;

	if (right) {
		return right->Minimum ();
	}

	x = (RedBlackNode *)this;
	for (ptr = parent; ptr && x == ptr->right; ptr = ptr->parent) {
		x = ptr;
	}
	return ptr;
}

// rotate node x to left *
void RedBlackNode::RotateLeft(RedBlackNode **head) 
{
	RedBlackNode *x;
	RedBlackNode *y;

	x = this;
	y = x->right;
	
	// establish x->right link 
	x->right = y->left;
	if (y->left != NULL) {
		y->left->parent = x;
	}
	
	// establish y->parent link 
	if (y != NULL) {
		y->parent = x->parent;
	}
	if (x->parent) {
		if (x == x->parent->left) {
			x->parent->left = y;
		} else {
			x->parent->right = y;
		}
	} else {
		*head = y;
	}
	
	// link x and y 
	y->left = x;
	if (x != NULL) {
		x->parent = y;
	}
}


// rotate node x to right *
void RedBlackNode::RotateRight(RedBlackNode **head) 
{
	RedBlackNode *x;
	RedBlackNode *y;

	x = this;
	y = x->left;

	// establish x->left link 
	x->left = y->right;
	if (y->right != NULL) {
		y->right->parent = x;
	}

	// establish y->parent link 
	if (y != NULL) {
		y->parent = x->parent;
	}
	if (x->parent) {
		if (x == x->parent->right) {
			x->parent->right = y;
		} else {
			x->parent->left = y;
		}
	} else {
		*head = y;
	}

	// link x and y 
	y->right = x;
	if (x != NULL) {
		x->parent = y;
	}
}


// maintain Red-Black tree balance after inserting node ptr   
void RedBlackNode::InsertFixup(RedBlackNode **head) 
{
	RedBlackNode *ptr;
	RedBlackNode *tmp;

	ptr = this;
	// check Red-Black properties 
	while ((ptr != *head) && (ptr->parent->color == RED)) {
		// we have a violation 
		if (ptr->parent == ptr->parent->parent->left) {
			tmp = ptr->parent->parent->right;
			if (tmp && (tmp->color == RED)) {
				// uncle is RED 
				ptr->parent->color = BLACK;
				tmp->color = BLACK;
				ptr->parent->parent->color = RED;
				ptr = ptr->parent->parent;
			} else {
				// uncle is BLACK 
				if (ptr == ptr->parent->right) {
					// make x a left child 
					ptr = ptr->parent;
					ptr->RotateLeft(head);
				}

				ptr->parent->color = BLACK;
				if (ptr->parent->parent) {
					ptr->parent->parent->color = RED;
					ptr->parent->parent->RotateRight(head);
				}
			}
		} else {
			_ASSERTE (ptr->parent == ptr->parent->parent->right);
			// mirror image of above code 
			tmp = ptr->parent->parent->left;
			if (tmp && (tmp->color == RED)) {
				//uncle is RED 
				ptr->parent->color = BLACK;
				tmp->color = BLACK;
				ptr->parent->parent->color = RED;
				ptr = ptr->parent->parent;
			} else {
				// uncle is BLACK 
				if (ptr == ptr->parent->left) {
					ptr = ptr->parent;
					ptr->RotateRight(head);
				}
				ptr->parent->color = BLACK;
				if (ptr->parent->parent->color) {
					ptr->parent->parent->color = RED;
				   ptr->parent->parent->RotateLeft (head); 
				}
			}
		}
	}
	(*head)->color = BLACK;
}


//maintain Red-Black tree balance after deleting node x 
void RedBlackNode::RemoveFixup (RedBlackNode *x, RedBlackNode **head) 
{
	RedBlackNode *p;
	RedBlackNode *w; 

	p = this;
	while ((x != *head) && (!x || x->color == BLACK)) {
		if (x == p->left) {
			if (!p) {
				return;
			}
			w = p->right;
			if (!w) {
				return;
			}
			if (w->color == RED) {
				w->color = BLACK;
				p->color = RED;
				p->RotateLeft (head);
				w = p->right;
				if (!p || !w) {
					return;
				}
			}
			if ((!w->left  || (w->left->color == BLACK)) && 
				 (!w->right || (w->right->color == BLACK))) {
				w->color = RED;
				x = p;
				p = p->parent;
				continue;
			} else if (!w->right || (w->right->color == BLACK)) {
				w->left->color = BLACK;
				w->color = RED;
				w->RotateRight (head);
				w = p->right;
				if (!p || !w) {
					return;
				}
			}
			w->color = p->color;
			if (w->right) {
				w->right->color = BLACK;
			}
			if (p) {
				p->color = BLACK;
				p->RotateLeft (head);
			}
			x = *head;

		} else {
		  	if (!p) {
				return;
		  	}
			w = p->left;
			if (!w) {
				return;
			}
			if (w->color == RED) {
				w->color = BLACK;
				p->color = RED;
				p->RotateRight (head);
				w = p->left;
				if (!p || !w) {
					return;
				}
			}

			if ((!w->right || (w->right->color == BLACK)) && 
				 (!w->left  || (w->left->color == BLACK))) {
				w->color = RED;
				x = p;
				p = p->parent;
				continue;
			} else if (!w->left || (w->left->color == BLACK)) {
				w->right->color = BLACK;
				w->color = RED;
				w->RotateLeft (head);
				w = p->left;
				if (!p || !w) {
					return;
				}
			}
			w->color = p->color;
			if (w->left) {
				w->left->color = BLACK;
			}
			if (p) {
				p->color = BLACK;
				p->RotateRight (head);
			}
			x = *head;
		}
	}
	if (x) {
		x->color = BLACK;
	}
}


void RedBlackNode::Remove (RedBlackNode **head) 
{
	unsigned oldcolor;
	RedBlackNode *child;
	RedBlackNode *end;
	RedBlackNode *endparent;

	inTree = false;	

	if (!left || !right) {
		// y has a NULL node as a child 
		end = this;
	
		// x is y's only child 
		child = end->right;
		if (end->left) {
			child = end->left;
		}
		
		// remove y from the parent chain 
		if (child) {
			child->parent = end->parent;
		}
		
		if (end->parent)	{
			if (end == end->parent->left) {
				end->parent->left = child;
			} else {
				end->parent->right = child;
			}
		} else {
			*head = child;
		}
		
		if (end->color == BLACK) {
			end->parent->RemoveFixup (child, head);
		}
	
	} else {

	  	// find tree successor with a NULL node as a child 
		end = right;
		while (end->left != NULL) {
			end = end->left;
		}
		
		_ASSERTE (end);
		_ASSERTE (end->parent);
		_ASSERTE (!end->left);
		
		// x is y's only child 
		child = end->right;
		
		_ASSERTE ((end != right) || !child || (child->parent == end));

		end->left = left;
		left->parent = end;
		
		endparent = end;
		if (end	!= right) {
			if (child) {
				child->parent = end->parent;
			}
			end->parent->left = child;
			end->right = right;
			right->parent = end;
			endparent = end->parent;
		}
		

		if (this == *head) {
			*head = end;
		} else if (this == parent->left) {
			parent->left = end;
		} else {
			parent->right = end;
		}
		end->parent = parent;
		
		oldcolor = end->color;
		end->color = color;
		color = oldcolor;
		
		if (oldcolor == BLACK) {
		  	endparent->RemoveFixup (child, head);
		}
	}
}
