#include "pch.h"
#include <iostream>
using namespace std;

int const INT_SIZE = sizeof(int);

class StackArray {
private:
	int *start;
	int *last;
	int capacity;

	void alloc() {
		auto *new_ptr = (int*)malloc(capacity * 2);
		capacity *= 2;
		last = (last - start) + new_ptr;
		for (auto i = 0; i < last - start; i += INT_SIZE) {
			*(new_ptr + i) = *(start + i);
			delete (start + i);
		}
		start = new_ptr;
	}

public:
	int size;

	void clear() {
		for (auto i = 0; i < last - start; i += INT_SIZE)
			delete (start + i);
		start = (int*)malloc(capacity);
		last = start;
		size = 0;
	}

	int peek() {
		if (size == 0)
			throw invalid_argument("Стек пуст\n");
		return *last;
	}

	int pop() {
		if (size == 0)
			throw invalid_argument("Стек пуст\n");
		size--;
		auto element = *last;
		last -= INT_SIZE;
		return element;
	}

	void push(int element) {
		if (last - start > capacity - 2 * INT_SIZE)
			alloc();
		size++;
		last += INT_SIZE;
		*last = element;
	}

	StackArray() {
		size = 0;
		capacity = 100;
		start = (int*)malloc(capacity);
		last = start;
	}
};

void test_stack_array() {
	StackArray stack;
	stack.push(-1);
	stack.push(101);
	stack.push(999);
	cout << stack.pop() << "\n";
	cout << stack.peek() << "\n";
	cout << stack.pop() << "\n";
	stack.push(353);
	stack.push(4000);
	cout << stack.pop() << "\n";
	cout << stack.pop() << "\n";
	cout << stack.pop() << "\n";
	stack.clear();
}

class LinearArray {
private:
	int *start;
	int *last;
	int capacity;

	void alloc() {
		int *new_ptr = (int*)malloc(capacity * 2);
		capacity *= 2;
		last = new_ptr + (last - start);
		for (auto i = 0; i < last - start; i += INT_SIZE) {
			*(new_ptr + i) = *(start + i);
			delete (start + i);
		}
		start = new_ptr;
	}

public:
	int size;

	void clear() {
		for (auto i = 0; i < last - start; i += INT_SIZE)
			delete (start + i);
		last = start;
		size = 0;
	}

	int &operator[](int index) {
		if (last - start > capacity - 2 * INT_SIZE)
			alloc();
		if (index > size - 1 || index < 0)
			throw invalid_argument("Недопустимое значение индекса\n");
		return *(start + index * INT_SIZE);
	}

	LinearArray(int size) {
		this->size = size;
		capacity = size * 100;
		start = (int*)malloc(capacity);
		last = start;
	}
};

void test_linear_array() {
	auto l_arr = LinearArray(10);
	l_arr[0] = 16;
	l_arr[9] = -5;
	cout << l_arr[0] << "\n";
	cout << l_arr[9] << "\n";
	try
	{
		l_arr[10] = 1;
	}
	catch (invalid_argument exception)
	{
		cout << exception.what();
	}
	l_arr.clear();
}

struct Node {
public:
	int data;
	Node *next;

	Node() {
		auto *node = (struct Node*)malloc(sizeof(struct Node));
		node->next = nullptr;
	};
};

class LinkedListArray {
private:
	Node *head;
	Node *tail;

	void delete_nodes(Node* start) {
		if (start == nullptr)
			return;
		if (start->next != nullptr)
			delete_nodes(start->next);
		free(start);
	}

public:
	int size;

	void clear() {
		delete_nodes(head);
		head = nullptr;
		tail = nullptr;
		size = 0;
	}

	int pop_at_start() {
		auto first = head;
		if (first == nullptr)
			throw invalid_argument("Массив пуст\n");
		auto *previous = first;
		head = previous->next;
		auto value = previous->data;
		first = first->next;
		free(previous);
		size--;
		return value;
	}

	void push_back(int element) {
		size++;
		auto *new_node = new Node();
		new_node->data = element;
		if (tail != nullptr)
			tail->next = new_node;
		tail = new_node;
		if (head == nullptr)
			head = new_node;
	}

	LinkedListArray() {
		head = nullptr;
		tail = nullptr;
		size = 0;
	}
};

void test_linked_list_array() {
	LinkedListArray l_list;
	l_list.push_back(3);
	l_list.push_back(-1);
	l_list.push_back(666);
	cout << l_list.pop_at_start() << "\n";
	cout << l_list.pop_at_start() << "\n";
	cout << l_list.pop_at_start() << "\n";
}

int main()
{
	setlocale(0, "rus");
	cout << "Стековый массив" << "\n";
	test_stack_array();
	cout << "Линейный массив" << "\n";
	test_linear_array();
	cout << "Массив на связном списке" << "\n";
	test_linked_list_array();
	return 0;
}
