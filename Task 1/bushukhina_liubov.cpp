#include <iostream>
#include <vector>
#include <list>
#include <stack>


template <typename T>
class Vector {
public:
    explicit Vector(size_t size = 0, const T& value = T())
        : _size(size)
        , _capacity(size)
        , _data_ptr(new T[size])
    {
        for (size_t i = 0; i < _size; ++i) {
            _data_ptr[i] = value;
        }
    }

    Vector(const Vector& v)
        : _size(v.size())
        , _capacity(v.size())
        , _data_ptr(new T[v.size()])
    {
        for (size_t i = 0; i < _size; ++i) {
            _data_ptr[i] = v[i];
        }
    }

    ~Vector() {
        _size = 0;
        _capacity = 0;
        delete[] _data_ptr;
        _data_ptr = nullptr;
    }

    void push_back(T new_element) {
        if (_size == _capacity) {
            if (empty()) {
                reserve(1);
            } else {
                reserve(_capacity * 2);
            }
        }
        _data_ptr[_size++] = new_element;
    }

    void pop_back() {
        _size--;
    }

    const T& operator[](size_t i) const {
        return _data_ptr[i];
    }

    T& operator[](size_t i) {
        return _data_ptr[i];
    }

    const T& back() const {
        return this->operator[](_size - 1);
    }

    T& back() {
        return this->operator[](_size - 1);
    }

    const T& front() const {
        return this->operator[](0);
    }

    T& front() {
        return this->operator[](0);
    }

    size_t size() const {
        return _size;
    }

    size_t capacity() const {
        return _capacity;
    }

    bool empty() const {
        return _size == 0;
    }

    void resize(size_t new_size, const T& value=T()) {
        if (new_size > _size) {
            reserve(new_size);
            for (size_t i = _size; i < new_size; ++i) {
                _data_ptr[i] = value;
            }
        }
        _size = new_size;
    }

    void reserve(size_t new_capacity) {
        if (new_capacity == _capacity) {
            return;
        }
        auto new_data_ptr = new T[new_capacity];
        auto end = (_capacity < new_capacity) ? _capacity : new_capacity;
        for (size_t i = 0; i < end; ++i) {
            new_data_ptr[i] = _data_ptr[i];
        }
        _capacity = new_capacity;
        delete[](_data_ptr);
        _data_ptr = new_data_ptr;
    }

private:
    T* _data_ptr;
    size_t _size;
    size_t _capacity;
};


template <typename T>
struct ListNode {
    ListNode(const T& value, ListNode<T>* next, ListNode<T>* prev)
        : value(value)
        , next(next)
        , prev(prev)
    {
    }
    T value;
    ListNode<T>* next;
    ListNode<T>* prev;
};

template <typename T>
class List {
public:
    explicit List(size_t size = 0, const T& value = T())
        : _head(nullptr)
        , _tail(nullptr)
        , _size(0)
    {
        for (size_t i = 0; i < size; ++i) {
            push_back(value);
        }
    }

    ~List() {
        if (empty()) {
            return;
        }
        auto ptr = _head;
        while (ptr->next != nullptr) {
            auto new_ptr = ptr->next;
            delete ptr;
            ptr = new_ptr;
        }
        delete ptr;
    }

    void push_back(const T& new_element) {
        if (empty()) {
            auto new_node = new ListNode<T>(new_element, nullptr, nullptr);
            _tail = _head = new_node;
        } else {
            auto new_node = new ListNode<T>(new_element, nullptr, _tail);
            _tail->next = new_node;
            _tail = new_node;
        }
        _size++;
    }

    const T& back() const {
        return _tail->value;
    }

    T& back() {
        return _tail->value;
    }

    void push_front(const T& new_element) {
        if (empty()) {
            auto new_node = new ListNode<T>(new_element, nullptr, nullptr);
            _tail = _head = new_node;
        } else {
            auto new_node = new ListNode<T>(new_element, _head, nullptr);
            _head->prev = new_node;
            _head = new_node;
        }
        _size++;
    }

