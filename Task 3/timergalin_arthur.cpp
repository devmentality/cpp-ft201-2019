#include <cstdlib>
#include <stdio.h>

template <class T>
class SmartPtr
{
	T* _ptr;
	size_t* _count;

public:
	explicit SmartPtr(T* ptr)
	{
        _ptr = ptr;
		_count = new size_t();
		*_count = 1;
	}
    
	T Get() const
	{
		return *_ptr;
	}
     
	void Set(T* Other)
	{
        Release();
		_count = new size_t();
		(*_count)++;
		_ptr = Other;
    }

    void Release()
	{
		if (_ptr)
		{
			(*_count)--;
			if ((*_count) == 0)
			{
				delete _ptr;
				delete _count;
			}
			_ptr = 0;
			_count = 0;
		}
	}

    SmartPtr& operator=(const SmartPtr& Other)
	{
		Release();
		Set(Other);
		return *this;
	}

	T* operator->() const
	{
		return _ptr;
	}

    T operator *()
	{
		return *_ptr;
	}

    ~SmartPtr()
	{
		Release();
	}
};

int main(){
    int test_value = 15;
    SmartPtr<int> int_smart_ptr = SmartPtr<int>(new int(test_value)); 
    int val = int_smart_ptr.Get();
    int val2 = *int_smart_ptr;

    printf("Get check: %i, %i and %i are equal?\n", test_value, val, val2);
    bool are_equal = (test_value == val) and (val == val2);
    if(are_equal)
        printf("\tYes\n");
    else
        printf("\tNo!\n");

    int second_test_value = 20;
    int_smart_ptr.Set(new int(second_test_value));
    printf("Set check: %i and %i are equal?\n", second_test_value, int_smart_ptr.Get());
    if(second_test_value ==  int_smart_ptr.Get())
        printf("\tYes");
    else
        printf("\tNo!");

    int_smart_ptr.Release();
}