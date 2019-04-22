#include <algorithm>
#include <cmath>
#include <iostream>

struct BlockDescriptor
{
	BlockDescriptor* prev;
	BlockDescriptor* next;
	unsigned int order;
	bool isFree;
};

template <unsigned int MaxOrder>
class MemoryManager
{
private:
	void* memoryStart;
	int maxUsableMemorySize;
	int sizeAvailable;
	int minOrderBlockSize;
	BlockDescriptor* orderFreeBlockListStart[MaxOrder + 1];

	int getBlockSize(const int order) const
	{
		return minOrderBlockSize << order;
	}

	BlockDescriptor* getBlockBuddy(BlockDescriptor* block) const
	{
		return (BlockDescriptor*)(size_t(memoryStart) + (getBlockOffset(block) ^ getBlockSize(block->order)));
	}

	BlockDescriptor* tryMergeFreeBuddies(BlockDescriptor* block)
	{
		if (block->order == MaxOrder)
			return nullptr;

		auto buddy = getBlockBuddy(block);
		if (!canMergeBuddies(block, buddy)) return nullptr;

		removeFreeBlock(block);
		removeFreeBlock(buddy);

		const auto mergedBlock = block < buddy ? block : buddy;
		mergedBlock->order++;
		insertFreeBlock(mergedBlock);
		return mergedBlock;
	}

	int getBlockOffset(BlockDescriptor* block) const
	{
		return size_t(block) - size_t(memoryStart);
	}

	static bool canMergeBuddies(BlockDescriptor* block1, BlockDescriptor* block2)
	{
		return block1->isFree && block2->isFree && block1->order == block2->order;
	}

	bool canMergeWithBuddy(BlockDescriptor* block) const
	{
		return canMergeBuddies(block, getBlockBuddy(block));;
	}

	void insertFreeBlock(BlockDescriptor* block)
	{
		BlockDescriptor* prev = nullptr;
		BlockDescriptor* current = orderFreeBlockListStart[block->order];
		if (current == nullptr || block < current)
		{
			orderFreeBlockListStart[block->order] = block;
			block->prev = nullptr;
			block->next = current;
			return;
		}

		while (current < block && current != nullptr)
		{
			prev = current;
			current = current->next;
		}

		if (current == block)
			return;

		block->prev = prev;
		block->next = current;

		prev->next = block;
		if (current != nullptr)
			current->prev = block;
	}

	void removeFreeBlock(BlockDescriptor* block)
	{
		if (block->next != nullptr)
			block->next->prev = block->prev;
		if (block->prev != nullptr)
			block->prev->next = block->next;

		if (orderFreeBlockListStart[block->order] == block)
			orderFreeBlockListStart[block->order] = block->next;

		block->next = block->prev = nullptr;
	}

	BlockDescriptor* splitToBuddies(BlockDescriptor* block)
	{
		if (block->order == 0) return block;

		removeFreeBlock(block);

		block->order--;

		insertFreeBlock(block);

		const auto newBlock = (BlockDescriptor*)(size_t(block) + getBlockSize(block->order));
		newBlock->order = block->order;
		newBlock->isFree = true;
		newBlock->next = block->next;
		newBlock->prev = block;
		block->next = newBlock;

		sizeAvailable -= sizeof(BlockDescriptor);

		return newBlock;
	}

public:
	explicit MemoryManager(const int size)
	{
		memoryStart = std::malloc(size);
		if (memoryStart == nullptr)
			return;

		const int flooredLog2 = int(std::floor(log2(size)));
		minOrderBlockSize = 1 << (flooredLog2 - MaxOrder);

		if (flooredLog2 < MaxOrder || minOrderBlockSize <= sizeof(BlockDescriptor))
			throw ("MaxOrder " + std::to_string(MaxOrder) + " is too big for size " + std::to_string(size));

		sizeAvailable = maxUsableMemorySize = getBlockSize(MaxOrder);

		for (int i = 0; i <= MaxOrder; ++i)
			orderFreeBlockListStart[i] = nullptr;

		const auto firstBlock = (BlockDescriptor*)memoryStart;
		firstBlock->prev = nullptr;
		firstBlock->next = nullptr;
		firstBlock->order = MaxOrder;
		firstBlock->isFree = true;
		insertFreeBlock(firstBlock);

		sizeAvailable -= sizeof(BlockDescriptor);
	}