    const T& front() const {
        return _head->value;
    }

    T& front() {
        return _head->value;
    }

    void pop_back() {
        if (_size == 1) {
            delete _tail;
            _tail = nullptr;
            _head = nullptr;
        } else {
            auto new_tail = _tail->prev;
            delete _tail;
            _tail = new_tail;
            new_tail->next = nullptr;
        }
        _size--;
    }

    void pop_front() {
        if (_size == 1) {
            delete _head;
            _tail = nullptr;
            _head = nullptr;
        } else {
            auto new_head = _head->next;
            delete _head;
            _head = new_head;
            new_head->prev = nullptr;
        }
        _size--;
    }

    void resize(size_t new_size, const T& value = T()) {
        if (new_size < _size) {
            auto count =  _size - new_size;
            for (size_t i = 0; i < count; ++i) {
                pop_back();
            }
        } else if (new_size > _size) {
            auto count =  new_size - _size;
            for (size_t i = 0; i < count; ++i) {
                push_back(value);
            }
        }
    }

    size_t size() const {
        return _size;
    }

    bool empty() const {
        return _size == 0;
    }

private:
    size_t _size;
    ListNode<T>* _head;
    ListNode<T>* _tail;
};

template <typename T>
class Stack {
public:

    void push(const T& new_element) {
        data.push_back(new_element);
    }

    void pop() {
        data.pop_back();
    }

    const T& top() const {
        return data.back();
    }

    T& top() {
        return data.back();
    }

    bool empty() const {
        return size() == 0;
    }

    size_t size() const {
        return data.size();
    }

private:
    Vector<T> data;
};

int main() {
    std::cout << "Test vectors" << std::endl;
    std::vector<int> vec_a;
    Vector<int> vec_b;
    for (int i = 0; i < 10; ++i) {
        vec_a.push_back(i * i);
        vec_b.push_back(i * i);
    }
    vec_a.back() = vec_a.front() = 144;
    vec_b.back() = vec_b.front() = 144;
    vec_a.resize(12);
    vec_b.resize(12);
    vec_a.push_back(14);
    vec_b.push_back(14);
    for (int i = 0; i < vec_a.size(); ++i) {
        std::cout << vec_a[i] << " " << vec_b[i] << std::endl;
    }
    std::cout << vec_a.size() << " " << vec_b.size() << std::endl;
    std::cout << vec_a.back() << " " << vec_b.back() << std::endl;
    std::cout << vec_a.front() << " " << vec_b.front() << std::endl;

    std::cout << std::endl << std::endl << "Test lists" << std::endl;
    std::list<int> lst_a;
    List<int> lst_b;
    for (int i = 0; i < 10; ++i) {
        lst_a.push_back(i);
        lst_b.push_back(i);
        lst_a.push_front(i * 2);
        lst_b.push_front(i * 2);
    }
    lst_a.back() = lst_a.front() = 123;
    lst_b.back() = lst_b.front() = 123;
    for (int i = 0; i < 20; ++i) {
        std::cout << lst_a.back() << " " << lst_b.back() << std::endl;
        lst_a.pop_back();
        lst_b.pop_back();
    }
    std::cout << lst_a.size() << " " << lst_b.size() << std::endl;


    std::cout << std::endl << std::endl << "Test stacks:" << std::endl;
    std::stack<int> stck_a;
    Stack<int> stck_b;
    for (int i = 0; i < 10; ++i) {
        stck_a.push(i * i * i - i * i);
        stck_b.push(i * i * i - i * i);
    }
    stck_a.top() = 12;
    stck_b.top() = 12;
    for (int i = 0; i < 10; ++i) {
        std::cout << stck_a.top() << " " << stck_b.top() << std::endl;
        stck_a.pop();
        stck_b.pop();
    }
    std::cout << stck_a.size() << " " << stck_b.size() << std::endl;
    return 0;
}