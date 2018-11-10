CXX = gcc
CFLAGS = -std=c99 -Wall -Wextra -pedantic -O3
CFLAGS += -Wno-error=unused-function -Wno-unused-function

MEMORY_SOURCES = ../src/memory.c
MEMORY_FILES = $(MEMORY_SOURCES) ../src/memory.h

STRUCTURE_SOURCES = $(MEMORY_SOURCES) ../src/structure.c ../src/rank.c ../src/select.c
STRUCTURE_FILES = $(STRUCTURE_SOURCES) $(MEMORY_FILES) ../src/structure.h

ORIGINAL_WAVELET_SOURCES =
ORIGINAL_WAVELET_FILES = ../other/Wavelet_tree/structure_ext.h ../other/RaS_Complex/memory.h
ORIGINAL_WAVELET_FILES += ../other/RaS_Complex/structure.h ../other/shared/utils.h

all: structure_

structure_: structure_compact structure_original

structure_compact: $(STRUCTURE_FILES) structure_compact.c structure_common.h
	$(CXX) $(CFLAGS) $(STRUCTURE_SOURCES) structure_compact.c -o $@ -lm

structure_original: $(ORIGINAL_WAVELET_FILES) structure_original.c structure_common.h
	$(CXX) $(CFLAGS) $(ORIGINAL_WAVELET_SOURCES) structure_original.c -o $@ -lm

.PHONY: clean_structure
clean_structure:
	rm -f structure_compact structure_original

clean: clean_structure

.PHONY: all clean
