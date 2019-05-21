#include <iostream>
using namespace std;

/*
 * Необходимо реализовать АВЛ-дерево (операции вставки-удаления-балансировки, печать всего дерева + тестовая программа).
*/

struct AVLTreeNode
{
	int value;
	unsigned char height;
	AVLTreeNode* left;
	AVLTreeNode* right;

	AVLTreeNode(int val)
	{
		value = val;
		left = right = NULL;
		height = 1;
	}

	int GetBalanceFactor()
	{
		unsigned char leftH = left ? left->height : 0;
		unsigned char rightH = right ? right->height : 0;

		return rightH - leftH;
	}

	void UpdateHeight()
	{
		unsigned char leftH = left ? left->height : 0;
		unsigned char rightH = right ? right->height : 0;

		height = leftH > rightH ? leftH : rightH + 1;
	}
};

class AVLTree
{
	AVLTreeNode* root;

	AVLTreeNode* BalanceNode(AVLTreeNode* p)
	{
		p->UpdateHeight();

		if (p->GetBalanceFactor() == 2) // right subtree height is bigger
		{
			if (p->right->GetBalanceFactor() < 0)
				p->right = RotateRight(p->right);
			return RotateLeft(p);
		}

		if (p->GetBalanceFactor() == -2) // left subtree height is bigger
		{
			if (p->left->GetBalanceFactor() > 0)
				p->left = RotateLeft(p->left);
			return RotateRight(p);
		}

		return p;
	}

	AVLTreeNode* RotateRight(AVLTreeNode* p)
	{
		auto q = p->left;
		p->left = q->right;
		q->right = p;

		p->UpdateHeight();
		q->UpdateHeight();

		return q;
	}

	AVLTreeNode* RotateLeft(AVLTreeNode* q)
	{
		auto p = q->right;
		q->right = p->left;
		p->left = q;

		q->UpdateHeight();
		p->UpdateHeight();

		return p;
	}

	AVLTreeNode* InsertRecursively(AVLTreeNode* p, AVLTreeNode* newNode)
	{
		if (p == NULL)
			return newNode;

		if (newNode->value < p->value)
			p->left = InsertRecursively(p->left, newNode);
		else
			p->right = InsertRecursively(p->right, newNode);

		return BalanceNode(p);
	}

	void PrintRecursively(AVLTreeNode* p)
	{
		if (p == NULL)
			return;

		PrintRecursively(p->left);
		cout << p->value << " ";
		PrintRecursively(p->right);
	}

	AVLTreeNode* Find(AVLTreeNode* p, int value)
	{
		if (p == NULL)
			return NULL;

		if (p->value == value)
			return p;

		if (p->value < value)
			return Find(p->right, value);

		return Find(p->left, value);
	}

	AVLTreeNode* GetMin(AVLTreeNode* p)
	{
		if (p->left == NULL)
			return p;
		return GetMin(p->left);
	}

	AVLTreeNode* EraseMin(AVLTreeNode* p)
	{
		if (p->left == NULL)
			return p->right;
		p->left = EraseMin(p->left);

		return BalanceNode(p);
	}

	AVLTreeNode* EraseRecursively(AVLTreeNode* p, int value)
	{
		if (p == NULL)
			return p;
		if (p->value < value)
			p->right = EraseRecursively(p->right, value);
		else if (p->value > value)
			p->left = EraseRecursively(p->left, value);
		else
		{
			auto l = p->left;
			auto r = p->right;
			delete p;

			if (r == NULL)
				return l;

			auto min = GetMin(r);
			min->left = l;
			min->right = EraseMin(r);
			return BalanceNode(min);
		}
		return BalanceNode(p);
	}
public:
	AVLTree()
	{
		root = NULL;
	}

	void Insert(int value)
	{
		auto newNode = new AVLTreeNode(value);
		root = InsertRecursively(root, newNode);
	}

	void Erase(int value)
	{
		root = EraseRecursively(root, value);
	}

	void Print()
	{
		PrintRecursively(root);
		cout << endl;
	}
};

void TestAddOneElement()
{
	cout << "TestAddOneElement" << endl;
	auto avl = new AVLTree();
	avl->Insert(100);

	avl->Print();
	cout << endl;
}

void TestAddAndEraseOneElement()
{
	cout << "TestAddAndEraseOneElement" << endl;
	auto avl = new AVLTree();
	avl->Insert(100);
	avl->Erase(100);

	avl->Print();
	cout << endl;
}

void TestEraseMissedElement()
{
	cout << "TestEraseMissedElement" << endl;
	auto avl = new AVLTree();
	avl->Erase(100);

	avl->Print();
	cout << endl;
}

void TestAddSeveralElements()
{
	cout << "TestAddSeveralElements" << endl;
	auto avl = new AVLTree();
	avl->Insert(10);
	avl->Insert(20);
	avl->Insert(-1);
	avl->Insert(10000);

	avl->Print();
	cout << endl;
}

void TestAddAndEraseSeveralElements()
{
	cout << "TestAddAndEraseSeveralElements" << endl;
	auto avl = new AVLTree();
	avl->Insert(10);
	avl->Insert(20);
	avl->Insert(-1);
	avl->Insert(10000);
	avl->Erase(20);
	avl->Erase(-1);

	avl->Print();
	cout << endl;
}

int main()
{
	TestAddOneElement();
	TestAddAndEraseOneElement();
	TestEraseMissedElement();
	TestAddSeveralElements();
	TestAddAndEraseSeveralElements();

	return 0;
}