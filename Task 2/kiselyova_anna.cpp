#include "pch.h"
#include <iostream> 
#include <cmath>

using namespace std;

class Chunk
{
public:
	int* ptr;
	bool is_free;
	int start;
	size_t size;
	Chunk* prev;
	Chunk* next;
	Chunk* left;
	Chunk* right;
	Chunk* parent;
	
	Chunk(size_t _size, Chunk* _parent)
	{
		size = _size;
		ptr = (int*)malloc(size);
		is_free = true;
		parent = _parent;
		prev = nullptr;
		next = nullptr;
		left = nullptr;
		right = nullptr;
	}
	
	void Split()
	{
		left = new Chunk(size / 2, this);
		right = new Chunk(size / 2, this);
		right->next = next;
		if (prev)
			prev->next = left;
		if (next)
			left->prev = prev;
		left->next = right;
		right->prev = left;
		is_free = false;
	}

	void Unite()
	{
		prev = left->prev;
		next = right->next;
		left = right->next;
		left->~Chunk();
		right->~Chunk();
	}

	~Chunk()
	{
		free(ptr);
		is_free = true;
	}
};

class Buddy
{
public:
	size_t size;
	Chunk* first;

	Buddy(size_t size)
	{
		this->size = size;
		size_t pointer = pow(2, ceil(log(size) / log(2)));
		first = new Chunk(size, nullptr);
	}

	void* Alloc(size_t size)
	{
		Chunk* current = first;
		size_t safe_size = pow(2, ceil(log(size) / log(2)));
		int* pointer = current->ptr;

		while (!(current->size == safe_size && current->is_free))
		{
			if (!current->is_free || (log2(current->size) < ceil(log2(size))))
			{
				current = current->next;
				pointer = current->ptr;
			}
			else
			{
				current->Split();
				if (current == first)
					first = current->left;
				current = current->left;
				pointer = current->ptr;
			}
			if (!current)
				throw new exception();
		}

		current->is_free = false;
		return pointer;
	}

	void Free(void * ptr)
	{
		Chunk* current = first;
		while (current->ptr != ptr)
			current = current->next;
		while (true)
		{
			current->is_free = true;
			current = current->parent;
			Chunk* left = current->left;
			Chunk* right = current->right;
			if (left->is_free && right->is_free)
			{
				if (current->left == first)
					first = current;
				current->Unite();
			}
			else
				break;
		}
	}

	void Dump()
	{
		Chunk* current = first;
		while (current)
		{
			cout << "Size: " << current->size << ", is_Free: " << current->is_free << ", ptr: " << current->ptr << endl;
			current = current->next;
		}
	}

	~Buddy()
	{
		free(first);
	}
};


int main()
{
	auto allocator = new Buddy(32);
	auto first = allocator->Alloc(16);
	auto second = allocator->Alloc(8);
	auto third = allocator->Alloc(8);
	allocator->Dump();
	cout << endl;
	allocator->Free(first);
	allocator->Free(third);
	allocator->Dump();
}