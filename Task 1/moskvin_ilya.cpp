
#include <cstdio>
#include <cstdlib>
#include <new>

class linear_allocator
{
	size_t size_;
	void * start_pointer_;
	void * pointer_;

public:
	explicit linear_allocator(const size_t size)
	{
		start_pointer_ = malloc(size);
		size_ = size;
		pointer_ = start_pointer_;
	}

	void * allocate(const size_t number_of_bytes)
	{
		const auto result = pointer_;
		pointer_ = static_cast<char*>(pointer_) + number_of_bytes;
		return result;
	}

	void free()
	{
		pointer_ = start_pointer_;
	}

	~linear_allocator()
	{
		::free(start_pointer_);
	}

	linear_allocator(const linear_allocator& other) = delete;
	linear_allocator& operator=(const linear_allocator& other) = delete;
};

class stack_allocator
{
	struct header
	{
		size_t size;
	};

	size_t size_;
	void * pointer_;
	void * start_pointer_;

public:
	explicit stack_allocator(const size_t size)
	{
		size_ = size;
		start_pointer_ = malloc(size_);
		pointer_ = start_pointer_;
	}

	void * allocate(const size_t number_of_bytes)
	{
		const auto result = pointer_;
		auto header_ptr = static_cast<header*>(pointer_) + number_of_bytes;
		pointer_ = static_cast<char*>(pointer_) + number_of_bytes + sizeof(header);
		new (header_ptr) header{ number_of_bytes };
		return result;
	}

	void pop_back()
	{
		const auto size = reinterpret_cast<header*>(static_cast<char*>(pointer_) - sizeof(header))->size;
		pointer_ = static_cast<char*>(pointer_) - size - sizeof(header);
	}

	void free()
	{
		pointer_ = start_pointer_;
	}

	~stack_allocator()
	{
		::free(start_pointer_);
	}

	stack_allocator(const stack_allocator& other) = delete;
	stack_allocator& operator=(const stack_allocator& other) = delete;
};

class list_allocator
{
#pragma pack(push, 1)
	struct item
	{
		size_t size;
		union
		{
			struct
			{
				item * next;
				item * prev;
			};
			char data[1];
		};
	};
#pragma  pack(pop)

	item * first_;
	size_t size_;
	void * pointer_;

	void add(item* another) const
	{
		const auto prev = first_->prev;
		another->prev = prev;
		prev->next = another;
		first_->prev = another;
		another->next = first_;
	}

	void remove(item* item)
	{
		if (first_->next == first_)
		{
			first_ = nullptr;
		}
		else
		{
			const auto prev = item->prev;
			const auto next = item->next;
			prev->next = next;
			next->prev = prev;
			if (item == first_)
			{
				first_ = next;
			}
		}
	}

	item * find_block(const size_t size) const
	{
		auto current = first_->next;
		while (true)
		{
			if (current->size >= size)
			{
				return current;
			}

			if (current == first_)
				break;

			current = current->next;
		}
		return nullptr;
	}

	void merge(item* block)
	{
		auto current = first_->next;
		while (true)
		{
			if (reinterpret_cast<char*>(current) == reinterpret_cast<char*>(block) + static_cast<item*>(block)->size + sizeof(size_t))
			{
				block->size = block->size + current->size;
				block->next = current->next;
				current->next->prev = block;
				remove(current);
			}

			if (reinterpret_cast<char*>(current) + current->size + sizeof(size_t) == reinterpret_cast<char*>(block))
			{
				current->size = current->size + block->size;
				current->next = block->next;
				block->next->prev = current;
				remove(block);
			}

			if (current == first_)
				break;

			current = current->next;
		}
	}

public:
	explicit list_allocator(const size_t size)
	{
		size_ = size;
		pointer_ = malloc(size_);
		first_ = new(pointer_) item{ size_ };
		first_->next = first_;
		first_->prev = first_;
	}

	void free(void * pointer)
	{
		const auto new_item = reinterpret_cast<item*>(static_cast<char*>(pointer) - sizeof(size_t));
		add(new_item);
		merge(new_item);
	}

	void * allocate(const size_t number_of_bytes)
	{
		const auto block = find_block(number_of_bytes);
		const auto old_size = block->size;
		block->size = number_of_bytes;
		const auto new_block = new (reinterpret_cast<item*>(reinterpret_cast<char*>(block) + number_of_bytes + sizeof(size_t))) item{ old_size - number_of_bytes };
		add(new_block);
		remove(block);
		return block->data;
	}

	~list_allocator()
	{
		::free(pointer_);
	}

	list_allocator(const list_allocator& other) = delete;
	list_allocator& operator=(const list_allocator& other) = delete;
};


class demonstration
{
public:
	static void test_linear()
	{
		printf("%s\n", "test_linear:");
		auto a = new linear_allocator(10);
		const auto p1 = a->allocate(5);
		const auto p2 = a->allocate(2);
		const auto p3 = a->allocate(2);
		printf("%p %p %p\n", p1, p2, p3);
		a->free();
		const auto p4 = a->allocate(7);
		printf("%p\n", p4);
		delete a;
	}

	static void test_stack()
	{
		printf("%s\n", "test_stack:");
		auto a = new stack_allocator(100);
		const auto p1 = a->allocate(5);
		const auto p2 = a->allocate(2);
		const auto p3 = a->allocate(2);
		printf("%p %p %p\n", p1, p2, p3);
		a->pop_back();
		const auto p4 = a->allocate(3);
		printf("%p\n", p4);
		a->free();
		const auto p5 = a->allocate(7);
		printf("%p\n", p5);
		delete a;
	}

	static void test_list()
	{
		printf("%s\n", "test_list:");
		auto a = new list_allocator(100);
		const auto p1 = a->allocate(40);
		const auto p2 = a->allocate(10);
		const auto p3 = a->allocate(20);
		printf("%p %p %p\n", p1, p2, p3);
		a->free(p2);
		a->free(p3);
		const auto p4 = a->allocate(30);
		printf("%p %p\n", p1, p4);
		delete a;
	}
};


int main()
{
	demonstration::test_linear();
	demonstration::test_stack();
	demonstration::test_list();
	system("pause");
	return 0;
}
