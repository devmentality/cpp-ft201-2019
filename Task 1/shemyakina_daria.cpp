#include <iostream>
#include <ppltasks.h>

size_t MEMORY_SIZE = pow(2, 10);

template <typename  T>
class Linear
{
	void* start;
	size_t offset;
	size_t size;
	size_t item_size;


public:
	Linear()
	{
		start = malloc(MEMORY_SIZE);
		size = 0;
		item_size = sizeof(T);
		offset = 0;
	}

	void push_back(T item)
	{
		void *item_start = static_cast<char*>(start) + offset;
		*static_cast<T*>(item_start) = item;
		size++;
		offset += item_size;
	}

	T get(size_t index)
	{
		if (index < 0 || index >= size)
			throw "Index out of range";
		void * item_ptr = static_cast<char*>(start) + (item_size * index);
		return *static_cast<T *>(item_ptr);
	}

	void remove_all()
	{
		size = 0;
		offset = 0;
	}

	void dump()
	{
		std::cout << "size: " << size;
		
		for (int i=0; i < size; i++)
			std::cout << "; " << i << ": " << get(i);		

		std::cout << std::endl;			
	}
};

template <typename T>
class Stack
{
	size_t capacity;
	void* start;
	size_t offset;
	size_t last_block_offset;
	size_t header_size;

	size_t find_last_block()
	{
		if (offset == 0)
			return 0;
		size_t current_offset = 0;
		size_t current_last_block = 0;
		while (current_offset < offset)
		{
			void *header_start = static_cast<char*>(start) + current_offset;
			size_t item_size = *static_cast<size_t*>(header_start);
			current_offset += header_size + item_size;
			current_last_block = current_offset - item_size;
		}
		return current_last_block;
	}

public:
	Stack()
	{
		capacity = MEMORY_SIZE;	
		start = malloc(capacity);
		if (!start)
			throw "Wasn't able to malloc the memory";
		header_size = sizeof(size_t);
		offset = 0;
		last_block_offset = 0;
	}

	T pop()
	{
		if (last_block_offset == 0)
			throw "Nothing to pop";
		void *item_start = static_cast<char*>(start) + last_block_offset;
		T item = *static_cast<T*>(item_start);
		offset = last_block_offset - header_size;
		last_block_offset = find_last_block();
		return item;
	}

	void push(T item)
	{
		size_t item_size = sizeof(item);
		void *header_start = static_cast<char*>(start) + offset;
		*static_cast<size_t*>(header_start) = item_size;
		void *item_start = static_cast<char*>(header_start) + header_size;
		*static_cast<T*>(item_start) = item;
		last_block_offset = offset + header_size;
		offset = last_block_offset + item_size;
	}

	void remove_all()
	{
		offset = 0;
		last_block_offset = 0;
		free(start);
		start = malloc(MEMORY_SIZE);
	}

	void dump()
	{
		if (offset == 0)
			return;
		size_t current_offset = 0;
		while (current_offset < offset)
		{
			void *header_start = static_cast<char*>(start) + current_offset;
			size_t item_size = *static_cast<size_t*>(header_start);
			void *item_start = static_cast<char*>(header_start) + header_size;
			T item = *static_cast<T*>(item_start);
			std::cout << "size: " << item_size << ", content: " << item;
			current_offset += header_size + item_size;
			if (current_offset < offset)
				std::cout <<  "; ";
		}
		std::cout << std::endl;	
	}
};

#pragma pack(push, 1) 
struct ListItem
{
	size_t size;
	union
	{
		struct
		{
			ListItem* next;
			ListItem* prev;
		};
		void* data[1];
	};
};
#pragma pack(pop)

class FreeListItems
{
	ListItem * first;

	void allocate_memory(size_t block_count, size_t item_size, size_t header_size)
	{
		auto block_size = header_size + item_size;
		auto memory_size = (header_size + item_size) * block_count;
		auto ptr = malloc(memory_size);
		for (size_t i = 0; i < block_count; i++)
		{			
			void *block_ptr = static_cast<char*>(ptr) + block_size * i;
			auto item = static_cast<ListItem*>(block_ptr);
			item->size = block_size;
			add(item);			
		}
	}


public:
	FreeListItems(size_t item_size, size_t block_count)
	{
		first = nullptr;
		allocate_memory(block_count, item_size, sizeof(ListItem));
	}

	void dump()
	{
		if (first == nullptr) {
			std::cout << "no free blocks" << std::endl;
			return;
		}
		auto current_block = first;
		do
		{
			std::cout << "size: " <<current_block->size;
			std::cout << ", location: " << current_block;
			current_block = current_block->next;
			if (current_block != first)
				std::cout << "; ";
		} while (current_block != first);
		std::cout << std::endl;
	}

