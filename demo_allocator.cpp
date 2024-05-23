#include "segment_allocator.h"
#include <iostream>
#include <optional>
#include <variant>
#include <vector>

// sudo apt install catch

struct Allocation {
	size_t size;
};

struct Free {
	size_t start;
	size_t size;
};

using Action = std::variant<Allocation, Free>;

void PrintSegments(SegmentAllocator& allocator) {
	const auto& segments = allocator.DebugSegmentsStart();
	if (!segments.empty()) {
		for (auto [start, size] : segments) {
			std::cout << "[" << start << "," << (start + size) << ") ";
		}
	} else {
		std::cout << "no free";
	}

	std::cout << "\n";
}

void OneTestActions(const std::string& test_name, SegmentAllocator& allocator, const std::vector<Action>& actions) {
	std::cout << "-----------------------------------------------------------------\n";
	std::cout << "Test " << test_name << ": ";
	PrintSegments(allocator);

	for (const auto& iter : actions) {
		if (const Allocation* allocation = std::get_if<Allocation>(&iter)) {
			std::cout << "\tAllocate " << allocation->size << "\t";
			allocator.Allocate(allocation->size);
			PrintSegments(allocator);
		} else if (const Free* action = std::get_if<Free>(&iter)) {
			std::cout << "\tFree " << action->start << "," << action->size << "\t";
			allocator.Free(action->start, action->size);
			PrintSegments(allocator);
		}
	}
}

void FixedTest() {
	SegmentAllocator allocator(0, 100, 0);

	std::vector<Action> actions;
	actions.push_back(Allocation{20});
	actions.push_back(Allocation{30});
	actions.push_back(Allocation{50});

	// no free segments
	actions.push_back(Free{40, 10});	// Free when no free segments
	actions.push_back(Free{5, 10});		// Free when exists only right segment
	actions.push_back(Free{80, 10});	// Free when exists only left segment
	actions.push_back(Free{52, 8});		// Free when exists left and right segment

	// now we have free segments: [5,15) [40,50) [52,60) [80,90)
	for (int i = 0; i < 10; i++) {
		actions.push_back(Allocation{3});
	}

	// now we have free segments: [14,15) [49,50) [58,60) [86,90)
	actions.push_back(Free{15, 34});	// Free + union with left
	actions.push_back(Free{85, 1});		// Free + union with right
	actions.push_back(Free{50, 8});		// Free + union with left and right

	OneTestActions("1", allocator, actions);
}

int main() {
	FixedTest();
	return 0;
}
