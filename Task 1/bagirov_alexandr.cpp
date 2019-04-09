#include <iostream>
using namespace std;

class Linear
{
private:
    size_t size;
    size_t allocated;

    void *start;
    void *offset;

public:
    explicit Linear(size_t arr_size)
    {
        size = arr_size;
        allocated = 0;
        start = malloc(arr_size);
        offset = start;
    }

    void *Allocate(size_t bytes)
    {
        if (allocated + bytes > size)
        {
            throw invalid_argument("memory overflow");
        }

        void *ptr = offset;

        offset = (char*)offset + bytes;
        allocated += bytes;

        return ptr;
    }

    void Free()
    {
        allocated = 0;
        offset = start;
    }
};

class Stack
{
private:
    size_t size;
    size_t allocated;

    void *start;
    void *offset;

public:
    explicit Stack(size_t arr_size)
    {
        size = arr_size;
        allocated = 0;
        start = malloc(arr_size);
        offset = start;
    }

    void *Allocate(size_t bytes)
    {
        if (allocated + bytes > size)
        {
            throw invalid_argument("memory overflow");
        }

        void *ptr = offset;

        offset = (char*)offset + bytes;
        allocated += bytes;

        return ptr;
    }

    void Free(void *ptr)
    {
        auto point = (size_t)ptr;

        if (point > (size_t)offset || point < (size_t)start)
        {
            throw invalid_argument("pointer out of range of the allocated memory");
        }

        offset = ptr;
    }

    void Free()
    {
        allocated = 0;
        offset = start;
    }
};

struct Node {
public:
    int value;
    Node *next;
};

class List {
private:
    Node *head;

public:
    explicit List()
    {
        head = nullptr;
    }

    void Add(int n)
    {
        if (head == nullptr)
        {
            head = new Node();
            head->value = n;
            return;
        }

        Node *current = head;
        while (current->next != nullptr)
        {
            current = current->next;
        }

        current->next = new Node();
        current->next->value = n;
    }

    void Remove(int n)
    {
        if (head == nullptr)
        {
            throw invalid_argument("no such element");
        }

        Node *current = head;
        if (current->value == n)
        {
            head = current->next;
            return;
        }

        while (current->next->value != n)
        {
            if (current->next == nullptr)
            {
                throw  invalid_argument("no such element");
            }
            current = current->next;
        }

        if (current->next->next != nullptr)
        {
            current->next = current->next->next;
        }
        else
        {
            current->next = nullptr;
        }
    }

    void Print()
    {
        if (head == nullptr)
        {
            return;
        }

        Node *current = head;
        cout << current->value << " ";

        while (current->next != nullptr)
        {
            current = current->next;
            cout << current->value << " ";
        }

        cout << endl;
    }
};
