#include <iostream>
#include <algorithm>
#include <vector>
using namespace std;

class BalancedTree
{
	struct TreeNode
	{
		TreeNode* left;
		TreeNode* right;
		int value;
		int height;

		int BalanceFactor() const
		{
			return (right ? right->height : 0) - (left ? left->height : 0);
		}

		void FixHeight()
		{
			height = max(right ? right->height : 0, left ? left->height : 0) + 1;
		}

		explicit TreeNode(const int value_)
		{
			value = value_;
			left = nullptr;
			right = nullptr;
			height = 1;
		}
	};

public:
	TreeNode* root;

	explicit BalancedTree(TreeNode* root_)
	{
		root = root_;
	}

	BalancedTree()
	{
		root = nullptr;
	}

	TreeNode* RotateRight(TreeNode* node)
	{
		auto another = node->left;
		node->left = another->right;
		another->right = node;
		node->FixHeight();
		another->FixHeight();
		return another;
	}

	TreeNode* RotateLeft(TreeNode* node)
	{
		auto another = node->right;
		node->right = another->left;
		another->left = node;
		node->FixHeight();
		another->FixHeight();
		return another;
	}

	int GetHeight(TreeNode* node)
	{
		return node ? node->height : 0;
	}

	TreeNode* Balance(TreeNode* node)
	{
		node->FixHeight();

		if (node->BalanceFactor() == 2)
		{
			if (GetHeight(node->right->left) > GetHeight(node->right->right))
			{
				node->right = RotateRight(node->right);
			}
			node = RotateLeft(node);
		}

		if (node->BalanceFactor() == -2)
		{
			if (GetHeight(node->left->right) > GetHeight(node->left->left))
			{
				node->left = RotateLeft(node->left);
			}
			node = RotateRight(node);
		}
		return node;
	}

	TreeNode* Insert(TreeNode* node, const int value)
	{
		if (!node)
		{
			return new TreeNode(value);
		}

		if (value < node->value)
		{
			node->left = Insert(node->left, value);
		}
		else
		{
			node->right = Insert(node->right, value);
		}
		return Balance(node);
	}

	TreeNode* Remove(TreeNode* node, const int value)
	{
		if (!node) return nullptr;
		if (node->value == value)
		{
			if (!node->right)
			{
				auto result = node->left;
				delete node;
				return result;
			}

			auto min = FindMin(node->right);
			auto right = RemoveMin(node->right);
			min->right = right;
			min->left = node->left;
			delete node;
			return Balance(min);
		}
		if (node->value >= value) node->left = Remove(node->left, value);
		else node->right = Remove(node->right, value);
		return Balance(node);
	}

	TreeNode* FindMin(TreeNode* node)
	{
		if (!node->left) return node;
		return FindMin(node->left);
	}

	TreeNode* RemoveMin(TreeNode* node)
	{
		if (!node->left) return node->right;
		node->left = RemoveMin(node->left);
		return Balance(node);
	}

	void Print(const string& prefix, TreeNode* node, bool isLeft) const
	{
		if (!node) return;
		cout << prefix;
		cout << "|--";
		cout << node->value << endl;
		Print(prefix + (isLeft ? "|   " : "    "), node->left, true);
		Print(prefix + (isLeft ? "|   " : "    "), node->right, false);
	}

	void Insert(const int value)
	{
		root = Insert(root, value);
	}

	void Print()
	{
		Print("", root, false);
	}

	void Remove(const int value)
	{
		root = Remove(root, value);
	}

	void InsertRange(const vector<int>& values)
	{
		for (auto x: values)
		{
			Insert(x);
		}
	}
};

void OneElement()
{
	cout << "OneElement" << endl;
	auto tree = new BalancedTree();
	tree->Insert(1);
	tree->Print();
}

void PrimitiveRemove()
{
	cout << "PrimitiveRemove" << endl;
	auto tree = new BalancedTree();
	tree->Insert(1);
	tree->Remove(1);
	tree->Print();
}

void ComplexRemove()
{
	cout << "ComplexRemove" << endl;
	auto tree = new BalancedTree();
	tree->InsertRange(vector<int> {1, 2, 3, 4, 5, 6, 7});
	tree->Remove(6);
	tree->Print();
}

void SimpleBalance()
{
	cout << "SimpleBalance" << endl;
	auto tree = new BalancedTree();
	tree->InsertRange(vector<int> {1, 2, 3, 4, 5, 6, 7});
	tree->Print();
}

void Insert()
{
	cout << "Insert" << endl;
	auto tree = new BalancedTree();
	tree->InsertRange(vector<int> {1, 2, 3, 8, 10});
	tree->Insert(5);
	tree->Print();
}


int main()
{
	OneElement();
	PrimitiveRemove();
	ComplexRemove();
	SimpleBalance();
	Insert();
	return 0;
}