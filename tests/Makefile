# include all shared rules with correct BASE_ROOT
BASE_ROOT = ..
include $(BASE_ROOT)/common.mk

# Variables shared by all tests
TEST_INC_DIRS = -I$(SHARED_DIR) -I$(MISC_DIR) $(UNITY_INC_DIRS)
TEST_ARGUMENTS = $(UNITY_SRC_FILES)

all: clean

.PHONY: download
download: .downloaded-unity
.downloaded-unity:
	wget -O unity.zip https://github.com/ThrowTheSwitch/Unity/archive/master.zip
	unzip unity.zip
	touch .downloaded-unity

all: test_bit_vector
test_bit_vector: $(DBV_DEPEND)
test_bit_vector: TEST_INC_DIRS += -I$(DBV_ROOT)
test_bit_vector: TEST_ARGUMENTS += $(DBV_SRC_FILES) \
	./Dynamic_BitVector/dbv.c ./Dynamic_BitVector.c

all: test_wavelet_tree
test_wavelet_tree: $(WT_DEPEND)
test_wavelet_tree: TEST_INC_DIRS += -I$(WT_ROOT) -I$(DBV_ROOT)
test_wavelet_tree: TEST_ARGUMENTS += $(DBV_SRC_FILES) $(WT_SRC_FILES) \
	./Wavelet_tree/optimized.c ./Wavelet_tree/universal.c \
	./Wavelet_tree.c

all: test_compressor
test_compressor: $(COMPRESSOR_DEPEND) tmp
test_compressor: TEST_INC_DIRS += -I$(COMPRESSOR_ROOT) -I$(ARITH_ROOT)
test_compressor: TEST_ARGUMENTS += $(COMPRESSOR_SRC_FILES) $(ARITH_SRC_FILES) \
	./Compressor/memory.c \
	./Compressor/binary_vector.c \
	./Compressor/wavelet_tree.c \
	./Compressor/int_vector.c \
	./Compressor/deBruijn.c \
	./Compressor/compressor.c \
	./Compressor.c

.PHONY: test_%
test_%: download
	$(CXX) $(CFLAGS) -g $(TEST_INC_DIRS) $(TEST_ARGUMENTS) -D_UNITY -o $@ -lm
	- ./$@ -v

tmp:
	mkdir tmp

.PHONY: clean
clean:
	rm -f test_*
	rm -rf tmp

.PHONY: purge
purge: clean
	rm -rf $(UNITY_ROOT) unity.zip .downloaded-unity
