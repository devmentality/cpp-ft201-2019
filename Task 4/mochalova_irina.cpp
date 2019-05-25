#include <iostream>
#include <list>
#include <math.h>
#include<queue> 
#include <string>

template <typename TV>
struct Node
{
	TV value;
	Node<TV>* rightChild;
	Node<TV>* leftChild;
	Node<TV>* parent;

	Node(const TV& value, Node<TV>* right_child, Node<TV>* left_child, Node<TV>* parent)
		: value(value),
		rightChild(right_child),
		leftChild(left_child),
		parent(parent)
	{}

	int getTreeSize()
	{
		int size = 1;
		if (leftChild != nullptr)
			size += leftChild->getTreeSize();
		if (rightChild != nullptr)
			size += rightChild->getTreeSize();

		return size;
	}

	Node<TV>* getMaxRoot()
	{
		return parent == nullptr ? this : parent->getMaxRoot();
	}

	Node<TV>* farthestLeftChild()
	{
		return leftChild == nullptr ? this : leftChild->farthestLeftChild();
	}

	Node<TV>* smallRightRotation()
	{
		Node<TV>* left = leftChild;
		leftChild = left->rightChild;
		left->rightChild = this;
		left->parent = parent;
		parent = left;
		if (left->parent != nullptr)
		{
			if (left->value > left->parent->value)
				left->parent->rightChild = left;
			else
				left->parent->leftChild = left;
		}
		return left;
	}

	Node<TV>* smallLeftRotation()
	{
		Node<TV>* right = rightChild;
		rightChild = right->leftChild;
		right->parent = parent;
		parent = right;
		right->leftChild = this;
		if (right->parent != nullptr)
		{
			if (right->value > right->parent->value)
				right->parent->rightChild = right;
			else
				right->parent->leftChild = right;
		}
		return right;
	}
};

template <typename TV>
int getNodeHeight(Node<TV>* node, const int parentLevel)
{
	if (node == nullptr)
		return parentLevel;

	return std::max(
		getNodeHeight(node->leftChild, parentLevel + 1),
		getNodeHeight(node->rightChild, parentLevel + 1)
	);
}


template <typename TV>
int getNodeHeight(Node<TV>* node)
{
	return getNodeHeight(node, 0);
}


template <typename TV>
int getBalanceFactor(Node<TV>* node)
{
	return getNodeHeight(node->rightChild) - getNodeHeight(node->leftChild);
}

template <typename TV>
bool isNodeBalanced(Node<TV>* node)
{
	return node == nullptr ||
		((node->leftChild == nullptr || isNodeBalanced(node->leftChild)) &&
		(node->rightChild == nullptr || isNodeBalanced(node->rightChild)) &&
			std::abs(getBalanceFactor(node)) < 2);
}


template <typename TV>
Node<TV>* rebalanceTree(Node<TV>* node)
{
	if (getBalanceFactor(node) == 2)
	{
		if (getBalanceFactor(node->rightChild) < 0)
			node->rightChild = node->rightChild->smallRightRotation();
		return node->smallLeftRotation();
	}
	else if (getBalanceFactor(node) == -2)
	{
		if (getBalanceFactor(node->leftChild) > 0)
			node->leftChild = node->leftChild->smallLeftRotation();
		return node->smallRightRotation();
	}
	return node;
}

template <typename TV>
class BST
{
private:
	Node<TV>* root = nullptr;

	Node<TV>* searchNode(TV value)
	{
		Node<TV>* current = root;

		while (current != nullptr)
		{
			if (value == current->value)
				return current;

			if (value < current->value)
				current = current->leftChild;
			else
				current = current->rightChild;
		}
		return nullptr;
	}

public:

	Node<TV>* getRoot() const
	{
		return root;
	}

	bool add(TV value)
	{
		Node<TV>* prev = root;
		Node<TV>* current = root;

		std::list<Node<TV>*> nodes;

		while (current != nullptr)
		{
			nodes.push_back(current);
			if (value == current->value)
				return false;

			prev = current;
			if (value < current->value)
				current = current->leftChild;
			else
				current = current->rightChild;
		}

		Node<TV>* newNode = new Node<TV>(value, nullptr, nullptr, prev);

		if (prev == nullptr)
			root = newNode;
		else
		{
			if (value < prev->value)
				prev->leftChild = newNode;
			else
				prev->rightChild = newNode;
		}

		for (auto node : nodes)
			rebalanceTree(node);

		root = root->getMaxRoot();
		return true;
	}

	bool tryGet(TV value)
	{
		auto searchResult = searchNode(value);

		if (searchResult == nullptr)
			return false;

		return true;
	}

