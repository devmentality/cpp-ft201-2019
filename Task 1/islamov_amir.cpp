#include <iostream>
#include <string.h>
#include <stdio.h>
#include <new>
#include <stdexcept>

template <class T>
class LinearArray{
    unsigned int _size;
    intptr_t _offset;
    T* _base_ptr;
    LinearArray* next;

public:
    explicit LinearArray(unsigned int size=128){
        _size = size;
        _base_ptr = (T*) malloc(size * sizeof(T));
        _offset = 0;
        if (_base_ptr == nullptr)
            throw std::bad_alloc();
        next = nullptr;
    }

    void Add(T item){
        if(_offset == _size) {
            if (next == nullptr)
                next = new LinearArray(_size);
            next->Add(item);
        }
        else
            memcpy(_base_ptr + _offset++, &item, sizeof(item));
    }

    T operator[](int i){
        if (i >= 0 && i < _offset)
            return *(_base_ptr + i);
        if (i < 0)
            throw std::out_of_range("LinearArray index out of range");
        if (next == nullptr)
            next = new LinearArray(static_cast<unsigned int>(i));
        return (*next)[i - _size];
    }

    ~LinearArray(){
        delete next;
        free(_base_ptr);
    }
};

template <class T>
class StackArray{
    unsigned int _size;
    intptr_t _offset;
    T* _base_ptr;

public:
    explicit StackArray(unsigned int size=128){
        _size = size;
        _offset = 0;
        _base_ptr = (T*) malloc(size * sizeof(T));
    }

    void Push(T item){
        if(_offset == _size)
            throw std::overflow_error("StackArray ran out of allocated memory");
        else
            memcpy(_base_ptr + _offset++, &item, sizeof(item));
    }

    T Pop(){
        if (_offset == 0)
            throw std::underflow_error("Cannot pop from empty StackArray");
        return *(_base_ptr + --_offset);
    }

    void Clear(){
        _offset = 0;
    }

    T operator[](int i){
        if (i >= 0 && i <= _offset)
            return *(_base_ptr + i);
        throw std::out_of_range("StackArray index out of range");
    }

    ~StackArray() {
        free(_base_ptr);
    }
};


template <class T>
struct LinkedItem{
    LinkedItem<T>* next;
    LinkedItem<T>* prev;
    T value;
};

template <class T>
class LinkedArray{
    LinkedItem<T>* head = nullptr;
    size_t amount = 0;

private:
    LinkedItem<T>* GetLinkedItem(size_t i){
        if (i >= amount)
            throw std::out_of_range("LinkedArray out of range");
        LinkedItem<T>* desired_item = head;
        for(int j = 0; j < i; j++)
            desired_item = desired_item->next;
        return desired_item;
    }
public:
    LinkedArray() = default;

    void Add(T item){
        amount++;
        auto * l_item = new LinkedItem<T>{nullptr, nullptr, item};
        if(head == nullptr){
            head = l_item;
            return;
        }
        LinkedItem<T>* last = GetLinkedItem(amount - 2);
        l_item->prev = last;
        last->next = l_item;
        last = l_item;
    }

    void Insert(T item, size_t i){
        if(i >= amount)
            throw std::out_of_range("LinkedArray index out of range");
        LinkedItem<T>* temp = GetLinkedItem(i);
        amount++;
        auto * new_item = new LinkedItem<T>{temp, temp->prev, item};
        temp->prev->next = new_item;
        temp->prev = new_item;
        if (temp == head)
            head = new_item;
    }

    T Remove(size_t i){
        if(i >= amount)
            throw std::out_of_range("LinkedArray index out of range");
        LinkedItem<T>* item = GetLinkedItem(i);
        amount--;
        T val = item->value;
        item->prev->next = item->next;
        item->next->prev = item->prev;
        if (item == head)
            head = item->next;
        return val;
    }

    T operator[](size_t i){
        if (i >= 0 && i <= amount)
            return GetLinkedItem(i)->value;
        throw std::out_of_range("LinkedArray index out of range");
    }
};

int main(){
    LinearArray<int> linear = LinearArray<int>(2);
    linear.Add(1);
    linear.Add(2);
    linear.Add(3);
    printf("Linear array: %i, %i, %i\n", linear[0], linear[1], linear[2]);

    StackArray<int> stack = StackArray<int>();
    stack.Push(1);
    stack.Push(2);
    stack.Push(3);
    int popped = stack.Pop();
    printf("Stack: %i, %i, %i\n", stack[0], stack[1], popped);
    stack.Clear();
//    stack.Pop();

    LinkedArray<int> linked = LinkedArray<int>();
    linked.Add(1);
    linked.Add(2);
    linked.Add(3);
    int removed_val = linked.Remove(1);
    printf("Linked list: %i, %i, %i\n", linked[0], removed_val, linked[2]);
}