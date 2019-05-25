#include<iostream>
#include<string>

using namespace std;

struct Node
{
	int key;
	unsigned char h;
	Node* l;
	Node* r;

	Node(int n) {
		key = n;
		l = NULL;
		r = NULL;
		h = 1;
	}

  	unsigned char heqight(Node* p) {
		return p ? p->h : 0;
	}
};

class AVL
{
private:
	Node* root = NULL;

	Node* insert(Node* p, int k)
	{
		if (!p)
			return new Node(k);
		if (k < p->key)
			p->l = insert(p->l, k);
		else
			p->r = insert(p->r, k);
		return balance_tree(p);
	}

	Node* remove(Node* p, int k)
	{
	if (!p) return 0;
		if (k < p->key)
			p->l = remove(p->l, k);
		else if (k > p->key)
			p->r = remove(p->r, k);
		else {
			Node* q = p->l;
			Node* r = p->r;
			delete p;
			if (!r) return q;
			Node* min = find_min(r);
			min->r = remove_min(r);
			min->l = q;
			return balance_tree(min);
		}
		return balance_tree(p);
	}

	Node* find_min(Node* node)
	{
		return node->l ? find_min(node->l) : node;
	}

	Node* remove_min(Node* node)
	{
		if (node->l == 0)
			return node->r;
		node->l = remove_min(node->l);
		return balance_tree(node);
	}

	Node* balance_tree(Node* node)
	{
		recount_height(node);
		if (get_balance_factor(node) == 2)
		{
			if (get_balance_factor(node->r) < 0)
				node->r = rotate_right(node->r);
			return rotate_left(node);
		}
		if (get_balance_factor(node) == -2)
		{
			if (get_balance_factor(node->l) > 0)
				node->l = rotate_left(node->l);
			return rotate_right(node);
		}
		return node;
	}

  void fixHeight(Node* p) {
		unsigned char hl = height(p->l);
		unsigned char hr = height(p->r);
		p->h = (hl > hr ? hl : hr) + 1;
	}


	Node* rotate_right(Node* p)
	{
		Node* q = p->l;
		p->l = q->r;
		q->r = p;
		fixHeight(p);
		fixHeight(q);
		return q;
	}

	Node* rotate_left(Node* q)
	{
		Node* p = q->r;
		q->r = p->l;
		p->l = q;
		fixHeight(q);
		fixHeight(p);
		return p;

	}

	unsigned char height(Node* node)
	{
		return node ? node->h : 0;
	}

	int get_balance_factor(Node* node)
	{
		return height(node->r) - height(node->l);
	}

	void recount_height(Node* node)
	{
		unsigned char left_h = height(node->l);
		unsigned char right_h = height(node->r);
		node->h = (left_h > right_h ? left_h : right_h) + 1;
	}

	void dump(Node* node, int indent = 0) {
		if (node) {
			for (int i = 0; i < indent; i++)
				cout << " ";
			cout << node->key << "\n ";
			if (node->l)
				dump(node->l, indent + 4);
			if (node->r)
				dump(node->r, indent + 4);

		}
	}

public:
	void insert(int key) {
		root = insert(root, key);
	}

	void balance() {
		root = balance_tree(root);
	}

	void remove(int key) {
		root = remove(root, key);
	}

	void dump() {
		dump(root);
	}
};

int main() {
	auto tree = new AVL();
	cout << endl << "_____add_3_elements_____" << endl;
	for (int i = 0; i < 3; i++) 
		tree->insert(i);		
	tree->dump();
	cout << endl << "_____insert_____" << endl;
	tree->insert(8);
	tree->dump();
	cout << endl << "_____remove_____" << endl;
	tree->remove(3);
	tree->dump();
	cout << endl << "_____remove_nonexistent_el_____" << endl;
	tree->remove(10);
	tree->dump();
	cout << endl << "_____remove_leaf_____" << endl;
	tree->remove(8);
	tree->dump();
}