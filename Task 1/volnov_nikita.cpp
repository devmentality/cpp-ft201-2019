#include <cstdlib>
#include <cstring>
#include <iostream>
#include <algorithm>
#include <vector>

using namespace std;

//------------------START VECTOR---------------------------

class Arr
{
private:
	const int RESIZE_COEFF = 2;
	size_t _size;
	size_t _capacity;
	int *_pool;

	void resize(size_t new_capacity)
	{
		size_t old_capacity = _capacity;
		
		int *new_pool = new int[new_capacity];
		for (int i = 0; i < min(new_capacity, old_capacity); i++)
			new_pool[i] = _pool[i];
		delete[] _pool;

		_pool = new_pool;
		_capacity = new_capacity;
	}
public:
	Arr()
	{
		_capacity = 0;
		_pool = new int[0];
		_size = 0;
		resize(32);
	}

	void push_back(int value)
	{
		if (_size >= _capacity)
			resize(_capacity  * RESIZE_COEFF);
		
		_pool[_size] = value;
		_size++;
	}

	void pop_back()
	{
		if (_size > 0)
		{
			_size--;
		}

		if (_size < _capacity / (RESIZE_COEFF * RESIZE_COEFF))
		{
			resize(_capacity / RESIZE_COEFF);
		}
	}

	int& operator [](size_t index)
	{
		if (index >= 0 && index < _size)
			return _pool[index];
		throw invalid_argument("index is out of range");
	}

	size_t size()
	{
		return _size;
	}
};

void ArrDemo()
{
	int s = 1000;
	Arr a;
	for (int i = 0; i < s; i++)
		a.push_back(i);

	for (int i = 0; i < s; i++)
		cout << a[i] << " ";

	for (int i = 0; i < s; i++)
		a.pop_back();
}

//------------------END VECTOR---------------------------

//------------------START LINEAR ALLOCATOR---------------------------

class LinearAllocator
{
private:
	size_t _capacity;
	void *_pool;
	size_t _allocated;
public:
	LinearAllocator(size_t capacity)
	{
		_capacity = capacity;
		_allocated = 0;
		_pool = malloc(capacity);
	}

	void *Allocate(size_t bytes)
	{
		if (_allocated + bytes > _capacity)
			throw invalid_argument("out of memory");
		
		void *ptrToChunk = (char*)_pool + _allocated;
		_allocated += bytes;
		
		return ptrToChunk;
	}

	void Free()
	{
		_allocated = 0;
	}
};

void LinearAllocatorDemo()
{
	LinearAllocator allocator(100);
	int cnt = 10;
	int *numbers1 = (int*)allocator.Allocate(40);

	for (int i = 0; i < cnt; i++)
		numbers1[i] = i * i;

	int *numbers2 = (int*)allocator.Allocate(40);

	for (int i = 0; i < cnt; i++)
		numbers2[i] = i * i * i;

	for (int i = 0; i < 2 * cnt; i++)
		cout << numbers1[i] << endl;

	allocator.Free();

	int* numbers3 = (int*)allocator.Allocate(40);

	for (int i = 0; i < cnt; i++)
		numbers3[i] = i * i * i * i;

	for (int i = 0; i < cnt; i++)
		cout << numbers1[i] << endl;
}

//------------------END LINEAR ALLOCATOR---------------------------

//------------------START STACK ALLOCATOR---------------------------

class StackAllocator
{
private:
	size_t _capacity;
	void *_pool;
	void* _top;

	struct Header
	{
		size_t blockSize;
	};
public:
	StackAllocator(size_t capacity)
	{
		_capacity = capacity;
		_pool = malloc(capacity);
		_top = _pool;
	}

	void *Allocate(size_t bytes)
	{
		if ((size_t)_top + bytes > (size_t)_pool + _capacity)
			throw invalid_argument("out of memory");

		void *ptrToChunk = _top;

		_top = (void*)((size_t)_top + bytes);
		return ptrToChunk;
	}

