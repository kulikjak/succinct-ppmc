CXX = gcc -Wall -Wextra -pedantic -Wno-long-long -g
CPLUS = g++ --std=c++0x -Wall -Wextra -pedantic -Wno-long-long -g

SHARED = ../misc/bit_sequence.h ../shared/utils.h

all: test benchmark

test: test.c *.h $(SHARED)
	$(CXX) test.c -lm -o $@

benchmark: benchmark.cc *.h $(SHARED)
	$(CPLUS) benchmark.cc -o $@

clean:
	rm -f test benchmark

.PHONY: clean
