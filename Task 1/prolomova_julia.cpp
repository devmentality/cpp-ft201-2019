#include<iostream>
using namespace std;

template <class T>
class LinearArray {
private:

	T *start;
	T offset;
	size_t const T_size = sizeof(T);

	void resize() {
		size *= 2;
		T *ptr = (T*)malloc(size);
		for (int i = 0; i < offset; i += T_size) {
			*(ptr + i) = *(start + i);
		}
		free(start);
		start = ptr;
	}


public:
	size_t size;

	LinearArray() {
		size = 1024;
		start = (T*)malloc(size);
		offset = 0;
	}

	void set(size_t index, T value) {
		if (offset > size * 0.75)
			resize();
		start[index] = value;
		offset++;
	}

	int get(int index) {
		if (index >= size)
		{
			exit(-1);
		}
		return start[index];
	}

	void remove_all() {
		free(start);
		offset = 0;
		size = 1024;
		start = (T*)malloc(size);
	}
};


template <class T>
class Stack {
private:

	T *start;
	T *cur;
	int const T_size = sizeof(T);

	void resize() {
		T *newPtr = (T*)malloc(size * 2);
		size *= 2;
		cur = newPtr + (cur - start);
		for (int i = 0; i < cur - start; i += T_size) {
			*(newPtr + i) = *(start + i);
			delete (start + i);
		}
		start = newPtr;
	}


public:
	int size;

	Stack() {
		size = 1024;
		start = (T*)malloc(size);
		cur = start;
	}

	void push(T value) {
		if (cur - start > size * 0.75)
			resize();
		*cur = value;
		cur += T_size;
	}

	int pop() {
		if (cur - T_size < start) exit(-1);
		cur -= T_size;
		T value = *cur;
		return value;
	}

	void remove_all() {
		for (int i = 0; i < cur - start; i += T_size) {
			delete (start + i);
		}
		start = (T*)malloc(size);
		cur = start;
	}
};


template <class T>
class Node {
public:
	T value;
	int index;
	Node* next;

	Node<T>() {
		Node<T> *res = (Node<T>*) malloc(sizeof(Node<T>));
		res->next = NULL;
	};
};


template <class T>
class LinkedList {
private:
	void delete_items(Node<T>* it) {
		if (it == nullptr) return;
		if (it->next) {
			delete_items(it->next);
		}
		free(it);
	}

	Node<T>* getNode(int n) {
		int counter = 0;
		Node<T>* head = first;
		while (counter < n && head->next) {
			head = head->next;
			counter++;
		}
		if (n != counter)
			exit(-1);
		return head;
	}
public:
	Node<T> *first;
	Node<T> *cur;
	int size;

	LinkedList() {
		first = nullptr;
		cur = nullptr;
		size = 0;
	}

	void push(T data) {
		size++;
		Node<T> *tmp = new Node<T>();
		tmp->value = data;
		if (cur)
			cur->next = tmp;
		cur = tmp;
		if (!(first)) first = tmp;
	}

	T get(int n)
	{
		auto node = getNode(n);
		if (node == nullptr) exit(-1);
		return node->value;
	}

	void set(int n, T value) {
		auto node = getNode(n);
		if (node == nullptr) exit(-1);
		node->value = value;
	}

	T pop(int n) {
		size--;
		auto head = getNode(n);
		if (head == nullptr) exit(-1);
		Node<T>* prev = head;
		if (n == 0) first = prev->next;
		T val = prev->value;
		head = head->next;
		free(prev);
		return val;
	}

	void remove_all() {
		delete_items(first);
		first = nullptr;
		cur = nullptr;
		size = 0;
	}
};



int main() {
	LinearArray<char> arr;
	arr.set(0, 0);
	cout << "set 0 0" << endl;
	arr.set(3, 3);
	cout << "set 3 3" << endl;

	cout << "get 0" << endl << arr.get(0) << endl;
	cout << "get 3" << endl << arr.get(3) << endl;

	arr.remove_all();
	cout << "remove all" << endl;

	cout << "-----------------------------------------------------------------" << endl;

	Stack<int> stack;

	stack.push(0);
	cout << "push 0" << endl;
	stack.push(1);
	cout << "push 1" << endl;
	stack.push(2);
	cout << "push 2" << endl;

	cout << "pop" << endl << stack.pop() << endl;
	cout << "pop" << endl << stack.pop() << endl;
	cout << "pop" << endl << stack.pop() << endl;

	cout << "push 3" << endl;
	stack.push(3);
	cout << "push 4" << endl;
	stack.push(4);
	cout << "pop" << endl << stack.pop() << endl;
	cout << "pop" << endl << stack.pop() << endl;
	stack.remove_all();
	cout << "remove all" << endl;

	cout << "-----------------------------------------------------------------" << endl;

	LinkedList<int> list;
	list.push(0);
	cout << "push 0" << endl;
	list.push(1);
	cout << "push 1" << endl;
	list.push(2);
	cout << "push 2" << endl;
	list.push(3);
	cout << "push 3" << endl;
	list.push(4);
	cout << "push 4" << endl;

	cout << "pop 0" << endl << list.pop(0) << endl;
	cout << "pop 0" << endl << list.pop(0) << endl;
	cout << "pop 0" << endl << list.pop(0) << endl;
	cout << "pop 0" << endl << list.pop(0) << endl;
	cout << "pop 0" << endl << list.pop(0) << endl;

	list.push(0);
	cout << "push 0" << endl;
	list.push(1);
	cout << "push 1" << endl;
	list.push(2);
	cout << "push 2" << endl;
	list.push(3);
	cout << "push 3" << endl;
	list.push(4);
	cout << "push 4" << endl;

	cout << "get 0" << endl << list.get(0) << endl;
	list.set(0, 5);
	cout << "set 0 5" << endl;
	cout << "get 0" << endl << list.get(0) << endl;
	cout <<  "pop 0" << endl << list.pop(0) << endl;
	cout << "pop 0" << endl << list.pop(0) << endl;

	list.push(0);
	cout << "push 0" << endl;

	list.remove_all();
	cout << "remove all" << endl;

}
