CXX = gcc -Wall -Wextra -pedantic -Wno-long-long -g

Main: main.c *.h
	$(CXX) main.c -lm -o $@

clean:
	rm -f Main

.PHONY: clean
