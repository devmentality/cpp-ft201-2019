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
};


int main()
{
    return 0;
}
