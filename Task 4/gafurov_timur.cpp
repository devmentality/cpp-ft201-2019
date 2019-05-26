// AVLTree.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include <stdlib.h>
#include <iostream>
#include <map>
#include<string>
using namespace std;

class AVLNode {
	bool isEmpty;
	size_t height;
	int nodeKey;
	AVLNode* left;
	AVLNode* right;

	size_t GetBalanceFactor() {
		size_t rightHeight = (right == nullptr) ? 0 : right->height;
		size_t leftHeight = (left == nullptr) ? 0 : left->height;
		return rightHeight - leftHeight;
	}

	void UpdateHeight()
	{
		size_t rightHeight = (right == nullptr) ? 0 : right->height;
		size_t leftHeight = (left == nullptr) ? 0 : left->height;
		height = (rightHeight > leftHeight ? rightHeight : leftHeight) + 1;
	}

	AVLNode* RotateRight()
	{
		AVLNode* temp = left;
		left = temp->right;
		temp->right = this;
		UpdateHeight();
		temp->UpdateHeight();
		return temp;
	}

	AVLNode* RotateLeft()
	{
		AVLNode* temp = right;
		right = temp->left;
		temp->left = this;
		UpdateHeight();
		temp->UpdateHeight();
		return temp;
	}

	AVLNode* DoBalance()
	{
		UpdateHeight();
		if (GetBalanceFactor() == 2)
		{
			if (right->GetBalanceFactor() < 0)
				right = right->RotateRight();
			return RotateLeft();
		}
		if (GetBalanceFactor() == -2)
		{
			if (left->GetBalanceFactor() > 0)
				left = left->RotateLeft();
			return RotateRight();
		}
		return this;
	}

	AVLNode* GetMin()
	{
		return left->isEmpty ? this : left->GetMin();
	}

	AVLNode* RemoveMin()
	{
		if (left->isEmpty)
			return right;
		left = left->RemoveMin();
		return DoBalance();
	}

	string GetOffset(int level) {
		string offset = "";
		for (int i = 0; i < level; i++)
			offset += "    ";
		return offset;
	}

public:
	AVLNode()
	{
		height = 0;
		isEmpty = true;
	}

	void Dump(int level=0) {
		string offset = GetOffset(level);
		if (!isEmpty) {
			cout << offset << "Node - " << nodeKey << " | Level - " << level << " | ";
			if (!left->isEmpty)
				cout << "Left node - " << left->nodeKey << " | ";
			if (!right->isEmpty)
				cout << "Right node - " << right->nodeKey << " | ";
			cout << endl;
		}
		level++;
		if (!left->isEmpty)
			left->Dump(level);
		if (!right->isEmpty)
			right->Dump(level);
		if (level == 1) {
			cout << "-----------------------------------------------------------";
			cout << "-----------------------------------------------------------" << endl;
		}		
	}

	AVLNode* Remove(int key)
	{
		if (isEmpty)
			return this;
		if (key < nodeKey)
			left = left->Remove(key);
		else if (key > nodeKey)
			right = right->Remove(key);
		else
		{
			AVLNode* oldLeft = left;
			AVLNode* oldRight = right;
			if (oldRight->isEmpty)
				return oldLeft;
			AVLNode* min = oldRight->GetMin();
			min->right = oldRight->RemoveMin();
			min->left = oldLeft;
			return min->DoBalance();
		}
		return DoBalance();
	}

	AVLNode* Add(int key) {
		if (isEmpty) {
			height++;
			nodeKey = key;
			isEmpty = false;
			left = new AVLNode();
			right = new AVLNode();
			return this;
		}
		if (key < nodeKey)
			left = left->Add(key);
		else if (key > nodeKey)
			right = right->Add(key);
		return DoBalance();
	}
};

class AVLTree {
	AVLNode* root;

public:
	AVLTree() {
		root = new AVLNode();
	}

	void Add(int key) {
		root = root->Add(key);
	}

	void Remove(int key) {
		root = root->Remove(key);
	}

	void Dump() {
		root->Dump();
	}
};

int main()
{
	AVLTree* tree = new AVLTree();
	tree->Add(2);
	tree->Add(1);
	tree->Add(4);
	tree->Add(3);
	cout << "Balanced after the addition" << endl << endl;
	tree->Dump();
	tree->Add(7);
	tree->Add(5);
	tree->Add(8);
	tree->Add(6);
	tree->Dump();
	cout << "Balanced after the removing" << endl << endl;
	tree->Remove(4);
	tree->Dump();
	tree->Remove(5);
	tree->Remove(6);
	tree->Dump();
    return 0;
}
