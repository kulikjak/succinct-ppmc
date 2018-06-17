CXX = gcc -Wall -Wextra -pedantic -Wno-long-long -g
CPLUS = g++ --std=c++0x -Wall -Wextra -pedantic -Wno-long-long -g

SHARED = ../shared/utils.h ../misc/char_sequence.h ../RaS_Complex/*.h

all: dna dnaext variable

dna: test.c structure.h $(SHARED)
	$(CXX) test.c -o $@

dnaext: test_ext.c structure_ext.h $(SHARED)
	$(CXX) test_ext.c -o $@

variable: test_variable.c variable.h variable_utils.h $(SHARED)
	$(CXX) test_variable.c -o $@


clean:
	rm -f dna dnaext variable

.PHONY: clean
