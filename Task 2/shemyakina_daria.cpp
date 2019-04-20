#include <complex>
#include <iostream>

#pragma pack(push, 1) 
struct Block
{
	size_t level;
	bool is_free;
	union
	{
		struct
		{
			Block* next;
			Block* prev;
		};
		void* data[1];
	};
};
#pragma pack(pop)

class FreeBlocks
{
	Block * first_;
	size_t level_;

public:
	void * start;

	FreeBlocks(size_t level)
	{
		first_ = nullptr;
		level_ = level;		
	}

	bool empty()
	{
		return !first_;
	}

	Block * first()
	{
		return first_;
	}

	void add(Block* item)
	{
		item->is_free = true;
		item->level = level_;
		if (!first_)
		{
			first_ = item;
			item->next = item;
			item->prev = item;
			return;
		}
		Block *prev = first_->prev;
		item->prev = prev;
		prev->next = item;
		first_->prev = item;
		item->next = first_;
	}

	void remove(Block* item)
	{
		item->is_free = false;
		if (first_->next == first_)
		{
			first_ = nullptr;
			return;
		}
		auto prev = item->prev;
		auto next = item->next;
		prev->next = next;
		next->prev = prev;
		if (item == first_)
			first_ = next;
	}

	void dump()
	{
		if (first_ == nullptr)
			return;
		std::cout << "level: " << level_ << std::endl;
		auto current_block = first_;
		do
		{
			std::cout << "location: " << current_block;
			current_block = current_block->next;
			if (current_block != first_)
				std::cout << "; ";
		} while (current_block != first_);
		std::cout << std::endl;
	}
};

class BuddyAllocator
{
	FreeBlocks ** free_blocks_;
	void * start_ptr;
	size_t min_level = 4;
	size_t max_level;

	void allocate_memory()
	{
		size_t memory_size = pow(2, max_level);
		start_ptr = malloc(memory_size);
		auto level_count = max_level - min_level + 1;
		free_blocks_ = new FreeBlocks*[level_count];
		for (size_t i = min_level; i <= max_level; i++)
		{
			free_blocks_[i - min_level] = new FreeBlocks(i);
		}
		free_blocks_[max_level - min_level]->add(static_cast<Block*>(start_ptr));
	}

	Block * find_suitable_block(size_t size)
	{
		size_t level = 0;
		size_t block_size = 0;
		for (size_t i = min_level; i <= max_level; i++)
		{
			block_size = 1 << i;
			if (size > block_size)
				continue;
			if (free_blocks_[i-min_level]->empty())
				continue;
			level = i;
			break;
		}

		auto block = free_blocks_[level-min_level]->first();
		block_size = 1 << block->level;
		while (block_size > size)
		{
			auto next_block_size = block_size / 2;
			if (next_block_size < size || next_block_size < 1 << min_level)
				return block;
			free_blocks_[level-min_level]->remove(block);
			auto buddy = static_cast<Block*>(static_cast<void*>(static_cast<char*>(static_cast<void*>(block)) + next_block_size));
			level--;
			free_blocks_[level - min_level]->add(block);
			free_blocks_[level - min_level]->add(buddy);
			block_size = next_block_size;		
			
		}
		return nullptr;
	}

	bool block_is_first(Block * block)
	{
		auto current_block = static_cast<Block *>(start_ptr);
		size_t i = 1;
		while (true)
		{
			auto current_position = static_cast<void*>(static_cast<char*>(static_cast<void*>(current_block)) + (1 << current_block->level));
			if (current_position >= static_cast<char*>(start_ptr) + (1 << max_level))
				break;
			current_block = static_cast<Block*>(current_position);
			i++;
		}
	}

	void merge_buddies(Block * block)
	{
		while (block->level < max_level)
		{
			auto block_size = 1 << block->level;
			auto relative_position = static_cast<char*>(static_cast<void*>(block)) - static_cast<char*>(start_ptr);
			auto is_first = (relative_position / block_size) % 2 == 0;
			auto left = static_cast<void*>(static_cast<char*>(static_cast<void*>(block)) - block_size);
			auto right = static_cast<void*>(static_cast<char*>(static_cast<void*>(block)) + block_size);

			auto first = is_first ? block : static_cast<Block*>(left);
			auto second = is_first ? static_cast<Block*>(right) : block;

			if (first->is_free && second->is_free && first->level == second->level)
			{
				free_blocks_[block->level - min_level]->remove(first);
				free_blocks_[block->level - min_level]->remove(second);
				block = first;
				block->level++;
				free_blocks_[block->level - min_level]->add(block);
			}
			else { break; }
		}
		
	}


public:
	BuddyAllocator(size_t power_of_two)
	{
		max_level = power_of_two;
		allocate_memory();
	}

	void* Alloc(size_t byte_count)
	{
		auto block = find_suitable_block(byte_count);
		free_blocks_[block->level - min_level]->remove(block);
		return block->data;
	}

	void Free(void *ptr)
	{
		void* block_start = static_cast<char*>(ptr) - sizeof(size_t) - sizeof(bool);
		auto block = static_cast<Block*>(block_start);
		free_blocks_[block->level - min_level]->add(block);
		merge_buddies(block);
	}

	void Dump()
	{
		std::cout << "FREE BLOCKS" << std::endl;
		for (size_t i = min_level; i <= max_level; i++)
		{
			free_blocks_[i-min_level]->dump();
		}
		std::cout << std::endl;
		std::cout << "ALL BLOCKS" << std::endl;
		auto current_block = static_cast<Block *>(start_ptr);
		size_t i = 1;
		while (true)
		{
			std::cout << "Block # " << i;
			std::cout << "; Level: " << current_block->level;
			std::cout << (current_block->is_free ? "; Free" : "; Occupied");
			std::cout << std::endl;
			auto current_position = static_cast<void*>(static_cast<char*>(static_cast<void*>(current_block)) + (1 << current_block->level));
			if (current_position >= static_cast<char*>(start_ptr) + (1 << max_level))
				break;
			current_block = static_cast<Block*>(current_position);
			i++;
		}
		std::cout << std::endl;
	}

};


int main()
{
	auto buddy_allocator = BuddyAllocator(static_cast<size_t>(10));
	buddy_allocator.Dump();
	auto ptr1 = buddy_allocator.Alloc(100);
	buddy_allocator.Dump();
	auto ptr2 = buddy_allocator.Alloc(20);
	buddy_allocator.Dump();
	buddy_allocator.Free(ptr1);
	buddy_allocator.Dump();
	buddy_allocator.Free(ptr2);
	buddy_allocator.Dump();
	return 0;
}