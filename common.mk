
CXX = gcc
CFLAGS = -std=c99 -Wall -Wextra -Werror -pedantic
CFLAGS += -Wno-error=unused-function -Wno-unused-function

# set base root if no other is given
BASE_ROOT ?= .

# main root folders
DBV_ROOT = $(BASE_ROOT)/Dynamic_BitVector
WT_ROOT = $(BASE_ROOT)/Wavelet_tree
COMPRESSOR_ROOT = $(BASE_ROOT)/src
ARITH_ROOT = $(COMPRESSOR_ROOT)/arith
UNITY_ROOT = $(BASE_ROOT)/tests/Unity-master

# additional directories
PROFILING_DIR = $(BASE_ROOT)/profiling
SHARED_DIR = $(BASE_ROOT)/shared
MISC_DIR = $(BASE_ROOT)/misc

# header and source files
DBV_SRC_FILES = $(DBV_ROOT)/dbv.c $(DBV_ROOT)/dbv_memory.c $(DBV_ROOT)/rankselect.c
DBV_SRC_FILES += $(DBV_ROOT)/insert.c $(DBV_ROOT)/delete.c
DBV_HEADER_FILES = $(DBV_ROOT)/dbv.h $(DBV_ROOT)/dbv_memory.h

OWT_SRC_FILES = $(WT_ROOT)/optimized.c
OWT_HEADER_FILES = $(WT_ROOT)/optimized.h

UWT_SRC_FILES = $(WT_ROOT)/universal.c
UWT_HEADER_FILES = $(WT_ROOT)/universal.h

WT_SRC_FILES = $(UWT_SRC_FILES) $(OWT_SRC_FILES)

PROFILING_SRC_FILES = $(PROFILING_DIR)/memory_profiling.c

COMPRESSOR_SRC_FILES = \
	$(COMPRESSOR_ROOT)/cache.c      \
	$(COMPRESSOR_ROOT)/compressor.c \
	$(COMPRESSOR_ROOT)/deBruijn.c   \
	$(COMPRESSOR_ROOT)/memory.c     \
	$(COMPRESSOR_ROOT)/rank.c       \
	$(COMPRESSOR_ROOT)/select.c     \
	$(COMPRESSOR_ROOT)/structure.c

COMPRESSOR_HEADER_FILES = \
	$(COMPRESSOR_ROOT)/cache.h      \
	$(COMPRESSOR_ROOT)/compressor.h \
	$(COMPRESSOR_ROOT)/deBruijn.h   \
	$(COMPRESSOR_ROOT)/defines.h    \
	$(COMPRESSOR_ROOT)/memory.h     \
	$(COMPRESSOR_ROOT)/stack.h      \
	$(COMPRESSOR_ROOT)/structure.h

ARITH_SRC_FILES = $(ARITH_ROOT)/bitio.c
ARITH_SRC_FILES += $(ARITH_ROOT)/arith.c
ARITH_HEADER_FILES = $(ARITH_ROOT)/bitio.h
ARITH_HEADER_FILES += $(ARITH_ROOT)/arith.h

# test framework files
UNITY_SRC_FILES = $(UNITY_ROOT)/src/unity.c $(UNITY_ROOT)/extras/fixture/src/unity_fixture.c

# all files for each group
DBV_DEPEND = $(DBV_SRC_FILES) $(DBV_HEADER_FILES)
OWT_DEPEND = $(OWT_SRC_FILES) $(OWT_HEADER_FILES) $(DBV_DEPEND)
UWT_DEPEND = $(UWT_SRC_FILES) $(UWT_HEADER_FILES) $(DBV_DEPEND)
WT_DEPEND = $(OWT_DEPEND) $(UWT_DEPEND)

ARITH_DEPEND = $(ARITH_SRC_FILES) $(ARITH_HEADER_FILES)
COMPRESSOR_DEPEND = $(COMPRESSOR_SRC_FILES) $(COMPRESSOR_HEADER_FILES) $(ARITH_DEPEND)

# included folders for test framework
UNITY_INC_DIRS = -I$(UNITY_ROOT)/src -I$(UNITY_ROOT)/extras/fixture/src
