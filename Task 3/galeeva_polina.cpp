#include<iostream>
using namespace std;

template <typename T>
class SmartPointer {
protected:
	T *pointer;
	unsigned *refs_count;

	void remove_ref() {
		--(*refs_count);
		if (refs_count == 0) {
			delete pointer;
			free(refs_count);
			pointer = NULL;
			refs_count = NULL;
		}
	}

public:
	SmartPointer() {
		pointer = NULL;
		refs_count = NULL;
	}

	~SmartPointer() {
		remove_ref(); 
	}

	T Get() {
		return *pointer;
	}

	void Set(T *pointer) {
		this->pointer = pointer;
		refs_count = new size_t(1);
	}

	void Release(){
		remove_ref();
	}

	T operator*() {
		return *pointer;
	}
	
	T* operator->() {
		return pointer;
	}

	SmartPointer &operator=(SmartPointer & other_pointer) {
		pointer = other_pointer.pointer;
		refs_count = other_pointer.refs_count;
		(*refs_count)++;

		return *this;
	}
};

int main() {
	SmartPointer<int> ptr_int1;
	ptr_int1.Set(new int(1));
	cout << ptr_int1.Get() << "-" << &ptr_int1 << "-" << *ptr_int1 << endl;

	SmartPointer<int> ptr_int2;
	ptr_int2.Set(new int(2));
	cout << ptr_int2.Get() << "-" << &ptr_int2 << "-" << *ptr_int2 << endl;
	
	
	ptr_int1 = ptr_int2;
	cout << ptr_int2.Get() << "-" << &ptr_int2 << "-" << *ptr_int2 << endl;

	ptr_int2.Release();
	ptr_int1.Release();
}