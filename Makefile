CXX = gcc
CFLAGS = -std=c99 -Wall -Wextra -pedantic -g
CFLAGS += -Wno-error=unused-function -Wno-unused-function

SOURCE_ROOT = src
ARITH_CODER_ROOT = $(SOURCE_ROOT)/arith

SRC_FILES += \
	$(SOURCE_ROOT)/memory.c \
	$(SOURCE_ROOT)/structure.c \
	$(SOURCE_ROOT)/rank.c \
	$(SOURCE_ROOT)/cache.c \
	$(SOURCE_ROOT)/select.c \
	$(SOURCE_ROOT)/deBruijn.c \
	$(SOURCE_ROOT)/compressor.c \
	$(ARITH_CODER_ROOT)/bitio.c \
	$(ARITH_CODER_ROOT)/arith.c \
	$(SOURCE_ROOT)/main.c

INC_DIRS = -I$(SOURCE_ROOT)

all: compressor dnagen

compressor: $(SRC_FILES)
	$(CXX) $(CFLAGS) $(INC_DIRS) $(SRC_FILES) -o $@ -lm	

test:
	$(MAKE) default -C tests

dnagen: misc/dnagen.c
	$(CXX) $(CFLAGS) $^ -o $@

clean:
	$(MAKE) clean -C tests
	rm compressor
	rm -f dnagen

purge: clean
	$(MAKE) purge -C tests

.PHONY: all test clean purge
