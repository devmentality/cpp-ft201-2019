#include <iostream>
using namespace std;

template <class T>
class CounterStrategy
{
protected:
	size_t *Count = NULL;

	T* AddRef(const CounterStrategy& other, T* ptr)
	{
		Count = other.Count;
		(*Count)++;
		return ptr;
	}

	void InitRef()
	{
		Count = new size_t;
		(*Count) = 1;
	}

	size_t ReleasePointer()
	{
		if (Count == NULL)
			return 0;

		(*Count)--;
		size_t r = (*Count);
		if ((*Count) == 0)
		{
			delete Count;
		}
		Count = NULL;
		return r;
	}
};

template <class T, template <class T> class HandlingStrategy = CounterStrategy>
class SmartPointer : HandlingStrategy<T>
{
private:
	T * ptr;
public:
	SmartPointer() { ptr = NULL; }

	explicit SmartPointer(T *arg_ptr)
	{
		ptr = arg_ptr;
		HandlingStrategy<T>::InitRef();
	}

	SmartPointer(const SmartPointer &other)
	{
		ptr = HandlingStrategy<T>::AddRef(other, other.ptr);
	}

	void Release()
	{
		if (ptr != NULL)
		{
			if (HandlingStrategy<T>::ReleasePointer() == 0)
				delete ptr;
			ptr = NULL;
		}
	}

	void Set(T *arg_ptr)
	{
		Release();
		ptr = arg_ptr;
		HandlingStrategy<T>::InitRef();
	}

	SmartPointer &operator=(const SmartPointer &other)
	{
		Release();
		ptr = HandlingStrategy<T>::AddRef(other, other.ptr);
	}

	T* Get()
	{
		return ptr;
	}

	T* operator->()
	{
		return ptr;
	}

	const T& operator *() const
	{
		return *ptr;
	}

	T& operator *()
	{
		return *ptr;
	}

	~SmartPointer()
	{
		Release();
	}
};

class TestObject
{
public:
	int value;

	TestObject(int _value)
	{
		value = _value;
		cout << "Test object created with value " << value << endl;
	}

	~TestObject()
	{
		cout << "Test object with value " << value << " is deleted" << endl;
	}
};

void TestSmartPointer_ReleasesObjectWhenNoRefs()
{
	cout << "---------------TestSmartPointer_ReleasesObjectWhenNoRefs---------------" << endl;
	
	SmartPointer<TestObject> pointer1(new TestObject(10));
	pointer1.Release();
	
	cout << "---------------Test end---------------" << endl << endl;
}

void TestSmartPointer_DoesntReleaseObjectWhenHasRefs()
{
	cout << "---------------TestSmartPointer_DoesntReleasesObjectWhenHasRefs---------------" << endl;

	TestObject *obj = new TestObject(10);
	SmartPointer<TestObject> pointer1(obj);
	SmartPointer<TestObject> pointer2(pointer1);
	pointer1.Release();

	cout << "---------------Test end---------------" << endl << endl;
}

void TestSmartPointer_GetReturnsPointerToObject()
{
	cout << "---------------TestSmartPointer_GetReturnsPointerToObject---------------" << endl;

	SmartPointer<TestObject> pointer1(new TestObject(10));
	cout << pointer1.Get()->value << endl;

	cout << "---------------Test end---------------" << endl << endl;
}

void TestSmartPointer_SetEnablesTrackingObject()
{
	cout << "---------------TestSmartPointer_SetEnablesTrackingObject---------------" << endl;

	SmartPointer<TestObject> pointer1;
	pointer1.Set(new TestObject(10));
	
	cout << "---------------Test end---------------" << endl << endl;
}

void TestSmartPointer_AssignmentEnablesTrackingObject()
{
	cout << "---------------TestSmartPointer_AssignmentEnablesTrackingObject---------------" << endl;

	SmartPointer<TestObject> pointer1(new TestObject(10));
	SmartPointer<TestObject> pointer2 = pointer1;
	pointer1.Release();

	cout << "---------------Test end---------------" << endl << endl;
}


void TestSmartPointer_ArrowOperator()
{
	cout << "---------------TestSmartPointer_ArrowOperator---------------" << endl;

	SmartPointer<TestObject> pointer1(new TestObject(10));
	cout << pointer1->value << endl;

	cout << "--------------Test end--------------" << endl << endl;
}

void TestSmartPointer_StarOperator()
{
	cout << "--------------TestSmartPointer_StarOperator--------------" << endl;

	SmartPointer<TestObject> pointer1(new TestObject(10));
	cout << (*pointer1).value << endl;

	cout << "--------------Test end--------------" << endl << endl;
}



int main()
{
	TestSmartPointer_ReleasesObjectWhenNoRefs();
	TestSmartPointer_DoesntReleaseObjectWhenHasRefs();
	TestSmartPointer_GetReturnsPointerToObject();
	TestSmartPointer_SetEnablesTrackingObject();
	TestSmartPointer_AssignmentEnablesTrackingObject();
	TestSmartPointer_ArrowOperator();
	TestSmartPointer_StarOperator();
	return 0;
}