CXX = gcc
CFLAGS = -std=c99 -Wall -Wextra -Werror -pedantic
#CFLAGS += -Wpointer-arith -Wcast-align -Wwrite-strings
#CFLAGS += -Wswitch-default -Wunreachable-code -Winit-self
#CFLAGS += -Wmissing-field-initializers -Wno-unknown-pragmas
#CFLAGS += -Wstrict-prototypes -Wundef -Wold-style-definition

TARGET = Test
UNITY_ROOT = Unity-master

INC_DIRS = -Isrc -I$(UNITY_ROOT)/src -I$(UNITY_ROOT)/extras/fixture/src
SRC_FILES = \
	$(UNITY_ROOT)/src/unity.c \
	$(UNITY_ROOT)/extras/fixture/src/unity_fixture.c \
	TestStructIntegerVector.c \
	Test.c

all: clean default

download: .downloaded
.downloaded:
	wget -O unity.zip https://github.com/ThrowTheSwitch/Unity/archive/master.zip
	unzip unity.zip
	touch .downloaded

default: download
	$(CXX) $(CFLAGS) $(INC_DIRS) $(SRC_FILES) -o $(TARGET)
	- ./$(TARGET) -v

clean:
	rm -f $(TARGET)

.PHONY: all clean default download
