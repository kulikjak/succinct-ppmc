CXX = gcc -Wall -Wextra -pedantic -Wno-long-long -g

SHARED = ../shared/utils.h

all: deBruijn

deBruijn: test.c structure.h $(SHARED)
	$(CXX) test.c -o $@

clean:
	rm -f deBruijn

.PHONY: clean
