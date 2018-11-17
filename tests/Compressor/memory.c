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
  int32_t i;
  MemPtr mptr;

  for (i = 0; i < MEMORY_SAMPLE_SIZE_; i++) {
    mptr = Memory_new_node(mem);

#ifdef INDEXED_MEMORY
    TEST_ASSERT_EQUAL(mptr, i * 2);
#endif
    TEST_ASSERT_FALSE(IS_LEAF(mptr));
  }
  for (i = 0; i < MEMORY_SAMPLE_SIZE_; i++) {
    mptr = Memory_new_leaf(mem);

#ifdef INDEXED_MEMORY
    TEST_ASSERT_EQUAL(mptr, (i * 2) + 1);
#endif
    TEST_ASSERT_TRUE(IS_LEAF(mptr));
  }
}

TEST_GROUP_RUNNER(Compressor_memory) {
  RUN_TEST_CASE(Compressor_memory, CorrectLeafNodeMapping);
}
