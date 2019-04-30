#include <iostream>


template <class T>
class CSimpleSmartPtr
{
	T* Ptr;
	size_t* Counter;

	void Attach(const CSimpleSmartPtr& Other)
	{
		Counter = Other.Counter;
		(*Counter)++;
		Ptr = Other.Ptr;
	}

public:

	CSimpleSmartPtr()
	{
		Ptr = 0;
		Counter = 0;
	}

	explicit CSimpleSmartPtr(T* V) : Ptr(V)
	{
		Counter = new size_t();
		*Counter = 1;
	}

	void Set(const CSimpleSmartPtr& Other)
	{
		Release();
		Attach(Other);
	}

	T* operator->() const
	{
		return Ptr;
	}

	CSimpleSmartPtr(const CSimpleSmartPtr& Other)
	{
		Attach(Other);
	}

	CSimpleSmartPtr& operator=(const CSimpleSmartPtr& Other)
	{
		Release();
		Attach(Other);
		return *this;
	}

	~CSimpleSmartPtr()
	{
		Release();
	}

	void Release()
	{
		if (Ptr)
		{
			(*Counter)--;
			if ((*Counter) == 0)
			{
				delete Ptr;
				delete Counter;
			}
			Ptr = 0;
			Counter = 0;
		}
	}

	operator T* ()
	{
		return Ptr;
	}

	operator void* ()
	{
		return Ptr;
	}

	T* Get() const
	{
		return Ptr;
	}

	explicit operator bool()
	{
		return Ptr != 0;
	}
};

class CTestObject
{

public:
	int X;
	CTestObject(const CTestObject& Other)
	{
		X = Other.X;
		printf("New Test Object %d\n", X);
	}

	CTestObject()
	{
		X = 0;
		printf("New Test Object 0\n");
	}

	CTestObject(int X1) : X(X1)
	{
		printf("New Test Object %d\n", X);
	}

	~CTestObject()
	{
		printf("Deleted Test Object, %d\n", X);
	}
};

void TestCanDelete()
{
	std::cout << "TestSmartPtrCanDelete\n";
	auto obj = new CTestObject();
	auto ptr = CSimpleSmartPtr<CTestObject>(obj);
}

void TestRelease()
{
	std::cout << "TestRelease\n";
	auto obj = new CTestObject();
	auto ptr = CSimpleSmartPtr<CTestObject>(obj);
	ptr.Release();
	auto newObj = new CTestObject(1);
	auto newPtr = CSimpleSmartPtr<CTestObject>(newObj);
}

void TestReleaseNoObject()
{
	std::cout << "TestReleaseNoObject\n";
	auto ptr = CSimpleSmartPtr<CTestObject>();
	ptr.Release();
}

void TestSet()
{
	std::cout << "TestSet\n";
	auto obj = new CTestObject();
	auto obj1 = new CTestObject(1);
	auto ptr = CSimpleSmartPtr<CTestObject>(obj);
	auto ptr1 = CSimpleSmartPtr<CTestObject>(obj1);
	ptr1.Set(ptr);
}

void TestOperator1()
{
	std::cout << "TestOperator1\n";
	auto obj = new CTestObject();
	auto obj1 = new CTestObject(1);
	auto ptr = CSimpleSmartPtr<CTestObject>(obj);
	auto ptr1 = CSimpleSmartPtr<CTestObject>(obj1);
	ptr1 = ptr;
}

void TestOperator2()
{
	std::cout << "TestOperator2\n";
	auto obj = new CTestObject();
	auto ptr = CSimpleSmartPtr<CTestObject>(obj);
	std::cout << ptr->X << std::endl;
}

void TestOperator3()
{
	std::cout << "TestOperator3\n";
	auto obj = new CTestObject();
	auto ptr = CSimpleSmartPtr<CTestObject>(obj);
	std::cout << (*ptr).X << std::endl;
}


int main()
{
	TestCanDelete();
	TestRelease();
	TestReleaseNoObject();
	TestSet();
	TestOperator1();
	TestOperator2();
	TestOperator3();
}