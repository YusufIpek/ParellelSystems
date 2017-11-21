#include <algorithm>
#include <iostream>
#include <random>
#include <omp.h>
#include "avltree_par.h"

unsigned* generate_numbers(unsigned amount)
{
	unsigned* numbers = static_cast<unsigned*>(malloc(sizeof(unsigned)*amount));
	if (!numbers)
	{
		std::cout << "error: could not allocate memory" << std::endl;
		return numbers;
	}
	
	std::default_random_engine rd(8088);  
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis(0, amount/8);
	
	for (unsigned i = 0; i < amount; i++)
		numbers[i] = dis(gen);
	
	return numbers;
}


void AVLtree::rotation_right(Node* n)
{
	Node* tmp = n->left;
	
	n->left = n->left->right;
	if (n->left)
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

void AVLtree::rotation_left(Node* n)
{
	
	Node* tmp = n->right;
	//tmp->is_locked = n->is_locked;
	n->right = n->right->left;
	
	if (n->right)
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
	//n->is_locked = false;
}


void AVLtree::rebalance(Node* n)
{
	
	if (n->len_left > n->len_right+1)
	{
		if (n->left->len_right > n->left->len_left) // inner child larger, double rotation
		{
			rotation_left(n->left);
			rotation_right(n);
		}
		else
		{
			rotation_right(n);
		}
	}
	else if (n->len_left+1 < n->len_right)
	{
		if (n->right->len_left > n->right->len_right) // inner child larger, double rotation
		{
			rotation_right(n->right);
			rotation_left(n);
			
		}
		else
		{
			rotation_left(n);
		}
	}
	
}

void AVLtree::release_locks(Node* n)
{
	n->is_locked = false;
	if (n->has_upper_lock)
	{
		release_locks(n->parent);
		n->has_upper_lock = false;
	}
}

unsigned AVLtree::insert(Node* n, unsigned new_key)
{
	if(n->len_left != n->len_right)
		n->is_locked = true;
	if(n->has_upper_lock)
		release_locks(n->parent);
	
	if (new_key < n->key)
	{
		if (!(n->left) )
		{
			n->left = new Node(n, new_key);
			n->len_left = 1;
			n->has_upper_lock = false;
			return std::max(n->len_left, n->len_right)+1;
		}
		else
		{
			while (n->left->is_locked) {};	// waiting
			if (n->is_locked || n->has_upper_lock)
				n->left->has_upper_lock = true;
			n->len_left = insert(n->left, new_key);
			
		}
		
	}
	else 
	{
		if (!(n->right) )
		{
			n->right = new Node(n, new_key);
			n->len_right = 1;
			n->has_upper_lock = false;
			return std::max(n->len_left, n->len_right)+1;
		}
		else
		{
			while (n->right->is_locked) {};	// waiting
			if (n->is_locked || n->has_upper_lock)
				n->right->has_upper_lock = true;
			n->len_right = insert(n->right, new_key);
		}
		
	}
	
	if (n->is_locked)
	{
		rebalance(n);
		n->is_locked = false;
	}
	
	n->has_upper_lock = false;
	
	return std::max(n->len_left, n->len_right)+1;
}


void AVLtree::printTree(Node* n)
{
	level++;
	if (n->left)
		printTree(n->left);
	
	std::cout << "L" << level;
	for (unsigned i = 0; i < level; i++)
		std::cout << "  ";
	
	std::cout << n->key << std::endl;
	
	if (n->right)
		printTree(n->right);
	level--;
	
}

int main(int argc, char* argv[])
{
	if (argc != 2)
	{
		std::cout << "Usage:\tavltree N\n\tN = Number of Nodes\n" << std::endl;
		return 1;
	}
	unsigned nrOfNodes = static_cast<unsigned>(atoi(argv[1]));
	
	unsigned* numbers = generate_numbers(nrOfNodes);
	
	AVLtree tree;
	tree.level = 0;
	tree.root_hook->left = new Node(tree.root_hook, numbers[0]);
	//std::cout << "insert node 0 \tkey = " << numbers[0] << std::endl;
	#pragma omp parallel for
	for (unsigned i = 1; i < nrOfNodes; i++)
	{
		std::cout << "insert node " << i << " \tkey = " << numbers[i] << std::endl; fflush(stdout);
		tree.insert(tree.root_hook->left, numbers[i]);
	}
	
	
	if (nrOfNodes <= 128)
		tree.printTree(tree.root_hook->left);
	free(numbers);
	return 0;
}
