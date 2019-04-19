#include <iostream>
using namespace std;

size_t get_power_of_two(int power)
{
	size_t result = 1;
	for (int p = 0; p < power; p++)
		result *= 2;
	return result;
}

// size is 1 + 1 + 8 = 10 bytes
#pragma pack(push, 1)
struct Chunk
{
	unsigned char level;
	bool is_free;
	union {
		struct {
			Chunk *prev;
			Chunk *next;
		};
		void *data[1];
	};
};
#pragma pack(pop)

class ChunkList
{
public:
	Chunk * head;
	int _level;

	ChunkList(int level)
	{
		head = NULL;
		_level = level;
	}

	void Add(Chunk *chunk)
	{
		if (head == NULL)
		{
			chunk->prev = chunk;
			chunk->next = chunk;
			head = chunk;
			return;
		}

		Chunk *insert_before = head;
		while (size_t(chunk) > size_t(insert_before))
		{

			insert_before = insert_before->next;
			if (insert_before == head)
				break;
		}

		Chunk *prev = insert_before->prev;
		prev->next = chunk;
		chunk->prev = prev;
		insert_before->prev = chunk;
		chunk->next = insert_before;

		if (size_t(chunk) < size_t(head))
			head = chunk;
	}

	void Remove(Chunk *chunk)
	{
		if (chunk->next == chunk)
		{
			head = NULL;
			return;
		}

		Chunk *prev = chunk->prev;
		Chunk *next = chunk->next;
		prev->next = next;
		next->prev = prev;

		if (chunk == head)
			head = head->next;
	}
};


class BuddyAllocator
{
private:
	int max_lvl;
	int min_lvl = 4; // 16 bytes is minimal block size
	ChunkList **chunkLists;
	void *pool;
	size_t capacity;

	void InitializeChunkLists()
	{
		chunkLists = new ChunkList*[max_lvl - min_lvl + 1];
		for (int lvl = min_lvl; lvl <= max_lvl; lvl++)
			chunkLists[lvl - min_lvl] = new ChunkList(lvl);

		Chunk *initialChunk = (Chunk*)pool;
		initialChunk->level = max_lvl;
		initialChunk->is_free = true;
		chunkLists[max_lvl - min_lvl]->Add(initialChunk);
	}

	// returns first buddy
	Chunk* SplitChunk(Chunk *chunk)
	{
		if (chunk->level < min_lvl)
			throw invalid_argument("can't split minimal chunk");
		if (!chunk->is_free)
			throw invalid_argument("cant't split filled block");

		chunkLists[chunk->level - min_lvl]->Remove(chunk);

		size_t chunk_size = get_power_of_two(chunk->level);
		Chunk *buddy = (Chunk*)(size_t(chunk) + chunk_size / 2);
		chunk->level--;
		buddy->level = chunk->level;
		buddy->is_free = true;

		chunkLists[chunk->level - min_lvl]->Add(chunk);
		chunkLists[chunk->level - min_lvl]->Add(buddy);

		return chunk;
	}

	Chunk* SplitWhilePossible(size_t bytes, Chunk *chunk_to_split)
	{
		size_t next_chunk_size = get_power_of_two(chunk_to_split->level) / 2;

		while ((bytes + sizeof(Chunk) <= next_chunk_size) && 
			   (chunk_to_split->level > min_lvl))
		{
			chunk_to_split = SplitChunk(chunk_to_split);
			next_chunk_size /= 2;
		}

		return chunk_to_split;
	}

	Chunk *MergeBuddies(Chunk *first, Chunk *second)
	{
		chunkLists[first->level - min_lvl]->Remove(first);
		chunkLists[second->level - min_lvl]->Remove(second);
		first->level++;
		chunkLists[first->level - min_lvl]->Add(first);

		return first;
	}

	void MergeWhilePossible(Chunk * chunk_to_merge)
	{
		size_t chunk_size = get_power_of_two(chunk_to_merge->level);

		while (chunk_to_merge->level < max_lvl)
		{
			size_t start_offset = size_t(chunk_to_merge) - size_t(pool);
			bool is_first_buddy = start_offset / chunk_size % 2 == 0;

			Chunk *first, *second;
			if (is_first_buddy)
			{
				first = chunk_to_merge;
				second = (Chunk*)(size_t(chunk_to_merge) + size_t(chunk_size));
			}
			else
			{
				first = (Chunk*)(size_t(chunk_to_merge) - size_t(chunk_size));
				second = chunk_to_merge;
			}

			if (first->is_free && second->is_free && first->level == second->level)
				chunk_to_merge = MergeBuddies(first, second);
			else
				break;
			
			chunk_size *= 2;
		}
	}
public:
	BuddyAllocator(int max_block_power)
	{
		if (max_block_power > 16)
			throw invalid_argument("can't reserve more than 2^16 bytes");
		if (max_block_power < min_lvl)
			throw invalid_argument("can't reserve less than 8 bytes");

		max_lvl = max_block_power;
		capacity = get_power_of_two(max_block_power);
		pool = malloc(capacity);
		InitializeChunkLists();
	}

