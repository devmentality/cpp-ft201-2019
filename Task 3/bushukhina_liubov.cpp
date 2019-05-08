#include<iostream>
using namespace std;

template <typename T>
class SmartPointer {

private:
	T *pointer;
	unsigned *ref_counter;	

	void AddRef() {
		(*ref_counter)++;
	}

	void DeleteRef() {
		(*ref_counter)--;
	}

public:
	SmartPointer() {
		pointer = nullptr;
		ref_counter = nullptr;
	}

	SmartPointer(const SmartPointer<T>& other) {
		pointer = other.pointer;
		ref_counter = other.ref_counter;
		AddRef();
	}

	T Get() {
		return *pointer;
	}

	void Set(T *ptr) {
		pointer = ptr;
		ref_counter = (unsigned*)malloc(sizeof(unsigned));
		*ref_counter = 1;
	}

	~SmartPointer() {
		Release();
	}

	void Release(){
		DeleteRef();
		if (ref_counter == 0) {
			delete pointer;
			free(ref_counter);
			pointer = nulptr;
			ref_counter = nullptr;
		}
	}

	T operator*() {
		if (pointer == nullptr)
			throw exception("Object is not set. Impossibe to use operator*");
		return *pointer;
	}
	
	T* operator->() {
		return pointer;
	}

	SmartPointer & operator=(SmartPointer & other) {
		if (*ref_counter > 0)
			Release();

		if (this != &other) {
			pointer = other.pointer;
			ref_counter = other.ref_counter;
			AddRef();
		}
		return *this;
	}

	unsigned GetRefCount() {
		//just for test
		return *ref_counter;
	}
};

int main() {
	SmartPointer<char> pointer1;
	SmartPointer<char> pointer2;
	pointer1.Set(new char('a'));
	pointer2.Set(new char('b'));

	cout << "Get   " << "&    " << "*   " << "ref count" << endl;
	cout << pointer1.Get() << " " << &pointer1 << " " << *pointer1 << " " << pointer1.GetRefCount() <<endl;
	cout << pointer2.Get() << " " << &pointer2 << " " << *pointer2 << " " << pointer2.GetRefCount() <<endl;
	SmartPointer<char> pointer3 = pointer1;
	cout << pointer3.Get() << " " << &pointer3 << " " << *pointer3 << " " <<pointer3.GetRefCount() << endl;
	pointer1 = pointer2;
	cout << pointer1.Get() << " " << &pointer1 << " " << *pointer1 << " " << pointer1.GetRefCount() << endl;
	
	pointer3.Release();
	pointer2.Release();
	pointer1.Release();
	
	system("pause");
	return 0;
}