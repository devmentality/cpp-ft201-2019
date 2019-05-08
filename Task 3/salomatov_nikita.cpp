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
		if (*counter > 0) {
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
	auto one = new int(1);
	auto smart_ptr1 = SmartPointer<int>(one);
	smart_ptr1.Dump();
}

