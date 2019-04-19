#pragma comment(linker, "/STACK:16777216")
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <vector>
#include <algorithm>
#include <map>
#include <set>
#include <string>
#include <queue>
#include <unordered_map>
#include <cmath>
#include <iterator>
#include <stack>
#include <unordered_set>
#include <bitset>
#include <climits>

using namespace std;

struct Block
{
	bool free;
	char depth;
	Block *prev;
	Block *next;
};

class LinkedList
{
public:
	Block * head;
	char currentDepth;

	LinkedList(int depth)
	{
		head = NULL;
		currentDepth = depth;
	}

	void Add(Block *block)
	{
		if (head == NULL)		//if empty list
		{
			block->prev = block;
			block->next = block;
			head = block;
			return;
		}
		Block *start = head;
		size_t block_position = (size_t)block;
		while (1)		//find first position after block
		{
			size_t position = (size_t)start;
			if (position > block_position)
				break;
			start = start->next;
			if (start == head)		//if position after last element
				break;
		}
		Block *insert = start->prev; //get position to insert
		insert->next = block;
		block->prev = insert;
		start->prev = block;
		block->next = start;
		if (start == head)	//if add before head then change head to block
			head = block;
	}

	void Remove(Block *block)
	{
		if (block == block->prev)	//if one element
		{
			block->next = NULL;
			block->prev = NULL;
			head = NULL;
			return;
		}
		Block *prev = block->prev;
		Block *next = block->next;
		prev->next = next;
		next->prev = prev;
		if (block == head)	//if block is head element, then change head to next
			head = head->next;
	}
};

const int MAXI_POW = 16;
const int MIN_POW = 4;

class Allocator
{
private:
	size_t size;
	void * memory;
	LinkedList ** lists;
	size_t MAX_POW;
public:
	Allocator(size_t _size)
	{
		if (_size > MAXI_POW)
			throw "Too big size";
		if (_size < MIN_POW)
			throw "Too small size";
		MAX_POW = _size;
		size = 1 << _size;
		memory = malloc(size);
		int len = MAX_POW - MIN_POW + 1;
		lists = new LinkedList*[len];
		for (size_t i = 0; i < len; i++)
			lists[i] = new LinkedList(MIN_POW + i);
		Block *first = (Block*)memory;
		first->free = true;
		first->depth = MAX_POW;
		lists[len - 1]->Add(first);
	}

	void* Alloc(size_t _size)
	{
		size_t blockSize = 1 << MIN_POW;
		for (size_t i = 0; i < MAX_POW - MIN_POW + 1; i++) //go from min to max size
		{
			if (blockSize < _size + sizeof(Block))	//find first good blockSize
			{
				blockSize *= 2;
				continue;
			}
			if (lists[i]->head == NULL)	//check the existence of block
			{
				blockSize *= 2;
				continue;
			}
			Block* goodBlock = lists[i]->head;
			size_t curSize = (1 << (goodBlock->depth)) / 2;
			while (curSize >= _size + sizeof(Block))	//if can split on 2 part(size / 2 > current)
			{
				lists[goodBlock->depth - MIN_POW]->Remove(goodBlock);	//remove current block and add 2 parts
				Block* secondBlock = (Block*)(size_t(goodBlock) + curSize);
				secondBlock->free = true;
				secondBlock->depth = goodBlock->depth - 1;
				goodBlock->depth--;
				lists[secondBlock->depth - MIN_POW]->Add(goodBlock);
				lists[secondBlock->depth - MIN_POW]->Add(secondBlock);
				curSize /= 2;
			}
			goodBlock->free = false;
			lists[goodBlock->depth - MIN_POW]->Remove(goodBlock);
			return goodBlock;
		}
		throw "Not enough memory for this block";
	}

	void Free(void *ptr)
	{
		size_t start = (size_t)ptr;
		Block* del = (Block*)start;
		size_t delSize = 1 << del->depth;
		del->free = true;
		lists[del->depth - MIN_POW]->Add(del);
		while (del->depth < MAX_POW)	//go up, while can union 2 parts
		{
			size_t change = size_t(del) - size_t(memory);
			bool leftElement = true;
			if (change / delSize % 2 == 1)
				leftElement = false;
			Block *left = del, *right = (Block*)(size_t(del) + delSize);
			if (!leftElement)
			{
				left = (Block*)(size_t(del) - delSize);
				right = del;
			}
			if (!(left->free && right->free && (left->depth == right->depth)))	//if not can union
				break;
			lists[left->depth - MIN_POW]->Remove(left);
			lists[right->depth - MIN_POW]->Remove(right);
			left->depth++;
			lists[left->depth - MIN_POW]->Add(left);
			del = left;
			delSize *= 2;
		}
	}

