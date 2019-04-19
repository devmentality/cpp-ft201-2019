#include<iostream>
using namespace std;

class Linear {
private:

	int *startPtr;
	int offset;
	size_t const int_size = sizeof(int);

	void resize() {
		SIZE *= 2;
		int *ptr = (int*)malloc(SIZE);
		for (int i = 0; i < offset; i ++) {
			*(ptr + i * int_size) = *(startPtr + i * int_size);
		}
		free(startPtr);
		startPtr = ptr;
	}


public:
	size_t SIZE 

	Linear() {
		SIZE = 1024;
		startPtr = (int*)malloc(SIZE);
		offset = 0;
	}

	int get(size_t index) {
		if (index >= SIZE)
		{
			exit(-1);
		}
		return startPtr[index];
	}

	void set(size_t index, int value) {
		if (offset * int_size > SIZE * 0.75)
			resize();
		startPtr[index] = value;
		offset++;
	}

	void remove_all() {
		for (int i = 0; i < offset; i++)
			startPtr[i] = INT_MIN;
		free(startPtr);
		offset = 0;
		SIZE = 1024;
		startPtr = (int*)malloc(SIZE);
	}
};


class Stack {
private:

	int *startPtr;
	size_t const int_size = sizeof(int);
	size_t SIZE;
	int offset;

	void resize() {
		SIZE *= 2;
		int *ptr = (int*)malloc(SIZE);
		for (int i = 0; i < offset; i++) {
			*(ptr + i * int_size) = *(startPtr + i * int_size);
		}
		free(startPtr);
		startPtr = ptr;
	}

public:
	Stack() {
		SIZE = 1024;
		startPtr = (int*)malloc(SIZE);
		offset = 0;
	}

	void push(int value) {
		if (offset *int_size > SIZE * 0.75)
			resize();
		startPtr[offset] = value;
		offset++;
	}

	int pop() {
		if (offset == 0)
			throw range_error("Empty stack");
		offset--;
		int value = startPtr[offset];
		startPtr[offset] = INT_MIN;
		return value;
	}

	void remove_all() {
		for (int i = 0; i < offset; i++)
			startPtr[i] = INT_MIN;
		free(startPtr);
		startPtr = (int*)malloc(SIZE);
		offset = 0;
		SIZE = 1024;
	}
};



struct Node {
	int value;
	Node *child;
	Node *parent;
};

class List {
private:

	Node *startPtr;
	Node *endPtr;
	int offset;
	size_t const int_size = sizeof(int);

public:
	List() {
		startPtr = NULL;
		endPtr = NULL;
		offset = 0;
	}

	void push(int value) {
		Node *ptr = (Node*)malloc(int_size);
		if (offset == 0)
			startPtr = ptr;
		if (endPtr != NULL)
			endPtr->child = ptr;
		ptr->value = value;
		ptr->child = NULL;
		ptr->parent = endPtr;
		endPtr = ptr;
		offset++;
	}

	int pop() {
		int value = endPtr->value;
		Node* toPop = endPtr;
		endPtr = endPtr->parent;
		endPtr->child = NULL;
		offset--;
		return value;
	}

	int get(int index) {
		if (index >= offset)
			throw range_error("Index out of range");
		Node *curr_node = startPtr;
		int i = 0;
		while (i != index) {
			curr_node = curr_node->child;
			i++;
		}
		return curr_node->value;
	}

	void set(int index, int value) {
		if (index >= offset)
			throw range_error("Index out of range");
		Node *curr_node = startPtr;
		int i = 0;
		while (i != index) {
			curr_node = curr_node->child;
			i++;
		}
		curr_node->value = value;
	}

	void remove_all() {
		offset = 0;
		Node *curr_node = startPtr;
		while (curr_node != NULL) {
			Node *child = curr_node->child;
			curr_node = child;
		}
		startPtr = NULL;
		endPtr = NULL;
	}
};


int main() {

	Linear arr;

	arr.set(0, 0);
	arr.set(1, 1);
	arr.set(2, 2);
	cout << "get 0" << endl << arr.get(0) << endl;
	arr.set(2, 3);
	cout << "get 1" << endl << arr.get(1) << endl;
	cout << "get 2" << endl << arr.get(2) << endl;
	arr.remove_all();
	cout << "remove all" << endl;

	cout << "-----------------------------------------------------------------" << endl;

	Stack stack;

	stack.push(0);
	cout << "push 0" << endl;
	stack.push(1);
	cout << "push 1" << endl;
	stack.push(2);
	cout << "push 2" << endl;

	cout << stack.pop() << endl;
	stack.push(3);
	cout << stack.pop() << endl;
	cout << stack.pop() << endl;
	cout << stack.pop() << endl;
	stack.remove_all();

	cout << "-----------------------------------------------------------------" << endl;

	List list;

	list.push(0);
	cout << "push 0" << endl;
	list.push(1);
	cout << "push 1" << endl;
	list.push(2);
	cout << "push 2" << endl;
	cout << list.pop() << endl;
	cout << list.get(0) << endl;
	cout << list.get(1) << endl;
	list.set(1, 4);
	cout << list.get(1) << endl;
	list.remove_all();
	cout << "remove all" << endl;
}
