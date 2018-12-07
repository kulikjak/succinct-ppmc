# include all shared rules with correct BASE_ROOT
BASE_ROOT = ..
include $(BASE_ROOT)/common.mk

# redefine c flags for better performance control
CFLAGS = -std=c99 -O3

BENCHMARK_SHARED = structure_compact.c structure_common.h

INCLUDE_DIRS = -I$(SHARED_DIR) -I$(MISC_DIR)

COMPRESSOR_STRUCT_SRC_FILES = \
	$(COMPRESSOR_ROOT)/cache.c      \
	$(COMPRESSOR_ROOT)/memory.c     \
	$(COMPRESSOR_ROOT)/rank.c       \
	$(COMPRESSOR_ROOT)/select.c     \
	$(COMPRESSOR_ROOT)/structure.c

all: structure_

structure_: structure_compact structure_universal

structure_compact: INCLUDE_DIRS += -I$(COMPRESSOR_ROOT)
structure_compact: $(COMPRESSOR_DEPEND) $(BENCHMARK_SHARED) structure_compact.c
	$(CXX) $(CFLAGS) $(INCLUDE_DIRS) $(COMPRESSOR_STRUCT_SRC_FILES) structure_compact.c -o $@ -lm

structure_universal: INCLUDE_DIRS += -I$(WT_ROOT) -I$(DBV_ROOT)
structure_universal: $(UWT_DEPEND) $(BENCHMARK_SHARED) structure_universal.c
	$(CXX) $(CFLAGS) $(INCLUDE_DIRS) $(UWT_SRC_FILES) $(DBV_SRC_FILES) structure_universal.c -o $@ -lm

.PHONY: clean_structure
clean_structure:
	rm -f structure_compact structure_universal

clean: clean_structure

.PHONY: all clean
