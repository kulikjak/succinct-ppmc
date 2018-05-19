CXX = gcc -Wall -Wextra -pedantic -Wno-long-long -g
CPLUS = g++ --std=c++0x -Wall -Wextra -pedantic -Wno-long-long -g

main: main.c *.h
	$(CXX) main.c -lm -o $@

benchmark: benchmark.cc *.h
	$(CPLUS) benchmark.cc -o $@

clean:
	rm -f main benchmark

.PHONY: clean
