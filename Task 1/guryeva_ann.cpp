#include <iostream>
#include <stdio.h>
#include <cstdlib>
#include <stdlib.h>

class LinearAllocator {
	size_t _size;
	size_t _current_pos;
	void* _start = nullptr;
	LinearAllocator* next = nullptr;

public:
	LinearAllocator(size_t size = 500) {
		_size = size;
		_current_pos = 0;
		_start = malloc(size);
	}

	void* Allocate(size_t size) {
		if (size > _size)
			return nullptr;
		if (size > _size - _current_pos)
		{
			if (next == nullptr)
				next = new LinearAllocator(_size);
			return (*next).Allocate(size);
		}

		void* mem_pointer = (char*)_start + _current_pos;
		_current_pos += size;

		return mem_pointer;
	}

	void Free() {
		if (next != nullptr)
		{
			(*next).Free();
			delete next;
		}
		free(_start);
		_start = nullptr;
		_size = 0;
		_current_pos = 0;
	}
};

void TestLinearAllocator() {
	LinearAllocator al = LinearAllocator(20);
	int* ar = (int *)al.Allocate(sizeof(int) * 4);
	int* arr = (int *)al.Allocate(sizeof(int) * 4);

	arr[0] = 1;
	arr[1] = 5;
	arr[2] = 25;
	arr[3] = 125;

	for (int i = 0; i < 4; i++)
		printf("%i. %i\n", i, arr[i]);
	al.Free();
}

struct StackHeader {
	StackHeader* prev_header;
	size_t mem_size;
};

class StackAllocator {
	size_t _size;
	size_t _current_pos;
	void* _start = nullptr;
	StackHeader* _prev_header = nullptr;
	StackAllocator* next = nullptr;

public:
	StackAllocator(size_t size = 500) {
		_size = size;
		_current_pos = 0;
		_start = malloc(size);
	}

	void* Allocate(size_t size) {
		size += sizeof(_prev_header);
		if (size > _size)
			return nullptr;
		if (size > _size - _current_pos)
		{
			if (next == nullptr)
				next = new StackAllocator(_size);
			return (*next).Allocate(size - sizeof(_prev_header));
		}

		StackHeader* new_header = new ((char*)_start + _current_pos) StackHeader{ _prev_header, size - sizeof(_prev_header) };
		_prev_header = new_header;

		void* mem_pointer = (char*)_start + _current_pos + sizeof(StackHeader);
		_current_pos += size;
		return mem_pointer;
	}

	void FreeLast() {
		if (_prev_header == nullptr)
			return;

		_current_pos -= _prev_header->mem_size + sizeof(_prev_header);

		auto _temp_prev_header = _prev_header->prev_header;
		_prev_header = _temp_prev_header;
	}

	void FreeAll() {
		if (next != nullptr)
		{
			(*next).FreeAll();
			delete next;
		}
		while (_prev_header != nullptr)
			FreeLast();

		free(_start);
		_start = nullptr;
		_size = 0;
		_current_pos = 0;
	}
};

void TestStackAllocator() {
	StackAllocator st_al = StackAllocator(30);
	int* st_ar = (int *)st_al.Allocate(sizeof(int) * 4);
	st_al.FreeLast();
	int* st_arr = (int *)st_al.Allocate(sizeof(int) * 4);
	st_ar[0] = 1;
	st_ar[1] = 5;
	st_ar[2] = 25;
	st_ar[3] = 125;

	for (int i = 0; i < 4; i++)
		printf("%i. %i\n", i, st_arr[i]);
	st_al.FreeAll();
}

struct BlockHeader {
	BlockHeader* next;
	size_t size;
	bool is_free;
};


class ListAllocator {
	size_t _size;
	void* _start;

public:
	ListAllocator(size_t size = 500) {
		_size = size;
		_start = malloc(size);
		new ((void *)_start) BlockHeader{ nullptr, size, true };
	}

	void* Allocate(size_t size) {
		void* cur_pos = _start;
		while (cur_pos != nullptr) {
			BlockHeader* header = (struct BlockHeader*)cur_pos;
			if (header->is_free && header->size - sizeof(BlockHeader) >= size) {
				header->is_free = false;
				if (header->size - size - sizeof(BlockHeader) <= sizeof(BlockHeader))
					return (char*)cur_pos + sizeof(BlockHeader);
				BlockHeader* new_header = new ((char*)cur_pos + sizeof(BlockHeader) + size) BlockHeader{ header->next, header->size - size - sizeof(BlockHeader), true };
				header->size = size + sizeof(BlockHeader);
				header->next = new_header;
				return (char*)cur_pos + sizeof(BlockHeader);
			}
			cur_pos = header->next;
		}

		return nullptr;
	}

	void Free(void* freed_ptr) {
		void* header_ptr = (char*)freed_ptr - sizeof(BlockHeader);
		BlockHeader* header = (struct BlockHeader*)header_ptr;
		header->is_free = true;
		Merge();
	}

	void Merge() {
		BlockHeader* prev = ((struct BlockHeader*)_start);
		BlockHeader* cur_pos = prev->next;
		while (cur_pos != nullptr) {
			if (cur_pos->is_free && prev->is_free)
			{
				prev->size += cur_pos->size;
				prev->next = cur_pos->next;
				cur_pos = cur_pos->next;
				continue;
			}
			prev = cur_pos;
			cur_pos = cur_pos->next;
		}
	}
};

void TestListAllocator() {
	ListAllocator lt_al = ListAllocator();
	int* first = (int *)lt_al.Allocate(sizeof(int) * 4);
	int* second = (int *)lt_al.Allocate(sizeof(int) * 4);
	int* third = (int *)lt_al.Allocate(sizeof(int) * 4);

	lt_al.Free(first);
	lt_al.Free(second);

	int* fourth = (int *)lt_al.Allocate(sizeof(int) * 5);

	first[0] = 1;
	first[1] = 5;
	first[2] = 25;
	first[3] = 125;
	for (int i = 0; i < 4; i++)
		printf("%i. %i\n", i, fourth[i]);
}



int main() {
	printf("Linear Allocator Test\n");
	TestLinearAllocator();
	

	printf("\n----------------------\n");
	printf("Stack Allocator Test\n");
	TestStackAllocator();

	printf("\n----------------------\n");
	printf("List Allocator Test\n");
	TestListAllocator();
}