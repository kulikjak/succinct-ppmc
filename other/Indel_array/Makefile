CXX = gcc -Wall -Wextra -pedantic -Wno-long-long -g

SHARED = ../misc/bit_sequence.h ../shared/utils.h

all: test

test: test.c *.h $(SHARED)
	$(CXX) test.c -lm -o $@

clean:
	rm -f test

.PHONY: clean
