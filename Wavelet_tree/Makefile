CXX = gcc -Wall -Wextra -pedantic -Wno-long-long -g
CPLUS = g++ --std=c++0x -Wall -Wextra -pedantic -Wno-long-long -g

all: variable

variable: test_variable.c variable.h variable_utils.h ../RaS_Complex/*.h
	$(CXX) test_variable.c -o $@

clean:
	rm -f variable

.PHONY: clean
