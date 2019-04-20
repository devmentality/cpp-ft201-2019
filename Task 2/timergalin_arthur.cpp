#include <cstdlib>
#include <iostream>
#include <stdio.h>
#include <cmath>
#include <new>

struct BlockInfo
{
    int level;
    char free;
    BlockInfo* next;
};


class BuddyAllocator{
    size_t _total_size;
    int _order;
    int _block_size;

    void* mem;
public:
    BuddyAllocator(int order, size_t min_size){
        _order = order;
        _block_size = min_size;
        _total_size = min_size * pow(2, order);
        mem = malloc(_total_size);

        new (mem) BlockInfo{order, 1, 0};
    }

    void* Alloc(size_t size){
        size += sizeof(BlockInfo);
        int amount_of_blocks = ceil((double)size/_block_size);
        int desired_level = ceil(log2(amount_of_blocks));
        
        //finding closest to our size free block
        int min_level = _order + 1;
        BlockInfo* min_block = 0;
        BlockInfo* next_block = (BlockInfo*) mem;
        while(next_block!=0){
            if(next_block->free && next_block->level >= desired_level && next_block->level <= min_level){
                min_level = next_block->level;
                min_block = next_block;
            }
            next_block = next_block->next;
            if(min_level == desired_level)
                break;
        }
        if(min_block == 0){
            printf("Not enough memory");
            return 0;
        }

        //divide current block while we can
        while(min_block->level > desired_level){
            size_t down_level_size = pow(2, min_block->level - 1) * _block_size;
            BlockInfo* new_block = new ((void*)min_block + down_level_size) BlockInfo{min_block->level - 1, 1, min_block->next};
            min_block->next = new_block;
            min_block->level--;
        }
        min_block->free = 0;
        return (void *)min_block + sizeof(BlockInfo);
    }

    void Free(void* freed){
        BlockInfo* current_block = (BlockInfo*)(freed - sizeof(BlockInfo));
        current_block->free = 1;

        //merge blocks
        while(current_block->level < _order){
            int n = ((size_t)current_block - (size_t)mem)/(int)(pow(2, current_block->level)*_block_size);
            BlockInfo* buddy = 0;
            if(n % 2 == 0)
            {
                buddy = (BlockInfo*)(size_t)((size_t)current_block + (int)(pow(2, current_block->level))*_block_size);
            }
            else
                buddy = (BlockInfo*)(size_t)((size_t)current_block - (int)(pow(2, current_block->level))*_block_size);
            if(!buddy->free)
                break;
            if(n % 2 == 0)
            {
                current_block->level++;
                current_block->next = buddy->next;
            }
            else{
                buddy->next = current_block->next;
                current_block = buddy;
                buddy->level++;
            }
        }
    }

    void PrintMemory(){
        BlockInfo* block = (BlockInfo*)mem;
        while(block!= 0)
        {
            printf("[level %i|free %i|addr %i]", block->level, block->free, block);
            block = block->next;
        }
        printf("\n");
    }
};

int main(){
    BuddyAllocator buddy = BuddyAllocator(4, 36);
    printf("Initial memory state:\n\t");
    buddy.PrintMemory();

    int* ar = (int*)buddy.Alloc(sizeof(int)*4);
    int* sm = (int*)buddy.Alloc(sizeof(int));
    printf("Memory after allocation:\n\t");
    buddy.PrintMemory();
    ar[0] = 1;
    ar[1] = 2;
    ar[2] = 3;
    ar[3] = 4;
    printf("Allocation results:\n\t %i, %i, %i, %i\n", ar[0], ar[1], ar[2], ar[3]);

    buddy.Free(ar);
    printf("First free:\n\t");
    buddy.PrintMemory();
    buddy.Free(sm);
    printf("Second free:\n\t");
    buddy.PrintMemory();
}