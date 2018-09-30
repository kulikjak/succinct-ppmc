CXX = gcc -Wall -Wextra -pedantic -Wno-long-long -g
CPLUS = g++ --std=c++0x -Wall -Wextra -pedantic -Wno-long-long -g

SHARED = ../misc/bit_sequence.h ../shared/utils.h ../shared/stack.h
UTILS = ../shared/utils.h

all: test

test:
	$(MAKE) default -C tests

clean:
	$(MAKE) clean -C tests

.PHONY: all test
