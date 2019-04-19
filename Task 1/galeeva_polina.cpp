#include <cstddef>
#include <iostream>
#include <string.h>
#include <algorithm>
#include <string>
#include <glob.h>

using namespace std;


class LinearAllocator {
    char *_start;
    size_t _size, _pos;

public:
    explicit LinearAllocator(size_t def_size);
    ~LinearAllocator();
    char *get_mem(size_t size);
    void free();
};

LinearAllocator::~LinearAllocator() {
    delete _start;
}

LinearAllocator::LinearAllocator(size_t def_size) {
    _start = new char[def_size];
    _size = def_size;
    _pos = 0;
}

char *LinearAllocator::get_mem(size_t size) {
    if (size + _pos < _size)
    {
        size_t old_pos = _pos;
        _pos += size;
        return _start + old_pos;
    }
    return nullptr;
}

void LinearAllocator::free()
{
    delete _start;
    _start = new char[_size];
    _pos = 0;
};


#define STACK_ALLOC_DEF_SIZE 128


#pragma pack(push, 1)
struct StackHeader
{
    StackHeader *left;
    size_t blockSize;
};
#pragma pack(pop)

class StackAllocator {
    size_t _defaultSize;
    void _insertBlock(char *source, StackHeader *stackHeader);
    StackHeader _readBlock(const char *source);
public:
    char *_start, *_offset;
    StackAllocator() : StackAllocator(STACK_ALLOC_DEF_SIZE) {}
    explicit StackAllocator(size_t _defaultSize);
    ~StackAllocator();
    char *push_mem(size_t size);
    char *pop_mem();
};

void StackAllocator::_insertBlock(char *source, StackHeader *stackHeader) {
    copy(static_cast<const char *>(static_cast<const void *>(stackHeader)),
         static_cast<const char *>(static_cast<const void *>(stackHeader)) + sizeof(StackHeader),
         source);
}

StackHeader StackAllocator::_readBlock(const char *source) {
    StackHeader stackHeader{};
    copy(source,
         source + sizeof(StackHeader),
         static_cast<char *>(static_cast<void *>(&stackHeader)));
    return stackHeader;
}

StackAllocator::StackAllocator(size_t _defaultSize) {
    this->_defaultSize = _defaultSize;
    _start = new char[this->_defaultSize];
    _offset = _start;
    StackHeader stackHeader = {nullptr, 0};
    _insertBlock(_offset, &stackHeader);
}

StackAllocator::~StackAllocator() {
    delete _start;
    _start = nullptr;
    _offset = nullptr;
    _defaultSize = 0;
}

char *StackAllocator::push_mem(size_t size) {
    StackHeader stackHeader = _readBlock(_offset);
    size_t curOffset = _offset - _start + stackHeader.blockSize;
    size_t futureOffset = curOffset + sizeof(StackHeader) + size;
    if ( futureOffset <= _defaultSize)
    {
        StackHeader *left = nullptr;
        if (stackHeader.left != nullptr || stackHeader.blockSize != 0)
            left = (StackHeader*)_offset;
        StackHeader newHeader = {left, sizeof(StackHeader) + size};
        _offset += stackHeader.blockSize;
        _insertBlock(_offset, &newHeader);

        StackHeader insertedBlock = _readBlock(_offset);
        return _offset + sizeof(StackHeader);
    }
    return nullptr;
}

char *StackAllocator::pop_mem() {
    StackHeader stackHeader = _readBlock(_offset);
    char * ret = _offset + sizeof(StackHeader); // start of the last block
    if (stackHeader.left != nullptr) {
        _offset = static_cast<char *>(static_cast<void *>(stackHeader.left));
        return ret;
    } else {
        if (stackHeader.blockSize != 0)
        {
            StackHeader startHeader = {nullptr, 0};
            _insertBlock(_offset, &startHeader);
            return ret;
        }
        return nullptr;
    }
}


#define DEFAULT_SIZE 1

using namespace std;


class VectorAllocator
{
    char *_start;
    size_t _size;
    int _pos;

    void Resize();
public:
    VectorAllocator();

    void push_back(char value);
    char pop_back();

    void clear();
    ~VectorAllocator();
};

void  VectorAllocator::Resize()
{
    char *new_start = new char[_size * 2];
    copy(_start, _start+_size, new_start);
    delete _start;
    _start = new_start;
    _size *= 2;
}

VectorAllocator::VectorAllocator()
{
    _start = new char[DEFAULT_SIZE];
    _size = DEFAULT_SIZE;
    _pos = 0;
}

