CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -pedantic -g
CFLAGS += -Wno-error=unused-function -Wno-unused-function

CPP = g++
CPPFLAGS = -Wall -Wextra -pedantic -g

ARCD_ROOT = arcd-master
ARCD_ARCHIVE = $(ARCD_ROOT).zip

SOURCE_ROOT = src
SRC_FILES += \
	$(SOURCE_ROOT)/memory.c \
	$(SOURCE_ROOT)/structure.c \
	$(SOURCE_ROOT)/rank.c \
	$(SOURCE_ROOT)/select.c \
	$(SOURCE_ROOT)/deBruijn.c \
	$(SOURCE_ROOT)/compression.c \
	$(SOURCE_ROOT)/arcd/arcd.c \
	$(SOURCE_ROOT)/main.c

INC_DIRS = -I$(SOURCE_ROOT)

all: test

download: .downloaded-arcd
.downloaded-arcd:
	wget -O $(ARCD_ARCHIVE) https://github.com/wonder-mice/arcd/archive/master.zip
	unzip $(ARCD_ARCHIVE)
	cp $(ARCD_ROOT)/arcd src/arcd
	touch .downloaded-arcd

compressor: $(SRC_FILES)
	$(CC) $(CFLAGS) $(INC_DIRS) $(SRC_FILES) -o $@ -lm	

test:
	$(MAKE) default -C tests

dnagen: dnagen.c
	$(CXX) $(CFLAGS) $^ -o $@

clean:
	$(MAKE) clean -C tests
	rm arcd.o compressor
	rm -f dnagen

purge: clean
	$(MAKE) purge -C tests
	rm -rf $(ARCD_ARCHIVE) $(ARCD_ROOT) .downloaded-arcd


.PHONY: all test clean download
