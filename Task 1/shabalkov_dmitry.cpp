#include <iostream>
#include <algorithm>
using namespace std;

class Arr
{
private:
	size_t _size;
	int *_arr;
	void _resize(size_t newSize)
	{
		int *newArr = new int[newSize];
		for (size_t i = 0; i < _size; i++)
			newArr[i] = _arr[i];
		delete[] _arr;
		_arr = newArr;
		_size = newSize;
	}
public:
	Arr(size_t size)
	{
		_size = size;
		_arr = new int[_size];
	}

	Arr()
	{
		_size = 64;
		_arr = new int[_size];
	}

	void SetValue(size_t index, int value)
	{
		if (index < 0)
			throw "Index out of range!";
		if (index >= _size)
			_resize(max(_size * 2, index / _size + 1));
		_arr[index] = value;

	}

	int GetValue(size_t index)
	{
		if (index >= 0 && index < _size)
			return _arr[index];
		throw "Index out of range!";
	}
};

class Stack
{
private:
	size_t _size;
	size_t _pointer;
	int *_arr;
public:
	Stack(size_t size)
	{
		_size = size;
		_pointer = 0;
		_arr = new int[_size];
	}

	Stack()
	{
		_size = 64;
		_pointer = 0;
		_arr = new int[_size];
	}

	void Push(int value)
	{
		if (_pointer < _size)
		{
			_arr[_pointer] = value;
			_pointer++;
		}
	}

	int Pop()
	{
		if (_pointer > 0)
			return _arr[--_pointer];
		throw "Invalid operation!";
	}
};

class ListElement
{
public:
	ListElement * Next;
	int value;
	ListElement(int value)
	{
		this->value = value;
		Next = nullptr;
	}
};

class List
{
public:
	ListElement * Head;
	List()
	{
		Head = nullptr;
	}

	void Add(int value)
	{
		if (Head == nullptr)
			Head = new ListElement(value);
		else
		{
			ListElement *currentElement = Head;
			while (currentElement->Next != nullptr)
				currentElement = currentElement->Next;
			currentElement->Next = new ListElement(value);
		}
	}

	void Remove(int value)
	{
		ListElement *currentElement = Head;
		ListElement *previousElement = nullptr;
		if (Head->value == value)
		{
			Head = Head->Next;
			return;
		}
		while (currentElement->value != value && currentElement != nullptr)
		{
			previousElement = currentElement;
			currentElement = currentElement->Next;
		}
		if (currentElement == nullptr)
			throw "Value not found!";
		previousElement->Next = currentElement->Next;

	}
};

class LinearAllocator
{
private:
	size_t _size;
	size_t _offset;
	void *_memory;
public:
	LinearAllocator(size_t size)
	{
		_size = size;
		_offset = 0;
		_memory = malloc(_size);
	}

	void* Allocate(size_t size)
	{
		if (_offset + size > _size)
			throw "Invalid operation!";
		size_t* result = (size_t*)_memory + _offset;
		_offset += size;
		return result;
	}

	void Free()
	{
		_offset = 0;
	}
};

class StackAllocator
{
private:
	size_t _size;
	size_t _offset;
	void *_memory;
public:
	StackAllocator(size_t size)
	{
		_size = size;
		_offset = 0;
		_memory = malloc(_size);
	}

	void* Allocate(size_t size)
	{
		if (_offset + size > _size)
			throw "Invalid operation!";
		size_t* result = (size_t*)_memory + _offset;
		_offset += size;
		return result;
	}

	void Free()
	{
		_offset = 0;
	}

	void Free(void* ref)
	{
		size_t refSize = (size_t)ref;
		if (refSize > (size_t)_memory + _offset || refSize < (size_t)_memory)
			throw "Invalid operation!";
		_offset = refSize - (size_t)_memory;
	}
};

