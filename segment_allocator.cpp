#include "segment_allocator.h"
#include <iostream>

SegmentAllocator::SegmentAllocator(size_t start, size_t size, size_t error_result) : error_result_(error_result) {
	Insert(start, size);
}

size_t SegmentAllocator::Allocate(size_t size) {
	auto iter = segments_size_.lower_bound({size, 0});
	if (iter == segments_size_.end()) {
		return error_result_;
	}
	size_t index = iter->second;

	if (size < iter->first) {
		Insert(index + size, iter->first - size);
	}

	segments_start_.erase({index, iter->first});
	segments_size_.erase(iter);

	return index;
}

bool SegmentAllocator::Free(size_t start, size_t size)
{
	auto iter_right = segments_start_.upper_bound({start, 0});
	auto iter_left = (iter_right == segments_start_.begin() ? segments_start_.end() : std::prev(iter_right)); // если тот что справа самый первый, то левее уже никого

	if ((iter_left != segments_start_.end()) && (iter_left->first + iter_left->second > start)) {
		return false; // Освобождаем в отрезке который и так свободен
	} else if ((iter_right != segments_start_.end()) && (start + size > iter_right->first)) {
		return false; // Освобождаем в отрезке который и так свободен
	}

	if ((iter_left != segments_start_.end()) && (iter_left->first + iter_left->second == start)) { // Объединяем с левым сегментом
		start = iter_left->first;
		size += iter_left->second;
		segments_size_.erase({iter_left->second, iter_left->first});
		segments_start_.erase(iter_left);
	}

	if ((iter_right != segments_start_.end()) && (start + size == iter_right->first)) {  // Объединяем с правым сегментом
		size += iter_right->second;
		segments_size_.erase({iter_right->second, iter_right->first});
		segments_start_.erase(iter_right);
	}

	Insert(start, size);
	return true;
}

void SegmentAllocator::Insert(size_t start, size_t size) {
	segments_start_.insert({start, size});
	segments_size_.insert({size, start});	
}

const std::set<std::pair<size_t, size_t>>& SegmentAllocator::DebugSegmentsStart() const {
	return segments_start_;
}

const std::set<std::pair<size_t, size_t>>& SegmentAllocator::DebugSegmentsSize() const {
	return segments_size_;
}
