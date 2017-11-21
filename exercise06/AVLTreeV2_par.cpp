// "AVLTree.cpp": Definiert den Einstiegspunkt für die Konsolenanwendung.
//

#include <iostream>
#include <vector>
#include <random>
#include <string>
#include <queue>
#include <tuple>
#include <cmath>
#include <omp.h>
#include "chrono_timer.h"

#ifdef _WIN32
	#include <windows.h>
	void sleep(unsigned milliseconds)
    {
        Sleep(milliseconds);
    }
#else
	#include <unistd.h>
    void sleep(unsigned milliseconds)
    {
        usleep(milliseconds * 1000); // takes microseconds
    }
#endif

template<typename T>
class AVLTree {


	struct Node {
		T m_value;
		int height;
		Node* left_child;
		Node* right_child;
		Node(T value) : m_value(value), left_child(nullptr), right_child(nullptr), height(1) {}
	};

	Node* head = nullptr;

	void m_print(std::queue<std::tuple<Node*, int,int>> q, std::queue<std::tuple<Node*, int,int>> children, int level) {
		if (q.empty())
			return;
		using elem = typename std::tuple<Node*, int, int>;
		elem queue_elem;
		Node* node;
		int shifting_factor = 4;
		int count_space = 0;
		int shift, pos, parent_spaces;
		std::string number;

		while (!q.empty()) {
			queue_elem = q.front();
			node = std::get<0>(queue_elem);
			parent_spaces = std::get<1>(queue_elem);
			pos = std::get<2>(queue_elem);
			q.pop();
			shift = (int) pow(2, level - 1);

			number = std::to_string(node->m_value);

			if (parent_spaces == 0) { //head node
				std::cout << std::string((parent_spaces + shift), ' ') << number;
			}
			else {
				if (pos == 0) { //left child
					count_space = (parent_spaces - shift) - count_space;
					std::cout << std::string( count_space, ' ') << number;
					count_space = parent_spaces - shift;
				}
				else{
					count_space = (parent_spaces + shift) - count_space;
					std::cout << std::string(count_space, ' ') << number;
					count_space = parent_spaces + shift;
				}

			}

			int parent_space_count = parent_spaces == 0 ? shift : pos == 0 ? parent_spaces - shift : parent_spaces + shift;
			if (node->left_child != nullptr) {
				children.push(std::make_tuple(node->left_child, parent_space_count,0));
			}

			if (node->right_child != nullptr) {
				children.push(std::make_tuple(node->right_child, parent_space_count,1));
			}

		}
		std::cout << std::endl;
		m_print(children, q, --level);
	}

	int get_height(Node* node) {
		if (node == nullptr)
			return 0;
		return node->height;
	}

	int max_height(int left_child_height, int right_child_height) {
		return left_child_height < right_child_height ? right_child_height : left_child_height;
	}

	int get_balance(Node* node) {
		if (node == nullptr)
			return 0;
		return get_height(node->right_child) - get_height(node->left_child);
	}

	Node* right_rotate(Node* node) {
		Node* left_child = node->left_child;
		node->left_child = left_child->right_child;
		left_child->right_child = node;
		node->height = max_height(get_height(node->left_child), get_height(node->right_child)) + 1;
		left_child->height = max_height(get_height(left_child->left_child), get_height(left_child->right_child)) + 1;
		return left_child;
	}

	Node* left_rotate(Node* node) {
		Node* right_child = node->right_child;
		node->right_child = right_child->left_child;
		right_child->left_child = node;
		node->height = max_height(get_height(node->left_child), get_height(node->right_child)) + 1;
		right_child->height = max_height(get_height(right_child->left_child), get_height(right_child->right_child)) + 1;
		return right_child;
	}
	
	omp_nest_lock_t read_lock, write_lock, balance_lock;
	
	Node* find_parent_node(Node* node, T value){	
		if(value < node->m_value){
			if(node->left_child == nullptr)
				return node;
			find_parent_node(node->left_child, value);
		}
		else if(value > node->m_value){
			if(node->right_child == nullptr)
				return node;
			find_parent_node(node->right_child, value);
		}
		else
			return node;
	}
	
	Node* insert(Node* node, T value, omp_nest_lock_t* locks) {
		
		if (node == nullptr){
			return new Node(value);
		}
		
		if (value < node->m_value)
			node->left_child = insert(node->left_child, value, locks);
		else if (value > node->m_value)
			node->right_child = insert(node->right_child, value, locks);
		else
			return node;
		

		omp_set_nest_lock(&balance_lock);
		node->height = 1 + max_height(get_height(node->left_child), get_height(node->right_child));

		int balance = get_balance(node);

		if (balance < -1 && value < node->left_child->m_value)
			node = right_rotate(node);
		else if (balance > 1 && value > node->right_child->m_value)
			node = left_rotate(node);
		else if (balance < -1 && value > node->left_child->m_value) {
			node->left_child = left_rotate(node->left_child);
			node = right_rotate(node);
		}
		else if (balance > 1 && value < node->right_child->m_value) {
			node->right_child = right_rotate(node->right_child);
			node = left_rotate(node);
		}
		head = node;
		omp_unset_nest_lock(&balance_lock);
		
		
		return node;
	}

public:
	AVLTree(T value) : head(new Node(value)) {
		omp_init_nest_lock(&read_lock);
		omp_init_nest_lock(&write_lock);
		omp_init_nest_lock(&balance_lock);
	} //initialize lock
	bool insert(T value, omp_nest_lock_t* locks) {
		
		Node* parent_node = find_parent_node(head, value);
		while(!omp_test_nest_lock(&locks[parent_node->m_value])){ //lock parent node
			parent_node = find_parent_node(head, value);
		};
		insert(head, value, locks);
		omp_unset_nest_lock(&locks[parent_node->m_value]); //unlock parent node
		
		return true;
	}

	void print() {
		//second element denotes the parent node's number of spaces; 
		//third element denotes the whether element is left or right child (0->left, 1->right)
		std::queue<std::tuple<Node*, int,int>> first, second;
		first.push(std::tuple<Node*, int,int>(head, 0,0));
		m_print(first, second, head->height);

	}




};

std::vector<unsigned int> generate_random_number(unsigned int N) {
	std::default_random_engine e(4556);
	std::uniform_int_distribution<> uniform_dist(0, N / 8);
	std::vector<unsigned int> numbers_vector(N);
	for (unsigned int i = 0; i < N; i++) {
		numbers_vector[i] = (uniform_dist(e));
	}
	return numbers_vector;
}


int main(int argc, char** argv)
{
	if (argc != 2) {
		std::cout << "Invalid Argument\nUsage example: ./AVLTree 5" << std::endl;
		return 1;
	}
	unsigned int N = atoi(argv[1]);
	auto numbers = generate_random_number(N);
	
	/*
	for (unsigned int i = 0; i < N; i++) {
		if (i == N - 1)
			std::cout << numbers[i] << std::endl;
		else
			std::cout << numbers[i] << ",";
	}
	*/


	{
		
		omp_nest_lock_t locks[N/8 +1];
		for(int i = 0; i < N/8 + 1; i++){
			omp_init_nest_lock(&locks[i]);
		}
		ChronoTimer t("AVLTree Parallel");
		AVLTree<int> tree(numbers[0]);
		#pragma omp parallel shared(tree, locks)
		{
			#pragma omp for schedule(dynamic) 
			for (unsigned int i = 1; i < N; i++) {
				tree.insert(numbers[i], locks);
			}
		}
		tree.print();
	}
	

	return 0;
}

