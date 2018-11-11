CXX = gcc
CFLAGS = -std=c99 -Wall -Wextra -pedantic -g
CFLAGS += -Wno-error=unused-function -Wno-unused-function

SHARED_DIR = ../shared
SHARED = $(SHARED_DIR)/utils.h ../misc/int_sequence.h

DBV_DIR = ../Dynamic_BitVector
DBV_SOURCE_FILES  = $(DBV_DIR)/dbv.c 
DBV_SOURCE_FILES += $(DBV_DIR)/memory.c 
DBV_SOURCE_FILES += $(DBV_DIR)/rankselect.c 
DBV_SOURCE_FILES += $(DBV_DIR)/insert.c
DBV_SOURCE_FILES += $(DBV_DIR)/delete.c
DBV_HEADER_FILES = $(DBV_DIR)/dbv.h $(DBV_DIR)/memory.h

INC_DIRS = -I$(DBV_DIR) -I$(SHARED_DIR)

all: test_optimized test_universal

test_optimized: test_optimized.c optimized.c optimized.h $(DBV_SOURCE_FILES) $(DBV_HEADER_FILES) $(SHARED)
	$(CXX) $(CFLAGS) $(INC_DIRS) $(DBV_SOURCE_FILES) optimized.c test_optimized.c -lm -o $@

test_universal: test_universal.c universal.c universal.h $(DBV_SOURCE_FILES) $(DBV_HEADER_FILES) $(SHARED)
	$(CXX) $(CFLAGS) $(INC_DIRS) $(DBV_SOURCE_FILES) universal.c test_universal.c -lm -o $@

clean:
	rm -f test_optimized test_universal

.PHONY: clean all
