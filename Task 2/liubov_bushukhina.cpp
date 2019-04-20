#include <cmath>
#include <vector>
#include <map>
#include <iostream>

using namespace std;

class Chunk {
public:
	size_t size;
	void* position;

	Chunk(size_t size1, void* position1)
	{
		size = size1;
		position = position1;
	}
};

class Allocator
{
private:
	void* memory;
	vector<vector<Chunk>> freeChunks;
	vector<vector<Chunk>> allocatedChunks;
	map<void*, tuple<int, int>> chunks;
	
	int MIN_SIZE;
	int MIN_POW = 5;
	int MAX_POW;

public:
	explicit Allocator(size_t size)
	{
		if ((size & (size - 1)) != 0)
			throw invalid_argument("Size must be a power of 2.");
		memory = malloc(size);
		MIN_SIZE = pow(2, MIN_POW);
		MAX_POW = GetLog2(size);
		for (int i = MIN_SIZE; i<=size; i*=2)
		{
			freeChunks.emplace_back();
			allocatedChunks.emplace_back();
		}
		freeChunks.back().emplace_back(Chunk(size, memory));
	}

	void* Alloc(size_t size)
	{
		int level = GetLog2(size) - MIN_POW;
		int neededLevel = level;
		while (freeChunks[level].empty()) {
			if (level > MAX_POW)
				throw invalid_argument("Not enough memory.");
			level += 1;
		}
		bool haveSuitableChunk = false;
		if (level == neededLevel)
			haveSuitableChunk = true;
		int neededSize = pow(2, neededLevel + MIN_POW);
		if (!haveSuitableChunk)
		{
			level = DevideChunks(neededSize, level);
		}
		Chunk chunkToFill = freeChunks[level].back();
		freeChunks[level].pop_back();
		allocatedChunks[level].push_back(chunkToFill);
		chunks[chunkToFill.position] = { level, allocatedChunks[level].size() - 1 };
		return chunkToFill.position;
	}

	void Free(void* ptr)
	{
		int power = get<0>(chunks[ptr]);
		int index = get<1>(chunks[ptr]);
		Chunk chunkToFree = allocatedChunks[power][index];
		allocatedChunks[power].erase(allocatedChunks[power].begin() + index);
		freeChunks[power].push_back(chunkToFree);
		UnionChunks(power);
	}

	void Dump()
	{
		cout << "SIZE[COUNT]: [POSITIONS]" << endl;
		
		cout << "FILLED" << endl;
		for (int i = 0; i < allocatedChunks.size(); i++)
		{
			cout << pow(2, i + MIN_POW) << "[" << allocatedChunks[i].size() << "]: ";
			for (Chunk & chunk : allocatedChunks[i])
				cout << chunk.position << " ";
			cout << endl;
		}

		cout << "EMPTY" << endl;
		for (int i = 0; i < freeChunks.size(); i++)
		{
			cout << pow(2, i + MIN_POW) << "[" << freeChunks[i].size() << "]: ";
			for (Chunk & chunk : freeChunks[i])
				cout << chunk.position << " ";
			cout << endl;
		}
		
		cin.get();
	}

private:
	int GetLog2(int number) {
		if (number < MIN_SIZE)
			return MIN_POW;
		int power = 0;
		for (int i = 1; i < number; i *= 2)
			power += 1;
		return power;
	}

	int DevideChunks(int neededSize, int level)
	{
		while (true)
		{
			Chunk chunk = freeChunks[level].back();
			freeChunks[level].pop_back();
			level -= 1;
			size_t newSize = chunk.size / 2;
			void* newStart = (void*)((size_t)chunk.position + newSize);
			freeChunks[level].push_back(Chunk(newSize, newStart));
			freeChunks[level].push_back(Chunk(newSize, chunk.position));
			if (neededSize == newSize)
				break;
		}
		return level;
	}

	void UnionChunks(int level)
	{
		Chunk chunk = freeChunks[level].back();
		void* start = chunk.position;
		void* end = (void*)((size_t)start + chunk.size);
		for (int i = 0; i < freeChunks[level].size() - 1; i++) {
			void *currStart = freeChunks[level][i].position;
			void *currEnd = (void *)((size_t)currStart + freeChunks[level][i].size);
			if (currStart == end or currEnd == start)
			{
				Chunk newChunk = Chunk(chunk.size * 2, currStart);
				if (currStart == end)
					newChunk.position = start;
				freeChunks[level].erase(freeChunks[level].end());
				freeChunks[level].erase(freeChunks[level].begin() + i + 1);
				freeChunks[level + 1].push_back(newChunk);
				UnionChunks(level + 1);
				break;
			}
		}
	}
};

int main() {
	Allocator alloc = Allocator(1024);
	
	void* pointer1 = alloc.Alloc(247);
	void* pointer2 = alloc.Alloc(63);
	void* pointer3 = alloc.Alloc(65);
	void* pointer4 = alloc.Alloc(30);
	void* pointer5 = alloc.Alloc(56);
	void* pointer6 = alloc.Alloc(130);
	
	alloc.Free(pointer1);
	alloc.Free(pointer2);
	alloc.Free(pointer3);
	alloc.Dump();
	
	return 0;
}