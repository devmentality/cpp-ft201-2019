#include <cstdlib>
#include <cstdint>
#include <vector>
#include <algorithm>
#include <iostream>

class buddy_allocator
{
	struct block
	{
		int level;
		bool free;
	};

	void * pointer_;
	uint16_t mask_;
	std::vector<block *> free_blocks_[16];

public:
	buddy_allocator()
	{
		pointer_ = malloc(1 << 15);
		mask_ = 1 << 15;
		*static_cast<block *>(pointer_) = block{ 15, true };
		free_blocks_[15].push_back(static_cast<block *>(pointer_));
	}

	void * allocate(const size_t number_of_bytes)
	{
		unsigned long index;
		_BitScanReverse(&index, number_of_bytes);
		const unsigned int m = (1 << (index + 1)) - 1;
		const auto mask = mask_ & ~m;
		_BitScanForward(&index, mask);

		while (number_of_bytes <= 1 << (index - 1))
		{
			auto current_block = free_blocks_[index].back();
			free_blocks_[index].pop_back();
			current_block->level--;
			const auto new_block = (block *)((char*)current_block + (1 << (index - 1)));
			*new_block = block{ (int)index - 1, true };
			free_blocks_[index - 1].push_back(current_block);
			free_blocks_[index - 1].push_back(new_block);
			mask_ |= 1 << index;
			index--;
		}
		const auto taken_block = free_blocks_[index].back();
		free_blocks_[index].pop_back();
		if (free_blocks_[index].empty()) mask_ &= ~(1 << index);
		taken_block->free = false;
		return (char*)taken_block + sizeof(block);
	}

	void free(void * ptr)
	{
		const auto b = (block *)((char*)ptr - sizeof(block));
		b->free = true;
		free_blocks_[b->level].push_back(b);
		if (free_blocks_[b->level].empty()) mask_ &= ~(1 << b->level);
		auto offset = (char*)b - (char*)pointer_;
		auto one_piece = 1 << b->level;
		auto n = offset / one_piece;
		if (n % 2 == 0)
		{
			for (auto free_block : free_blocks_[b->level])
			{
				if ((block*)((char*)b + (1 << b->level)) == free_block)
				{
					merge_blocks(b->level, free_block, b);
					break;
				}
			}
		}
		else
		{
			for (auto free_block : free_blocks_[b->level])
			{
				if ((block*)((char*)free_block + (1 << b->level)) == b)
				{
					merge_blocks(b->level, b, free_block);
					break;
				}
			}
		}
	}

	void merge_blocks(const int level, block * free_block, block * b)
	{
		free_blocks_[level].pop_back();
		free_blocks_[level].erase(std::remove(
			free_blocks_[level].begin(), free_blocks_[level].end(), b), free_blocks_[level].end());
		b->level++;
		free((char*)b + sizeof(block));
	}

	void dump() const
	{
		for (const auto& i : free_blocks_)
		{
			for (auto free_block : i)
			{
				printf("%d %d\n", free_block->level, free_block->free);
			}
		}
	}

	~buddy_allocator()
	{
		::free(pointer_);
	}
	buddy_allocator(const buddy_allocator& other) = delete;
	buddy_allocator& operator = (const buddy_allocator& other) = delete;
};


void should_have_only_one_free_block_after_creation()
{
	std::cout << "Test " << "should_have_only_one_free_block_after_creation" << std::endl;
	auto a = new buddy_allocator();
	a->dump();
	delete a;
}


void should_select_suitable_block()
{
	std::cout << "Test " << "should_select_suitable_block" << std::endl;
	auto a = new buddy_allocator();
	a->allocate(200);
	a->dump();
	delete a;
}

void should_have_one_big_block_after_free()
{
	std::cout << "Test " << "should_have_one_big_block_after_free" << std::endl;
	auto a = new buddy_allocator();
	auto block = a->allocate(200);
	a->free(block);
	a->dump();
	delete a;
}

void should_allocate_several_blocks()
{
	std::cout << "Test " << "should_allocate_several_blocks" << std::endl;
	auto a = new buddy_allocator();
	a->allocate(200);
	a->allocate(500);
	a->dump();
	delete a;
}

void should_remove_after_allocating_several_blocks()
{
	std::cout << "Test " << "should_remove_after_allocating_several_blocks" << std::endl;
	auto a = new buddy_allocator();
	auto block = a->allocate(200);
	a->allocate(500);
	a->free(block);
	a->dump();
	delete a;
}




int main()
{
	should_have_only_one_free_block_after_creation();
	should_select_suitable_block();
	should_have_one_big_block_after_free();
	should_allocate_several_blocks();
	should_remove_after_allocating_several_blocks();
	system("pause");
	return 0;
}
