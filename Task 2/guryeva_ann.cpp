#include <algorithm>
#include <assert.h>
#include <string>
#include <cstdio>

#define MIN_SIZE 26
#define DEFAULT_BUDDIES 3


struct CBlockHeader {
	char level;
	bool isFree;
};

class CBuddyBlockDescriptor {
public:
	CBlockHeader Header;
	union {
		struct {
			CBuddyBlockDescriptor* Prev;
			CBuddyBlockDescriptor* Next;
		};
		char Data[8];
	};
	CBuddyBlockDescriptor(CBlockHeader header, CBuddyBlockDescriptor* prev, CBuddyBlockDescriptor* next) :
		Header(header), Prev(prev), Next(next) {}
};


class BuddyAllocator {
	size_t _start;
	CBuddyBlockDescriptor** _table;
	void _Remove(CBuddyBlockDescriptor *ptr);
	void _Add(CBuddyBlockDescriptor *ptr, char level);
	void* _AllocItem(size_t size);
	void _DivideBuddy(CBuddyBlockDescriptor* ptr);
	void _MergeRight(CBuddyBlockDescriptor* ptr);
public:
	BuddyAllocator();
	void *Alloc(size_t size);
	void Free(void * ptr);

};

void *BuddyAllocator::Alloc(size_t size) {
	return _AllocItem(size);
}

void BuddyAllocator::Free(void *ptr) {
	auto Ptr = (CBuddyBlockDescriptor*)((char*)ptr - sizeof(CBlockHeader));
	_Add(Ptr, Ptr->Header.level);

	auto left_block = (CBuddyBlockDescriptor*)((char*)Ptr - (MIN_SIZE << (DEFAULT_BUDDIES - Ptr->Header.level)));
	auto right_block = (CBuddyBlockDescriptor*)((char*)Ptr + (MIN_SIZE << (DEFAULT_BUDDIES - Ptr->Header.level)));

	if ((size_t)left_block >= _start && left_block->Header.level == Ptr->Header.level && ((size_t)left_block - _start) % 2 == 0)
		_MergeRight(left_block);

	if (right_block->Header.isFree && right_block->Header.level == Ptr->Header.level && ((size_t)ptr - _start) % 2 == 0)
		_MergeRight(Ptr);
}

BuddyAllocator::BuddyAllocator() {
	_start = (size_t) new char[MIN_SIZE << DEFAULT_BUDDIES];
	_table = new CBuddyBlockDescriptor*[DEFAULT_BUDDIES + 1];
	new ((char*)_start) CBuddyBlockDescriptor(
		{ 0, true }, (CBuddyBlockDescriptor*)_start, (CBuddyBlockDescriptor*)_start);
	for (int i = 0; i <= DEFAULT_BUDDIES; ++i)
		_table[i] = nullptr;
	_table[0] = (CBuddyBlockDescriptor*)_start;
}


void BuddyAllocator::_DivideBuddy(CBuddyBlockDescriptor *ptr) {
	_Remove(ptr);

	char new_level = static_cast<char>(ptr->Header.level + 1);

	auto left_half = CBuddyBlockDescriptor({ new_level, true }, nullptr, nullptr);
	auto right_half = CBuddyBlockDescriptor({ new_level, true }, nullptr, nullptr);

	auto left_address = ptr;
	auto right_address = (CBuddyBlockDescriptor*)((char*)ptr + (MIN_SIZE << (DEFAULT_BUDDIES - new_level)));

	new (left_address) CBuddyBlockDescriptor(left_half);
	new (right_address) CBuddyBlockDescriptor(right_half);

	_Add(left_address, left_half.Header.level);
	_Add(right_address, right_half.Header.level);
}

void BuddyAllocator::_MergeRight(CBuddyBlockDescriptor *ptr) {
	_Remove(ptr);
	_Remove((CBuddyBlockDescriptor*)((char*)ptr + (MIN_SIZE << (DEFAULT_BUDDIES - ptr->Header.level))));


	char new_level = static_cast<char>(ptr->Header.level - 1);
	ptr->Header.level = new_level;
	_Add(ptr, ptr->Header.level);
}

void* BuddyAllocator::_AllocItem(size_t size) {
	for (char i = DEFAULT_BUDDIES; i >= 0; --i) {
		int block_size = MIN_SIZE << (DEFAULT_BUDDIES - i);
		if (_table[i] != nullptr) {
			auto cur_block = _table[i];
			if ((int)size <= (block_size - 4) / 2) {
				_DivideBuddy(cur_block);
				return _AllocItem(size);
			}
			if ((int)size <= block_size - 2) {
				_Remove(cur_block);
				new ((char*)cur_block) CBuddyBlockDescriptor(
					{ i, false }, nullptr, nullptr);
				return (char*)cur_block + sizeof(CBlockHeader);
			}
		}
	}
	throw "Big block size (BuddyAllocator::_AllocItem): " + std::to_string(size);
}

void BuddyAllocator::_Remove(CBuddyBlockDescriptor *ptr) {
	if (_table[ptr->Header.level]->Next == _table[ptr->Header.level])
		_table[ptr->Header.level] = nullptr;
	else {
		auto Prev = ptr->Prev;
		auto Next = ptr->Next;

		Prev->Next = Next;
		Next->Prev = Prev;

		if (_table[ptr->Header.level] == ptr)
			_table[ptr->Header.level] = Next;
	}
}

void BuddyAllocator::_Add(CBuddyBlockDescriptor *ptr, char level) {
	auto Next = ptr;
	auto First = _table[level];
	if (First == nullptr) {
		First = ptr;
		_table[level] = ptr;
	}
	else {
		Next = First->Next;
	}

	First->Next = ptr;

	ptr->Prev = ptr;
	ptr->Next = Next;

	Next->Prev = ptr;

	ptr->Header.isFree = true;
}

void main()
{
	auto ba = BuddyAllocator();
	char *buf1 = (char*)ba.Alloc(24);
	char *buf2 = (char*)ba.Alloc(24);

	char s1[] = "012345678_012345678_018";
	char s2[] = "abcdefghj_abcdefghj_abc";

	ba.Free(buf1);
	ba.Free(buf2);

	char *buf3 = (char*)ba.Alloc(50);
	char *buf4 = (char*)ba.Alloc(50);
	for (int i = 0; i < 50; i++)
		printf("%c", buf4[i]);
	printf("\n");
}


