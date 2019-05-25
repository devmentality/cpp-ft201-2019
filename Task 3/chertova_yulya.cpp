#include<iostream>
using namespace std;

template <class T>
class SmartPointer {
private:
	T *pntr;
	unsigned *ref_cnt;

public:
	SmartPointer() {
		pntr = NULL;
		ref_cnt = NULL;
	}

	~SmartPointer() {
		Release();
	}

	void Release(){
		--(*ref_cnt);
		if (ref_cnt == 0) {
			delete pntr;
			free(ref_cnt);
			ref_cnt = NULL;
			pntr = NULL;
	}
  }

	T Get() {
    return *pntr;
  }

	void Set(T *ptr) {
		pntr = ptr;
		ref_cnt = (unsigned*)malloc(sizeof(unsigned));
		*ref_cnt = 1;
	}

	SmartPointer & operator=(SmartPointer & other_ptr) {
		if (*ref_cnt > 0) {
			--(*ref_cnt);
			if (ref_cnt == 0) {
				delete pntr;
				free(ref_cnt);
				ref_cnt = NULL;
				pntr = NULL;
		}
    }
		if (this != &other_ptr) {
			pntr = other_ptr.pntr;
			ref_cnt = other_ptr.ref_cnt;
			++(*ref_cnt);
		}
		return *this;
	}

	T* operator->() {
		return Get();
	}

	T operator*() {
		return *this->pntr;
	}
};

int main() {
	SmartPointer<int> ptr_int;
	ptr_int.Set(new int(1));

	cout << ptr_int.Get() << " " << &ptr_int << " " << *ptr_int << endl;

	ptr_int.Release();
}