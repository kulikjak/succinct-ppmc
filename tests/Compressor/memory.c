#include "memory.h"
#include "unity_fixture.h"

TEST_GROUP(Compressor_memory);

#define MEMORY_SAMPLE_SIZE_ 1000

MemObj mem;

TEST_SETUP(Compressor_memory) {
  mem = Memory_init();
}

TEST_TEAR_DOWN(Compressor_memory) {
  Memory_free(&mem);
}

TEST(Compressor_memory, CorrectLeafNodeMapping) {
  int32_t i, idx;

  for (i = 0; i < MEMORY_SAMPLE_SIZE_; i++) {
    idx = Memory_new_node(mem);

    TEST_ASSERT_EQUAL(idx, i * 2);
    TEST_ASSERT_FALSE(IS_LEAF(idx));
  }
  for (i = 0; i < MEMORY_SAMPLE_SIZE_; i++) {
    idx = Memory_new_leaf(mem);

    TEST_ASSERT_EQUAL(idx, (i * 2) + 1);
    TEST_ASSERT_TRUE(IS_LEAF(idx));
  }
}

TEST_GROUP_RUNNER(Compressor_memory) {
  RUN_TEST_CASE(Compressor_memory, CorrectLeafNodeMapping);
}