	void *Allocate(size_t bytes)
	{
		size_t chunck_sz = get_power_of_two(min_lvl);

		for (int lvl = min_lvl; lvl <= max_lvl; lvl++, chunck_sz *= 2)
			if ((chunck_sz >= bytes + sizeof(Chunk)) &&
				(chunkLists[lvl - min_lvl]->head != NULL))
			{
				Chunk *chunk_to_use = chunkLists[lvl - min_lvl]->head;
				chunk_to_use = SplitWhilePossible(bytes, chunk_to_use);
				chunkLists[chunk_to_use->level - min_lvl]->Remove(chunk_to_use);
				chunk_to_use->is_free = false;
				return chunk_to_use->data;
			}

		throw invalid_argument("block is too big");
	}

	void Free(void* ptr)
	{
		size_t chunk_start = size_t(ptr) - 2;

		if (size_t(pool) > chunk_start || chunk_start >= size_t(pool) + capacity)
			throw invalid_argument("memory is outsize a pool");

		Chunk *chunk_to_freeup = (Chunk*)(size_t(ptr) - 2);
		size_t start_offset = size_t(ptr) - 2 - size_t(pool);
		size_t chunk_size = get_power_of_two(chunk_to_freeup->level);

		if (start_offset % chunk_size != 0)
			throw invalid_argument("buddy alignment corrupt");

		chunk_to_freeup->is_free = true;
		chunkLists[chunk_to_freeup->level - min_lvl]->Add(chunk_to_freeup);
		MergeWhilePossible(chunk_to_freeup);
	}

	void Dump()
	{
		cout << "START DUMP" << endl;
		cout << "POOL STRUCTURE" << endl;
		DumpPool();
		cout << endl;
		cout << "FREE LISTS" << endl;
		DumpLists();
		cout << "END DUMP" << endl << endl;
	}

	void DumpPool()
	{
		Chunk *current = (Chunk*)pool;
		int chunk_n = 1;

		while (true)
		{
			cout << "Chunk " << chunk_n << endl;
			cout << "Level = " << int(current->level) << " " << (current->is_free ? "free" : "full") << endl;
			size_t size = get_power_of_two(current->level);
			if (size_t(current) + size >= size_t(pool) + capacity)
				break;
			current = (Chunk*)(size_t(current) + size);
			chunk_n++;
		}
	}

	void DumpLists()
	{
		for (int lvl = min_lvl; lvl <= max_lvl; lvl++)
		{
			cout << "List for level " << lvl << endl;
			if (chunkLists[lvl - min_lvl]->head == NULL)
				cout << "\tEmpty" << endl;
			else
			{
				Chunk *curr = chunkLists[lvl - min_lvl]->head;
				int index = 0;
				do
				{
					index++;
					cout << "\tchunk with level " << int(curr->level)  << " at " << size_t(curr) << endl;
					curr->next;
				} while (curr != chunkLists[lvl - min_lvl]->head);

				cout << "\tHas " << index << " chunks" << endl;
			}
		}
	}
};

void ShoudHaveOnlyOneFreeBlock_AfterCreation()
{
	cout << "Test " << "ShoudHaveOnlyOneFreeBlock_AfterCreation" << endl;
	BuddyAllocator alloc(8);
	alloc.Dump();
}

void ShoudHaveOnlyOneFullBlock_AfterAllocatingBigBlock()
{
	cout << "Test " << "ShoudHaveOnlyOneFullBlock_AfterAllocatingBigBlock" << endl;
	BuddyAllocator alloc(8);
	alloc.Allocate(200);
	alloc.Dump();
}

void ShoudHavePartitions_AfterAllocatingLittleBlock()
{
	cout << "Test " << "ShoudHavePartitions_AfterAllocatingLittleBlock" << endl;
	BuddyAllocator alloc(8);
	alloc.Allocate(20);
	alloc.Dump();
}

void ShouldHaveOneBigBlock_AfterFree_WithoutMerge()
{
	cout << "Test " << "ShouldHaveOneBigBlock_AfterFree_WithoutMerge" << endl;
	BuddyAllocator alloc(8);
	void* block = alloc.Allocate(200);
	alloc.Free(block);
	alloc.Dump();
}

void ShouldHaveOneBigBlock_AfterFree_WithMerge()
{
	cout << "Test " << "ShouldHaveOneBigBlock_AfterFree_WithMerge" << endl;
	BuddyAllocator alloc(8);
	void *block = alloc.Allocate(20);
	alloc.Free(block);
	alloc.Dump();
}

void ShouldAllocateSeveralBlocks()
{
	cout << "Test " << "ShouldAllocateSeveralBlocks" << endl;
	BuddyAllocator alloc(8);
	alloc.Allocate(20);
	alloc.Allocate(40);
	alloc.Dump();
}

void ShouldRemoveAfterAllocatingSeveralBlocks()
{
	cout << "Test " << "ShouldRemoveAfterAllocatingSeveralBlocks" << endl;
	BuddyAllocator alloc(8);
	void *block = alloc.Allocate(20);
	alloc.Allocate(40);
	alloc.Free(block);
	alloc.Dump();
}


int main()
{
	ShoudHaveOnlyOneFreeBlock_AfterCreation();
	ShoudHaveOnlyOneFullBlock_AfterAllocatingBigBlock();
	ShoudHavePartitions_AfterAllocatingLittleBlock();
	ShouldHaveOneBigBlock_AfterFree_WithoutMerge();
	ShouldHaveOneBigBlock_AfterFree_WithMerge();
	ShouldAllocateSeveralBlocks();
	ShouldRemoveAfterAllocatingSeveralBlocks();
	return 0;
}