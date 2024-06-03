#pragma once

#include <cstddef>
#include <set>

class LogAllocator
{
public:
	LogAllocator(size_t start, size_t size, size_t error_result);
	size_t Allocate(size_t size);
	bool Free(size_t start, size_t size);

	const std::set<std::pair<size_t, size_t>>& DebugSegmentsStart() const;
	const std::set<std::pair<size_t, size_t>>& DebugSegmentsSize() const;

private:
	std::set<std::pair<size_t, size_t>> segments_start_;
	std::set<std::pair<size_t, size_t>> segments_size_;
	size_t error_result_;

	void Insert(size_t start, size_t size);
};
