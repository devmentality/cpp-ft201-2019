// AllocatorDoubleSystem.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include <stdlib.h>
#include <iostream>
#include <map>
using namespace std;

class BinaryTree {
public:
	bool IsEmpty;
	void* Pointer;
	size_t Size;
	BinaryTree* Parent;
	BinaryTree* Left;
	BinaryTree* Right;

	BinaryTree(size_t size)
	{
		IsEmpty = true;
		Pointer = malloc(size);
		Size = size;
	}

	BinaryTree(BinaryTree* parent, bool isLeft)
	{
		IsEmpty = true;
		Parent = parent;
		Size = parent->Size / 2;
		if (isLeft) {
			parent->Left = this;
			Pointer = parent->Pointer;
		}
		else {
			parent->Right = this;
			Pointer = (void*)((size_t)parent->Pointer + Size);
		}
	}

	bool ChildsAreEmpty() {
		return (Left == nullptr || Left->IsEmpty) && (Right == nullptr || Right->IsEmpty);
	}
};

class BuddyAllocator {
	private:
		static const size_t minimalSize = 4;
		BinaryTree* root;
		map <size_t, map <void*, BinaryTree*>> freeBlocks;
		map <void*, BinaryTree*> busyBlocks;

		size_t getRightBlockSize(size_t size) {
			size_t blockSize = minimalSize;
			while (blockSize < size)
				blockSize *= 2;
			return blockSize;
		}

		BinaryTree* AllocateFirstBlock(size_t size) {
			auto iter = freeBlocks[size].begin();
			BinaryTree* block = (*iter).second;
			block->IsEmpty = false;
			freeBlocks[size].erase(block->Pointer);
			return block;
		}
	public:
		BuddyAllocator(size_t size) {
			if (size < minimalSize)
				throw "Size is too small";
			if ((size & (size - 1)) != 0)
				throw "Size is not power of two";
			root = new BinaryTree(size);
			freeBlocks[size][root->Pointer] = root;
		}

		void* Allocate(size_t size) {
			size_t rightSize = getRightBlockSize(size);
			size_t findedSize = 0;
			for (size_t blockSize = rightSize; blockSize <= root->Size; blockSize *= 2) {
				if (!freeBlocks[blockSize].empty()) {
					findedSize = blockSize;
					break;
				}
			}
			if (findedSize == 0)
				throw "All blocks are busy";
			BinaryTree* block = this->AllocateFirstBlock(findedSize);
			while (findedSize > rightSize)
			{			
				findedSize /= 2;
				BinaryTree* secondChildBlock = new BinaryTree(block, false);
				block = new BinaryTree(block, true);
				block->IsEmpty = false;
				freeBlocks[findedSize][secondChildBlock->Pointer] = secondChildBlock;	
			}
			busyBlocks[block->Pointer] = block;
			return block->Pointer;
		}

		void Deallocate(void* pointer) {
			BinaryTree* block = busyBlocks[pointer];
			busyBlocks.erase(pointer);
			block->IsEmpty = true;
			freeBlocks[block->Size][block->Pointer] = block;
			while (block->Parent != nullptr && block->Parent->ChildsAreEmpty())
			{
				size_t size = block->Size;
				block = block->Parent;			
				freeBlocks[size].erase(block->Right->Pointer);
				freeBlocks[size].erase(block->Left->Pointer);
				block->IsEmpty = true;
				freeBlocks[block->Size][block->Pointer] = block;				
				block->Left = nullptr;
				block->Right = nullptr;				
			}
		}

		void Dump() {
			cout << "BUSY BLOCKS:" << endl;
			for (auto iter = busyBlocks.begin(); iter != busyBlocks.end(); iter++) {
				auto pair = *iter;
				size_t size = pair.second->Size;
				cout << "  Address: " << pair.first << " ; Size: " << size << endl;
			}
			cout << endl;
			cout << "FREE BLOCKS:" << endl;
			for (auto iter = freeBlocks.begin(); iter != freeBlocks.end(); iter++) {
				auto pair = *iter;
				size_t size = pair.first;
				if (freeBlocks[size].empty())
					continue;
				cout << "  Blocks addresses with size: " << pair.first << endl;
				for (auto it = freeBlocks[size].begin(); it != freeBlocks[size].end(); it++)
					cout << "    " << (*it).first << endl;
			}
			cout << "------------------------------------------" << endl;
		}
};

int main()
{
	BuddyAllocator* alloc = new BuddyAllocator(1024);
	auto ptr2 = alloc->Allocate(200);
	alloc->Dump();
	auto ptr3 = alloc->Allocate(100);
	alloc->Dump();
	auto ptr1 = alloc->Allocate(400);
	alloc->Dump();
	auto ptr5 = alloc->Allocate(25);
	alloc->Dump();
	auto ptr4 = alloc->Allocate(50);
	alloc->Dump();
	alloc->Deallocate(ptr3);
	alloc->Dump();
	alloc->Deallocate(ptr5);
	alloc->Dump();
	alloc->Deallocate(ptr2);
	alloc->Dump();
	alloc->Deallocate(ptr4);
	alloc->Dump();
	alloc->Deallocate(ptr1);
	alloc->Dump();
	return 0;
}
