#include "pch.h"
#include <iostream>

using namespace std;

struct TreeAVL
{
	int key;
	unsigned char height;
	TreeAVL* left;
	TreeAVL* right;

	TreeAVL(int k)
	{
		key = k;
		left = right = NULL;
		height = 1;
	}
};

unsigned char getHeight(TreeAVL* p)
{
	return p ? p->height : 0;
}

int getBalanceFactor(TreeAVL* p)
{
	return getHeight(p->right) - getHeight(p->left);
}

void updateHeight(TreeAVL* p)
{
	unsigned char hl = getHeight(p->left);
	unsigned char hr = getHeight(p->right);
	p->height = (hl > hr ? hl : hr) + 1;
}

TreeAVL* rotateRight(TreeAVL* p)
{
	TreeAVL* q = p->left;
	p->left = q->right;
	q->right = p;
	updateHeight(p);
	updateHeight(q);
	return q;
}

TreeAVL* rotateLeft(TreeAVL* q)
{
	TreeAVL* p = q->right;
	q->right = p->left;
	p->left = q;
	updateHeight(q);
	updateHeight(p);
	return p;
}

TreeAVL* balance(TreeAVL* p)
{
	updateHeight(p);
	if (getBalanceFactor(p) == 2)
	{
		if (getBalanceFactor(p->right) < 0)
			p->right = rotateRight(p->right);
		return rotateLeft(p);
	}
	if (getBalanceFactor(p) == -2)
	{
		if (getBalanceFactor(p->left) > 0)
			p->left = rotateLeft(p->left);
		return rotateRight(p);
	}
	return p;
}

TreeAVL* findMin(TreeAVL* p)
{
	return p->left ? findMin(p->left) : p;
}

TreeAVL* removeMin(TreeAVL* p)
{
	if (!p->left)
		return p->right;
	p->left = removeMin(p->left);
	return balance(p);
}

TreeAVL* remove(TreeAVL* p, int k)
{
	if (!p) return 0;
	if (k < p->key)
		p->left = remove(p->left, k);
	else if (k > p->key)
		p->right = remove(p->right, k);
	else
	{
		TreeAVL* q = p->left;
		TreeAVL* r = p->right;
		delete p;
		if (!r) return q;
		TreeAVL* min = findMin(r);
		min->right = removeMin(r);
		min->left = q;
		return balance(min);
	}
	return balance(p);
}

TreeAVL* add(TreeAVL* p, int k)
{
	if (!p) return new TreeAVL(k);
	if (k < p->key)
		p->left = add(p->left, k);
	else
		p->right = add(p->right, k);
	return balance(p);
}

static void print(TreeAVL* p)
{
	if (p == NULL)
		return;

	print(p->left);
	cout << p->key << " ";
	print(p->right);
}


void testAddOne() {
	cout << "ADD ONE ELEMENT" << endl;
	auto tree = new TreeAVL(5);
	print(tree);
}

void testAddMany() {
	cout << '\n' << "ADD MANY ELEMENTS" << endl;
	auto tree = new TreeAVL(5);
	add(tree, 3);
	add(tree, 6);
	add(tree, 2);
	add(tree, 1);
	print(tree);
}

void testAddAndRemoveMany() {
	cout << '\n' << "ADD AND REMOVE MANY ELEMENTS" << endl;
	auto tree = new TreeAVL(5);
	add(tree, 3);
	add(tree, 6);
	add(tree, 2);
	add(tree, 1);
	remove(tree, 3);
	remove(tree, 2);
	print(tree);
}

void testAddManyIncorrectRemove() {
	cout << '\n' << "INCORRECT REMOVE TEST" << endl;
	auto tree = new TreeAVL(5);
	add(tree, 3);
	add(tree, 6);
	remove(tree, 7);
	print(tree);
}


int main()
{
	testAddOne();
	testAddMany();
	testAddAndRemoveMany();
	testAddManyIncorrectRemove();
	cout << '\n';
}
