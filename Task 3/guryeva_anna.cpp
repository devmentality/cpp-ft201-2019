#include <iostream>


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

	T* Get() const
	{
		return obj_ptr;
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
		Set(other)
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