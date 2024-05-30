#include <iostream>
#include <map>
#include <vector>

#include "segment_allocator.h"

template <typename AllocatorType>
void PrintCurrentStat(const AllocatorType& allocator) {
    std::cout << "\t\t";
    for (uint16_t size = 0; size <= 64; size++) {
        const auto* info = allocator.GetBlocksStat();
        info += size;
        if (info->used_blocks != 0 || info->busy_blocks != 0 || info->used_segments != 0) {
            std::cout << size << "(" << info->used_blocks << "," << info->busy_blocks << "," << info->used_segments << ")  ";
        }
    }
    auto [errors_external, errors_internal] = allocator.GetErrors();
    std::cout << "\t" << errors_external << "," << errors_internal << "\n";
}

int main() {

    static constexpr uint32_t error_result = 0;
    SegmentAllocator<15, 6*64, 2*64, 64, error_result> allocator;
    std::vector<uint32_t> all_sizes = {27, 23};
    std::map<uint32_t, std::vector<uint32_t>> allocations;

    std::cout << "Start:";
    PrintCurrentStat(allocator);

    for (int i = 0; i < 20; i++) {
        uint32_t size = all_sizes[rand() % all_sizes.size()];
        uint32_t index = allocator.Allocate(size);
        if (index == error_result) {
            std::cout << size << ": null";
        } else {
            std::cout << size << ": " << index;
        }
        if (index != error_result) {
            allocations[size].push_back(index);
        }
        PrintCurrentStat(allocator);
    }

    std::cout << "--------------------------------\n";
    for (const auto& [size, data] : allocations) {
        for (uint32_t index : data) {
            std::cout << "Free " << index << "," << size << ": " << allocator.Free(index, size);
            PrintCurrentStat(allocator);
        }
    }

    return 0;
}
