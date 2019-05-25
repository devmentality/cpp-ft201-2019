#include <iostream>
#include <conio.h>


template <class T>
class SmartPtr
{
private:
	T* obj_ptr;
	size_t* counter_ptr;

	void Attach(const SmartPtr& other)
	{
		counter_ptr = other.counter_ptr;
		(*counter_ptr)++;
		obj_ptr = other.obj_ptr;
	}

public:
	explicit SmartPtr(T* V) : obj_ptr(V)
	{
		counter_ptr = new size_t();
		*counter_ptr = 1;
	}

	T Get() const
	{
		return *obj_ptr;
	}

	void Set(const SmartPtr& other)
	{
		Release();
		Attach(other);
	}

	void Release()
	{
		if (obj_ptr)
		{
			(*counter_ptr)--;
			if ((*counter_ptr) == 0)
			{
				delete obj_ptr;
				delete counter_ptr;
			}
			obj_ptr = 0;
			counter_ptr = 0;
		}
	}

	SmartPtr& operator=(const SmartPtr& other)
	{
		Set(other);
		return *this;
	}

	operator T* ()
	{
		return obj_ptr;
	}

	T* operator->() const
	{
		return obj_ptr;
	}

	~SmartPtr()
	{
		Release();
	}

};


int main() {
	int test = 42;
	SmartPtr<int> int_smart_ptr = SmartPtr<int>(new int(test));
	int val = int_smart_ptr.Get();
	int val2 = *int_smart_ptr;

	printf("%i, %i and %i are equal\n", test, val, val2);

	int second_test_value = 7;
	int_smart_ptr.Set(SmartPtr<int>(new int(second_test_value)));
	printf("%i and %i are equal", second_test_value, (int)int_smart_ptr.Get());

	int_smart_ptr.Release();
	_getch();
	return 0;
}