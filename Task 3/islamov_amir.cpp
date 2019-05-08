#include <iostream>

template <typename T>
class SmartPtr {
    T* owned_obj;
    size_t* count;
public:
    explicit SmartPtr(T* obj): owned_obj(obj), count(new size_t(1)) {}

    SmartPtr(const SmartPtr &ptr): owned_obj(ptr.owned_obj), count(ptr.count){
        *count += 1;
    }

    ~SmartPtr() { Release(); }

    T Get() { return *owned_obj; }

    void Set(T* new_obj) {
        if (owned_obj == new_obj)
            return;
        Release();
        count = new size_t(1);
        owned_obj = new_obj;
    }

    void Release() {
        if (owned_obj) {
            *count -= 1;
            if (*count == 0) {
                delete owned_obj;
                delete count;
            }
            owned_obj = nullptr;
            count = nullptr;
        }
    };

    T* operator->() { return owned_obj; }

    T operator* () { return *owned_obj; }

    SmartPtr& operator=(const SmartPtr& ptr) {
        Release();
        owned_obj = ptr.owned_obj;
        count = ptr.count;
        if (owned_obj)
            *count += 1;
        return *this;
    }

    size_t GetRefCount() {
        if (!count)
            return 0;
        return *count; }
};


class TestClass {
public:
    int Val;

    explicit TestClass(int val): Val(val) {}

    ~TestClass() {
        std::cout << "Destructing TestClass instance_" << Val << "..." << std::endl;
    }
};


int main() {
    using namespace std;

    auto ptr_1 = SmartPtr<TestClass>(new TestClass(255));
    cout << ptr_1.GetRefCount() << endl; // 1

    auto ptr_2 = ptr_1;
    cout << ptr_1.GetRefCount() << endl; // 2
    cout << ptr_2.GetRefCount() << endl; // 2

    ptr_1.Release();
    cout << ptr_1.GetRefCount() << endl; // 0
    cout << ptr_2.GetRefCount() << endl; // 1

    auto ptr_3 = ptr_2;
    cout << ptr_3->Val << endl; // 255
    cout << ptr_2.Get().Val << endl; // 255

    ptr_2.Set(new TestClass(511)); // Destructing TestClass instance... (instance_255 refcount reaches 0)
    cout << ptr_3.GetRefCount() << endl; // 1
    cout << ptr_2.GetRefCount() << endl; // 1
    cout << ptr_2.Get().Val << endl; // 511

    ptr_3.Release(); // Destructing TestClass instance...

    cout << (*ptr_2).Val << endl; // 511
    return 0; // Destructing TestClass instance... (Along with ptr_2 as the last owner)
}