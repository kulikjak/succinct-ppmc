# include all shared rules with correct BASE_ROOT
BASE_ROOT = ..
include $(BASE_ROOT)/common.mk

CPPXX = g++
CPPFLAGS = --std=c++0x -Wall -Wextra -pedantic -Wno-long-long
CPPFLAGS += -Wno-error=unused-function -Wno-unused-function -Wno-unused-variable

benchmark: benchmark.cc $(DBV_DEPEND) $(SHARED_DIR)
	$(CPPXX) $(CPPFLAGS) -I$(SHARED_DIR) $(DBV_SRC_FILES) benchmark.cc -o $@

.PHONY: clean
clean:
	rm -f benchmark
