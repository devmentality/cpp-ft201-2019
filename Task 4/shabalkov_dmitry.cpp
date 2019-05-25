#include <iostream>
using namespace std;

class Node
{
public:
	Node *Left;
	Node *Right;
	int Height;
	int Value;

	Node(char value)
	{
		Left = nullptr;
		Right = nullptr;
		Value = value;
		Height = 1;
	}

};

class AVLTree
{
private:
	Node * _root;
	void CalculateHeight(Node *node)
	{
		int leftHeight = node->Left == nullptr ? 0 : node->Left->Height;
		int rightHeight = node->Right == nullptr ? 0 : node->Right->Height;
		node->Height = leftHeight > rightHeight ? leftHeight + 1 : rightHeight + 1;
	}

	int CalculateBalanceFactor(Node* node)
	{
		int leftHeight = node->Left == nullptr ? 0 : node->Left->Height;
		int rightHeight = node->Right == nullptr ? 0 : node->Right->Height;
		return rightHeight - leftHeight;
	}

	Node* DoRightRotation(Node* node)
	{
		Node *leftChild = node->Left;
		node->Left = leftChild->Right;
		leftChild->Right = node;
		CalculateHeight(node);
		CalculateHeight(leftChild);
		return leftChild;
	}

	Node* DoLeftRotation(Node* node)
	{
		Node* rightChild = node->Right;
		node->Right = rightChild->Left;
		rightChild->Left = node;
		CalculateHeight(node);
		CalculateHeight(rightChild);
		return rightChild;
	}

	Node* BalanceNode(Node* node)
	{
		CalculateHeight(node);
		if(CalculateBalanceFactor(node) == 2)
		{
			if (CalculateBalanceFactor(node->Right) < 0)
				node->Right = DoRightRotation(node->Right);
			return DoLeftRotation(node);
		}
		if(CalculateBalanceFactor(node) == -2)
		{
			if (CalculateBalanceFactor(node->Left) > 0)
				node->Left = DoLeftRotation(node->Left);
			return DoRightRotation(node);
		}
		return node;
	}

	Node* FindFirstElement(Node *node)
	{
		if (node->Left != nullptr)
			return FindFirstElement(node->Left);
		return node;
	}

	Node* DeleteFirstElement(Node* node)
	{
		if (node->Left == nullptr)
			return node->Right;
		node->Left = DeleteFirstElement(node->Left);
		return BalanceNode(node);
	}

	Node* InsertValue(Node* node, int value)
	{
		if (node == nullptr)
		return new Node(value);
		if (value < node->Value)
		node->Left = InsertValue(node->Left, value);
		else
		node->Right = InsertValue(node->Right, value);
		return BalanceNode(node);
	}

	Node* DeleteElement(Node *node, int value)
	{
		if (!node)
			return nullptr;
		if (value < node->Value)
			node->Left = DeleteElement(node->Left, value);
		else
		{
			if (value > node->Value)
				node->Right = DeleteElement(node->Right, value);
			else
			{
				Node* leftChild = node->Left;
				Node* rightChild = node->Right;
				delete node;
				if (rightChild == nullptr)
					return leftChild;
				Node* firstNode = FindFirstElement(rightChild);
				firstNode->Right = DeleteFirstElement(rightChild);
				firstNode->Left = leftChild;
				return BalanceNode(firstNode);
			}
		}
		return BalanceNode(node);
	}

public:

	AVLTree()
	{
		_root = nullptr;
	}

	void Insert(int value)
	{
		Node* node = new Node(value);
		_root = InsertValue(_root, value);
	}

	void Remove(int value)
	{
		_root = DeleteElement(_root, value);
	}

	void PrintTree()
	{
		Print(_root);
	}

	void Print(Node* node)
	{
		if (node->Left != nullptr)
			Print(node->Left);
		cout << "Value : " << node->Value << " Height : "<< node->Height << endl;
		if (node->Right != nullptr)
			Print(node->Right);
	}
};

int main()
{
	AVLTree* tree = new AVLTree();
	tree->Insert(1);
	tree->Insert(3);
	tree->Insert(2);
	cout << "3 nodes added" << endl;
	tree->PrintTree();
	tree->Insert(5);
	tree->Insert(7);
	tree->Insert(6);
	cout << "6 nodes added" << endl;
	tree->PrintTree();
	tree->Remove(5);
	cout << "5th node is removed" << endl;
	tree->PrintTree();
	system("pause");
}