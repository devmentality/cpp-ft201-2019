#include <iostream>
using namespace std;
#define _CRT_SECURE_NO_WARNINGS


#include "windows.h"


class DynamicArr {
private:
	int * arr;
	int len;
public:
	DynamicArr() {
		arr = NULL;
		len = 0;
	}
	DynamicArr(int size) {
		arr = (int*)malloc(size*sizeof(int));
		len = size;
	}
	~DynamicArr() {
		if (arr != NULL) free(arr);
	}
	void push(int value) {
		len++;
		arr = (int*)realloc(arr, len * sizeof(int));
		arr[len - 1] = value;
	}
	int get(int i) {
		return arr[i];
	}
	bool set(int i, int value) {
		if (i >= len) return false;
		arr[i] = value;
		return true;
	}
};


struct Stek
{
	int key;
	Stek* next;
};

class Stack {
private:
	Stek *next;
public:
	Stack() {};

	void Push(int d) {
		Stek *pv = new Stek;
		pv->key = d;
		pv->next = next;
		next = pv;
	}
	int Pop()
	{
		int temp = next->key;
		Stek *pv = next;

		next = next->next;
		delete pv;
		return temp;
	}
};







template<typename T>
class List
{
public:
	List();
	~List();
	void pop_front();

	void add(T data);

	void clear();

	int GetSize() { return Size; }

	T& operator[](const int index);


	void removeAt(int index);

	void pop_back();

private:


	template<typename T>
	class Node
	{
	public:
		Node * pNext;
		T data;

		Node(T data = T(), Node *pNext = nullptr)
		{
			this->data = data;
			this->pNext = pNext;
		}
	};
	int Size;
	Node<T> *head;
};


template<typename T>
List<T>::List()
{
	Size = 0;
	head = nullptr;
}


template<typename T>
List<T>::~List()
{
	clear();
}


template<typename T>
void List<T>::pop_front()
{
	Node<T> *temp = head;

	head = head->pNext;

	delete temp;

	Size--;

}

template<typename T>
void List<T>::add(T data)
{
	if (head == nullptr)
	{
		head = new Node<T>(data);
	}
	else
	{
		Node<T> *current = this->head;

		while (current->pNext != nullptr)
		{
			current = current->pNext;
		}
		current->pNext = new Node<T>(data);
	}
	Size++;
}


template<typename T>
void List<T>::clear()
{
	while (Size)
	{
		pop_front();
	}
}


template<typename T>
T & List<T>::operator[](const int index)
{
	int counter = 0;

	Node<T> *current = this->head;

	while (current != nullptr)
	{
		if (counter == index)
		{
			return current->data;
		}
		current = current->pNext;
		counter++;
	}
}

template<typename T>
void List<T>::removeAt(int index)
{
	if (index == 0)
	{
		pop_front();
	}
	else
	{
		Node<T> *previous = this->head;
		for (int i = 0; i < index - 1; i++)
		{
			previous = previous->pNext;
		}
		Node<T> *toDelete = previous->pNext;
		previous->pNext = toDelete->pNext;
		delete toDelete;
		Size--;
	}
}

template<typename T>
void List<T>::pop_back()
{
	removeAt(Size - 1);
}


int main()
{
	setlocale(LC_ALL, "ru");

	DynamicArr arr;
	arr.push(1);
	arr.push(2);
	arr.push(40);

	for (int i(0); i < 3; i++)
	{
		cout << arr.get(i) << endl;
	}

	Stack st;

	for (int i(0); i < 4; i++)
	{
		st.Push(i + 1);
	}

	for (int i(0); i < 4; i++)
	{
		cout << st.Pop() << endl;
	}
	
	List<int> lst;
	lst.add(1);
	lst.add(2);
	lst.add(3);

	for (int i = 0; i < lst.GetSize(); i++)
	{
		cout << lst[i] << endl;
	}


	lst.pop_back();

	for (int i = 0; i < lst.GetSize(); i++)
	{
		cout << lst[i] << endl;
	}
	return 0;
}




