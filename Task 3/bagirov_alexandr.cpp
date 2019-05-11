#include <iostream>
using namespace std;


template <class T>
class SmartPtr
{
private:
    T* ptr;
    size_t* ref_cnt{};

public:
    explicit SmartPtr()
    {
        this->ptr = nullptr;
        this->ref_cnt = nullptr;
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

        (*ref_cnt)--;
        if (!this->ref_cnt)
        {
            delete ptr;
            delete ref_cnt;
        }

        this->ptr = nullptr;
        this->ref_cnt = nullptr;
    }

    void Set(T* other)
    {
        this->ptr = other;
        this->ref_cnt = (size_t*)malloc(sizeof(size_t));
        *ref_cnt = 1;
    }

    T Get()
    {
        return *ptr;
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
        if (*ref_cnt > 0)
        {
            Release();
        }
        if (this != &other)
        {
            this->ptr = other.ptr;
            this->ref_cnt = other.ref_cnt;
            (*ref_cnt)++;
        }
        return *this;
    }

    void Print()
    {
        cout << "Object: " << Get() << "\nNumber of links: " << *ref_cnt << "\n========\n";
    }
};


int main() {
    auto p1 = SmartPtr<string>();
    p1.Set(new string("test 1"));
    p1.Print();

    auto p2 = SmartPtr<string>();
    p2.Set(new string("test 2"));
    p2.Print();

    p1 = p2;
    p1.Print();

    p2.Release();
    p1.Print();

    p1.Release();
    return 0;
}