	void Show()
	{
		for (int i = MIN_POW; i <= MAX_POW; i++)
		{
			cout << "Depth=" << i << ": ";
			if (lists[i - MIN_POW]->head == NULL)
			{
				cout << "empty\n";
				continue;
			}
			auto start = lists[i - MIN_POW]->head;
			cout << size_t(start) - size_t(memory) << " ";
			start = start->next;
			while (start != lists[i - MIN_POW]->head)
			{
				cout << size_t(start) - size_t(memory) << " ";	//print offset from start position
				start = start->next;
			}
			cout << "\n";
		}
	}
};

void TestOneBigBlock()
{
	cout << "------------------------------------\n";
	cout << "START TestOneBigBlock\n";
	Allocator alloc(10);	//2^10
	alloc.Alloc(700);
	alloc.Show();	//all empty, no free memory
	cout << "END TestOneBigBlock\n";
	cout << "------------------------------------\n\n";
}

void TestAllocAndFree()		//In the end one free block
{
	cout << "------------------------------------\n";
	cout << "START TestAllocAndFree\n";
	Allocator alloc(10);
	auto elem = alloc.Alloc(500);
	alloc.Free(elem);
	alloc.Show();
	cout << "END TestAllocAndFree\n";
	cout << "------------------------------------\n\n";
}

void TestAllocAllMemory()	//Check that can fill all memory
{
	cout << "------------------------------------\n";
	cout << "START TestAllocAllMemory\n";
	Allocator alloc(10);
	for (int i = 0; i < 8; i++)
	{
		alloc.Alloc(128 - sizeof(Block));
	}
	alloc.Show();
	cout << "END TestAllocAllMemory\n";
	cout << "------------------------------------\n\n";
}


void TestAllocManyBlocksThenFreeOne()
{
	cout << "------------------------------------\n";
	cout << "START TestAllocManyBlocksThenFreeOne\n";
	Allocator alloc(10);
	alloc.Alloc((1 << 4) - sizeof(Block));
	alloc.Alloc((1 << 5) - sizeof(Block));
	alloc.Alloc((1 << 6) - sizeof(Block));
	auto t = alloc.Alloc((1 << 7) - sizeof(Block));
	alloc.Alloc((1 << 8) - sizeof(Block));
	alloc.Free(t);
	alloc.Show();
	cout << "END TestAllocManyBlocksThenFreeOne\n";
	cout << "------------------------------------\n\n";
}

void TestTooBigAlloc()
{
	cout << "------------------------------------\n";
	cout << "START TestTooBigAlloc\n";
	Allocator alloc(10);
	try
	{
		alloc.Alloc(2000);
	}
	catch (char* a)
	{
		cout << a << "\n";
	}
	//alloc.Show();
	cout << "END TestTooBigAlloc\n";
	cout << "------------------------------------\n\n";
}

void TestTooBigAllocator()
{
	cout << "------------------------------------\n";
	cout << "START TestTooBigAllocator\n";
	try
	{
		Allocator alloc(50);
	}
	catch (char* a)
	{
		cout << a << "\n";
	}
	//alloc.Show();
	cout << "END TestTooBigAllocator\n";
	cout << "------------------------------------\n\n";
}

void TestTooSmallAllocator()
{
	cout << "------------------------------------\n";
	cout << "START TestTooSmallAllocator\n";
	try
	{
		Allocator alloc(1);
	}
	catch (char* a)
	{
		cout << a << "\n";
	}
	//alloc.Show();
	cout << "END TestTooSmallAllocator\n";
	cout << "------------------------------------\n\n";
}

void TestRandomAllocThenFreeAll()
{
	cout << "------------------------------------\n";
	cout << "START TestRandomAllocThenFreeAll\n";
	Allocator alloc(10);
	vector<void*> all;
	for (int i = 0; i < 10; i++)
	{
		auto t = alloc.Alloc(((rand() % 50) + 50) % 50 + 16);
		all.push_back(t);
	}
	for (auto e : all)
	{
		alloc.Free(e);
	}
	alloc.Show();
	cout << "END TestRandomAllocThenFreeAll\n";
	cout << "------------------------------------\n\n";
}

int main()
{
	TestOneBigBlock();
	TestAllocAndFree();
	TestAllocAllMemory();
	TestAllocManyBlocksThenFreeOne();
	TestTooBigAlloc();
	TestTooBigAllocator();
	TestTooSmallAllocator();
	TestRandomAllocThenFreeAll();
	return 0;
}
