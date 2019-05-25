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
	auto value1 = 42;
	auto ptr = SmartPtr<int>(new int (value1));
	auto get1 = ptr.Get();
	cout << "Test Get: " << get1 << endl;
	if (ptr.object == nullptr)
		cout << "Object: null" << endl;
	else
		cout << "Object: " << *ptr.object << endl;
	if (value1 == get1)
		cout << "OK" << endl;
	else
		cout << "NOT OK" << endl;	
	cout << endl;

	auto value2 = 43;
	ptr.Set(new int(value2));
	auto get2 = ptr.Get();
	cout << "Test Set: " << get2 << endl;
	if (ptr.object == nullptr)
		cout << "Object: null" << endl;
	else
		cout << "Object: " << *ptr.object << endl;
	if (value2 == get2)
		cout << "OK" << endl;
	else
		cout << "NOT OK" << endl;
	cout << endl;

	ptr.Release();
	cout << "Test Release: " << endl;
	if (ptr.object == nullptr)
		cout << "Object: null" << endl;
	else
		cout << "Object: " << *ptr.object << endl;
	if (ptr.object == nullptr && ptr.counter == nullptr)
		cout << "OK" << endl;
	else
		cout << "NOT OK" << endl;
	cout << endl;

	return 0;
}
