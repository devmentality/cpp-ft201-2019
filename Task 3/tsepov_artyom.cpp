#pragma comment(linker, "/STACK:16777216")
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <vector>
#include <algorithm>
#include <map>
#include <set>
#include <string>
#include <queue>
#include <unordered_map>
#include <cmath>
#include <iterator>
#include <stack>
#include <bitset>
#include <unordered_set>


using namespace std;


template <class T>
class SmartPointer
{
private:
	T* ptr;
	size_t* count;
public:
	SmartPointer(T* _ptr)
	{
		count = new size_t();
		*count = 1;
		ptr = _ptr;
	}
	
	T* Get() 
	{
		return ptr;
	}
	
	void Set(T* other) 
	{
		Release();
		ptr = other;
		count = new size_t();
		*count = 1;
	}
	
	void Release() 
	{
		if (!ptr)
			return;
		(*count)--;
		if (*count == 0)
		{
			delete ptr;
			delete count;
		}
		ptr = NULL;
		count = new size_t();
		*count = 0;
	}

	T* operator->() 
	{
		return ptr;
	}

	T& operator*()
	{
		return *ptr;
	}

	SmartPointer& operator=(SmartPointer& other) 
	{
		Release();
		ptr = other.ptr;
		count = other.count;
		(*count)++;
		return *this;
	}

	~SmartPointer()
	{
		Release();
	}

	void Show()
	{
		cout << "Object: " << *Get() << " Links: " << *count << "\n\n";
	}
};

void test()
{
	int* obj = new int(0);
	SmartPointer<int> ptr = SmartPointer<int>(obj);
	ptr.Show();

	SmartPointer<int> ptr2 = ptr;
	ptr.Show();
	ptr2.Show();

	int* obj2 = new int(10);
	SmartPointer<int> ptr3 = SmartPointer<int>(obj2);
	ptr3.Show();

	int* obj3 = new int(100);
	ptr2.Set(obj3);
	ptr2.Show();

	ptr3 = ptr2;
	ptr3.Show();
	ptr2.Show();

	int* obj4 = new int(101);
	ptr3.Set(obj4);
	ptr3.Show();
	ptr2.Show();

	ptr.Release();
	ptr2.Release();
	ptr3.Release();
}


int main()
{
	test();
	return 0;
}
