#include <iostream>

using namespace std;

template <class T>
class SmartPointer {
    T* obj;
    size_t* counter;

public:
    void Dump()
    {
        if (obj == nullptr)
            cout << "Object: null" << endl;
        else
            cout << "Object: " << *obj << endl;
        cout << "Counter: " << *counter << endl;
    }

    SmartPointer(T* obj)
    {
        this->obj = obj;
        counter = new size_t();
        *counter = 1;
    }

    SmartPointer()
    {
        counter = new size_t();
        *counter = 0;
        obj = nullptr;
    }

    ~SmartPointer() {
        Release();
    }

    void Release()
    {
        (*counter)--;
        if (counter == 0)
        {
            delete obj;
        }
        obj = nullptr;
        counter = new size_t(0);
    }

    void Set(T* obj)
    {
        if (obj != nullptr)
            Release();
        this->obj = obj;
        counter = new size_t(1);
    }

    T* Get()
    {
        return *obj;
    }

    T* operator-> ()
    {
        return obj;
    }

    T& operator* ()
    {
        return *obj;
    }

    SmartPointer & operator=(SmartPointer & new_obj) {
        if (*counter > 0) {
            Release();
        }
        if (this != &new_obj) {
            obj = new_obj.obj;
            counter = new_obj.counter;
            (*counter)++;
        }
        return *this;
    }
};

int main() {
    int* test = new int(1);
    auto ptr1 = SmartPointer<int>(test);
    ptr1.Dump();

    cout << endl << "Test operator=:" << endl;
    SmartPointer<int> ptr2;
    ptr2 = ptr1;
    ptr1.Dump();
    ptr2.Dump();

    cout << endl << "Test Release():" << endl;
    ptr2.Release();
    ptr1.Dump();
    ptr2.Dump();

    cout << endl << "Test Set():" << endl;
    SmartPointer<int> ptr3;
    ptr3 = ptr1;
    ptr3.Dump();
    ptr1.Dump();
    ptr3.Set(new int(2));
    ptr3.Dump();
    ptr1.Dump();

    return 0;
}