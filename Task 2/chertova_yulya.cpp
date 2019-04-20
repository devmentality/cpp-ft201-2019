#include <vector>
#include <iostream>
#include <algorithm>

#ifndef MALLOC_MY_ALLOCATOR_H
#define MALLOC_MY_ALLOCATOR_H

using namespace std;

class MemoryControlBlock {
public:
    MemoryControlBlock(int size, void *ptr, bool free = false) {
        this->size = size;
        this->memoryPointer = ptr;
        this->isFree = free;
    }

    bool isFree;
    int size;
    void *memoryPointer;
};

class MyAllocator {
private:
    int maxSize = 80;
    int sizeTaken = 0;
    void *firstAddress;
    void *currentAddress;
    vector<MemoryControlBlock> memoryBlocks;
    vector<int> freeBlocksIndexes;
public:
    MyAllocator() {
        firstAddress = malloc(static_cast<size_t>(maxSize));
        currentAddress = firstAddress;
    }

    void *alloc(int num_bytes) {
        if (memoryBlocks.empty() || freeBlocksIndexes.empty())
            return insertNewBlock(num_bytes);
        sort(freeBlocksIndexes.begin(), freeBlocksIndexes.end());
        for (int i = 0; i < freeBlocksIndexes.size(); ++i) {
            int freeBlockIndex = freeBlocksIndexes[i];
            MemoryControlBlock *freeBlock = &memoryBlocks[freeBlockIndex];
            if (freeBlock->size < num_bytes && !canMergeWithNextBlockToGetEnoughSpace(i, freeBlockIndex, num_bytes))
                continue;

            void *result = freeBlock->memoryPointer;
            if (canMergeWithNextBlockToGetEnoughSpace(i, freeBlockIndex, num_bytes))
                mergeTwoAdjacentBlocks(freeBlock, freeBlockIndex, i);
            if (isNextBlockFree(freeBlockIndex, i)) {
                addThisRemainderToNextBlock(i, freeBlockIndex, freeBlock->size - num_bytes, result + num_bytes);
            }
            else if (freeBlock->size > num_bytes) {
                MemoryControlBlock newBlock = MemoryControlBlock(freeBlock->size - num_bytes,
                                                                 freeBlock->memoryPointer + num_bytes,
                                                                 true);
                memoryBlocks.insert(memoryBlocks.begin() + freeBlockIndex + 1, newBlock);
                freeBlocksIndexes.erase(freeBlocksIndexes.begin() + i);
                freeBlocksIndexes.insert(freeBlocksIndexes.begin() + i, freeBlockIndex + 1);
                for (int j = i + 1; j < freeBlocksIndexes.size(); ++j) {
                    freeBlocksIndexes[j]++;
                }

            }

            freeBlock->size = num_bytes;
            freeBlock->isFree = false;
            return result;
        }
        return insertNewBlock(num_bytes);


    }

    void free(void *ptr) {
        int idx = -1;
        for (int i = 0; i < memoryBlocks.size(); ++i) {
            if (memoryBlocks[i].memoryPointer == ptr) {
                idx = i;
                sizeTaken -= memoryBlocks[i].size;
                memoryBlocks[i].isFree = true;
                freeBlocksIndexes.push_back(i);
                break;
            }
        }
        if (idx == -1)
            throw exception();
    }

    void Dump() {
        cout << "Taken " + to_string(sizeTaken) + " out of " + to_string(maxSize) << endl;
        string outer_result;
        for (auto &memoryBlock : memoryBlocks) {
            string insideResult = "|";
            string sep = memoryBlock.isFree ? "_" : "#";
            int upper = memoryBlock.size / 2;
            for (int j = 0; j < upper; ++j) {
                insideResult += sep;
            }
            insideResult += "|";
            outer_result += insideResult;
        }
        cout << outer_result << endl;
        string free;
        for (int i = 0; i < freeBlocksIndexes.size(); ++i) {
            free += to_string(freeBlocksIndexes[i]) + " ";
        }
        cout << free << endl;
    }

private:
    void *insertNewBlock(int num_bytes) {
        if (sizeTaken + num_bytes > maxSize) {
            cout << "Out of memory!" << endl;
            throw bad_alloc();
        }
        auto result = currentAddress;
        MemoryControlBlock block = MemoryControlBlock(num_bytes, result);
        currentAddress += num_bytes;
        sizeTaken += num_bytes;
        memoryBlocks.push_back(block);
        return result;
    }

    bool isNextBlockFree(int currentFreeBlockIndex, int i) {
        return i + 1 < freeBlocksIndexes.size() && currentFreeBlockIndex + 1 == freeBlocksIndexes[i + 1];
    }

    bool canMergeWithNextBlockToGetEnoughSpace(int i, int currentFreeBlockIndex, int size_needed) {
        if (!isNextBlockFree(currentFreeBlockIndex, i))
            return false;
        int currentSize = memoryBlocks[currentFreeBlockIndex].size;
        int nextSize = memoryBlocks[currentFreeBlockIndex + 1].size;
        return currentSize + nextSize >= size_needed;

    }

    void mergeTwoAdjacentBlocks(MemoryControlBlock *currentFreeBlock, int freeBlockIndex, int i) {
        MemoryControlBlock *nextFreeBlock = &memoryBlocks[freeBlockIndex + 1];
        currentFreeBlock->size += nextFreeBlock->size;
        freeBlocksIndexes.erase(freeBlocksIndexes.begin() + i + 1);
        for (int j = i + 1; j < freeBlocksIndexes.size(); ++j) {
            freeBlocksIndexes[j]--;
        }
        memoryBlocks.erase(memoryBlocks.begin() + freeBlockIndex + 1);
    }

    void addThisRemainderToNextBlock(int i, int freeBlockIndex, int size_to_add, void *ptr) {
        MemoryControlBlock *nextFreeBlock = &memoryBlocks[freeBlockIndex + 1];
        nextFreeBlock->memoryPointer = ptr;
        nextFreeBlock->size += size_to_add;
        freeBlocksIndexes.erase(freeBlocksIndexes.begin() + i);

    }

    int main() {
    MyAllocator alloc = MyAllocator();
    vector<void *> pointers;
    for (int i = 0; i < 5; ++i) {
        pointers.push_back(alloc.alloc(sizeof(test)));
        alloc.Dump();
    }

    auto last = pointers[1];
    alloc.free(last);
    alloc.Dump();

    last = pointers[2];
    alloc.free(last);
    alloc.Dump();

    last = pointers[4];
    alloc.free(last);
    alloc.Dump();

    pointers.push_back(alloc.alloc(sizeof(veryBigTest)));
    alloc.Dump();
    pointers.push_back(alloc.alloc(sizeof(littleTest)));
    alloc.Dump();


    pointers.push_back(alloc.alloc(sizeof(littleTest)));
    alloc.Dump();
    pointers.push_back(alloc.alloc(sizeof(test)));
    alloc.Dump();
    pointers.push_back(alloc.alloc(sizeof(test)));
    alloc.Dump();

}

};
