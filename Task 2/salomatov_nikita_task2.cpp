
#include <cstring>
#include <cstdint>
#include <cassert>
#include <iostream>
#include <Windows.h>
#include <cmath>

using namespace std;

struct Block {
	Block * next;
	Block * self;
	bool free;
	int size;
};

class  BuddyAllocator
{
public:
	Block * start;

	int count;
	int size;

	Block * free_Array[32];

	int powof2(int val)
	{
		int i = 0;
		while (val >>= 1)
			i++;
		return i;
	}

	void add(Block * block, int l)
	{
		block->next = free_Array[l];
		free_Array[l] = block;
	}

	void remove(Block * block, int l)
	{
		if (!block)
			return;
		Block * new_block = free_Array[l];

		if (block == new_block) {
			free_Array[l] = block->next;
			return;
		}

		while (new_block) {
			if (new_block->next == block)
				new_block->next = block->next;
			new_block = new_block->next;
		}
	}

	Block * divide(Block * block, int k)
	{
		int size = ((block->size + sizeof(Block)) / 2) - sizeof(Block);
		remove(block, k);
		block->free = true;
		block->size = size;
		block->self = block;
		Block * buddy;
		buddy = (Block *)((uint8_t *)block + sizeof(Block) + size);
		buddy->free = true;
		buddy->size = size;
		buddy->self = buddy;
		add(buddy, k - 1);
		return block;
	}
	

	Block * merge(Block * block)
	{
		Block * buddy;
		int l = powof2(block->size + sizeof(Block));
		long addr = ((uint8_t *)block - (uint8_t *)start);
		buddy = (Block *)((addr ^= (1 << l)) + (size_t)start);
		if (!buddy->free || buddy->size != block->size)
			return NULL;

		if (block > buddy) {
			Block * x = block;
			block = buddy;
			buddy = x;
		}

		remove(block, l);
		remove(buddy, l);

		block->size = block->size * 2 + sizeof(Block);
		block->free = true;
		block->self = block;
		add(block, l + 1);
		return block;
	}

	explicit  BuddyAllocator(int memSize)
	{
		static uint8_t memPool[3 * 1048576];
		int p = powof2(memSize);
		memSize = 1 << p;
		start = (Block *)memPool;
		start->size = memSize - sizeof(Block);
		start->free = true;
		start->self = start;
		count = 0;
		size = start->size;

		for (int i = 0; i < 32; i++)
			free_Array[i] = NULL;

		add(start, p);
	}

	void* Alloc(size_t size)
	{
		int l = powof2(size + sizeof(Block)) + 1;

		while (!free_Array[l] && l < 32)
			l++;

		if (l >= 32)
			return NULL;

		Block * new_block;
		new_block = free_Array[l];

		remove(new_block, l);

		while ((new_block->size + sizeof(Block)) / 2 >= size + sizeof(Block)) {
			new_block = divide(new_block, l);
			l--;
		}

		new_block->free = false;
		new_block->self = new_block;

		count++;

		return new_block + 1;
	}
	
	void Free(void * blk)
	{
		Block * new_block = (Block *)((uint8_t *)blk - sizeof(Block));
		if (new_block->self == new_block) {
			new_block->free = true;
			add(new_block, powof2(new_block->size + sizeof(Block)));
			while (new_block)
				if (new_block->size == size)
					break;
				else
					new_block = merge(new_block);
			count--;
		}
	}

	void Dump()
	{
		cout << count << "\n";
	}
};


int main(int argc, char * argv[])
{
	uint8_t * p0, *p1, *p2, *p3, *p4;
	int	pendingBlk;
	BuddyAllocator alloc = BuddyAllocator(2097152);
	alloc.Dump();
	
	void* g1 = alloc.Alloc(100);
	alloc.Dump();
	
	void* g2 = alloc.Alloc(33);
	alloc.Dump();
	
	void* g3 = alloc.Alloc(16);
	void* g4 = alloc.Alloc(8);
	alloc.Dump();
	void* g5 = alloc.Alloc(6);
	void* g6 = alloc.Alloc(5);
	alloc.Dump();
	void* g7 = alloc.Alloc(8);
	alloc.Free(g1);
	alloc.Dump();
	alloc.Free(g3);
	alloc.Free(g2);
	alloc.Dump();
	Sleep(5000);
	
}

