#include "pch.h"
#include<iostream>

using namespace std;


size_t GetPowOfTwo(int number)
{
	auto power = log2(number);
	if (pow(2, power) >= number)
		return pow(2, power);
	return pow(2, power + 1);
}

class Block {
public:
	unsigned int *pointer;
	size_t size;
	bool is_empty;
	Block *next;
	Block *previous;
	Block *first_block;
	Block *second_block;
	Block *parent;

	Block(size_t new_size, Block* parent)
	{
		pointer = (unsigned int*)malloc(size);
		is_empty = true;
		size = new_size;
		parent = parent;
		next = nullptr;
		previous = nullptr;
		first_block = nullptr;
		second_block = nullptr;
	}

	void Split()
	{
		auto size_half = size / 2;
		first_block = new Block(size_half, this);
		second_block = new Block(size_half, this);
		second_block->next = next;
		if (next != nullptr)
			next->previous = second_block;
		if (previous != nullptr)
			previous->next = first_block;
		first_block->previous = previous;
		first_block->next = second_block;
		second_block->previous = first_block;
		next = nullptr;
		previous = nullptr;
		is_empty = false;
	}

	~Block()
	{
		is_empty = true;
		free(pointer);
		first_block = nullptr;
		second_block = nullptr;
		previous = nullptr;
		next = nullptr;
		parent = nullptr;
	}

	void Unite()
	{
		previous = first_block->previous;
		next = second_block->next;
		first_block = second_block->next;
		first_block->~Block();
		second_block->~Block();
		first_block = nullptr;
		second_block = nullptr;
	}
};


class BuddyAllocator {
private:
	size_t size;
	Block* first_block;

public:
	BuddyAllocator(size_t new_size) {
		size = new_size;
		size_t index = GetPowOfTwo(new_size);
		first_block = new Block(new_size, nullptr);
	}

	void Free(void *pointer) {
		auto current_block = first_block;
		while (current_block->pointer != pointer)
			current_block = current_block->next;
		while (true)
		{
			current_block->is_empty = true;
			current_block = current_block->parent;
			if (current_block == nullptr || !current_block->first_block->is_empty || !current_block->second_block->is_empty)
				break;
			else
			{
				if (current_block->first_block == first_block)
					first_block = current_block;
				current_block->Unite();
			}
		}
	}

	void* Alloc(size_t size) {
		auto current_block = first_block;
		if (current_block == nullptr)
			throw invalid_argument("Закончилась память");
		while (current_block->size != size || !current_block->is_empty)
		{
			if (!current_block->is_empty || log2(current_block->size) <= ceil(log2(size)))
				current_block = current_block->next;

			else {
				current_block->Split();
				if (current_block == first_block)
					first_block = current_block->first_block;
				current_block = current_block->first_block;
			}
			if (current_block == nullptr)
				throw invalid_argument("Закончилась память");
		}
		current_block->is_empty = false;
		return current_block->pointer;
	}

	void Dump() {
		auto current_block = first_block;
		while (current_block != nullptr) {
			cout << "Адрес: " << current_block->pointer << "\n";
			cout << "Размер: " << current_block->size << "\n";
			if (current_block->is_empty)
				cout << "Пустой блок" << "\n";
			else
				cout << "Занятый блок" << "\n";
			current_block = current_block->next;
		}
	}
};


int main() {
	setlocale(0, "rus");
	auto buddy_allocator = BuddyAllocator(512);
	auto address = buddy_allocator.Alloc(64);
	buddy_allocator.Alloc(128);
	cout << "\n";
	buddy_allocator.Dump();
	cout << "___________" << "\n\n";
	buddy_allocator.Free(address);
	buddy_allocator.Dump();
	cout << "___________" << "\n\n";
	auto buddy_allocator2 = BuddyAllocator(32);
	buddy_allocator2.Alloc(2);
	auto address2 = buddy_allocator2.Alloc(2);
	buddy_allocator2.Alloc(16);
	buddy_allocator2.Dump();
	cout << "___________" << "\n\n";
	buddy_allocator2.Free(address2);
	buddy_allocator2.Dump();
}
