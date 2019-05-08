#include<iostream>
using namespace std;

template <typename T>
class SmartPointer {
private:
	T *pointer;
	size_t *ref_counter;

public:
	SmartPointer() {
		pointer = nullptr;
		ref_counter = nullptr;
	}

	~SmartPointer() {
		Release();
	}

	void Set(T *ptr) {
		pointer = ptr;
		auto size = sizeof(size_t);
		ref_counter = (size_t*)malloc(size);
		*ref_counter = 1;
	}

	T Get() {
		return *pointer;
	}

	void Release() {
		if (!pointer) return;
		(*ref_counter)--;
		if (*ref_counter == 0) {
			delete pointer;
			free(ref_counter);
			pointer = nullptr;
			ref_counter = nullptr;
		}
	}

	SmartPointer & operator=(SmartPointer & other) {
		if (*ref_counter > 0)
			Release();
		if (this != &other) 
		{
			pointer = other.pointer;
			ref_counter = other.ref_counter;
			(*ref_counter)++;
		}
		return *this;
	}

	T operator*() {
		return *pointer;
	}

	T* operator->() {
		return pointer;
	}

	void Show()
	{
		cout << "object " << Get() << endl;
		cout << "number of references " << *ref_counter << endl;
	}
};

int main() {
	auto first = SmartPointer<int>();
	first.Set(new int(1));
	first.Show();
	cout << "----------------------------" << endl;

	auto second = SmartPointer<int>();
	second.Set(new int(2));
	second.Show();
	cout << "----------------------------" << endl;

	second = first;
	first.Show();
	cout << "----------------------------" << endl;

	second.Show();
	cout << "----------------------------" << endl;

	second.Release();
	first.Show();

	first.Release();
}
