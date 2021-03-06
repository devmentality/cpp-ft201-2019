#include "pch.h"
#include <iostream>
#include <stack>
#include <string>
using namespace std;

class LinearAllocator
{
private:
	int size;

public:
	int offset;
	void *prtheader;

    LinearAllocator(int _size)
	{
		size = _size;
		offset = 0;
		prtheader = malloc(size);
	}

	LinearAllocator()
	{
		size = 1024 * 1024;
		offset = 0;
		prtheader = malloc(size);
	}

	void* Alloc(int needSize)
	{
		if (needSize + offset >= size)
		{
			void *newPrtheader = malloc(size * 2);
			memcpy(newPrtheader, prtheader, offset);
			prtheader = newPrtheader;
			cout << "Size was doubled" << endl;
			size = size * 2;
		}
		int _offset = offset;
		offset += needSize;
		return (size_t*)prtheader + _offset;
	}

	void Dealloc(int needSize)
	{
		offset = offset - needSize > 0 ? offset - needSize : 0;
	}
};

class MemoryStack
{
public:
	int offset;
	void *prtheader;
	MemoryStack *previous;
	std::stack<int> offsets;

	MemoryStack()
	{
		offset = 0;
	}

	MemoryStack(int _offset, void* _prtheader, MemoryStack *_previous)
	{
		previous = _previous;
		offset = _offset;
		prtheader = _prtheader;
	}
};

class StackAllocator
{
private:
	int size;
public:
	MemoryStack stack;


	StackAllocator(int _size)
	{
		size = _size;
		stack = MemoryStack(0, malloc(size), nullptr);
		stack.offsets.push(0);
	}

	StackAllocator()
	{
		size = 1024 * 1024;
		stack = MemoryStack(0, malloc(size), nullptr);
		stack.offsets.push(0);
	}

	void* Push(int needSize)
	{
		if (needSize + stack.offset >= size)
		{
			MemoryStack newStack = MemoryStack(0, malloc(size * 2), &stack);
			size = size * 2;
			cout << "Size was doubled" << endl;
			cout << "Refreshed stack" << endl;
			stack = newStack;
			stack.offsets.push(0);
		}
		cout << "Pushing" << endl;
		int _offset = stack.offset;
		stack.offset += needSize;
		stack.offsets.push(_offset);
		cout << stack.offset << endl;
		return (size_t*)stack.prtheader + _offset;
	}

	void Pop()
	{
		cout << "Poping" << endl;
		int offset = stack.offsets.top();
		cout << "Poped" << endl;
		stack.offsets.pop();
		if (offset == 0)
		{
			if (stack.previous != nullptr)
			{
				stack = *stack.previous;
				cout << "Refreshed stack" << endl;
				cout << offset << endl;
				cout << stack.prtheader << endl;
				return;
			}
		}
		stack.offset = offset;
		cout << offset << endl;
		cout << stack.prtheader << endl;
	}
};

class ListNode
{
public:
	ListNode *next = nullptr;
	int value;
};

class List
{
private:
	ListNode *head;
public:
	List()
	{
		head = nullptr;
	}

	void Add(int value)
	{
		if (head == nullptr)
		{
			head = new ListNode();
			head->value = value;
			return;
		}

		ListNode *curNode = head;
		while (curNode->next != nullptr)
		{
			curNode = curNode->next;
		}

		curNode->next = new ListNode();
		curNode->next->value = value;
	}

	void Remove(int value)
	{
		ListNode *current = head;
		if (current->value == value)
		{
			head = current->next;
			return;
		}

		while (current->next->value != value)
			current = current->next;

		if (current->next->next != nullptr)
			current->next = current->next->next;
		else
			current->next = nullptr;
	}
};



int main()
{

}
