#include <algorithm>
#include <iostream>
#include "avltree.h"


void AVLtree::rebalance(Node* n)
{
	if (n->len_left > n->len_right+1)
	{
		Node* tmp = n->left;
		
		n->left = n->left->right;
		n->left->parent = n;
		
		tmp->parent = n->parent;
		n->parent = tmp;
		tmp->right = n;
		
		
		n->len_left = tmp->len_right;
		tmp->len_right = std::max(n->len_left, n->len_right)+1;
		
		if (tmp->parent->left == n)
		{
			tmp->parent->left = tmp;
			tmp->parent->len_left = std::max(tmp->len_left, tmp->len_right)+1;
		}
		else
		{
			tmp->parent->right = tmp;
			tmp->parent->len_right = std::max(tmp->len_left, tmp->len_right)+1;
		}
			
	}
	else if (n->len_left+1 < n->len_right)
	{
		Node* tmp = n->right;
		
		n->right = n->right->left;
		n->right->parent = n;
		
		tmp->parent = n->parent;
		n->parent = tmp;
		tmp->left = n;
		
		
		n->len_right = tmp->len_left;
		tmp->len_left = std::max(n->len_left, n->len_right)+1;
		
		if (tmp->parent->left == n)
		{
			tmp->parent->left = tmp;
			tmp->parent->len_left = std::max(tmp->len_left, tmp->len_right)+1;
		}
		else
		{
			tmp->parent->right = tmp;
			tmp->parent->len_right = std::max(tmp->len_left, tmp->len_right)+1;
		}
			
	}
	
}


unsigned AVLtree::insert(Node* n, unsigned new_key)
{
	if (new_key < n->key)
	{
		if (!(n->left) )
		{
			n->left = new Node(n, new_key);
			n->len_left = 1;
			return std::max(n->len_left, n->len_right)+1;
		}
		else
			n->len_left = insert(n->left, new_key);
		
	}
	else 
	{
		if (!(n->right) )
		{
			n->right = new Node(n, new_key);
			n->len_right = 1;
			return std::max(n->len_left, n->len_right)+1;
		}
		else
			n->len_right = insert(n->right, new_key);
		
	}	
	
	rebalance(n);
	
	return std::max(n->len_left, n->len_right)+1;
}



int main(int argc, char* argv[])
{
	if (argc != 2)
	{
		std::cout << "Usage:\tavltree N\n\tN = Number of Nodes\n" << std::endl;
		return 1;
	}
	unsigned nrOfNodes = static_cast<unsigned>(atoi(argv[1]));
	
	
}
