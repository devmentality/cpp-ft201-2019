#include <iostream>
using namespace std;


class Node
{
public:
	int value;
	int height;
	Node *left;
	Node *right;

	Node(char value)
	{
		left = nullptr;
		right = nullptr;
		value = value;
		height = 1;
	}

	unsigned char GetRightHeight(Node* p)
	{
		return p ? p->height : 0;
	}

	void GetHeight()
	{
		unsigned char hl = GetRightHeight(this->left);
		unsigned char hr = GetRightHeight(this->right);
		this->height = (hl>hr ? hl : hr) + 1;
	}

	int GetBfactor()
	{
		return GetRightHeight(this->right) - GetRightHeight(this->left);
	}
};



class AVLTree
{
private:
	Node * _root;

	Node* RotateRight(Node* node)
	{
		Node* q = node->left;
		node->left = q->right;
		q->right = node;
		node->GetHeight();
		q->GetHeight();
		return q;
	}

	Node* RotateLeft(Node* node)
	{
		Node* p = node->right;
		node->right = p->left;
		p->left = node;
		node->GetHeight();
		p->GetHeight();
		return p;
	}

	Node* BalanceNode(Node* node)
	{
		node->GetHeight();
		if (node->GetBfactor() == 2)
		{
			if (node->right->GetBfactor() < 0)
				node->right = RotateRight(node->right);
			return RotateLeft(node);
		}
		if (node->GetBfactor() == -2)
		{
			if (node->left->GetBfactor() > 0)
				node->left = RotateLeft(node->left);
			return RotateRight(node);
		}
		return node;
	}

	Node* FindMinElement(Node *node)
	{
		return node->left ? FindMinElement(node->left) : node;
	}

	Node* DeleteMinElement(Node* node)
	{
		if (node->left == nullptr)
			return node->right;
		node->left = DeleteMinElement(node->left);
		return BalanceNode(node);
	}

	Node* InsertValue(Node* node, int value)
	{
		if (node == nullptr)
			return new Node(value);
		if (value < node->value)
			node->left = InsertValue(node->left, value);
		else
			node->right = InsertValue(node->right, value);
		return BalanceNode(node);
	}

	Node* DeleteElement(Node *node, int value)
	{
		if (!node)
			return nullptr;
		if (value < node->value)
			node->left = DeleteElement(node->left, value);
		else
		{
			if (value > node->value)
				node->right = DeleteElement(node->right, value);
			else
			{
				Node* leftChild = node->left;
				Node* rightChild = node->right;
				delete node;
				if (rightChild == nullptr)
					return leftChild;
				Node* firstNode = FindMinElement(rightChild);
				firstNode->right = DeleteMinElement(rightChild);
				firstNode->left = leftChild;
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
		if (node->left != nullptr)
			Print(node->left);
		cout << "Value : " << node->value << " Height : " << node->height << endl;
		if (node->right != nullptr)
			Print(node->right);
	}

};

int main()
{
	AVLTree* tree = new AVLTree();
	tree->Insert(2);
	tree->Insert(2);
	tree->Insert(3);
	tree->PrintTree();
	tree->Insert(4);
	tree->Insert(9);
	tree->PrintTree();
	tree->Remove(4);
	tree->PrintTree();
}