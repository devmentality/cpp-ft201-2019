#include <iostream>
#include <vector>
#include <map>
#include <set>

using namespace std;


class BuddyAllocator {
private:
    char* _base_ptr;
    size_t _size;
    vector<set<unsigned long>> _free_blocks_by_order;
    map<unsigned long, uint> _order_of_allocated_block;

    void _try_create_at_order(uint order) {
        auto i = order;
        for(; _free_blocks_by_order[i].empty(); i++)
            if (i == _free_blocks_by_order.size()) return;
        auto offset = *_free_blocks_by_order[i].begin();
        _free_blocks_by_order[i].erase(offset);
        for(auto j = i - 1; j >= order; j--)
            _free_blocks_by_order[j].insert(offset + (1 << j));
        _free_blocks_by_order[order].insert(offset);
    }

    unsigned long _get_buddy_offset(ptrdiff_t offset, uint order) {
        auto size = static_cast<size_t>(1 << order);
        if (offset % size)
            return offset - size;
        return offset + size;
    }

public:
    explicit BuddyAllocator(size_t max_order = 20) {
        _size = static_cast<size_t>(1 << max_order);
        _base_ptr = static_cast<char *>(malloc(_size));
        _free_blocks_by_order = vector<set<unsigned long>>(max_order + 1);
        for (int i = 0; i < max_order; ++i)
            _free_blocks_by_order[i] = set<unsigned long>();
        _free_blocks_by_order[max_order].insert(0);
        _order_of_allocated_block = map<unsigned long, uint>();
    }

    void* Allocate(size_t size) {
        if (size > _size)
            return nullptr;
        auto __size = 1;
        uint order = 0;
        for (; __size < size; __size <<= 1)
            order++;
        if (_free_blocks_by_order[order].empty())
            _try_create_at_order(order);
        if (_free_blocks_by_order[order].empty())
            return nullptr;
        auto offset = *_free_blocks_by_order[order].begin();
        _free_blocks_by_order[order].erase(offset);
        _order_of_allocated_block[offset] = order;
        return _base_ptr + offset;
    }

    void Free(void* pointer) {
        auto offset = static_cast<char*>(pointer) - _base_ptr;
        if (!_order_of_allocated_block.count(offset))
            throw invalid_argument("No memory was allocated from given pointer.");
        auto order = _order_of_allocated_block[offset];
        auto buddy_offset = _get_buddy_offset(offset, _order_of_allocated_block[offset]);
        _order_of_allocated_block.erase(offset);
        while (!_order_of_allocated_block.count(buddy_offset) && order < _free_blocks_by_order.size() - 1) {
            if (!_free_blocks_by_order[order].count((buddy_offset)))
                throw exception();
            _free_blocks_by_order[order].erase(buddy_offset);
            if (buddy_offset < offset)
                offset = buddy_offset;
            order++;
            buddy_offset = _get_buddy_offset(offset, order);
        }
        _free_blocks_by_order[order].insert(offset);
    }
    
    vector<pair<unsigned long, long>> Dump() {
        auto result = vector<pair<unsigned long, long>>();
        for (auto kvp: _order_of_allocated_block)
            result.emplace_back(pair<unsigned long, long>(kvp.first, 1 << kvp.second));
        return result;
    }
};


int main() {
    auto buddy_allocator = BuddyAllocator();
    auto a = buddy_allocator.Allocate(1500);
    auto dump_a = buddy_allocator.Dump(); //returns a vector of pairs kind of <offset, size>;
    auto b = buddy_allocator.Allocate(380);
    buddy_allocator.Free(a);
    dump_a = buddy_allocator.Dump();
    return 0;
}