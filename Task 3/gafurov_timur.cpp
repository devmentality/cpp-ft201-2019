// SmartPointer.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include <iostream>
using namespace std;

template <class T>
class SmartPointer
{
private:
	T* object;
	size_t* counter;
public:
	SmartPointer() {
	}

	SmartPointer(T* obj) {
		object = obj;
		counter = new size_t(1);
	}

	~SmartPointer()
	{
		Release();
	}

	T* Get() {
		return object;
	}

	void Release() {
		if (object == nullptr)
			return;
		(*counter)--;
		if (counter == 0) {
			delete object;
			delete counter;
		}
		object = nullptr;
		counter = nullptr;
	}

	void Set(T* obj) {
		Release();
		object = obj;
		counter = new size_t(1);
	}

	T& operator* ()
	{
		return *object;
	}

	T* operator-> ()
	{
		return object;
	}

	SmartPointer& operator= (SmartPointer& pointer)
	{
		Release();
		if (this != &pointer)
		{
			object = pointer.object;
			counter = pointer.counter;
			if (object != nullptr)
				(*counter)++;
		}
		return *this;
	}

	void Dump()
	{
		cout << "object: " << object << endl;
		if (counter != nullptr)
			cout << "counter: " << *counter << endl;
		cout << "-------------------------" << endl;
	}
};

int main()
{
	auto a = SmartPointer<int>(new int(1));
	a.Dump();
	auto b = SmartPointer<int>(new int(2));
	a = b;
	a.Dump();
	b.Dump();
	b.Release();
	a.Dump();
	b.Dump();
    return 0;
}
