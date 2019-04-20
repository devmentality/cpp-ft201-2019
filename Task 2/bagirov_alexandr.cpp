#include <iostream>
#include <cmath>

using namespace std;


struct Node
{
    int level;
    bool empty;
    Node *prev;
    Node *next;
};


class NodeList
{
private:
    int level;

public:
    Node *first;

    explicit NodeList(int level)
    {
        this->first = nullptr;
        this->level = level;
    }

    void Add(Node *node)
    {
        if (first == nullptr)
        {
            first = node;
            node->prev = node;
            node->next = node;
            return;
        }

        Node *current = first;
        while (true)
        {
            if (current->next != first && size_t(current) > size_t(node))
            {
                current = current->next;
            }
            else
            {
                break;
            }
        }

        Node *prev = current->prev;
        node->next = prev->next;
        node->prev = prev;
        current->prev = node;
        current->next = node;
    }

    void Remove(Node *node)
    {
        if (first->next == first && first == node)
        {
            first = nullptr;
            return;
        }

        Node *prev = node->prev;
        Node *next = node->next;
        prev->next = next;
        next->prev = prev;
    }
};


class Buddy
{
private:
    int min_level;
    int max_level;
    size_t size;

    NodeList **layers;
    void *pool;

public:
    explicit Buddy(int min_level = 4, int max_level = 16)
    {
        if (min_level < 4)
        {
            throw invalid_argument("Min level cannot be less than 4");
        }
        if (max_level > 16)
        {
            throw invalid_argument("Max level cannot be more than 16");
        }
        if (max_level < min_level)
        {
            throw invalid_argument("Max level cannot be less than min level");
        }

        this->min_level = min_level;
        this->max_level = max_level;

        this->size = (int)pow(2, max_level);
        pool = malloc(size);

        this->layers = new NodeList*[max_level - min_level + 1];

        for (int i = 0; i <= max_level - min_level; i++)
        {
            layers[i] = new NodeList(i + min_level + 1);
        }

        Node *node = (Node*)this->pool;
        node->empty = true;
        node->level = max_level;

        layers[max_level - min_level - 1]->Add(node);
    }

    void *Alloc(size_t block_size)
    {
        size_t needed_size = (int)pow(2, min_level);
        int needed_level = min_level;

        while (needed_size < block_size + sizeof(Node))
        {
            needed_size *= 2;
            needed_level++;
        }

        if (needed_level > max_level)
        {
            throw invalid_argument("Block size too big");
        }

        int current_level = needed_level;
        auto current_block = layers[current_level - min_level]->first;

        if (layers[needed_level - min_level]->first == nullptr) {
            while (layers[current_level - min_level]->first == nullptr && current_level <= max_level) {
                current_level++;
            }

            if (current_level > max_level) {
                throw invalid_argument("Block size too big");
            }

            current_block = layers[current_level - min_level]->first;

            while (current_level != needed_level) {
                layers[current_level - min_level]->Remove(current_block);

                Node* buddyNode = (Node*) ((size_t(current_block) + sizeof(current_block) / 2));
                buddyNode->empty = true;
                buddyNode->level = current_block->level - 1;

                current_block->level--;

                layers[current_block->level - min_level]->Add(current_block);
                layers[buddyNode->level - min_level]->Add(buddyNode);

                current_level--;
            }
        }

        current_block->empty = false;
        layers[current_block->level - min_level]->Remove(current_block);
        return current_block;
    }

    void Free(void *ptr)
    {
        auto block_address = (size_t)ptr;
        Node* block = (Node*) block_address;

        auto block_size = (size_t)pow(2, block->level);
        block->empty = true;

        layers[block->level - min_level]->Remove(block);
    }
};
