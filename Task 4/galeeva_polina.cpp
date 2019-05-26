#include<iostream>
#include<string>

using namespace std;

struct Node
{
	int key;
	unsigned char height;
	Node* left;
	Node* right;

	Node(int n) {
		key = n;
		left = NULL;
		right = NULL;
		height = 1;
	}
};

class AVLTree
{
private:
	Node * root = NULL;

	Node* Insert(Node* node, int key)
	{
		if (!node) return new Node(key);

		if (key < node->key)
			node->left = Insert(node->left, key);
		else
			node->right = Insert(node->right, key);

		return Balance(node);
	}

	Node* Balance(Node* node)
	{
		RecountHeight(node);
		if (BalanceFactor(node) == 2)
		{
			if (BalanceFactor(node->right) < 0)
				node->right = RotateRight(node->right);
			return RotateLeft(node);
		}
		if (BalanceFactor(node) == -2)
		{
			if (BalanceFactor(node->left) > 0)
				node->left = RotateLeft(node->left);
			return RotateRight(node);
		}
		return node;
	}

	Node* Remove(Node* node, int key)
	{
		if (!node)
			return 0;
		if (key < node->key)
			node->left = Remove(node->left, key);
		else if (key > node->key)
			node->right = Remove(node->right, key);
		else  
		{
			Node* left = node->left;
			Node* right = node->right;
			delete node;
			if (!right)
				return left;
			Node* min = FindMin(right);
			min->right = RemoveMin(right);
			min->left = left;
			return Balance(min);
		}
		return Balance(node);
	}

	Node* FindMin(Node* node)
	{
		return node->left ? FindMin(node->left) : node;
	}
	

	Node* RemoveMin(Node* node)
	{
		if (node->left == 0)
			return node->right;
		node->left = RemoveMin(node->left);
		return Balance(node);
	}

	Node* RotateRight(Node* node)
	{
		Node* left = node->left;
		node->left = left->right;
		left->right = node;
		RecountHeight(node);
		RecountHeight(left);
		return left;
	}

	Node* RotateLeft(Node* node)
	{
		Node* right = node->right;
		node->right = right->left;
		right->left = node;
		RecountHeight(node);
		RecountHeight(right);
		return right;
	}

	int Height(Node* node)
	{
		return node ? node->height : 0;
	}

	int BalanceFactor(Node* node)
	{
		return Height(node->right) - Height(node->left);
	}


	void RecountHeight(Node* node)
	{
		if (Height(node->left) > Height(node->right))
			node->height = Height(node->left);
		else
			node->height = Height(node->right);
		node->height += 1;
	}

	void Print(Node* node, int indent = 0) {
		if (node) {
			for (int i = 0; i < indent; i++)
				cout << " ";
			cout << node->key << "\n ";
			if (node->left)
				Print(node->left, indent + 4);
			if (node->right)
				Print(node->right, indent + 4);
		}
	}

public:

    AVLTree()
    {
        root = NULL;
    }
    
    void Print()
    {
        Print(root, 0);
    }

    void Insert(int k)
    {
        root = Insert(root, k);
    }

    void Remove(int k)
    {
        root = Remove(root, k);
    }

    void Balance()
    {
        root = Balance(root);
    }
};

int main()
{
	auto tree = new AVLTree();
	tree->Insert(2);
	tree->Insert(2);
	tree->Insert(2);
	tree->Insert(3);
	tree->Insert(1);
	tree->Print();
	tree->Remove(2);
	tree->Print();
}
