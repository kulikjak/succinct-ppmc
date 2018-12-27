# include all shared rules with correct BASE_ROOT
BASE_ROOT = .
include $(BASE_ROOT)/common.mk

.PHONY: all
all: compressor dnagen

COMPRESSOR_ALL = $(COMPRESSOR_SRC_FILES)
COMPRESSOR_ALL += $(ARITH_SRC_FILES)
COMPRESSOR_ALL += $(COMPRESSOR_ROOT)/main.c

COMPRESSOR_INCLUDES = -I$(COMPRESSOR_ROOT)
COMPRESSOR_INCLUDES += -I$(SHARED_DIR)

# variables for use with RAS context shortening
COMPRESSOR_ALL += $(UWT_SRC_FILES)
COMPRESSOR_ALL += $(DBV_SRC_FILES)
COMPRESSOR_INCLUDES += -I$(WT_ROOT)
COMPRESSOR_INCLUDES += -I$(DBV_ROOT)

override CMDFLAGS +=

compressor: $(COMPRESSOR_DEPEND) $(UWT_DEPEND)
	$(CXX) $(CFLAGS) $(COMPRESSOR_INCLUDES) $(COMPRESSOR_ALL) \
	$(PROFILING_SRC_FILES) $(CMDFLAGS) -o $@ -lm

dnagen: $(MISC_DIR)/dnagen.c
	$(CXX) $(CFLAGS) $^ -o $@

.PHONY: test
test:
	$(MAKE) all -C tests

.PHONY: clean
clean:
	$(MAKE) clean -C tests
	rm -f compressor* dna_in* dnagen
	rm -rf results

.PHONY: purge
purge: clean
	$(MAKE) purge -C tests
