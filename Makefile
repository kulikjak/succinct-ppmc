CXX = gcc
CFLAGS = -Wall -Wextra -Werror -pedantic

all: test

test:
	$(MAKE) default -C tests

dnagen: dnagen.c
	$(CXX) $(CFLAGS) $^ -o $@

clean:
	$(MAKE) clean -C tests
	rm -f dnagen

.PHONY: all test clean
