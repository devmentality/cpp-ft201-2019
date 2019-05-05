#include <iostream>
using namespace std;


template <class T>
class SmartPtr
{
private:
	T* ptr;
	size_t* ref_cnt;

public:
	explicit SmartPtr(T* ptr)
	{
		this->ptr = ptr;
		this->ref_cnt = new size_t();
		this->*ref_cnt = 1;
	}

    ~SmartPtr()
    {
        Release();
    }

	void Release()
	{
		if (!this->ptr)
        {
            return;
        }
		
        (*this->ref_cnt)--;
        if (!*this->ref_cnt)
        {
            delete ptr;
            delete ref_cnt;
        }
        
        this->ptr = 0;
        this->ref_cnt = 0;
	}

    void Set(T* other)
    {
        Release();
        this->ptr = other;
        this->ref_cnt = 1;
    }

	T* Get()
	{
		return this->ptr;
	}

    T* operator->()
    {
        return Get();
    }

    T& operator*()
    {
        return *this->ptr;
    }

	SmartPtr& operator=(SmartPtr& other)
	{
		Release();
		this->ptr = other.ptr;
		this->ref_cnt = other.ref_cnt;
		(*ref_cnt)++;
		return *this;
	}
};
