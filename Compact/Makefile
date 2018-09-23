CXX = gcc -Wall -Wextra -pedantic -Wno-long-long -g
CPLUS = g++ --std=c++0x -Wall -Wextra -pedantic -Wno-long-long -g

SHARED = ../misc/bit_sequence.h ../shared/utils.h ../shared/stack.h
UTILS = ../shared/utils.h

all: tests benchmark

tests: test_memory

test_memory: test_memory.c memory.h $(UTILS)
	$(CXX) test_memory.c -lm -o $@

test: test.c *.h $(SHARED)
	$(CXX) test.c -lm -o $@

benchmark: benchmark.cc *.h $(SHARED)
	$(CPLUS) benchmark.cc -o $@

clean:
	rm -f test_memory
	rm -f test benchmark

.PHONY: all tests clean
