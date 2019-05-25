#include "pch.h"
#include <iostream>
#include <stack>

using namespace std;

class LinearAloc
{
private:
	int size;
	void *start;
	int offset;

public:
	LinearAloc(int _size = 1024)
	{
		size = _size;
		start = malloc(size);
		offset = 0;
	}

	void* Allocate(int _size) {
		auto newOffset = offset + _size;
		auto flag = false;
		while (size < newOffset)
		{
			size = size * 2;
			flag = true;
		}
		if (flag)
		{
			void *newStart = malloc(size);
			memcpy(newStart, start, offset);
			start = newStart;
		}
		void* result = (size_t*)start + offset;
		offset = newOffset;
		return result;
	}

	void Release()
	{
		start = malloc(size);
		offset = 0;
	}

	void Dump()
	{
		cout << "Size: " << size << endl;
		cout << "Start: " << start << endl;
		cout << "Offset: " << offset << endl;
		cout << endl;
	}
};

class Stack
{
public:
	int offset;
	void *start;
	Stack *prev;
	std::stack<int> offsets;

	Stack(int _offset = 0, void* _start = nullptr, Stack *_prev = nullptr)
	{
		prev = _prev;
		offset = _offset;
		start = _start;
	}
};

class StackAloc
{
public:
	int size;
	Stack stack;

	StackAloc(int _size = 1024)
	{
		size = _size;
		stack = Stack(0, malloc(size), nullptr);
		stack.offsets.push(0);
	}

	void* Push(int _size)
	{
		auto newOffset = stack.offset + _size;
		auto flag = false;
		while (size < newOffset)
		{
			size = size * 2;
			flag = true;
		}
		if (flag)
		{
			auto newStack = Stack(0, malloc(size), &stack);
			stack = newStack;
			stack.offsets.push(0);
		}
		auto result = (size_t*)stack.start + stack.offset;
		stack.offsets.push(stack.offset);
		stack.offset += _size;
		return result;
	}

	void Pop()
	{
		auto offset = stack.offsets.top();

		stack.offsets.pop();
		if (offset == 0 && stack.prev != nullptr)
		{
			stack = *stack.prev;
			return;
		}
		stack.offset = offset;
	}

	void Dump()
	{
		cout << "Size: " << size << endl;
		cout << "Start: " << stack.start << endl;
		cout << "Offset: " << stack.offset << endl;
		cout << endl;
	}
};

class ListNode
{
public:
	int value;
	ListNode* next;

	ListNode(int _value, ListNode* _next)
	{
		value = _value;
		next = _next;
	}
};

class ListAloc
{
public:
	ListNode* head = nullptr;
	ListNode* tail = nullptr;

	void Add(int value)
	{
		if (head == nullptr)
		{
			head = new ListNode(value, nullptr);
			tail = head;
		}
		else
		{
			tail->next = new ListNode(value, nullptr);
			tail = tail->next;
		}
	}

	void Remove(int _value)
	{
		if (head == nullptr)
			return;
		ListNode* current = head;
		ListNode* prev = head;
		while (true)
		{
			if (current->value == _value)
			{
				if (current == head)
					head = current->next;
				else
					prev->next = current->next;
				break;
			}
			prev = current;
			current = current->next;
		}
	}

	void Dump()
	{
		cout << "Values: ";
		ListNode* curr = head;
		while (curr != nullptr)
		{
			cout << curr->value << " ";
			curr = curr->next;
		}
		cout << endl << endl;
	}
};


int main()
{
	cout << "Test LinearAloc:" << endl;
	cout << endl;
	auto aloc1 = LinearAloc();
	aloc1.Dump();
	auto aloc2 = LinearAloc(42);
	aloc2.Dump();
	aloc2.Allocate(1000);
	aloc2.Dump();
	aloc2.Release();
	aloc2.Dump();
	cout << endl;

	cout << "Test StackAloc:" << endl;
	cout << endl;
	auto aloc3 = StackAloc();
	aloc3.Dump();
	auto aloc4 = StackAloc(42);
	aloc4.Dump();
	aloc4.Push(30);
	aloc4.Dump();
	aloc4.Push(130);
	aloc4.Dump();
	cout << endl;

	cout << "Test ListAloc:" << endl;
	cout << endl;
	auto arr = new ListAloc();
	arr->Add(3);
	arr->Add(6);
	arr->Dump();
	arr->Remove(6);
	arr->Dump();
	arr->Add(2);
	arr->Dump();

}