void VectorAllocator::push_back(char value)
{
    if (_pos >= _size)
        Resize();
    _start[_pos++] = value;
}

char VectorAllocator::pop_back()
{
    if (!_pos)
        throw "Empty sequence";
    return _start[_pos--];
}

void VectorAllocator::clear()
{
    memset(_start, 0, _size - 1);
}

VectorAllocator::~VectorAllocator() {
    delete _start;
}


#define POOL_DEF_SIZE 2048

#pragma pack(push, 1)
struct CBlockDescriptor {
    size_t size;
    bool isFree;
};
#pragma pack(pop)

#pragma pack(push, 1)
class CItem{
public:
    CBlockDescriptor header;
    union {
        struct {
            CItem *Prev;
            CItem *Next;
        };
        char Data[1];
    };
    CItem(void * curAddress, CBlockDescriptor header, CItem *Prev, CItem *Next);
    CItem();
};
#pragma pack(pop)

class PoolAllocator {
    char * _start;
    size_t _size;
    CItem *First;
    void Add(CItem *N);
    void Remove(CItem *X);
    void Merge();
    CItem *_findFreeEnough(size_t size);

public:
    PoolAllocator() : PoolAllocator(POOL_DEF_SIZE) {}
    explicit PoolAllocator(size_t size);
    ~PoolAllocator();
    void *Alloc(size_t size);
    void Free(void *p);
};

void PoolAllocator::Add(CItem *N) {\
    if (First) {
        CItem *Prev = First->Prev;
        N->Prev = Prev;
        Prev->Next = N;

        N->Next = First;
        First->Prev = N;
    } else {
        First = N;
        First->Prev = First->Next =  N;
    }
    N->header.isFree = true;
}

void PoolAllocator::Free(void *p) {
    auto *N = (CItem*)((char*)p - sizeof(CBlockDescriptor));
    Add(N);
    Merge();
}

PoolAllocator::PoolAllocator(size_t size) {
    if (size < sizeof(CItem))
        throw "Small pool size (PoolAllocator) : " + std::to_string(size);
    _start = new char[size];
    _size = size;
    new (_start) CItem(_start, {size, true}, (CItem*)_start, (CItem*)_start);
    First = (CItem*)_start;
}

void *PoolAllocator::Alloc(size_t size) {
    size_t actualSize = size + 2 * sizeof(CBlockDescriptor);
    auto X = _findFreeEnough(actualSize);
    char * NXAddress = (char*)X + actualSize;

    CItem NX;
    NX.header.size = X->header.size- actualSize;
    if (_start + _size < NXAddress + sizeof(CItem))
        First = nullptr;
    else {
        if (X == X->Next && X == First)
            First = NX.Next = NX.Prev = (CItem *) (NXAddress);
        else {
            NX.Prev = X->Prev;
            NX.Next = X->Next;

            NX.Prev->Next = (CItem *) (NXAddress);
            NX.Next->Prev = (CItem *) (NXAddress);
        }
        new(NXAddress) CItem(NXAddress, {NX.header.size, true}, NX.Prev, NX.Next);
    }

    new (X) CItem (X, {actualSize, false}, nullptr, nullptr);
    return (char*)X + sizeof(CBlockDescriptor);
}

void PoolAllocator::Remove(CItem *X) {
    if (First->Next == First)
        First = nullptr;
    else {
        auto Prev = X->Prev;
        auto Next = X->Next;
        Prev->Next = Next;
        Next->Prev = Prev;

        if (X == First)
            First = Next;
    }
}

PoolAllocator::~PoolAllocator() {
    delete[] _start;
}

void PoolAllocator::Merge() {
    auto X = (CItem*)_start;
    auto END = (CItem*)(_start + _size);
    while (true)
    {
        auto rCItem = (CItem*)((char*)X + X->header.size);
        if (rCItem == END)
            break;
        if (X->header.isFree && rCItem->header.isFree) {
            new(X) CItem(X, {X->header.size + rCItem->header.size, true}, nullptr, nullptr);
            rCItem = X;
        }
        X = rCItem;
    }
    First = nullptr;
    X = (CItem*)_start;
    while (X != END) {
        if (X->header.isFree)
            Add(X);
        X = (CItem*)((char*)X + X->header.size);
    }
}

CItem *PoolAllocator::_findFreeEnough(size_t size) {
    CItem *X = First;
    while (true) {
        if (X->header.size >= size)
            break;
        if (X->Next == First)
            throw "Big block size (PoolAllocator::_findFreeEnough) : " + std::to_string(size);
        X = X->Next;
    }
    return X;
}


