CXX = gcc -Wall -Wextra -pedantic -Wno-long-long -g
CPLUS = g++ --std=c++0x -Wall -Wextra -pedantic -Wno-long-long -g

all: test

test: test.c *.h
	$(CXX) test.c -lm -o $@

clean:
	rm -f test

.PHONY: clean
