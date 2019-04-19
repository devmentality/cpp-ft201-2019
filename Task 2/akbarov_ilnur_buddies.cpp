#include <iostream>
#include <list>
#include <cmath>
#include <vector>
#include <map>

using namespace std;


class Block
{
public:
    Block(void* start, size_t size)
    {
        this->size = size;
        this->start = start;
    }
    void* start;
    size_t size;
};


class Allocator
{
public:
    explicit Allocator(size_t size)
    {
        if ((size & (size - 1)) != 0)
            throw invalid_argument("size is not exp of 2");
        pool = malloc(size);
        minSize = pow(2, MIN_EXP);
        int i = minSize;
        maxExp = PowOf2(size);
        while (i != size)
        {
            list<Block> test = {};
            emptyBlocks.emplace_back();
            filledBlocks.emplace_back();
            i *= 2;
        }
        filledBlocks.emplace_back();
        emptyBlocks.emplace_back();
        emptyBlocks.back().emplace_back(Block(pool, size));
    }

    void* Alloc(size_t size)
    {
        int exp = PowOf2(size) - MIN_EXP;
        int neededExp = exp;
        while (emptyBlocks[exp].empty()) {
            if (exp > maxExp)
                throw invalid_argument("no empty blocks");
            exp += 1;
        }
        bool findedNeededBlock = false;
        if (exp == neededExp)
            findedNeededBlock = true;
        int neededSize = pow(2, neededExp + MIN_EXP);
        while (!findedNeededBlock)
        {
            Block block = emptyBlocks[exp].back();
            emptyBlocks[exp].pop_back();
            exp -= 1;
            size_t newSize = block.size / 2;
            void* newStart = (void*)((size_t)block.start + newSize);
            emptyBlocks[exp].push_back(Block(newStart, newSize));
            emptyBlocks[exp].push_back(Block(block.start, newSize));
            if (neededSize == newSize)
                break;
        }
        Block blockToFill = emptyBlocks[exp].back();
        emptyBlocks[exp].pop_back();
        filledBlocks[exp].push_back(blockToFill);
        blocks[blockToFill.start] = {exp, filledBlocks[exp].size() - 1};
        return blockToFill.start;
    }

    void Free(void* ptr)
    {
        int exp = get<0>(blocks[ptr]);
        int id = get<1>(blocks[ptr]);
        Block emptyBlock = filledBlocks[exp][id];
        filledBlocks[exp].erase(filledBlocks[exp].begin() + id);
        emptyBlocks[exp].push_back(emptyBlock);
        MergeBlocks(exp);
    }

    void Dump()
    {
        cout << "EMPTY BLOCKS" << endl;
        cout << "[size]: [count]: [starts of block]" << endl;
        for (int i = 0; i < emptyBlocks.size(); i++)
        {
            cout << pow(2, i + MIN_EXP) << ": " << emptyBlocks[i].size() << ": ";
            for (auto & j : emptyBlocks[i])
                cout << j.start << " ";
            cout << endl;
        }
        cout << "FILLED BLOCKS" << endl;
        cout << "[size]: [count]: [starts of block]" << endl;
        for (int i = 0; i < filledBlocks.size(); i++)
        {
            cout << pow(2, i + MIN_EXP) << ": " << filledBlocks[i].size() << ": ";
            for (auto & j : filledBlocks[i])
                cout << j.start << " ";
            cout << endl;
        }
        cout << endl;
    }

private:
    void* pool;
    int minSize;
    const int MIN_EXP = 3;
    int maxExp;
    vector<vector<Block>> emptyBlocks;
    vector<vector<Block>> filledBlocks;
    map<void*, tuple<int, int>> blocks;

    void MergeBlocks(int exp)
    {
        Block block = emptyBlocks[exp].back();
        void* start = block.start;
        void* end = (void*)((size_t)start + block.size);
        for (int i = 0; i < emptyBlocks[exp].size() - 1; i++) {
            void *startCurrent = emptyBlocks[exp][i].start;
            void *endCurrent = (void *) ((size_t) startCurrent + emptyBlocks[exp][i].size);
            if (startCurrent == end or endCurrent == start)
            {
                Block newBlock = Block(startCurrent, block.size * 2);
                if (startCurrent == end)
                    newBlock.start = start;
                emptyBlocks[exp].erase(emptyBlocks[exp].end());
                emptyBlocks[exp].erase(emptyBlocks[exp].begin() + i + 1);
                emptyBlocks[exp + 1].push_back(newBlock);
                MergeBlocks(exp + 1);
                break;
            }
        }
    }

    int PowOf2(int val) {
        if (val < minSize)
            return 3;
        int i = 1;
        int j = 0;
        while (i < val)
        {
            i <<= 1;
            j += 1;
        }
        return j;
    }
};

int main() {
    Allocator alloc = Allocator(1024);
    void* g1 = alloc.Alloc(247);
    void* g2 = alloc.Alloc(31);
    void* g3 = alloc.Alloc(8);
    void* g4 = alloc.Alloc(8);
    void* g5 = alloc.Alloc(8);
    void* g6 = alloc.Alloc(8);
    void* g7 = alloc.Alloc(8);
    alloc.Free(g1);
    alloc.Free(g3);
    alloc.Free(g2);
    alloc.Dump();
    return 0;
}