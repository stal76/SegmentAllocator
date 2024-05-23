all:
	g++ segment_allocator.cpp test_allocator.cpp -o test_allocator
	g++ segment_allocator.cpp demo_allocator.cpp -o demo_allocator
