#include "pch.h"
#include <cmath>
#include <iostream>
using namespace std;

const unsigned int BLOCK_SIZE = 64;

class BuddyAllocator {
public:
	struct Header
	{
		bool free;
		unsigned int level = 31;
	};

#pragma pack(push, 1)
	struct Block : public Header
	{
		struct LinkedBlock
		{
			Block *next;
			Block *previous;
		};
		union
		{
			LinkedBlock linked_block;
			char available_part[BLOCK_SIZE - sizeof(Header)];
		};
	};
#pragma pack(pop)

private:
	unsigned int power;
	unsigned int blocks_count;
	Block *pool;
	Block *blocks_list;

	void free_link(Block &block) {
		if (block.linked_block.next)
			block.linked_block.next->linked_block.previous = block.linked_block.previous;
		if (block.linked_block.previous)
			block.linked_block.previous->linked_block.next = block.linked_block.next;
		block.linked_block.next = block.linked_block.previous = &block;
	}

	void insert(Block &source_block, Block &block) {
		block.linked_block.next = source_block.linked_block.next;
		block.linked_block.previous = &source_block;
		if (source_block.linked_block.next)
			source_block.linked_block.next->linked_block.previous = &block;
		source_block.linked_block.next = &block;
	}

	Block &Buddy(Block &block) {
		auto address = (unsigned int)(reinterpret_cast<unsigned int>(&block) + pow(2, block.level));
		return *(reinterpret_cast<Block *>(address));
	}

public:
	BuddyAllocator(size_t size) {
		power = get_two_power(size);
		blocks_count = pow(2, power) / BLOCK_SIZE;
		pool = new Block[blocks_count + power + 1];
		blocks_list = pool + blocks_count;
		for (auto i = 0; i < power; i++) {
			blocks_list[i].linked_block.next = &blocks_list[i];
			blocks_list[i].linked_block.previous = &blocks_list[i];
		}
		auto &head = pool[0];
		head.free = true;
		head.level = power;
		insert(blocks_list[power], head);
	}

	void *Alloc(size_t size) {
		auto two_power = get_two_power(size + sizeof(Header));
		auto i = two_power;
		while (i < power && blocks_list[i].linked_block.next == &blocks_list[i])
			i++;
		if (i > power)
			throw invalid_argument("Закончилась память");
		auto &block = *blocks_list[i].linked_block.next;
		free_link(block);
		while (block.level > two_power) {
			block.level -= 1;
			auto &buddy = Buddy(block);
			buddy.free = true;
			buddy.level = block.level;
			insert(blocks_list[buddy.level], buddy);
		}
		block.free = false;
		return block.available_part;
	}

	void Free(void *ptr) {
		auto &block = *reinterpret_cast<Block *>(
			reinterpret_cast<Header *>(ptr) - 1);
		if (&block < pool || &block >= pool + blocks_count)
			throw invalid_argument("Такого указателя не существует");
		block.free = true;
		auto *block_pointer = &block;
		for (block_pointer; block_pointer->level < power; block_pointer->level += 1) {
			auto &buddy = Buddy(*block_pointer);
			if (!buddy.free || buddy.level != block_pointer->level)
				break;
			free_link(buddy);
			if (&buddy < block_pointer)
				block_pointer = &buddy;
		}
		insert(blocks_list[block_pointer->level], *block_pointer);
	}
};

unsigned int get_two_power(int power)
{
	auto result = 1;
	for (auto i = 0; i < power; i++)
		result *= 2;
	return result;
}

int main()
{
	setlocale(0, "rus");
	return 0;
}
