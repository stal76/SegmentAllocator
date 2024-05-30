#include <gtest/gtest.h>
#include <iostream>

#include "log_allocator.h"

using Segments = std::vector<std::pair<size_t, size_t>>;

std::ostream& operator<<(std::ostream& out_stream, std::pair<size_t, size_t> segment) {
	out_stream << "[" << segment.first << "," << segment.second << ")";
	return out_stream;
}

std::ostream& operator<<(std::ostream& out_stream, const Segments& segments) {
	out_stream << "{";
	for (auto segment : segments) {
		out_stream << segment;
	}
	out_stream << "}";
	return out_stream;
}

Segments GetSegments(const SegmentAllocator& allocator) {
	auto segments_start = allocator.DebugSegmentsStart();
	auto segments_size = allocator.DebugSegmentsSize();
	if (segments_start.size() != segments_size.size()) {
		std::cerr << "Different sizes of two sets\n";
		exit(1);
	}

	std::vector<std::pair<size_t, size_t>> result;
	for (auto [start, size] : segments_start) {
		result.push_back({start, start + size});
		if (segments_size.find({size, start}) == segments_size.end()) {
			std::cerr << "Not found element in segments_size\n";
			exit(1);
		}
	}

	return result;
}

static size_t error_result = 9999;

SegmentAllocator Start() {
	SegmentAllocator allocator(0, 100, error_result);
	
	EXPECT_EQ(allocator.Allocate(20), 0);
	EXPECT_EQ(allocator.Allocate(30), 20);
	EXPECT_EQ(allocator.Allocate(50), 50);
	EXPECT_EQ(GetSegments(allocator), Segments());

	// no free segments
	EXPECT_TRUE(allocator.Free(40, 10));	// Free when no free segments
	EXPECT_TRUE(allocator.Free(5, 10));		// Free when exists only right segment
	EXPECT_TRUE(allocator.Free(80, 10));	// Free when exists only left segment
	EXPECT_TRUE(allocator.Free(52, 8));	// Free when exists left and right segment
	EXPECT_EQ(GetSegments(allocator), Segments({{5, 15}, {40, 50}, {52,60}, {80,90}}));

	// now we have free segments: [5,15) [40,50) [52,60) [80,90)
	return allocator;
}

TEST(LogAllocator, Simple) {
	SegmentAllocator allocator = Start();
	
	EXPECT_EQ(allocator.Allocate(3), 52);
	EXPECT_EQ(allocator.Allocate(3), 55);
	EXPECT_EQ(allocator.Allocate(3), 5);
	EXPECT_EQ(allocator.Allocate(3), 8);
	EXPECT_EQ(allocator.Allocate(3), 11);
	EXPECT_EQ(allocator.Allocate(3), 40);
	EXPECT_EQ(allocator.Allocate(3), 43);
	EXPECT_EQ(allocator.Allocate(3), 46);
	EXPECT_EQ(allocator.Allocate(3), 80);
	EXPECT_EQ(allocator.Allocate(3), 83);
	EXPECT_EQ(GetSegments(allocator), Segments({{14, 15}, {49, 50}, {58,60}, {86,90}}));

	// now we have free segments: [14,15) [49,50) [58,60) [86,90)
	EXPECT_TRUE(allocator.Free(15, 34));	// Free + union with left
	EXPECT_TRUE(allocator.Free(85, 1));		// Free + union with right
	EXPECT_TRUE(allocator.Free(50, 8));		// Free + union with left and right
	EXPECT_EQ(GetSegments(allocator), Segments({{14, 60}, {85,90}}));
}

TEST(LogAllocator, Errors) {
	SegmentAllocator allocator = Start();

	EXPECT_EQ(allocator.Allocate(11), error_result);

	// Intersections with [5, 15)
	EXPECT_FALSE(allocator.Free(13, 10));
	EXPECT_FALSE(allocator.Free(5, 10));
	EXPECT_FALSE(allocator.Free(5, 15));
	EXPECT_FALSE(allocator.Free(5, 8));
	EXPECT_FALSE(allocator.Free(0, 6));
	EXPECT_FALSE(allocator.Free(2, 30));
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv); 
  return RUN_ALL_TESTS();
}
