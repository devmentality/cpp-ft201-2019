#include "pch.h"
#include <iostream>

using namespace std;

class Node
{
public:
	int value;
	int height;
	Node* left;
	Node* right;

	Node(int _value)
	{ 
		value = _value;
		height = 1;
		left = 0;
		right = 0;
	}
};

class Tree
{
private:

	Node* root;

	int GetHeight(Node* node)
	{
		if (node != nullptr)
			return node->height;
		return 0;
	}

	int GetBalanceFactor(Node* node)
	{
		return GetHeight(node->right) - GetHeight(node->left);
	}

	void FixHeight(Node* node)
	{
		if (GetHeight(node->left) > GetHeight(node->right))
			node->height = GetHeight(node->left);
		else
			node->height = GetHeight(node->right);
		node->height += 1;
	}

	Node* Rotate(Node* node1, bool isItRight)
	{
		Node* node2;
		if (isItRight)
		{
			node2 = node1->left;
			node1->left = node2->right;
			node2->right = node1;
		}
		else
		{
			node2 = node1->right;
			node1->right = node2->left;
			node2->left = node1;
		}
		FixHeight(node1);
		FixHeight(node2);
		return node2;
	}

	Node* Balance(Node* node)
	{
		FixHeight(node);
		if (GetBalanceFactor(node) == 2)
		{
			if (GetBalanceFactor(node->right) < 0)
				node->right = Rotate(node->right, true);
			return Rotate(node, false);
		}
		if (GetBalanceFactor(node) == -2)
		{
			if (GetBalanceFactor(node->left) > 0)
				node->left = Rotate(node->left, false);
			return Rotate(node, true);
		}
		return node;
	}

	Node* AddValue(Node* node, int value)
	{
		if (node == nullptr)
			return new Node(value);
		if (value < node->value)
			node->left = AddValue(node->left, value);
		else
			node->right = AddValue(node->right, value);
		return Balance(node);
	}

	Node* Findmin(Node* _node)
	{
		if (_node->left)
			return Findmin(_node->left);
		return _node;
	}

	Node* RemoveMin(Node* node)
	{
		if (node->left == 0)
			return node->right;
		node->left = RemoveMin(node->left);
		return Balance(node);
	}

	Node* RemoveValue(Node* node, int value)
	{
		if (!node)
			return 0;
		if (value < node->value)
			node->left = RemoveValue(node->left, value);
		else if (value > node->value)
			node->right = RemoveValue(node->right, value);
		else
		{
			auto left = node->left;
			auto right = node->right;
			delete node;
			if (!right)
				return left;
			auto min = Findmin(right);
			min->right = RemoveMin(right);
			min->left = left;
			return Balance(min);
		}
		return Balance(node);
	}

	void PrintValue(Node* node)
	{
		if (node->left)
			PrintValue(node->left);
		cout << "Value : " << node->value << " Height : " << node->height << endl;
		if (node->right)
			PrintValue(node->right);
	}

public:

	Tree(int value)
	{
		root = new Node(value);
	}

	void Add(int value)
	{
		root = AddValue(root, value);
	}

	void Remove(int value)
	{
		root = RemoveValue(root, value);
	}

	void Print()
	{
		PrintValue(root);
		cout << endl;
	}
};

int main()
{
	auto tree = new Tree(1);
	cout << "Test Add:"<< endl;
	tree->Add(2);
	tree->Add(2);
	tree->Add(3);
	tree->Add(1);
	tree->Print();
	cout << "Test Remove:" << endl;
	tree->Remove(2);
	tree->Print();
}
