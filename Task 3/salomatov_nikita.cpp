#include <iostream>
using namespace std;

template <class T>
class SmartPointer
{
private:
	T* pointer;
	size_t* reference_counter;

public:
	SmartPointer(T* pointer)
	{
		this->pointer = pointer;
		reference_counter = new size_t(1);
	}

	SmartPointer()
	{
		reference_counter = new size_t();
		*reference_counter = 0;
		pointer = nullptr;
	}

	void Release()
	{
		if (pointer != nullptr)
		{
			(*reference_counter)--;
			if (*reference_counter == 0)
			{
				delete pointer;
				delete reference_counter;
			}
		}
	}

	void Set(T* pointer)
	{
		if (pointer != nullptr)
			Release();
		this->pointer = pointer;
		reference_counter = new size_t(1);
	}

	T* Get()
	{
		return pointer;
	}

	T* operator->()
	{
		return pointer;
	}

	T& operator*()
	{
		return *pointer;
	}

	SmartPointer& operator=(SmartPointer& other)
	{
		if (*reference_counter > 0) {
			Release();
		}
		Release();
		pointer = other.pointer;
		reference_counter = other.reference_counter;
		(*reference_counter)++;
		return *this;
	}

	~SmartPointer()
	{
		Release();
	}

	void Dump()
	{
		cout << "Object: " << pointer << endl;
		cout << "Count: " << *reference_counter << endl;
	}

};

int main()
{
	auto test = new int(1);
	auto ptr1 = SmartPointer<int>(test);
	ptr1.Dump();

	SmartPointer<int> ptr2;
	ptr2 = ptr1;
	ptr1.Dump();
	ptr2.Dump();

	ptr2.Release();
	ptr1.Dump();
	ptr2.Dump();

	SmartPointer<int> ptr3;
	ptr3 = ptr1;
	ptr3.Dump();
	ptr1.Dump();
	ptr3.Set(new int(5));
	ptr3.Dump();
	ptr1.Dump();

	return 0;
}