CItem::CItem(void *currentAddress, CBlockDescriptor header, CItem *Prev, CItem *Next) {
    char* tailHeader = (char*)currentAddress + header.size - sizeof(CBlockDescriptor);

    this->header = header;
    this->Prev = Prev;
    this->Next = Next;

    new (tailHeader) CBlockDescriptor {
            header.size,
            header.isFree
    };
}

CItem::CItem() {
    header = CBlockDescriptor{};
    Prev = nullptr;
    Next = nullptr;
}


void tryVectorAllocator()
{
    auto vc = VectorAllocator();
    std::string str = "!dlrow ,olleh!";
    for(char& c : str) {
        vc.push_back(c);
    }

    char k;
    while (true)
    {
        try {
            k = vc.pop_back();
            printf("%c", k);
        } catch (const char* &e)
        {
            printf("\n%s\n", e);
            break;
        }

    }
}

void tryLinearAllocator()
{
    auto la = LinearAllocator(32);
    char *part_10 = la.get_mem(10);
    char *part_22 = la.get_mem(21);
    char *part_null_1 = la.get_mem(1);

    part_10 = const_cast<char *>("012345678");
    part_22 = const_cast<char *>("0123456789_0123456789_");

    printf("%s\n%s\n", part_10, part_22);
    printf("%zu\n", (size_t)part_null_1);
}

void tryStackAllocator()
{
    auto sa = StackAllocator(58);
    // 16 байт на StackHeader
    char *part1 = sa.push_mem(2);
    char *part2 = sa.push_mem(3);
    char *part3 = sa.push_mem(4);
    char *expected_null = sa.push_mem(10);

    char a[] = {'1', '\0'};
    std::copy(a, a + 2, part1);

    char b[] = {'_', '2', '\0'};
    std::copy(b, b + 3, part2);

    char c[] = {'+', '-', '3', '\0'};
    std::copy(c, c + 4, part3);

    printf("%s\n", (const char*)part1);
    printf("%s\n", (const char*)part2);
    printf("%s\n", (const char*)part3);
    printf("%zu\n", (size_t)expected_null);

    while (true) {
        char *last_part = sa.pop_mem();
        if (last_part == nullptr)
            break;
        printf("%s\n", last_part);
    }

    char *whole_part = sa.push_mem(42);
    for(int i=0; i<42; ++i)
        printf("%c", whole_part[i]);
    printf("\n");
}

void tryPoolAllocator1()
{
    auto pa = PoolAllocator(155 + 27);
    char *buf1 = (char*)pa.Alloc(32);
    string s1 = "012345678_012345678_012345678_x";
    copy(s1.begin(), s1.end(), buf1);

    char *buf2 = (char*)pa.Alloc(64);
    string s2 = "012345678_012345678_012345678_012345678_012345678_012345678_12x";
    copy(s2.begin(), s2.end(), buf2);

    char *buf3 = (char*)pa.Alloc(32);
    string s3 = "abcdefghi_abcdefghi_abcdefghi_X";
    copy(s3.begin(), s3.end(), buf3);


    printf("%s\n%s\n%s\n", buf1, buf2, buf3);
    pa.Free(buf2);

    buf2 = (char*)pa.Alloc(32);
    for(int i=0; i<64; i++)
        printf("%c", buf2[i]);
    printf("\n");
}

void tryPoolAllocator2()
{
    auto pa = PoolAllocator(24 * 3 + 18 * 3);
    char *buf1 = (char*)pa.Alloc(24);
    char *buf2 = (char*)pa.Alloc(24);
    char *buf3 = (char*)pa.Alloc(24);

    string s = "0123456789_0123456789_01|";
    copy(s.begin(), s.end(), buf1);
    s = "abcdefghi_abcdefghi_ab|";
    copy(s.begin(), s.end(), buf2);
    s = "абвгдежзи_абвгдежзи_аб|";
    copy(s.begin(), s.end(), buf3);


    pa.Free(buf1);
    pa.Free(buf3);
    pa.Free(buf2);
    char *buf4 = (char*)pa.Alloc(108);
    for(int i=0; i<108; ++i)
        printf("%c", buf4[i]);
    printf("\n");
}


int main() {
    try {
        tryPoolAllocator1();
        tryPoolAllocator2();
    } catch (const char * ex)
    {
        cout << "Exception: " << ex << endl;
    } catch (string ex)
    {
        cout << "Exception: " << ex << endl;
    }
    return 0;
}
