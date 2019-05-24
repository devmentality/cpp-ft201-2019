#pragma comment(linker, "/STACK:16777216")
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <vector>
#include <algorithm>
#include <map>
#include <set>
#include <string>
#include <queue>
#include <unordered_map>
#include <cmath>
#include <iterator>
#include <stack>
#include <bitset>
#include <unordered_set>


using namespace std;


struct Node
{
	int key;
	char height;
	Node* left;
	Node* right;
	Node(int k) 
	{ 
		key = k; left = NULL; right = NULL; height = 1;
	}
};

class Tree
{
private:
	Node* root = NULL;

	char GetHeight(Node* node)
	{
		if (!node)
			return 0;
		
		return node->height;
	}

	int GetRightLeftDifference(Node* node)
	{
		return GetHeight(node->right) - GetHeight(node->left);
	}

	void UpdateHeight(Node* node)
	{
		node->height = max(GetHeight(node->left), GetHeight(node->right)) + 1;
	}

	Node* RotateRight(Node* node)
	{
		Node* left = node->left;
		node->left = left->right;
		left->right = node;
		UpdateHeight(node);
		UpdateHeight(left);
		
		return left;
	}

	Node* RotateLeft(Node* node)
	{
		Node* right = node->right;
		node->right = right->left;
		right->left = node;
		UpdateHeight(node);
		UpdateHeight(right);
		
		return right;
	}

	Node* BalanceNode(Node* node)
	{
		UpdateHeight(node);
		
		if (GetRightLeftDifference(node) == 2)
		{
			if (GetRightLeftDifference(node->right) < 0)
				node->right = RotateRight(node->right);
			return RotateLeft(node);
		}
		
		if (GetRightLeftDifference(node) == -2)
		{
			if (GetRightLeftDifference(node->left) > 0)
				node->left = RotateLeft(node->left);
			return RotateRight(node);
		}
		
		return node;
	}

	Node* InsertKey(Node* node, int key)
	{
		if (!node) return new Node(key);
		
		if (key < node->key)
			node->left = InsertKey(node->left, key);
		else
			node->right = InsertKey(node->right, key);
		
		return BalanceNode(node);
	}

	Node* GetMin(Node* node)
	{
		if (node->left == NULL)
			return node;
		return GetMin(node->left);
	}

	Node* RemoveMin(Node* node)
	{
		if (node->left == 0)
			return node->right;
		
		node->left = RemoveMin(node->left);
		
		return BalanceNode(node);
	}

	Node* RemoveByKey(Node* node, int key)
	{
		if (!node) return 0;
		
		if (key < node->key)
			node->left = RemoveByKey(node->left, key);
		else if (key > node->key)
			node->right = RemoveByKey(node->right, key);
		else
		{
			Node* left = node->left;
			Node* right = node->right;
			delete node;
			
			if (!right) return left;
			
			Node* min = GetMin(right);
			min->right = RemoveMin(right);
			min->left = left;
		
			return BalanceNode(min);
		}

		return BalanceNode(node);
	}

	void ShowTree(Node* node)
	{
		if (node)
		{
			ShowTree(node->left);
			cout << node->key << " ";
			ShowTree(node->right);
		}
	}

	void ShowLeftRightDifference(Node* node)
	{
		if (node)
		{
			ShowLeftRightDifference(node->left);
			
			int leftHeight = 0;
			int rightHeight = 0;
			if (node->left != NULL)
				leftHeight = node->left->height;
			if (node->right != NULL)
				rightHeight = node->right->height;
			
			//All numbers 0 or 1
			cout << "Difference between height in element '" << node->key << "' = " 
				<< abs(rightHeight - leftHeight) << "\n";
			
			ShowLeftRightDifference(node->right);
		}
	}
public:
	void Add(int key)
	{
		root = InsertKey(root, key);
	}

	void Remove(int key)
	{
		root = RemoveByKey(root, key);
	}

	void Show()
	{
		cout << "Keys: ";
		ShowTree(root);
		cout << "\n";

		cout << "Differences:\n";
		ShowLeftRightDifference(root);
		cout << "\n";
	}
};


void testEmptyTree()
{
	Tree tree = Tree();

	cout << "##### Empty tree #####" << endl;
	tree.Show();
}


void testCorrectAddKey()
{
	Tree tree = Tree();
	cout << "##### Add key #####" << endl;
	tree.Add(0);
	tree.Show();
}


void testRemoveNonExistentKey()
{
	Tree tree = Tree();
	cout << "##### Correct remove of non-existent key #####" << endl;
	tree.Remove(11);
	tree.Show();
}


void testCorrectAddIncorrectRemove()
{
	Tree tree = Tree();
	cout << "##### Correct add and remove of non-existent key #####" << endl;
	tree.Add(10);
	tree.Remove(11);
	tree.Show();

}


void testAddManyElements()
{
	Tree tree = Tree();
	cout << "##### Add elements #####" << endl;
	tree.Add(1);
	tree.Add(10);
	tree.Add(100);
	tree.Add(1000);
	tree.Add(-1);
	tree.Add(-11);
	tree.Add(-5);
	tree.Add(-25);
	tree.Add(-128);
	tree.Show();
}


void testAddAndRemoveManyElements()
{
	Tree tree = Tree();
	cout << "##### Add and remove elements #####" << endl;
	tree.Add(1);
	tree.Add(-100);
	tree.Add(250);
	tree.Remove(1);
	tree.Add(300);
	tree.Add(-22222);
	tree.Remove(300);
	tree.Show();
}


void testAddAfterRemove()
{
	Tree tree = Tree();
	cout << "##### Add, remove and add one element #####" << endl;
	tree.Add(1);
	tree.Remove(1);
	tree.Add(1);
	tree.Show();
}


int main()
{
	testEmptyTree();
	testCorrectAddKey();
	testRemoveNonExistentKey();
	testAddManyElements();
	testAddAndRemoveManyElements();
	testCorrectAddIncorrectRemove();
	testAddAfterRemove();
	return 0;
}
