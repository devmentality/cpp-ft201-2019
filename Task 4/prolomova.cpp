#include<iostream>
#include<string>
using namespace std;


class AVLTree
{
private:
	struct Node
	{
		int Value;
		int Height;
		Node* Left;
		Node* Right;
		Node(int value, Node* left = nullptr, Node* right = nullptr, int height = 1) {
			Value = value;
			Left = left;
			Right = right;
			Height = height;
		}
	};

	Node* root = nullptr;

	int countBalanceFactor(Node* p)
	{
		auto rHeight = (p->Right != nullptr) ? p->Right->Height : 0;
		auto lHeight = (p->Left != nullptr) ? p->Left->Height : 0;
		return rHeight - lHeight;
	}

	void RefreshHeight(Node* node)
	{
		auto rHeight = (node->Right != nullptr) ? node->Right->Height : 0;
		auto lHeight = (node->Left != nullptr) ? node->Left->Height : 0;

		node->Height = (lHeight > rHeight ? lHeight : rHeight) + 1;
	}

	Node* RotateRight(Node* node)
	{
		auto left = node->Left;
		node->Left = left->Right;
		left->Right = node;
		RefreshHeight(node);
		RefreshHeight(left);
		return left;
	}

	Node* RotateLeft(Node* node)
	{
		auto right = node->Right;
		node->Right = right->Left;
		right->Left = node;
		RefreshHeight(node);
		RefreshHeight(right);
		return right;
	}

	Node* Balance(Node* node)
	{
		RefreshHeight(node);
		auto bf = countBalanceFactor(node);
		if (bf != 2 && bf != 2) return node;
		if (bf == 2)
		{
			if (countBalanceFactor(node->Right) < 0)
				node->Right = RotateRight(node->Right);
			return RotateLeft(node);
		}
		else
		{
			if (countBalanceFactor(node->Left) > 0)
				node->Left = RotateLeft(node->Left);
			return RotateRight(node);
		}
	}

	Node* InsertNode(Node* node, int value)
	{
		if (node == nullptr) {
			root = new Node(value);
			return root;
		}
		if (value < node->Value)
			node->Left = InsertNode(node->Left, value);
		else
			node->Right = InsertNode(node->Right, value);
		return Balance(node);
	}

	Node* FindMinNode(Node* node)
	{
		if (node->Left) 
			return FindMinNode(node->Left);
		return node;
	}

	Node* RemoveMinNode(Node* node)
	{
		if (node->Left == nullptr)
			return node->Right;
		node->Left = RemoveMinNode(node->Left);
		return Balance(node);
	}

	Node* RemoveNode(Node* node, int value)
	{
		if (node == nullptr) return nullptr;
		if (value < node->Value)
			node->Left = RemoveNode(node->Left, value);
		else if (value > node->Value)
			node->Right = RemoveNode(node->Right, value);
		else 
		{
			auto left = node->Left;
			auto right = node->Right;
			delete node;
			if (right == nullptr) return left;
			auto min = FindMinNode(right);
			min->Right = RemoveMinNode(right);
			min->Left = left;
			return Balance(min);
		}
		return Balance(node);
	}

	void PrintTree(Node* node, int n = 0)
	{
		if (node == nullptr) return;
		string tab = "";
		for (int i = 0; i < n; i++)
			tab += "    ";
		cout << tab << node->Value << endl;
		PrintTree(node->Left, n + 1);
		PrintTree(node->Right, n + 1);
	}

public:
	AVLTree()
	{
		root = nullptr;
	}

	void Remove(int value)
	{
		cout << "Remove " << value << endl;
		root = RemoveNode(root, value);
	}

	void Add(int value)
	{
		cout << "Add " << value << endl;
		root = InsertNode(root, value);
	}

	void Print()
	{
		PrintTree(root);
		cout << endl;
	}
};

int main()
{
	cout << "Add items test" << endl;
	auto tree1 = new AVLTree();
	tree1->Add(1);
	tree1->Add(2);
	tree1->Add(3);
	tree1->Add(4);
	tree1->Add(5);
	tree1->Add(6);

	tree1->Print();
	cout << "------------------------------" << endl;

	cout << "Remove item test" << endl;
	auto tree2 = new AVLTree();
	tree2->Add(1);
	tree2->Add(2);
	tree2->Add(3);

	tree2->Print();

	tree2->Remove(1);

	tree2->Print();
	cout << "------------------------------" << endl;

	cout << "Remove all items test" << endl;
	auto tree3 = new AVLTree();
	tree3->Add(1);
	tree3->Add(2);
	tree3->Add(3);

	tree3->Print();

	tree3->Remove(1);
	tree3->Remove(2);
	tree3->Remove(3);

	tree3->Print();
	cout << "------------------------------" << endl;

	cout << "Remove nonexistent item test" << endl;
	auto tree4 = new AVLTree();
	tree4->Add(1);
	tree4->Add(2);
	tree4->Add(3);

	tree4->Print();

	tree4->Remove(4);

	tree4->Print();
	cout << "------------------------------" << endl;

	cout << "Add the same item test" << endl;
	auto tree5 = new AVLTree();
	tree5->Add(1);
	tree5->Add(1);

	tree5->Print();
	cout << "------------------------------" << endl;

	cout << "Remove root test" << endl;
	auto tree6 = new AVLTree();
	tree6->Add(1);
	tree6->Add(2);
	tree6->Add(3);
	tree6->Add(4);
	tree6->Add(5);
	tree6->Add(6);

	tree6->Print();

	tree6->Remove(4);

	tree6->Print();
	cout << "------------------------------" << endl;
}
