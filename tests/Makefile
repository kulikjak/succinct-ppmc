CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -Werror -pedantic -g
CFLAGS += -Wno-error=unused-function -Wno-unused-function
#CFLAGS += -Wpointer-arith -Wcast-align -Wwrite-strings
#CFLAGS += -Wswitch-default -Wunreachable-code -Winit-self
#CFLAGS += -Wmissing-field-initializers -Wno-unknown-pragmas
#CFLAGS += -Wstrict-prototypes -Wundef -Wold-style-definition

# Arithmetic coding library cannot be compiled with C compiler
# due to its preprocessor calculations.
CPP = g++
CPPFLAGS = -Wall -Wextra -pedantic -g

TARGET = Test
UNITY_ROOT = Unity-master
SOURCE_ROOT = ../src

INC_DIRS = -Isrc -I$(UNITY_ROOT)/src -I$(UNITY_ROOT)/extras/fixture/src -I$(SOURCE_ROOT)

SRC_FILES = \
	$(UNITY_ROOT)/src/unity.c $(UNITY_ROOT)/extras/fixture/src/unity_fixture.c

SRC_FILES += \
	$(SOURCE_ROOT)/memory.c \
	$(SOURCE_ROOT)/structure.c \
	$(SOURCE_ROOT)/rank.c \
	$(SOURCE_ROOT)/select.c \
	$(SOURCE_ROOT)/deBruijn.c \
	$(SOURCE_ROOT)/compressor.c \
	arcd.o

SRC_FILES += \
	TestStructMemory.c \
	TestStructBinaryVector.c \
	TestStructWaveletTree.c \
	TestStructIntegerVector.c \
	TestDeBruijn.c \
	TestCompressor.c \
	TestAll.c

all: clean default

download: .downloaded-unity
.downloaded-unity:
	wget -O unity.zip https://github.com/ThrowTheSwitch/Unity/archive/master.zip
	unzip unity.zip
	touch .downloaded-unity

default: download
	$(CPP) $(CPPFLAGS) -c $(SOURCE_ROOT)/arcd/arcd.c
	$(CC) $(CFLAGS) $(INC_DIRS) $(SRC_FILES) -D_UNITY -o $(TARGET) -lm
	mkdir -p tmp
	- ./$(TARGET) -v

clean:
	rm -f $(TARGET)

purge: clean
	rm -rf $(UNITY_ROOT) unity.zip .downloaded-unity

.PHONY: all clean default download
