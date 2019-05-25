#include <stdio.h>
#include <iostream>
#include <vector>
#include <map>
#include <cmath>

/*
Программа протестирована в C++ 14. Вывод доступен по ссылке:
https://drive.google.com/file/d/1OygWP-lVG7jTMl3oTUFfpsHLYvM4mbWQ/view?usp=sharing
*/

using namespace std;

struct Node
{
public:
    Node(void* start, size_t size)
    {
        this->start = start;
        this->size = size;
    }
    void* start;
    size_t size;
};

class NodeList
{
public:
    vector<Node> chunks;
};

class Buddy {
private:
    void *start;
    size_t size;
    NodeList layers[32];
    map<void*, int> sizes;

    void mergeNodes(int power)
    {
        auto item = layers[power].chunks.back();
        int len = layers[power].chunks.size() - 1;
        void* current_size = item.start;
        auto end = (uintptr_t)current_size + item.size;

        for (int i = 0; i < len; i++)
        {
            void *currPtr = layers[power].chunks.at(i).start;
            void *currEnd = (void *)((size_t)currPtr + layers[power].chunks[i].size);
            if ((uintptr_t)currPtr - end == 0 || (uintptr_t)current_size - (uintptr_t)currEnd == 0)
            {
                sizes.erase(layers[power].chunks.back().start);
                layers[power].chunks.pop_back();
                sizes.erase(layers[power].chunks.back().start);
                layers[power].chunks.pop_back();

                layers[power + 1].chunks.emplace_back(((uintptr_t)currPtr == end) ? currPtr : current_size, item.size * 2);
                mergeNodes(power + 1);
                return;
            }
        }
    }

public:
    explicit Buddy(size_t length)
    {
        size = getPower(length);
        start = malloc(length);
        int index = (int)log2(length);
        layers[index] = NodeList();
        layers[index].chunks.emplace_back(start, length);
    }

    void* Alloc(size_t needed_size)
    {
        needed_size = getPower(needed_size);
        auto needed_level = (int)log2(needed_size);

        while (layers[needed_level].chunks.empty() && needed_level <= 32)
        {
            needed_level++;
        }

        if (needed_level > 32)
        {
            throw invalid_argument("Block size too big");
        }

        if (layers[needed_level].chunks.empty())
        {
            throw invalid_argument("Not enough memory");
        }

        auto item = layers[needed_level].chunks.back();
        layers[needed_level].chunks.pop_back();
        auto ptr = item.start;

        while (pow(2, needed_level) != needed_size)
        {
            needed_level--;
            ptr = ((char*)ptr + (int)pow(2, needed_level));
            sizes[ptr] = (int)pow(2, needed_level);
            layers[needed_level].chunks.emplace_back(ptr, (int)pow(2, needed_level));
        }

        ptr = ((char*)ptr + (int)pow(2, needed_level));
        sizes[ptr] = (int)pow(2, needed_level);
        return ptr;
    }

    void Free(void* ptr)
    {
        int s = sizes[ptr];
        auto power = (int)log2(s);

        if (!layers[power].chunks.empty())
        {
            layers[power].chunks.emplace_back((void*)ptr, (int)s);
            mergeNodes(power);
            return;
        }

        layers[power].chunks.emplace_back(ptr, s);
        sizes.erase(ptr);
        mergeNodes(power);
    }

    void Dump()
    {
        for (auto & FreeChunk : layers)
        {
            for (auto & chunk : FreeChunk.chunks)
            {
                cout << chunk.size << " " << chunk.start << endl;
            }
        }
    }

    static int getPower(int number)
    {
        auto power = (int)log2(number);
        if (pow(2, power) < number)
        {
            return (int)pow(2, power + 1);
        }
        return (int)pow(2, power);
    }
};

int main()
{
    auto buddy = Buddy(1024);

    cout << "Изначальное состояние" << endl;
    buddy.Dump();

    cout << "Выделяем память так, чтобы получить наименьший возможный блок" << endl;
    void *one = buddy.Alloc(6);
    buddy.Dump();

    cout << "Освобождаем блок" << endl;
    buddy.Free(one);
    buddy.Dump();

    cout << "Выделяем блок другого уровня" << endl;
    void *two = buddy.Alloc(54);
    buddy.Dump();

    cout << "Освобождаем первый блок" << endl;
    buddy.Free(two);
    buddy.Dump();

    cout << "Итоговое состояние (должно совпадать с изначальным)" << endl;
    buddy.Dump();

    return 0;
}
