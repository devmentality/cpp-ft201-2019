#include <iostream>

using namespace std;

template<class T>
class SmartPointer
{
public:
	size_t * count;
	T * obj;
	SmartPointer(T *obj)
	{
		this.obj = obj;
		count = new size_t();
		*count = 1;
	}

	SmartPointer()
	{
		obj = nullptr;
		count = new size_t();
		*count = 0;
	}

	~SmartPointer()
	{
		Release();
	}

	T* Get()
	{
		return obj;
	}

	void Set (T *obj)
	{
		if (this->obj != nullptr)
			Release();
		this->obj = obj;
		*count = 1;
	}

	void Release()
	{
		if (obj != nullptr && *count > 0)
		{
			(*count)--;
			if (*count == 0)
				delete obj;
		}
	}

	SmartPointer& operator= (const SmartPointer& otherPointer)
	{
		Release();
		obj = otherPointer.obj;
		count = otherPointer.count;
		(*count)++;
		return *this;
	}

	T& operator* ()
	{
		return *obj;
	}

	T* operator-> ()
	{
		return  obj;
	}

	void Dump()
	{
		cout << "Object: " << obj << endl;
		cout << "Count: " << count << endl;
	}

};

class TestClass
{
	
};

int main()
{
	SmartPointer<TestClass> *a = new SmartPointer<TestClass>();
	SmartPointer<TestClass> *b = new SmartPointer<TestClass>();
	a->Dump();
	a->Set(new TestClass());
	cout << a->Get() << endl;
	a->Dump();
	b = a;
	b->Dump();
	system("pause");
};