	bool remove(TV value)
	{
		Node<TV>* searchResult = searchNode(value);
		if (searchResult == nullptr)
			return false;

		Node<TV>* parent = searchResult->parent;

		std::list<Node<TV>*> nodes;
		Node<TV>* current = parent;
		while (current != nullptr)
		{
			nodes.push_front(current);
			current = current->parent;
		}

		const bool isLeft = parent != nullptr && searchResult->value < parent->value;
		if (searchResult->leftChild == nullptr && searchResult->rightChild == nullptr)
		{
			if (parent != nullptr)
			{
				if (isLeft)
					parent->leftChild = nullptr;
				else
					parent->rightChild = nullptr;

				for (auto node : nodes)
					rebalanceTree(node);
				root = root->getMaxRoot();
			}
			else
			{
				root = nullptr;
			}

			delete searchResult;
			return true;
		}

		Node<TV>* singleChild = nullptr;

		if (searchResult->leftChild != nullptr && searchResult->rightChild == nullptr)
			singleChild = searchResult->leftChild;
		if (searchResult->rightChild != nullptr && searchResult->leftChild == nullptr)
			singleChild = searchResult->rightChild;

		if (singleChild != nullptr)
		{
			if (parent != nullptr)
			{
				if (isLeft)
				{
					parent->leftChild = singleChild;
				}
				else
					parent->rightChild = singleChild;

				for (auto node : nodes)
					rebalanceTree(node);
				root = root->getMaxRoot();
			}
			else
			{
				root = singleChild;
			}

			singleChild->parent = parent;
			delete searchResult;
			return true;
		}

		Node<TV>* minRightNode = searchResult->rightChild->farthestLeftChild();

		TV minRightNodeValue = minRightNode->value;

		remove(minRightNodeValue);

		searchResult->value = minRightNodeValue;

		rebalanceTree(searchResult);
		for (auto node : nodes)
			rebalanceTree(node);
		root = root->getMaxRoot();

		return true;
	}

	void printTree()
	{
		int height = getTreeHeight();
		int size = pow(2, height);

		TV maxVal = getMaxNode(root);
		int n = log10(maxVal) + 1;

		std::queue<Node<TV>*> elements;
		elements.push(root);

		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < size / 2; j++)
				for (int q = 0; q < n; q++)
					std::cout << " ";
			for (int k = 0; k < pow(2, i); k++)
			{
				Node<TV>* e = elements.front();
				elements.pop();

				if (e != nullptr)
				{
					elements.push(e->leftChild);
					elements.push(e->rightChild);

					std::cout << e->value;
					for (int a = 0; a < n - log10(e->value) - 1; a++)
						std::cout << " ";
				}
				else
				{
					for (int a = 0; a < n; a++)
						std::cout << " ";
				}
				for (int j = 0; j < size - 1; j++)
					for (int q = 0; q < n; q++)
						std::cout << " ";
			}
			std::cout << std::endl;
			size = size / 2;
		}
	}
	
	TV getMaxNode(Node<TV>* node)
	{
		if (node->rightChild == nullptr)
			return node->value;
		return getMaxNode(node->rightChild);
	}

	int getTreeHeight()
	{
		return getNodeHeight(root);
	}

	int getTreeSize()
	{
		return root == nullptr ? 0 : root->getTreeSize();
	}

	bool isTreeBalanced()
	{
		return isNodeBalanced(root);
	}
};

struct TestCase
{
	int value;
	std::string command;
	bool expected;


	TestCase(int value, const std::string& command, bool expected)
		: value(value),
		command(command),
		expected(expected)
	{
	}
};

std::string Add = "add";
std::string Remove = "remove";
std::string Find = "find";

std::vector<TestCase> TestCases = {
	TestCase(4, Add, true),
	TestCase(4, Add, false),
	TestCase(4, Remove, true),
	TestCase(4, Remove, false),
	TestCase(4, Add, true),
	TestCase(1, Add, true),
	TestCase(6, Add, true),
	TestCase(5, Add, true),
	TestCase(7, Add, true),
	TestCase(4, Remove, true),
	TestCase(4, Find, false),
	TestCase(5, Find, true),
	TestCase(1, Find, true),
	TestCase(7, Find, true),

	TestCase(8, Add, true),
	TestCase(9, Add, true),
	TestCase(110, Add, true),
	TestCase(111, Add, true),
	TestCase(112, Add, true),
	TestCase(113, Add, true),
	TestCase(4, Add, true),
	TestCase(6, Remove, true),
	TestCase(110, Remove, true),

};

int main()
{
	BST<int> bst;
	int i = 0;
	bool result = false;
	for (auto case_ : TestCases)
	{
		++i;
		switch (case_.command[0])
		{
		case 'a':
			result = bst.add(case_.value);
			break;
		case 'f':
			result = bst.tryGet(case_.value);
			if (result != case_.expected)
				std::cout << "Test #" << i << ": value: " << case_.value << " not found " << std::endl;
			break;
		case 'r':
			result = bst.remove(case_.value);
			break;
		}
		std::cout << "Test #" << i << ": ";
		if (result == case_.expected)
			std::cout << "OK";
		else
			std::cout << "Error: expected " << case_.expected << std::endl;
		std::cout << std::endl;

		if (!bst.isTreeBalanced())
			std::cout << "\tTree is not balanced enough (size: " << bst.getTreeSize() << ", height: " << bst.getTreeHeight() <<
			")" << std::endl;
		else
			std::cout << "\tBalance: OK" << std::endl;
	}
	bst.printTree();
	return 0;
}
