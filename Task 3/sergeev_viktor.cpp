#include <cstdint>
#include <cstdio>
#include <cstddef>

class CTestObject
{
    int V;
public:
    CTestObject(const CTestObject& T)
    {
        V = T.V;

        printf("Test object has been copied %d\r\n", V);
    }

    CTestObject& operator=(const CTestObject& T)
    {
        V = T.V;
        printf("Test object has been assignmented %d\r\n", V);
        return *this;
    }

    CTestObject(int T) : V(T)
    {
        printf("Test object has been created %d\r\n", V);

    }

    ~CTestObject()
    {
        printf("Test object has been destroyed %d\r\n", V);
    }

    int Value() const
    {
        return V;
    }
};

class SmartPtr
{
    CTestObject* Ptr{};
    size_t* Counter{};

    void Attach(const SmartPtr& other)
    {
        Ptr = other.Ptr;
        Counter = other.Counter;
        (*Counter)++;
        printf("Attached: (%d, %lu)\r\n", Ptr->Value(), *Counter);
    }

    void Init(CTestObject* V)
    {
        Ptr = V;
        Counter = new size_t();
        *Counter = 1;

        printf("Created: (%d, %lu)\r\n", Ptr->Value(), *Counter);
    }
public:
    explicit SmartPtr(CTestObject* V)
    {
        Init(V);
    }

    SmartPtr(const SmartPtr& Other)
    {
        Attach(Other);
    }

    SmartPtr& operator=(const SmartPtr& Other)
    {
        Release();
        Attach(Other);
        return *this;
    }

    ~SmartPtr()
    {
        printf("Decondtructor: (%d, %lu)\r\n",Ptr->Value(), *Counter );
        Release();

    }

    void Release()
    {
        if (Ptr)
        {
            printf("Released: (%d, %lu)\r\n", Ptr->Value(), *Counter);
            (*Counter)--;
            if ((*Counter) == 0)
            {
                printf("Deleted: %lu\r\n", Ptr);
                delete Ptr;
                delete Counter;
            }
            Ptr = 0;
            Counter = 0;
        }
    }
    CTestObject operator*() const
    {
        return *Ptr;
    }

    CTestObject* operator->() const
    {
        return Ptr;
    }

    CTestObject* Get() const
    {
        return Ptr;
    }

    void Set(CTestObject* V)
    {
        Release();
        Init(V);
    }

};

int main() {
    auto Ptr = SmartPtr(new CTestObject(7));
    auto Ptr1 = Ptr;
    Ptr.Release();

    Ptr.Set(new CTestObject(8));
    auto Ptr2 = Ptr;

    Ptr = Ptr1;

    printf("----------------------------\r\n");
    printf("Value through `*`: %d\r\n", (*Ptr).Value());
    printf("Value through `->`: %d\r\n", Ptr->Value());
    printf("Value through `Get()`: %d\r\n", Ptr.Get()->Value());
    printf("----------------------------\r\n");

    return 0;
}
