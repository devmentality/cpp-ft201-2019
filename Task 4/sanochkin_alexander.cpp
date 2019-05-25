#include "pch.h"
#include <iostream>
#include <algorithm>

using namespace std;

struct Node
{
	int value;
	Node* left;
	Node* right;
	unsigned char height;
	Node(int val) : value(val), left(nullptr), right(nullptr), height(1) {}
};

class AvlTree
{
	Node* root;

	void UpdateHeight(Node* n)
	{
		auto left_height = Height(n->left);
		auto right_height = Height(n->right);
		n->height = (right_height < left_height ? left_height : right_height) + 1;
	}

	int GetBalanceFactor(Node* n)
	{
		return Height(n->right) - Height(n->left);
	}

	Node* RightRotate(Node* &n)
	{
		Node* p = n->left;
		n->left = p->right;
		p->right = n;
		UpdateHeight(n);
		UpdateHeight(p);
		return p;
	}

	Node* LeftRotate(Node* &n)
	{
		Node* p = n->right;
		n->right = p->left;
		p->left = n;
		UpdateHeight(n);
		UpdateHeight(p);
		return p;
	}

	Node* Balance(Node* n)
	{
		UpdateHeight(n);
		if (GetBalanceFactor(n) == 2)
		{
			if (GetBalanceFactor(n->right) < 0)
				n->right = RightRotate(n->right);
			return LeftRotate(n);
		}
		if (GetBalanceFactor(n) == -2)
		{
			if (GetBalanceFactor(n->left) > 0)
				n->left = LeftRotate(n->left);
			return RightRotate(n);
		}
		return n;
	}

	Node* Insert(int v, Node* n)
	{
		if (!n)
			return new Node(v);
		if (v < n->value)
			n->left = Insert(v, n->left);
		else if (v > n->value)
			n->right = Insert(v, n->right);
		return Balance(n);
	}

	Node* FindMin(Node* n)
	{
		return n->left ? FindMin(n->left) : n;
	}

	Node* RemoveMin(Node* n)
	{
		if (n->left == nullptr)
			return n->right;
		n->left = RemoveMin(n->left);
		return Balance(n);
	}

	Node* Remove(int v, Node* n)
	{
		if (n == nullptr)
			return nullptr;

		if (v < n->value)
			n->left = Remove(v, n->left);
		else if (v > n->value)
			n->right = Remove(v, n->right);
		else
		{
			Node* left = n->left;
			Node* right = n->right;
			delete n;
			if (!right) return left;
			Node* min = FindMin(right);
			min->right = RemoveMin(right);
			min->left = left;
			return Balance(min);
		}
		return Balance(n);
	}

	void InOrder(Node* n)
	{
		if (n == nullptr)
			return;
		InOrder(n->left);
		cout << n->value << " ";
		InOrder(n->right);
	}

	void Dispose(Node* n)
	{
		if (n == nullptr)
			return;
		Dispose(n->left);
		Dispose(n->right);
		delete n;
	}
public:
	AvlTree(): root(nullptr) {}

	unsigned char Height(Node* p)
	{
		return p ? p->height : 0;
	}

	void Insert(int x)
	{
		root = Insert(x, root);
	}

	void InsertMany(initializer_list<int> args)
	{		
		for(const auto& x: args)
			root = Insert(x, root);
	}

	void Remove(int x)
	{
		root = Remove(x, root);
	}

	void PrintInOrder()
	{
		InOrder(root);
		cout << endl;
	}

	~AvlTree()
	{
		Dispose(root);
	}
};

int main()
{
	AvlTree tree;
	tree.InsertMany({9,7,8,1,5,2,6,4,3});
	tree.PrintInOrder();
	tree.Remove(5);
	tree.Remove(10);
	tree.Remove(2);
	tree.Remove(8);
	tree.PrintInOrder();
}