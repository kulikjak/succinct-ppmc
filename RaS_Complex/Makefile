CXX = gcc -Wall -Wextra -pedantic -Wno-long-long -g
CPLUS = g++ --std=c++0x -Wall -Wextra -pedantic -Wno-long-long -g

all: test benchmark

test: test.c *.h ../misc/bit_sequence.h ../shared/utils.h
	$(CXX) test.c -lm -o $@

benchmark: benchmark.cc *.h ../misc/bit_sequence.h ../shared/utils.h
	$(CPLUS) benchmark.cc -o $@

clean:
	rm -f test benchmark

.PHONY: clean
