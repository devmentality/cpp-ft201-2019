#include <iostream>

template <class T>
class SmartPointer
{
	T* ptr;
	size_t* counter;

public:
	SmartPointer()
	{
		ptr = nullptr;
		counter = 0;
	}

	void Set(T* object_ptr)
	{
		Release();
		ptr = object_ptr;
		counter = new size_t();
		*counter = 1;
	}

	T* Get()
	{
		return ptr;
	}

	SmartPointer& operator=(const SmartPointer& other)
	{
		Release();
		counter = other.counter;
		(*counter)++;
		ptr = other.ptr;
		return *this;
	}

	T* operator->()
	{
		return ptr;
	}

	T& operator*()
	{
		return *ptr;
	}


	void Release()
	{
		if (ptr) {
			(*counter)--;
			if (*counter == 0)
			{
				delete ptr;
				delete counter;
			}
			ptr = 0;
			counter = 0;
		}
	}

	void Dump()
	{
		if (ptr)
			std::cout << "value: " << *ptr;
		else
			std::cout << "value: " << "None";
		if (counter)
			std::cout << "; count: " << *counter;
		else
			std::cout << "; count: " << "None";
		std::cout << std::endl;
	}

	~SmartPointer()
	{
		Release();
	}
};

void test()
{
	auto obj = new int(8);
	auto obj1 = new int(9);
	auto p1 = SmartPointer<int>();
	auto p2 = SmartPointer<int>();
	auto p3 = SmartPointer<int>();
	std::cout << "Set object twice" << std::endl;
	p1.Set(obj1);
	p1.Dump();
	p1.Set(obj);
	p1.Dump();
	
	std::cout << std::endl;
	std::cout << "Create second ref" << std::endl;
	p2 = p1;
	p1.Dump();
	p2.Dump();
	
	std::cout << std::endl;
	std::cout << "Create third ref" << std::endl;
	p3 = p1;
	p1.Dump();
	p2.Dump();
	p3.Dump();
	
	std::cout << std::endl;
	std::cout << "Release one ref" << std::endl;
	p3.Release();
	p1.Dump();
	p2.Dump();
	p3.Dump();

	std::cout << std::endl;
	std::cout << "Use *" << std::endl;
	std::cout << *p1 << std::endl;
}

int main()
{
	test();
	return 0;
}
