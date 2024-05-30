CPP_FLAGS = -std=c++17 -Wall -Wextra

all:
	g++ $(CPP_FLAGS) log_allocator.cpp test_log.cpp -lgtest -o test_log 
	g++ $(CPP_FLAGS) log_allocator.cpp demo_log.cpp -o demo_log
	g++ $(CPP_FLAGS) test_segment.cpp -lgtest -o test_segment
	g++ $(CPP_FLAGS) demo_segment.cpp -o demo_segment