	int allocate(const int size)
	{
		if (sizeAvailable < size)
			return -1;

		const int sizeRequired = size + sizeof(BlockDescriptor);
		const int orderRequired = int(std::ceil(log2(std::max(1.0, (double)sizeRequired / minOrderBlockSize))));
		BlockDescriptor* selectedBlock = nullptr;

		for (auto order = orderRequired; order <= MaxOrder; ++order)
		{
			if (orderFreeBlockListStart[order] != nullptr)
			{
				selectedBlock = orderFreeBlockListStart[order];
				while (order > orderRequired)
				{
					splitToBuddies(selectedBlock);
					--order;
				}
				break;
			}
		}

		if (selectedBlock == nullptr)
			return -1;

		removeFreeBlock(selectedBlock);
		selectedBlock->isFree = false;
		return int(getBlockOffset(selectedBlock) + sizeof(BlockDescriptor));
	}

	bool free(const int pointer)
	{
		if (pointer < sizeof(BlockDescriptor))
			return false;

		auto block = (BlockDescriptor*)(size_t(memoryStart) + pointer - sizeof(BlockDescriptor));
		if (block->isFree)
			return false;
		block->isFree = true;

		insertFreeBlock(block);
		auto freedSize = getBlockSize(block->order);

		while (canMergeWithBuddy(block))
		{
			block = tryMergeFreeBuddies(block);
			freedSize += sizeof(BlockDescriptor);
		}

		sizeAvailable += freedSize;
		return true;
	}

	void dump()
	{
		auto block = (BlockDescriptor*)memoryStart;
		std::printf("\n");
		while (block != nullptr && getBlockOffset(block) < maxUsableMemorySize)
		{
			std::printf("%i %s\n", getBlockSize(block->order), block->isFree ? "bytes is free" : "bytes is busy");
			block = (BlockDescriptor*)(size_t(block) + getBlockSize(block->order));
		}
		std::printf("\n");
	}

	~MemoryManager()
	{
		std::free(memoryStart);
	}
};

template <unsigned int MaxOrder, unsigned int Size>
class TestCase
{
private:
	MemoryManager<MaxOrder> manager = MemoryManager<MaxOrder>(Size);

public:
	void runTest(std::string testName, int expectedValue, int actualValue)
	{
		std::printf("%s:\t", testName.c_str());
		if (actualValue == expectedValue)
		{
			std::printf("OK\n");
		}
		else
		{
			std::printf("Failed\nExpected: %i\nActual: %i\n", expectedValue, actualValue);
		}
	}

	void testAllocate1()
	{
		runTest("testAllocate1", -1, manager.allocate(1024));
	}

	void testAllocate2()
	{
		runTest("testAllocate2", 16, manager.allocate(34));
	}

	void testAllocate3()
	{
		runTest("testAllocate3", 144, manager.allocate(66));
	}

	void testAllocate4()
	{
		runTest("testAllocate4", 80, manager.allocate(35));
	}

	void testAllocate5()
	{
		runTest("testAllocate5", 272, manager.allocate(67));
	}

	void testAllocate6()
	{
		runTest("testAllocate6", -1, manager.allocate(512));
	}

	void testFreeMemory1()
	{
		runTest("testFreeMemory1", true, manager.free(144));
	}

	void testFreeMemory2()
	{
		runTest("testFreeMemory2", false, manager.free(144));
	}

	void testFreeMemory3()
	{
		runTest("testFreeMemory3", true, manager.free(272));
	}

	void testFreeMemory4()
	{
		runTest("testFreeMemory4", true, manager.free(16));
	}

	void testFreeMemory5()
	{
		runTest("testFreeMemory5", true, manager.free(80));
	}

	void runTests()
	{
		testAllocate1();
		testAllocate2();
		testAllocate3();
		testAllocate4();
		testAllocate5();
		testAllocate6();
		manager.dump();

		testFreeMemory1();
		testFreeMemory2();
		testFreeMemory3();
		testFreeMemory4();
		testFreeMemory5();
		manager.dump();
	}
};

int main()
{
	auto test = TestCase<5,1024>();
	test.runTests();

	return 0;
}