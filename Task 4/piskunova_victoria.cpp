#include "pch.h"
#include <iostream>
using namespace std;

class Node {
	unsigned char height;

public:
	int key;

	Node* left;
	Node* right;

	Node(int value)
	{
		key = value;
		right = left = nullptr;
		height = 1;
	}

	static unsigned char GetHeight(Node* pointer)
	{
		if (pointer)
			return pointer->height;
		return 0;
	}

	int GetBalanceFactor()
	{
		return GetHeight(right) - GetHeight(left);
	}

	void RestoreHeight()
	{
		auto right_height = GetHeight(right);
		auto left_height = GetHeight(left);
		if (left_height > right_height)
			height = left_height + 1;
		else
			height = right_height + 1;
	}
};

class AVLTree
{
private:
	Node* root;

	static Node* BalanceNode(Node* pointer)
	{
		pointer->RestoreHeight();
		auto balance_factor = pointer->GetBalanceFactor();
		if (balance_factor == 2)
		{
			if (pointer->right->GetBalanceFactor() < 0)
				pointer->right = RotateRight(pointer->right);
			return RotateLeft(pointer);
		}
		if (balance_factor == -2)
		{
			if (pointer->left->GetBalanceFactor() > 0)
				pointer->left = RotateLeft(pointer->left);
			return RotateRight(pointer);
		}
		return pointer;
	}

	static Node* RotateRight(Node* pointer)
	{
		auto new_pointer = pointer->left;
		pointer->left = new_pointer->right;
		new_pointer->right = pointer;
		pointer->RestoreHeight();
		new_pointer->RestoreHeight();
		return new_pointer;
	}

	static Node* RotateLeft(Node* pointer)
	{
		auto new_pointer = pointer->right;
		pointer->right = new_pointer->left;
		new_pointer->left = pointer;
		pointer->RestoreHeight();
		new_pointer->RestoreHeight();
		return new_pointer;
	}

	static Node* InsertKey(Node* pointer, int key) {
		if (!pointer)
			return new Node(key);
		if (key < pointer->key)
			pointer->left = InsertKey(pointer->left, key);
		else
			pointer->right = InsertKey(pointer->right, key);
		return BalanceNode(pointer);
	}

	static Node* FindMinKey(Node* pointer) {
		if (pointer->left)
			FindMinKey(pointer->left);
		else
			return pointer;
	}

	static Node* DeleteMinKey(Node* pointer)
	{
		if (pointer->left == 0)
			return pointer->right;
		pointer->left = DeleteMinKey(pointer->left);
		return BalanceNode(pointer);
	}

	static Node* Delete(Node* pointer, int key)
	{
		if (!pointer)
			return 0;
		if (key < pointer->key)
			pointer->left = Delete(pointer->left, key);
		else if (key > pointer->key)
			pointer->right = Delete(pointer->right, key);
		else
		{
			auto left = pointer->left;
			auto right = pointer->right;
			delete pointer;
			if (!right) return left;
			auto min_key = FindMinKey(right);
			min_key->right = DeleteMinKey(right);
			min_key->left = left;
			return BalanceNode(min_key);
		}
		return BalanceNode(pointer);
	}

public:
	AVLTree()
	{
		root = nullptr;
	}

	void Insert(int key)
	{
		root = InsertKey(root, key);
	}

	void Remove(int key)
	{
		root = Delete(root, key);
	}

	void PrintNodes(Node* pointer)
	{
		if (pointer != nullptr)
		{
			PrintNodes(pointer->left);
			cout << pointer->key << " ";
			PrintNodes(pointer->right);
		}
	}

	void Print()
	{
		PrintNodes(root);
		cout << "\n";
	}
};

void Test1()
{
	auto avl_tree = AVLTree();
	avl_tree.Insert(1);
	avl_tree.Insert(2);
	avl_tree.Insert(3);
	avl_tree.Print();
	avl_tree.Remove(2);
	avl_tree.Print();
	avl_tree.Remove(1);
	avl_tree.Remove(3);
	avl_tree.Print();
}

void Test2()
{
	auto avl_tree = AVLTree();
	avl_tree.Print();
	avl_tree.Insert(-15);
	avl_tree.Print();
	avl_tree.Remove(-10);
	avl_tree.Print();
	avl_tree.Remove(-15);
	avl_tree.Print();
}

void Test3()
{
	auto avl_tree = AVLTree();
	avl_tree.Insert(-27);
	avl_tree.Insert(0);
	avl_tree.Insert(20);
	avl_tree.Insert(1012);
	avl_tree.Insert(-180);
	avl_tree.Insert(20);
	avl_tree.Print();
	avl_tree.Remove(0);
	avl_tree.Print();
	avl_tree.Remove(20);
	avl_tree.Print();
	avl_tree.Insert(7007000);
	avl_tree.Insert(-99987476);
	avl_tree.Print();
	avl_tree.Remove(-99987476);
	avl_tree.Insert(-5);
	avl_tree.Insert(5);
	avl_tree.Print();
	avl_tree.Insert(1);
	avl_tree.Print();
}

int main()
{
	setlocale(0, "rus");
	cout << "\n";
	Test1();
	cout << "___________" << "\n";
	Test2();
	cout << "___________" << "\n\n";
	Test3();
}
