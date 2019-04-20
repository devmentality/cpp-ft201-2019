#include <stdio.h>
#include <iostream>
#include <vector>
#include <map>
using namespace std;

class Item
{
public:
	Item(void* start, size_t size)
	{
		this->start = start;
		this->size = size;
	}
	void* start;
	size_t size;
};

class OneSizeChunks
{
public:
	vector<Item> chunks;
};

class Allocator {
private:
	void *start;
	size_t Length;
	OneSizeChunks FreeChunks[32];
	map<void*, int> sizes;

	int getPowerOf2(int number)
	{
		int power = log2(number);
		if (pow(2, power) < number) return pow(2, power + 1);
		return pow(2, power);
	}

	void merge(int power)
	{
		auto item = FreeChunks[power].chunks.back();
		int len = FreeChunks[power].chunks.size() - 1;
		void* start = item.start;
		int end = (int)start + item.size;
		for (int i = 0; i < len; i++)
		{
			void *currPtr = FreeChunks[power].chunks.at(i).start;
			void *currEnd = (void *)((size_t)currPtr + FreeChunks[power].chunks[i].size);
			if ((int)currPtr - end == 0 || (int)start - (int)currEnd == 0)
			{
				sizes.erase(FreeChunks[power].chunks.back().start);
				FreeChunks[power].chunks.pop_back();
				sizes.erase(FreeChunks[power].chunks.back().start);
				FreeChunks[power].chunks.pop_back();
				FreeChunks[power + 1].chunks.push_back(Item(((int)currPtr == end) ? currPtr : start, item.size * 2));
				merge(power + 1);
				return;
			}
		}
	}

public:
	Allocator(size_t length)
	{
		Length = getPowerOf2(length);
		start = malloc(length);
		int index = log2(length);
		FreeChunks[index] = OneSizeChunks();
		FreeChunks[index].chunks.push_back(Item(start, length));
	}

	void* Allocate(size_t length)
	{
		length = getPowerOf2(length);
		int power = log2(length);
		while (!FreeChunks[power].chunks.size() && power <= 32)
			power++;
		if (power > 32)
			return nullptr;
		if (FreeChunks[power].chunks.size() == 0)
			return nullptr;

		auto item = FreeChunks[power].chunks.back();
		FreeChunks[power].chunks.pop_back();
		auto ptr = item.start;

		while (!(pow(2, power) == length))
		{
			power--;
			ptr = ((char*)ptr + (int)pow(2, power));
			sizes[ptr] = (int)pow(2, power);
			FreeChunks[power].chunks.push_back(Item(ptr, (int)pow(2, power)));
		}
		ptr = ((char*)ptr + (int)pow(2, power));
		sizes[ptr] = (int)pow(2, power);
		return ptr;
	}

	void Free(void* ptr)
	{
		int size = sizes[ptr];
		int power = log2(size);
		void *buddy = nullptr;
		int find_buddy = ((((char*)start - (char*)ptr)) / size);
		if (find_buddy % 2 != 0)
			buddy = (char*)ptr - size;
		else buddy = (char*)ptr + size;
		if (buddy < start || (size_t)buddy - Length >(size_t)start)
			buddy = nullptr;
		if (FreeChunks[power].chunks.size())
			for (int i = 0; i < FreeChunks[power].chunks.size(); i++)
			{
				FreeChunks[power].chunks.push_back(Item((void*)ptr, (int)size));
				merge(power);
				return;
				Free(ptr);
			}
		FreeChunks[power].chunks.push_back(Item(ptr, size));
		sizes.erase(ptr);
		merge(power);
	}

	void Dump()
	{
		for (int i = 0; i < 32; i++)
		{
			int len = FreeChunks[i].chunks.size();
			for (int j = 0; j < len; j++)
				cout << FreeChunks[i].chunks[j].size << " " << FreeChunks[i].chunks[j].start << endl;
		}
	}
};

int main()
{
	auto allocator = Allocator(1024);
	void *addr1 = allocator.Allocate(8);
	allocator.Dump();
	cout << "-----------------" << endl;;
	allocator.Free(addr1);
	allocator.Dump();
	cout << "-----------------" << endl;;
	void *addr2 = allocator.Allocate(65);
	allocator.Dump();
	cout << "-----------------" << endl;;
	allocator.Free(addr2);
	allocator.Dump();
	return 0;
}