	void Free(void *ptr)
	{
		size_t address = (size_t)ptr;
		if (address > (size_t)_top || address < (size_t)_pool)
			throw invalid_argument("can't deallocate not allocated memory");
		_top = ptr;
	}

	void Free()
	{
		_top = _pool;
	}
};

void StackAllocatorDemo()
{
	StackAllocator allocator(100);

	int *numbers = (int*)allocator.Allocate(40);
	for (int i = 0; i < 10; i++)
		numbers[i] = i;

	for (int i = 0; i < 10; i++)
		cout << numbers[i] << endl;

	allocator.Free(numbers + 5);
	int *numbers_ = (int*)allocator.Allocate(20);

	for (int i = 0; i < 5; i++)
		numbers_[i] = -i;

	for (int i = 0; i < 10; i++)
		cout << numbers[i] << endl;
}

//------------------END STACK ALLOCATOR---------------------------

//------------------START LIST ALLOCATOR---------------------------

#pragma pack(push, 1)
struct Chunk
{
	size_t size;
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
private:
	bool AreClose(Chunk *first, Chunk *second)
	{
		size_t first_left = (size_t)first;
		size_t first_right = first_left + first->size;

		size_t second_left = (size_t)second;
		size_t second_right = second_left + second->size;

		return first_left == second_right || second_left == first_right;
	}

	Chunk *MergeChunks(Chunk *first, Chunk *second)
	{
		Remove(first);
		Remove(second);
		first->size = first->size + second->size;
		Add(first, false);
		
		return first;
	}

	void Merge(Chunk *chunk)
	{
		Chunk *current = head;

		Chunk *before = NULL;
		Chunk *after = NULL;

		do
		{
			if (AreClose(chunk, current))
			{
				if ((size_t)chunk < (size_t)current)
					after = current;
				else
					before = current;
			}
			current = current->next;
		} while (current != head);

		if (before != NULL)
			chunk = MergeChunks(before, chunk);
		if (after != NULL)
			chunk = MergeChunks(chunk, after);
	}

public:
	Chunk * head;

	ChunkList()
	{
		head = NULL;
	}

