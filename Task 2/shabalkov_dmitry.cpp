#include<iostream>
using namespace std;

int GetClosiestPowOfTwo(int number)
{
	int pow = 0;
	int temp = 1;
	while (temp < number)
	{
		temp *= 2;
		pow++;
	}
	return pow;
}

int PowOfTwo(int pow)
{
	int result = 1;
	for (int i = 0; i < pow; i++)
		result *= 2;
	return result;
}

class Node
{
public:
	Node *left;
	Node *right;
	int level;
	bool isEmpty;
	bool isHaveChild;
	void *pool;
	
	Node(int level, void *pool)
	{
		this->level = level;
		isEmpty = true;
		isHaveChild = false;
		this->pool = pool;
	}
};

class BinaryTree
{
private:
	size_t _size;
	void *_pool;
	int _countOfFull;
	int _countOfHaveChild;
public:
	Node *header;

	BinaryTree(size_t size)
	{
		int pow = GetClosiestPowOfTwo(size);
		_size = PowOfTwo(pow);
		_pool = malloc(_size);
		_countOfFull = 0;
		_countOfHaveChild = 0;
		header = new Node(1, _pool);
		RecursiveAdd(header, pow);
	}

	void RecursiveAdd(Node* node, int maxlevel)
	{
		if (node->level == maxlevel)
			return;
		Add(node, node->level + 1);
		RecursiveAdd(node->left, maxlevel);
		RecursiveAdd(node->right, maxlevel);
	}

	void Add(Node* node, int level)
	{
		if (node->left != nullptr)
			node->left = new Node(level, (void*)(GetRefOnMemory(node) + (size_t)_pool));
		if (node->right != nullptr)
			node->right = new Node(level, (void*)(GetRefOnMemory(node) + (size_t)_pool));
	}

	void* Find(Node* node, int level)
	{
		if (!node->isEmpty)
			return (void*)nullptr;
		if (node->level == level)
		{
			node->isEmpty = false;
			_countOfFull += 1;
			return node->pool;
		}
		if (node->left != nullptr)
			Find(node->left, level);
		if (node->right != nullptr)
			Find(node->right, level);
		node->isHaveChild = true;
		_countOfHaveChild += 1;
	}

	void Remove(Node *node, int level)
	{
		if (node->level == level)
		{
			node->isEmpty = true;
			_countOfFull -= 1;
			return;
		}
		if (node->left != nullptr)
			Remove(node->left, level);
		if(node->right != nullptr)
			Remove(node->right, level);
		node->isHaveChild = false;
		_countOfHaveChild -= 1;
	}

	void CoutInfoForDump(Node *node)
	{
		cout << node << " " << node->isEmpty << " " << node->level << node->pool << " " << endl;
		if (node->left != nullptr)
			CoutInfoForDump(node->left);
		if (node->right != nullptr)
			CoutInfoForDump(node->right);
	}

	size_t GetRefOnMemory(Node * node)
	{
		int level = node->level;
		int pow = GetClosiestPowOfTwo(_size);
		return _size /((pow + 1) + _countOfFull * PowOfTwo(pow));
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
		return _tree->Find(_tree->header, size);
	}

	void Free(size_t size)
	{
		_tree->Remove(_tree->header, size);
	}

	void Dump()
	{
		_tree->CoutInfoForDump(_tree->header);
	}
};
int main()
{
	
}