	// добавление блока в список свободных блоков
	void add(ListItem* item)
	{
		if (!first)
		{
			first = item;
			item->next = item;
			item->prev = item;
			return;
		}
		ListItem *prev = first->prev;
		item->prev = prev;
		prev->next = item;
		first->prev = item;
		item->next = first;
	}

	// удаление блока из списка свободных блоков
	void remove(ListItem* item)
	{
		if (first->next == first)
		{
			first = nullptr;
			return;
		}
		auto prev = item->prev;
		auto next = item->next;
		prev->next = next;
		next->prev = prev;
		if (item == first)
			first = next;
	}

	ListItem * get_block()
	{
		if (first == nullptr)
			throw "No free blocks";
		auto block = first;
		remove(first);
		return block;
	}
};

template <typename T>
class ListNode
{
public:
	ListItem * item;
	T* value_ptr;
	ListNode<T> * next;
	ListNode<T> * prev;

	ListNode<T>(ListItem * item_ptr)
	{
		void * ptr = item_ptr + sizeof(size_t);
		value_ptr = static_cast<T *>(ptr);
		item = item_ptr;
		next = nullptr;
		prev = nullptr;
	}

	void set_value(T value)
	{
		*value_ptr = value;		
	}

	T get_value()
	{
		return  *value_ptr;
	}
};

template <typename T>
class LinkedList
{
	FreeListItems * free_blocks;
	ListNode<T> * head;
	ListNode<T> * tail;

	ListNode<T> * get_node(T value)
	{
		auto block = free_blocks->get_block();
		auto node = new ListNode<T>(block);
		node->set_value(value);
		return node;
	}
	

public:
	LinkedList<T>()
	{
		size_t block_count = 5;
		free_blocks = new FreeListItems(sizeof(T), block_count);
		head = tail = nullptr;
	}

	void add_first(T value)
	{
		auto node = get_node(value);
		if (head == nullptr) {

			head = node;
			tail = node;
		}
		else
		{
			node->next = head;
			head = node;
		}
	}

	T get_first()
	{
		return head->get_value();
	}

	void delete_first()
	{
		if (head == nullptr)
			return;
		free_blocks->add(head->item);
		head = head->next;		
	}

	void add_last(T value)
	{
		auto node = get_node(value);
		if (head == nullptr) {

			head = node;
			tail = node;
		}
		else
		{
			node->prev = tail;
			tail->next = node;
			tail = node;
		}
	}

	T get_last()
	{
		return tail->get_value();
	}

	void delete_last()
	{
		if (tail == nullptr)
			return;
		free_blocks->add(tail->item);
		tail->prev->next = nullptr;
		tail = tail->prev;
	}

	void remove_all()
	{
		auto current = head;
		while (current != nullptr)
		{
			auto next = current->next;
			free_blocks->add(current->item);
			delete current;
			current = next;			
		}
		head = nullptr;
		tail = nullptr;
	}

	void dump()
	{
		if (!head) {
			std::cout << "no elements" << std::endl;
			return;
		}
		auto current = head;

		while (current != nullptr)
		{
			std::cout << current->get_value();
			current = current->next;
			if (current != nullptr)
				std::cout << "; ";			
		}

		std::cout << std::endl;
	}
};

void Linear_Test()
{
	auto arr = Linear<int>();
	arr.dump();
	arr.push_back(10);
	arr.dump();
	arr.push_back(2);
	arr.dump();
	arr.push_back(9);
	arr.dump();
	arr.push_back(7);
	arr.dump();
	arr.remove_all();
	arr.dump();
	arr.push_back(8);
	arr.dump();
}


void LinkedList_Test()
{
	auto list = LinkedList<int>();
	list.dump();
	list.add_first(6);
	list.dump();
	list.add_first(7);
	list.dump();
	list.add_last(8);
	list.dump();
	list.add_last(9);
	list.dump();
	std::cout << "first: " << list.get_first() << std::endl;
	std::cout << "last: " << list.get_last() << std::endl;
	list.delete_first();
	list.dump();
	list.delete_last();
	list.dump();
	list.remove_all();
	list.dump();

	
}

void Stack_Test()
{
	auto stack = Stack<int>();
	stack.push(9);
	stack.dump();
	stack.push(8);
	stack.dump();
	stack.push(100);
	stack.dump();
	std::cout << "popped item: " << stack.pop() << std::endl;
	stack.dump();
	stack.push(90);
	stack.dump();
	stack.remove_all();
	stack.dump();
	stack.push(7);
	stack.dump();
}

int main()
{
	std::cout << "Linear Test"<<std::endl;
	Linear_Test();
	std::cout << "Stack Test" << std::endl;
	Stack_Test();
	std::cout << "LinkedList Test" << std::endl;
	LinkedList_Test();
	return 0;
}