CXX = gcc
CFLAGS = -std=c99 -Wall -Wextra -pedantic -g
CFLAGS += -Wno-error=unused-function -Wno-unused-function

CPPXX = g++
CPPFLAGS = --std=c++0x -Wall -Wextra -pedantic -Wno-long-long
CPPFLAGS += -Wno-error=unused-function -Wno-unused-function -Wno-unused-variable

SHARED_DIR = ../shared
SHARED = $(SHARED_DIR)/utils.h ../misc/bit_sequence.h

SOURCE_FILES = dbv.c memory.c rankselect.c insert.c delete.c
HEADER_FILES = dbv.h memory.h

INC_DIRS = -I$(SHARED_DIR)

all: test benchmark

test: test.c $(SOURCE_FILES) $(HEADER_FILES) $(SHARED)
	$(CXX) $(CFLAGS) $(INC_DIRS) $(SOURCE_FILES) test.c -lm -o $@

benchmark: benchmark.cc $(SOURCE_FILES) $(HEADER_FILES) $(SHARED)
	$(CPPXX) $(CPPFLAGS) $(INC_DIRS) $(SOURCE_FILES) benchmark.cc -o $@

clean:
	rm -f test benchmark

.PHONY: all clean
