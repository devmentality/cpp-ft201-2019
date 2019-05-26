#include "pch.h"
#include <iostream>

class Node
{
public:
	int Value;
	int Height;
	Node* Left;
	Node* Right;
	
	Node(int k) 
	{	
		Value = k; 
		Left = nullptr;
		Right = 0; 
		Height = 1; 
	}

	static int GetHeight(Node* node)
	{
		if (node == nullptr)
			return 0;
		return node->Height;
	}

	int ChildsHeightDifference()
	{
		return GetHeight(Right) - GetHeight(Left);
	}

	void CalculateHeightFromChilds()
	{
		Height = GetHeight(Left) > GetHeight(Left) ? GetHeight(Left) : GetHeight(Left) + 1;
	}

	Node* RightRotate()
	{
		Node* q = Left;
		Left = q->Right;
		q->Right = this;
		CalculateHeightFromChilds();
		q->CalculateHeightFromChilds();
		return q;
	}

	Node* LeftRotate()
	{
		Node* p = Right;
		Right = p->Left;
		p->Left = this;
		CalculateHeightFromChilds();
		p->CalculateHeightFromChilds();
		return p;
	}

	Node* BalanceNode()
	{
		CalculateHeightFromChilds();
		if (ChildsHeightDifference() == 2)
		{
			if (Right != nullptr && Right->ChildsHeightDifference() < 0)
				Right = Right->RightRotate();
			return LeftRotate();
		}
		if (ChildsHeightDifference() == -2)
		{
			if (Left != nullptr && Left->ChildsHeightDifference() > 0)
				Left = Left->LeftRotate();
			return RightRotate();
		}
		return this;
	}

	Node* InsertNewNode(int value)
	{
		if (value < Value)
		{
			if (Left == nullptr)
				Left = new Node(value);
			else
				Left = Left->InsertNewNode(value);
		}
		else
		{
			if (Right == nullptr)
				Right = new Node(value);
			else
				Right = Right->InsertNewNode(value);
		}
		CalculateHeightFromChilds();
		return BalanceNode();
	}

	Node* FindNodeWithMinValue()
	{
		if (Left == nullptr)
			return this;
		return Left->FindNodeWithMinValue();
	}

	Node* RemoveNodeWithMinValue()
	{
		if (Left == nullptr)
			return Right;
		Left = Left->RemoveNodeWithMinValue();
		return BalanceNode();
	}

	Node* RemoveNode(int value)
	{
		if (value < Value)
			Left = Left->RemoveNode(value);
		else if (value > Value)
			Right = Right->RemoveNode(value);
		else
		{
			Node* q = Left;
			Node* r = Right;
			
			if (!r) return q;
			Node* min = r->FindNodeWithMinValue();
			min->Right = r->RemoveNodeWithMinValue();
			min->Left = q;
			delete this;
			return min->BalanceNode();
		}
		return BalanceNode();
	}
};

class Tree
{
private:
	Node* root;

public:

	Tree(int value)
	{
		root = new Node(value);
	}

	Tree()
	{

	}

	void Insert(int value)
	{
		if (root == nullptr)
			root = new Node(value);
		else
			root = root->InsertNewNode(value);
	}

	void Remove(int value)
	{
		root = root->RemoveNode(value);
	}

	void Print()
	{
		std::cout << "Root: ";
		if (root == nullptr)
			std::cout << "NULL" << std::endl;
		else
			Print(root, 1);
	}


private:
	void Print(Node* node, int i)
	{
		std::cout << node->Value << std::endl;
		for (int j = 0; j < i; j++)
			std::cout << "    ";
		std::cout << "Left: ";
		if (node->Left != nullptr)
			Print(node->Left, i + 1);
		else
			std::cout << "NULL" << std::endl;
		for (int j = 0; j < i; j++)
			std::cout << "    ";
		std::cout << "Right: ";
		if (node->Right != nullptr)
			Print(node->Right, i + 1);
		else
			std::cout << "NULL" << std::endl;
	}
};

int main()
{
	Tree* tree = new Tree();
	std::cout << "Init empty tree" << std::endl;
	tree->Print();
	std::cout << "----------------------------------------" << std::endl;
	tree->Insert(1);
	tree->Insert(10);
	tree->Insert(5);
	tree->Insert(20);
	tree->Insert(25);
	std::cout << "Added 1, 10, 5, 20, 25" << std::endl;
	tree->Print();
	std::cout << "----------------------------------------" << std::endl;
	tree->Remove(5);
	std::cout << "Removed 5" << std::endl;
	tree->Print();
	std::cout << "----------------------------------------" << std::endl;
}