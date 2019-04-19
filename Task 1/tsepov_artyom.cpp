#pragma comment(linker, "/STACK:16777216")
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <vector>
#include <algorithm>
#include <map>
#include <set>
#include <string>
#include <queue>
#include <unordered_map>
#include <cmath>
#include <iterator>
#include <stack>
#include <unordered_set>
#include <bitset>
#include <climits>

using namespace std;

class LinearAllocator
{
private:
	size_t size;
	size_t allocated;
	void * memory;
public:
	LinearAllocator(size_t _size)
	{
		memory = malloc(_size);
		size = _size;
		allocated = 0;
	}

	void *Alloc(size_t _size)
	{
		if (allocated + _size > size)
		{
			throw "LOL, no memory";
		}
		size_t *start = (size_t*)memory + allocated;
		allocated += _size;
		return start;
	}

	void Free()
	{
		allocated = 0;
	}

	~LinearAllocator()
	{
		free(memory);
	}
};

void Test_LinearAllocator()
{
	cout << "---------------------------------------\n";
	cout << "Test Linear Allocator\n";
	LinearAllocator allocator(25 * sizeof(size_t));
	size_t value = 10;

	try
	{
		size_t *too_big = (size_t*)allocator.Alloc(200);
	}
	catch (char* a)
	{
		cout << a << "\n";
	}

	size_t *test_array = (size_t*)allocator.Alloc(10);

	for (int i = 0; i < value; i++)
		test_array[i] = i;

	size_t *test_array2 = (size_t*)allocator.Alloc(10);

	for (int i = 0; i < value; i++)
		test_array2[i] = 1000 + rand() % 100;

	for (int i = 0; i < value; i++)
		cout << test_array[i] << " ";
	cout << "\n";

	for (int i = 0; i < value; i++)       //Same value to next for
		cout << test_array2[i] << " ";
	cout << "\n";

	for (int i = value; i < 2 * value; i++) //Same value to previuos for
		cout << test_array[i] << " ";
	cout << "\n";


	allocator.Free();


	size_t *test_array3 = (size_t*)allocator.Alloc(10);

	for (int i = 0; i < value; i++)
		test_array3[i] = rand() % 100;


	for (int i = 0; i < value; i++)		//Same value to next for
		cout << test_array[i] << " ";
	cout << "\n";

	for (int i = 0; i < value; i++)
		cout << test_array3[i] << " ";	//Same value to previuos for
	cout << "\n";

	cout << "End Test Linear Allocator\n";
	cout << "---------------------------------------\n\n";
}



class StackAllocator
{
private:
	void * memory;
	size_t size;
	size_t offset;
public:
	StackAllocator(size_t _size)
	{
		size = _size;
		memory = malloc(_size);
		offset = 0;
	}

	void *Alloc(size_t _size)
	{
		if (size_t(offset) + _size > size)
		{
			throw "LOL, not enough memory";
		}

		void * start = (size_t*)memory + offset;
		offset += _size;
		return start;
	}

	void Free(void *ptr)
	{
		size_t current = (size_t)ptr;
		if (current > offset * sizeof(size_t) + (size_t)memory || current < (size_t)memory)
			throw "Incorrect free operation";
		offset = (current - (size_t)memory) / sizeof(size_t);
	}

	void Free()
	{
		offset = 0;
	}

	~StackAllocator()
	{
		free(memory);
	}
};


void Test_StackAllocator()
{
	cout << "---------------------------------------\n";
	cout << "Test Stack Allocator\n";
	StackAllocator allocator(25 * sizeof(size_t));	//100 bytes if size_t = 4 bytes
	size_t *test_array1 = (size_t*)allocator.Alloc(10);  //10 size_t = 40 bytes

	for (int i = 0; i < 10; i++)
		test_array1[i] = i + 5;

	size_t *test_array2 = (size_t*)allocator.Alloc(10);

	for (int i = 0; i < 10; i++)
		test_array2[i] = i + 105;

	for (int i = 0; i < 10; i++)							//10 numbers 100
		cout << test_array2[i] - test_array1[i] << " ";
	cout << "\n";

	allocator.Free(test_array2 + 5);

	size_t *test_array3 = (size_t*)allocator.Alloc(10);
	for (int i = 0; i < 10; i++)
		test_array3[i] = i + 205;

	for (int i = 0; i < 10; i++)
		cout << test_array1[i] << " ";
	cout << "\n";

	for (int i = 0; i < 10; i++)		//5 second numbers equal to 5 first numbers in next for
		cout << test_array2[i] << " ";
	cout << "\n";

	for (int i = 0; i < 10; i++)
		cout << test_array3[i] << " ";
	cout << "\n";

	allocator.Free();

	cout << "End Test Stack Allocator\n";
	cout << "---------------------------------------\n\n";
}


class Node
{
public:
	int _data;
	Node * next;
	Node(int data)
	{
		_data = data;
	}
};

class Linkedlist
{
public:
	Node * head;
	Node * tail;

	void Add(int value)
	{
		Node * node = new Node(value);
		if (head == NULL)	// if List is empty
			head = node;
		else                // if not empty
			tail->next = node;

		tail = node;
	}

	void Remove(int value)
	{
		Node * start = head;
		Node * pre = NULL;
		while (start != NULL && start->_data != value)	//find Node with value
		{
			pre = start;
			start = start->next;
		}

		if (start == NULL)		//No such element
			return;

		if (pre == NULL)
		{
			head = start->next;
			if (start->next == NULL)
				tail = NULL;
		}
		else
		{
			pre->next = start->next;
			if (pre->next == NULL)
				tail = pre;
		}
	}

	void Show(Node * start)
	{
		while (start != NULL)
		{
			cout << start->_data << " ";
			start = start->next;
		}
		cout << endl;
	}
};

void Test_LinkedList()
{
	cout << "---------------------------------------\n";
	cout << "Test LinkedList\n";

	Linkedlist * linkedList = new Linkedlist();
	for (int i = 0; i < 10; i++)
		(*linkedList).Add(i + 1);
	(*linkedList).Show(linkedList->head);

	(*linkedList).Remove(5);
	(*linkedList).Show(linkedList->head);

	(*linkedList).Remove(7);
	(*linkedList).Show(linkedList->head);

	(*linkedList).Remove(7);				//Correct remove(if no such element)
	(*linkedList).Show(linkedList->head);

	(*linkedList).Remove(1);				//Correct remove first element
	(*linkedList).Show(linkedList->head);

	(*linkedList).Remove(10);				//Correct remove last element
	(*linkedList).Show(linkedList->head);

	cout << "End Test LinkedList\n";
	cout << "---------------------------------------\n\n";
}

int main()
{
	Test_LinearAllocator();
	Test_StackAllocator();
	Test_LinkedList();
	return 0;
}
