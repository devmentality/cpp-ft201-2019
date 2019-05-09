#include "pch.h"
#include <iostream>

using namespace std;

template <class T>
class SmartPtr
{
public:
	T* object;
	size_t* counter;

	SmartPtr()
	{
		object = nullptr;
		counter = new size_t(0);
	}

	SmartPtr(T* _object)
	{
		object = _object;
		counter = new size_t(1);
	}

	~SmartPtr()	{ Release(); }

	T Get() const { return *object; }

	void Set(T* _object)
	{
		Release();
		counter = new size_t();
		(*counter) += 1;
		object = _object;
	}

	void Release()
	{
		if (object)
		{
			(*counter) -= 1;
			if ((*counter) == 0)
			{
				delete object;
				delete counter;
			}
			object = nullptr;
			counter = nullptr;
		}
	}

	SmartPtr& operator=(const SmartPtr& _object)
	{
		Release();
		Set(_object);
		return *this;
	}

	T* operator->() const { return object; }

	T& operator*() const { return *object; }
};

int main()
{
}