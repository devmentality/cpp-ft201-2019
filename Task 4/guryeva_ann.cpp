#include <iostream>

using namespace std;

class Node
{
	int key;
	int height;
	Node *left;
	Node *right;
};

class AvlTree {
	Node *head = NULL;

	int height(Node *N)
	{
		if (N == NULL)
			return 0;
		return N->height;
	}

	int max(int a, int b)
	{
		return (a > b) ? a : b;
	}

	Node* newNode(int key)
	{
		Node* node = new Node();
		node->key = key;
		node->left = NULL;
		node->right = NULL;
		node->height = 1;
		return(node);
	}

	Node *rightRotate(Node *y)
	{
		Node *x = y->left;
		Node *T2 = x->right;

		x->right = y;
		y->left = T2;

		y->height = max(height(y->left),
			height(y->right)) + 1;
		x->height = max(height(x->left),
			height(x->right)) + 1;

		return x;
	}

	Node *leftRotate(Node *x)
	{
		Node *y = x->right;
		Node *T2 = y->left;

 		y->left = x;
		x->right = T2;

		x->height = max(height(x->left),
			height(x->right)) + 1;
		y->height = max(height(y->left),
			height(y->right)) + 1;

		return y;
	}

	int getBalance(Node *N)
	{
		if (N == NULL)
			return 0;
		return height(N->left) -
			height(N->right);
	}

	Node* insert(Node* node, int key)
	{
		if (node == NULL)
			return(newNode(key));

		if (key < node->key)
			node->left = insert(node->left, key);
		else if (key > node->key)
			node->right = insert(node->right, key);
		else
			return node;

		node->height = 1 + max(height(node->left),
			height(node->right));

		return balancedTree(node);
	}

	Node * minValueNode(Node* node)
	{
		Node* current = node;

		while (current->left != NULL)
			current = current->left;

		return current;
	}

	Node* deleteNode(Node* root, int key)
	{

		if (root == NULL)
			return root;

		if (key < root->key)
			root->left = deleteNode(root->left, key);
		else if (key > root->key)
			root->right = deleteNode(root->right, key);
		else
		{
			if ((root->left == NULL) ||
				(root->right == NULL))
			{
				Node *temp = root->left ?
					root->left :
					root->right;

				if (temp == NULL)
				{
					temp = root;
					root = NULL;
				}
				else
					*root = *temp;
				free(temp);
			}
			else
			{
				Node* temp = minValueNode(root->right);

				root->key = temp->key;
				root->right = deleteNode(root->right,
					temp->key);
			}
		}

		if (root == NULL)
			return root;

		root->height = 1 + max(height(root->left),
			height(root->right));

		return balancedTree(root);
	}

	Node *balancedTree(Node *root) {
		int balance = getBalance(root);

		if (balance > 1 &&
			getBalance(root->left) >= 0)
			return rightRotate(root);

		if (balance > 1 &&
			getBalance(root->left) < 0)
		{
			root->left = leftRotate(root->left);
			return rightRotate(root);
		}

		if (balance < -1 &&
			getBalance(root->right) <= 0)
			return leftRotate(root);

		if (balance < -1 &&
			getBalance(root->right) > 0)
		{
			root->right = rightRotate(root->right);
			return leftRotate(root);
		}

		return root;
	}

	void printTree(Node *root, int level)
	{

		if (root != NULL)
		{
			printTree(root->right, level + 3);
			for (int i = 0; i < level; i++) cout << "   ";
			cout << root->key << endl;
			printTree(root->left, level + 3);
		}
	}

public:
	void insert(int value) {
		head = insert(head, value);
	}

	void deleteByValue(int value) {
		head = deleteNode(head, value);
	}

	void balance() {
		head = balancedTree(head);
	}

	void print() {
		printTree(head, 0);
	}
};


int main()
{
	AvlTree *tree = new AvlTree();
	tree->insert(9);
	tree->insert(5);
	tree->insert(10);
	tree->insert(0);
	tree->insert(6);

	cout << "********************\n";
	tree->print();

	tree->insert(11);
	tree->insert(-1);
	tree->insert(1);
	tree->insert(2);

	cout << "********************\n";
	tree->print();

	tree->deleteByValue(10);
	cout << "********************\n";
	tree->print();

	tree->deleteByValue(5);
	cout << "********************\n";
	tree->print();

	for (int i = 0; i < 10; i++) {
		tree->insert(i);
	}

	tree->deleteByValue(3);
	cout << "********************\n";
	tree->print();

	return 0;
}