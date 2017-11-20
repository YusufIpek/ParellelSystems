

class Node
{
public:
	Node* parent;
	Node* left;
	Node* right;
	unsigned key, len_left, len_right;
	
	Node(Node* _parent, unsigned _key) 
	: parent(_parent), key(_key) , len_left(0), len_right(0)
	{ }
};


class AVLtree
{
public: 
	Node* root_hook;
	unsigned level;
	
	unsigned insert(Node* n, unsigned new_key);
	void rebalance(Node* n);
	void printTree(Node* n);
	void rotation_left(Node* n);
	void rotation_right(Node*n);
	
	AVLtree()
	{
		root_hook = new Node(nullptr, 0);
	}
	
};

