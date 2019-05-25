#include <iostream>

class AVLNode
{
	int height;

	static int Height(AVLNode* n)
	{
		return n ? n->height : 0;
	}

public:
	int key;
	AVLNode* left;
	AVLNode* right;

	AVLNode(int k)
	{
		key = k;
		height = 1; // высота поддерева с корнем
		left = right = nullptr;
	}

	int GetBalanceFactor()
	{
		return Height(right) - Height(left);
	}

	void CorrectHeight() // обе высоты в дочерних должны быть корректными
	{
		auto left_height = Height(left);
		auto right_height = Height(right);
		height = (left_height > right_height ? left_height : right_height) + 1;
	}

};

class AVLNodeTools
{
public:
	static AVLNode* InsertRecursively(AVLNode* currentNode, int key)
	{
		if (!currentNode)		
			return  new AVLNode(key);
		
		if (key < currentNode->key)
			currentNode->left = InsertRecursively(currentNode->left, key);
		else
			currentNode->right = InsertRecursively(currentNode->right, key);

		return  BalanceNode(currentNode);
	}

	static AVLNode* GetMinNode(AVLNode* node)
	{
		return  node->left ? GetMinNode(node->left) : node;
	}

	static AVLNode* RemoveMinNode(AVLNode* node)
	{
		if (!node->left)
			return node->right;
		node->left = RemoveMinNode(node->left);
		return BalanceNode(node);
	}

	static AVLNode* RemoveRecursively(AVLNode* currentNode, int key)
	{
		if (!currentNode)
			return nullptr;

		if (key < currentNode->key)
			currentNode->left = RemoveRecursively(currentNode->left, key);
		else if (key > currentNode->key)
			currentNode->right = RemoveRecursively(currentNode->right, key);
		else
		{
			auto current_left = currentNode->left;
			auto current_right = currentNode->right;
			delete currentNode;
			if (!current_right) return current_left;
			auto min = GetMinNode(current_right);
			min->right = RemoveMinNode(current_right);
			min->left = current_left;
			return BalanceNode(min);
		}
		return BalanceNode(currentNode);
	}

	static AVLNode* RotateRight(AVLNode* node)
	{
		auto newNode = node->left;
		node->left = newNode->right;
		newNode->right = node;
		node->CorrectHeight();
		newNode->CorrectHeight();
		return newNode;
	}

	static AVLNode* RotateLeft(AVLNode* node)
	{
		auto newNode = node->right;
		node->right = newNode->left;
		newNode->left = node;
		node->CorrectHeight();
		newNode->CorrectHeight();
		return newNode;
	}

	static AVLNode* BalanceNode(AVLNode* node)
	{
		node->CorrectHeight();

		auto balance_factor = node->GetBalanceFactor();

		if (balance_factor == 2)
		{
			if (node->right->GetBalanceFactor() < 0)
				node->right = RotateRight(node->right);
			return  RotateLeft(node);
		}
		if (balance_factor == -2)
		{
			if (node->left->GetBalanceFactor() > 0)
				node->left = RotateLeft(node->left);
			return  RotateRight(node);
		}
		return node;
	}

	static void PrintTreeLevel(AVLNode* node, int level)
	{
		if (!node)
			return;
		PrintTreeLevel(node->right, level + 1);
		for (int i = 0; i < level; i++)
			std::cout << "   ";
		std::cout << node->key << "" << std::endl;
		PrintTreeLevel(node->left, level + 1);
	}
};


class AVLTree
{
	AVLNode* root;
	

public:
	AVLTree()
	{
		root = nullptr;
	}

	void Insert(int key)
	{
		root = AVLNodeTools::InsertRecursively(root, key);
	}

	void Delete(int key)
	{
		root = AVLNodeTools::RemoveRecursively(root, key);
	}

	void Print()
	{
		// дерево выводится слева направо, сверху -- правое поддерево, снизу левое
		AVLNodeTools::PrintTreeLevel(root, 0);
		std::cout << "------------------" << std::endl;

	}
};


void test()
{
	auto tree = AVLTree();

	std::cout << "Build the tree" << std::endl;
	std::cout << "Add 1" << std::endl;
	tree.Insert(1);
	tree.Print();
	std::cout << "Add 3" << std::endl;
	tree.Insert(3);
	tree.Print();
	std::cout << "Add 7" << std::endl;
	tree.Insert(7);
	tree.Print();
	std::cout << "Add 5" << std::endl;
	tree.Insert(5);
	tree.Print();
	std::cout << "Add 0" << std::endl;
	tree.Insert(0);
	tree.Print();
	std::cout << "Add 6" << std::endl;
	tree.Insert(6);
	tree.Print();
	std::cout << "Add 8" << std::endl;
	tree.Insert(8);
	tree.Print();
	std::cout << std::endl;

	std::cout << "Delete the root 3" << std::endl;
	tree.Delete(3);
	tree.Print();
	std::cout << std::endl;

	std::cout << "Delete the node 7" << std::endl;
	tree.Delete(7);
	tree.Print();
	std::cout << std::endl;

	std::cout << "Delete the leaf 6" << std::endl;	
	tree.Delete(6);
	tree.Print();
	std::cout << std::endl;

	std::cout << "Balance after deleting leaf 8" << std::endl;
	tree.Delete(8);
	tree.Print();
	std::cout << std::endl;
}


int main()
{
	// операции вставки-удаления-балансировки, печать всего дерева, тестовая программа
	test();
	return 0;
}