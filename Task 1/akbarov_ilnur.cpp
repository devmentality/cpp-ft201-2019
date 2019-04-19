#include <stdio.h>
#include <cstdlib>
#include <malloc.h>
#include <iostream>
#include <cstring>
#include <list>
#include <tuple>

using namespace std;


class LinearAllocator
{
public:
    LinearAllocator(size_t size)
    {
        poolSize = size;
        pool = malloc(size);
        current = pool;
        cout << "Created allocator with size: " << poolSize << endl;
    }

    void* Allocate(size_t size)
    {
        if ((size_t)current + size > poolSize + (size_t)pool)
            throw bad_alloc();
        void* result = current;
        current = (void*)((size_t)current + size);
        cout << "Allocated: " << size << " bytes on " << result << endl;
        return result;
    }

    void Free()
    {
        pool = malloc(poolSize);
        current = pool;
    }

private:
    void* pool;
    size_t poolSize;
    void* current;
};

class StackAllocator
{
public:
    StackAllocator(size_t size)
    {
        poolSize = size;
        pool = malloc(size);
        current = nullptr;
        cout << "Created allocator with size: " << poolSize << endl;
    }

    void* Allocate(size_t size)
    {
        if ((size_t)current + size > poolSize + (size_t)pool)
            throw bad_alloc();
        if (current != nullptr)
            current = (void*)((size_t)current + size);
        else
            current = pool;
        *(size_t*)current = size;
        current = (void*)((size_t)current + sizeof(size_t));
        cout << "Allocated: " << size << " bytes on " << current << endl;
        return current;
    }

    void Free()
    {
        current = (void*)((size_t)current - sizeof(size_t));
    }

private:
    void* pool;
    size_t poolSize;
    void* current;

};

class ListNode
{
public:
    int data;
    ListNode* next;
    ListNode(int data, ListNode* next)
    {
        this->data = data;
        this->next = next;
    }
};

class ListArr
{
public:
    void Add(int value)
    {
        if (head == nullptr)
        {
            head = new ListNode(value, nullptr);
            tail = head;
        }
        else
        {
            tail->next = new ListNode(value, nullptr);
            tail = tail->next;
        }
    }

    void PrintElements()
    {
        ListNode* curr = head;
        while (curr != nullptr)
        {
            cout << curr->data << " ";
            curr = curr->next;
        }
        cout << endl;
    }

    void Insert(int index, int value)
    {
        ListNode* curr = head;
        if (index < 0)
            throw invalid_argument("out of range");
        if (index == 0)
        {
            head = new ListNode(value, head);
            return;
        }
        index--;
        for (int i = 0; i < index; i++)
        {
            if (curr->next == nullptr)
                throw invalid_argument("out of range");
            curr = curr->next;
        }
        curr->next = new ListNode(value, curr->next);
    }

    void Remove(int value)
    {
        if (head == nullptr)
            throw invalid_argument("list is empty");
        ListNode* current = head;
        ListNode* prev = head;
        while (true)
        {
            if (current->data == value)
            {
                if (current == head)
                    head = current->next;
                else
                    prev->next = current->next;
                break;
            }
            prev = current;
            current = current->next;
        }

    }
private:
    ListNode* head = nullptr;
    ListNode* tail = nullptr;
};

int main()
{
    auto arr = new ListArr();
    arr->Add(3);
    arr->Add(6);
    arr->Add(2);
    arr->PrintElements();
    arr->Insert(0, 9);
    arr->Remove(6);
    arr->PrintElements();
    return 0;
}
