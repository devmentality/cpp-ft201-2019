// Allocators.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
//#include <iostream>
//#include <cstdlib>
#include <stdlib.h>
#include <stack>
#include <iostream>
using namespace std;

class LinearAllocator {
	private:
		size_t totalSize;
		void* startPointer;
		size_t offset;
	public:
		LinearAllocator(size_t size) {
			startPointer = malloc(size);
			totalSize = size;
			offset = 0;
		}

		void* Allocate(size_t size) {
			if (offset + size > totalSize) {
				while (size + offset > totalSize)
					totalSize *= 2;
				void *newPointer = malloc(totalSize);
				memcpy(newPointer, startPointer, offset);
				startPointer = newPointer;
			}
			offset += size;
			return (void*)((size_t)startPointer + offset - size);
		}

		void Deallocate(size_t size) {
			if (size <= offset)
				offset -= size;
			else
				offset = 0;
		}
};

class StackAllocator {
private:
	size_t totalSize;
	void* startPointer;
	size_t offset;
	size_t offsetSize;
public:
	StackAllocator(size_t size, size_t _offsetSize) {
		startPointer = malloc(size);
		totalSize = size;
		offsetSize = _offsetSize;
		offset = 0;
	}

	void* Allocate() {
		if (offset + offsetSize > totalSize)
			throw "Out of memory exception: stack memory is over!";
		offset += offsetSize;
		return (void*)((size_t)startPointer + offset - offsetSize);
	}

	void Deallocate() {
		if (offset > 0)
			offset -= offsetSize;
	}
};

class ListElement {
	public:
		ListElement* Previous;
		ListElement* Next;
		int Value;

		ListElement(int value) {
			Value = value;
			Previous = nullptr;
			Next = nullptr;
		}
};

class LinkedList {
	private:
		ListElement* head;
		ListElement* tail;

		bool CheckPush(int value) {
			if (head == nullptr || tail == nullptr) {
				head = new ListElement(value);
				tail = head;
				return false;
			}
			return true;
		}

		bool CheckPop() {
			return head != nullptr && tail != nullptr;
		}
	public:
		LinkedList() {
			head = nullptr;
			tail = nullptr;
		}

		void PushRight(int value) {
			if (!CheckPush(value))
				return;
			head->Next = new ListElement(value);
			head->Next->Previous = head;
			head = head->Next;
		}

		void PushLeft(int value) {
			if (!CheckPush(value))
				return;
			tail->Previous = new ListElement(value);
			tail->Previous->Next = tail;
			tail = tail->Previous;
		}

		int PopRight() {
			if (!CheckPop())
				throw "Error: Linked list is empty!";			
			ListElement* element = head;
			head = head->Previous;
			if (head == nullptr)
				tail = nullptr;
			else
				head->Next = nullptr;
			return element->Value;
		}

		int PopLeft() {
			if (!CheckPop())
				throw "Error: Linked list is empty!";
			ListElement* element = tail;
			tail = tail->Next;
			if (tail == nullptr)
				head = nullptr;
			else
				tail->Previous = nullptr;	
			return element->Value;
		}

		int PeekRight() {
			if (head == nullptr)
				throw "Error: Linked list is empty!";
			return head->Value;
		}

		int PeekLeft() {
			if (tail == nullptr)
				throw "Error: Linked list is empty!";
			return tail->Value;
		}
};

int main()
{
	LinkedList* list = new LinkedList();
	for (int i = 0; i < 10; i++) {
		list->PushRight(i);
		cout << list->PeekRight() << endl;
		cout << list->PeekLeft() << endl;
	}
	for (int i = 10; i < 20; i++) {
		list->PushLeft(i);
		cout << list->PeekRight() << endl;
		cout << list->PeekLeft() << endl;
	}
	for (int i = 0; i < 10; i++) {
		cout << list->PopRight() << endl;
		cout << list->PopLeft() << endl;
	}

	LinearAllocator* lin = new LinearAllocator(1024);
	void* ptr = lin->Allocate(512);
	cout << ptr << endl;
	ptr = lin->Allocate(512);
	cout << ptr << endl;
	ptr = lin->Allocate(512);
	cout << ptr << endl;

	StackAllocator* stack = new StackAllocator(1024, 512);
	ptr = stack->Allocate();
	cout << ptr << endl;
	ptr = stack->Allocate();
	cout << ptr << endl;
	stack->Deallocate();
	stack->Deallocate();
	return 0;
}