	void Add(Chunk *chunk, bool enable_merge=true)
	{
		if (head == NULL)
		{
			chunk->prev = chunk;
			chunk->next = chunk;
			head = chunk;
			return;
		}

		Chunk *prev = head->prev;
		prev->next = chunk;
		chunk->prev = prev;
		head->prev = chunk;
		chunk->next = head;

		if (enable_merge)
			Merge(chunk);
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

class ListAllocator
{
private:
	size_t MIN_CHUNK_SIZE = sizeof(Chunk);
	size_t _capacity;
	void *_pool;
	ChunkList *_freeChunks;

	Chunk *FindSuitableChunk(size_t bytes)
	{
		Chunk *current = _freeChunks->head;
		do
		{
			if (current->size >= bytes)
				return current;

			current = current->next;
		} while (current != _freeChunks->head);

		return NULL;
	}

	bool NeedsSplit(Chunk *chunk, size_t piece)
	{
		return chunk->size >= piece + MIN_CHUNK_SIZE;
	}

	Chunk *SplitChunk(Chunk *chunk, size_t piece)
	{
		size_t oldSize = chunk->size;
		Chunk *newChunk = (Chunk *)((size_t)chunk + piece);
		newChunk->size = oldSize - piece;
		chunk->size = piece;
		return newChunk;
	}

public:
	ListAllocator(size_t capacity)
	{
		_capacity = capacity;
		_pool = malloc(capacity);
		_freeChunks = new ChunkList();
		
		Chunk *_firstChunk = (Chunk*)_pool;
		_firstChunk->size = _capacity;
		_freeChunks->Add(_firstChunk);
	}

	void *Allocate(size_t bytes)
	{
		size_t realSize = bytes + sizeof(size_t);
		Chunk *chunk = FindSuitableChunk(realSize);

		if (chunk == NULL)
			throw new invalid_argument("can't allocate block");

		_freeChunks->Remove(chunk);
		if (NeedsSplit(chunk, realSize))
		{
			Chunk *newChunk = SplitChunk(chunk, realSize);
			_freeChunks->Add(newChunk);
		}
		/*cout << "DEBUG: " << chunk->size << endl;
		cout << "DEBUG: " << (size_t)chunk << endl;
		cout << "DEBUG: " << (size_t)chunk->data << endl;*/
		return chunk->data;
	}

	void Free(void *ptr)
	{
		Chunk *freedChunk = (Chunk *)((size_t)ptr - sizeof(size_t));
		_freeChunks->Add(freedChunk);
	}

	void DumpHeap()
	{
		cout << "FREE CHUNKS" << endl;
		Chunk *current = _freeChunks->head;
		do
		{
			cout << "at " << (size_t)current << endl;
			cout << "size " << current->size << endl;
			cout << "next at " << (size_t)current->next << endl;
			cout << endl;

			current = current->next;
		} while (current != _freeChunks->head);
	}

	~ListAllocator()
	{
		delete[]_pool;
	}
};

void DumpAllocatedChunks(const vector<void *> &allocated)
{
	cout << "ALLOCATED CHUNKS" << endl;

	for (size_t iter = 0; iter < allocated.size(); iter++)
	{
		Chunk *chunk = (Chunk *)((size_t)allocated[iter] - sizeof(size_t));
		cout << "at " << (size_t)chunk << endl;
		cout << "size " << chunk->size << endl;
		cout << endl;
	}
}

void ListAllocator_HasOneFreeChunk_AfterCreation()
{
	cout << "ListAllocator_HasOneFreeChunk_AfterCreation" << endl;
	ListAllocator alloc(100);
	alloc.DumpHeap();
	cout << endl;
}

void ListAllocator_AllocatesOneChunk()
{
	cout << "ListAllocator_AllocatesOneChunk" << endl;
	ListAllocator alloc(100);
	vector<void *> allocated;
	allocated.push_back(alloc.Allocate(50));

	alloc.DumpHeap();
	DumpAllocatedChunks(allocated);
	cout << endl;
}

void ListAllocator_AllocatesSeveralChunks()
{
	cout << "ListAllocator_AllocatesSeveralChunks" << endl;
	ListAllocator alloc(100);
	vector<void *> allocated;
	allocated.push_back(alloc.Allocate(20));
	allocated.push_back(alloc.Allocate(20));
	allocated.push_back(alloc.Allocate(20));

	alloc.DumpHeap();
	DumpAllocatedChunks(allocated);
	cout << endl;
}

void ListAllocator_FreesUpSeveralChunks()
{
	cout << "ListAllocator_FreesUpSeveralChunks" << endl;
	ListAllocator alloc(100);
	void *p1 = alloc.Allocate(20);
	void *p2 = alloc.Allocate(20);
	void *p3 = alloc.Allocate(20);
	alloc.Free(p2);
	alloc.Free(p3);
	alloc.DumpHeap();
	cout << endl;
}

void ListAllocator_FreesUpChunk()
{
	cout << "ListAllocator_FreesUpChunk" << endl;
	ListAllocator alloc(100);
	void *ptr = alloc.Allocate(50);
	alloc.Free(ptr);
	alloc.DumpHeap();
	cout << endl;
}


//------------------END LIST ALLOCATOR---------------------------


int main()
{
	//LinearAllocatorDemo();
	//StackAllocatorDemo();
	ListAllocator_HasOneFreeChunk_AfterCreation();
	ListAllocator_AllocatesOneChunk();
	ListAllocator_AllocatesSeveralChunks();
	ListAllocator_FreesUpChunk();
	ListAllocator_FreesUpSeveralChunks();
	return 0;
}