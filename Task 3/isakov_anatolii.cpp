#include "pch.h"
#include <iostream>

template <class T>
class SmartPointer
{
private:
	T *obj;
	size_t *counter;

public:
	SmartPointer()
	{
		counter = new size_t(0);
		obj = nullptr;
	}

	SmartPointer(T *_obj)
	{
		obj = _obj;
		counter = new size_t(1);
	}

	T* Get()
	{
		return *obj;
	}

	void Set(T *_obj)
	{
		if (obj != nullptr)
			Release();
		obj = _obj;
		counter = new size_t(1);
	}

	void Release()
	{
		if (obj != nullptr)
		{
			(*counter)--;
			if (*counter == 0)
			{
				delete counter;
				delete obj;
			}
		}
	}

	SmartPointer& operator= (const SmartPointer& otherPointer)
	{
		Release();
		obj = otherPointer.obj;
		counter = otherPointer.counter;
		(*counter)++;
		return *this;
	}

	T* operator-> ()
	{
		return obj;
	}

	T& operator* ()
	{
		return *obj;
	}

	void Dump()
	{
		if (obj == nullptr)
			std::cout << "Value: " << "null";
		else
			std::cout << "Value: " << *obj;

		if (counter == nullptr)
			std::cout << ". Count: " << "null";
		else
			std::cout << ". Count: " << *counter;
		std::cout << std::endl;
	}

};


int main()
{

}