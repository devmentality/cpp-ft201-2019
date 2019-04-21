#include<iostream>
using namespace std;

int PowOfTwo(int pow)
{
	int res = 1;
	for (int i = 0; i < pow; i++)
		res *= 2;
	return res;
}


int GetClosePowOfTwoToNumber(int n)
{
	int i = 1;
	int current = 0;
	while (i < n)
	{
		i *= 2;
		current++;
	}
	return current;
}



class BinaryTreeNode
{
public:
	BinaryTreeNode *left;
	BinaryTreeNode *right;

	bool isEmpty;
	bool isHaveChild;
	int currentLevel;
	void *prtheader;

	BinaryTreeNode(int level, void *_prtheader)
	{
		currentLevel = level;
		isEmpty = true;
		isHaveChild = false;
		this->prtheader = _prtheader;
	}
};

class BinaryTree
{
private:
	size_t _size;
	void *_prtheader;
	int _countOfFull;
	int _countOfHasChild;

	void* Find(BinaryTreeNode* node, int level)
	{
		if (!node->isEmpty)
			return (void*)nullptr;

		if (node->currentLevel == level)
		{
			node->isEmpty = false;
			_countOfFull += 1;
			return node->prtheader;
		}

		if (node->left != nullptr)
			Find(node->left, level);
		if (node->right != nullptr)
			Find(node->right, level);

		node->isHaveChild = true;
		_countOfHasChild += 1;
	}


	void RecursiveAdd(BinaryTreeNode* node, int maxlevel)
	{
		if (node->currentLevel == maxlevel)
			return;

		Add(node, node->currentLevel + 1);
		RecursiveAdd(node->right, maxlevel);
		RecursiveAdd(node->left, maxlevel);
	}

public:
	BinaryTreeNode *header;

	BinaryTree(size_t size)
	{
		int pow = GetClosePowOfTwoToNumber(size);
		_size = PowOfTwo(pow);
		_countOfFull = 0;
		_countOfHasChild = 0;
		_prtheader = malloc(_size);

		header = new BinaryTreeNode(1, _prtheader);
		RecursiveAdd(header, pow);
	}

	void Add(BinaryTreeNode* node, int level)
	{
		if (node->right != nullptr)
			node->right = new BinaryTreeNode(level, (void*)(GetRefNodeHeader(node) + (size_t)_prtheader));
		if (node->left != nullptr)
			node->left = new BinaryTreeNode(level, (void*)(GetRefNodeHeader(node) + (size_t)_prtheader));
	}

	void* Find(int level)
	{
		return Find(header, level);
	}

	void Remove(BinaryTreeNode *node, int level)
	{
		if (node->currentLevel == level)
		{
			node->isEmpty = true;
			_countOfFull -= 1;
			return;
		}
		if (node->left != nullptr)
			Remove(node->left, level);
		if (node->right != nullptr)
			Remove(node->right, level);

		node->isHaveChild = false;
		_countOfHasChild -= 1;
	}

	void Dump(BinaryTreeNode *node)
	{
		cout << node << " " << node->isEmpty << " " << "Level: " << node->currentLevel << " Header: " << node->prtheader << " " << endl;
		if (node->left != nullptr)
			Dump(node->left);
		if (node->right != nullptr)
			Dump(node->right);
	}

	size_t GetRefNodeHeader(BinaryTreeNode * node)
	{
		int pow = GetClosePowOfTwoToNumber(_size);
		return _size / ((pow + 1) + _countOfFull * PowOfTwo(pow));
	}
};

class Allocator
{
private:
	BinaryTree * _tree;
	size_t _size;
public:
	Allocator(size_t size)
	{
		_size = size;
		_tree = new BinaryTree(_size);
	}

	void* Allocate(size_t size)
	{
		return _tree->Find(size);
	}

	void Free(size_t size)
	{
		_tree->Remove(_tree->header, size);
	}

	void Dump()
	{
		_tree->Dump(_tree->header);
	}
};
int main()
{

}