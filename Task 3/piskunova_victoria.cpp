#include "pch.h"
#include <iostream>
using namespace std;


template <class T>
class SmartPointer 
{
private:
	T* pointer;
	size_t* ref_counter;

public:
	SmartPointer(T* ptr) 
	{
		pointer = ptr;
		ref_counter = new size_t();
		*ref_counter = 1;
	}

	void Release() 
	{
		if (pointer) 
		{
			(*ref_counter)--;
			if (*ref_counter == 0) 
			{
				delete pointer;
				delete ref_counter;
			}
			pointer = 0;
			ref_counter = 0;
		}
	}

	T* Get()
	{
		return pointer;
	}

	void Set(T* other_pointer) 
	{
		Release();
		pointer = other_pointer;
		ref_counter = new size_t();
		*ref_counter = 1;
	}

	T* operator->()
	{
		return pointer;
	}

	T& operator*()
	{
		return *pointer;
	}

	SmartPointer& operator=(SmartPointer& other_pointer) 
	{
		Release();
		pointer = other_pointer.pointer;
		ref_counter = other_pointer.ref_counter;
		(*ref_counter)++;
		return *this;
	}

	~SmartPointer() 
	{
		Release();
	}

	void Print()
	{
		cout << "Объект: " << *Get() << "\n";
		cout << "Количество ссылок: " << *ref_counter << "\n";
	}
};

int main()
{
	setlocale(0, "rus");
	auto one = new int(1);
	auto smart_ptr1 = SmartPointer<int>(one);
	smart_ptr1.Print();
	cout << "\n";
	auto smart_ptr2 = smart_ptr1;
	smart_ptr1.Print();
	smart_ptr2.Print();
	cout << "\n";
	auto three = new int(3);
	auto smart_ptr3 = SmartPointer<int>(three);
	smart_ptr3.Print();
	auto two = new int(2);
	smart_ptr2.Set(two);
	smart_ptr3 = smart_ptr2;
	smart_ptr3.Print();
	smart_ptr2.Print();
	cout << "\n";
	smart_ptr3.Release();
	smart_ptr2.Release();
	smart_ptr1.Release();
}
