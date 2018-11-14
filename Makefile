# include all shared rules with correct BASE_ROOT
BASE_ROOT = .
include $(BASE_ROOT)/common.mk

.PHONY: all
all: compressor dnagen

COMPRESSOR_ALL = $(COMPRESSOR_SRC_FILES)
COMPRESSOR_ALL += $(ARITH_SRC_FILES)
COMPRESSOR_ALL += $(COMPRESSOR_ROOT)/main.c

compressor: $(COMPRESSOR_DEPEND)
	$(CXX) $(CFLAGS) -I$(COMPRESSOR_ROOT) $(COMPRESSOR_ALL) -o $@ -lm

dnagen: $(MISC_DIR)/dnagen.c
	$(CXX) $(CFLAGS) $^ -o $@

.PHONY: test
test:
	$(MAKE) all -C tests

.PHONY: clean
clean:
	$(MAKE) clean -C tests
	rm compressor
	rm -f dnagen

.PHONY: purge
purge: clean
	$(MAKE) purge -C tests
