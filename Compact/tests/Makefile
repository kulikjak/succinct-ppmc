CXX = gcc
CFLAGS = -std=c99 -Wall -Wextra -Werror -pedantic
CFLAGS = -Wno-error=unused-function -Wno-unused-function
#CFLAGS += -Wpointer-arith -Wcast-align -Wwrite-strings
#CFLAGS += -Wswitch-default -Wunreachable-code -Winit-self
#CFLAGS += -Wmissing-field-initializers -Wno-unknown-pragmas
#CFLAGS += -Wstrict-prototypes -Wundef -Wold-style-definition

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
	$(SOURCE_ROOT)/select.c

SRC_FILES += \
	TestStructMemory.c \
	TestStructBinaryVector.c \
	TestStructWaveletTree.c \
	TestStructIntegerVector.c \
	TestAll.c

all: clean default

download: .downloaded
.downloaded:
	wget -O unity.zip https://github.com/ThrowTheSwitch/Unity/archive/master.zip
	unzip unity.zip
	touch .downloaded

default: download
	$(CXX) $(CFLAGS) $(INC_DIRS) $(SRC_FILES) -o $(TARGET) -lm
	- ./$(TARGET) -v

clean:
	rm -f $(TARGET)

purge: clean
	rm -rf $(UNITY_ROOT) unity.zip .downloaded

.PHONY: all clean default download
