#include "memory.h"
#include "unity_fixture.h"

TEST_GROUP(StructMemory);

#define MEMORY_SAMPLE_SIZE_ 1000

MemObj mem;

TEST_SETUP(StructMemory) {
  mem = Memory_init();
}

TEST_TEAR_DOWN(StructMemory) {
  Memory_free(&mem);
}

TEST(StructMemory, CorrectLeafNodeMapping) {
  int32_t i, idx;

  for (i = 0; i < MEMORY_SAMPLE_SIZE_; i++) {
    idx = Memory_new_node(mem);

    // check whether nodes are on even indexes
    TEST_ASSERT_EQUAL(idx, i * 2);
    // check if IS_LEAF macro is working correctly
    TEST_ASSERT_FALSE(IS_LEAF(idx));
  }
  for (i = 0; i < MEMORY_SAMPLE_SIZE_; i++) {
    idx = Memory_new_leaf(mem);

    // check whether leafs are on odd indexes
    TEST_ASSERT_EQUAL(idx, (i * 2) + 1);
    // check if IS_LEAF macro is working correctly
    TEST_ASSERT_TRUE(IS_LEAF(idx));
  }
}

TEST_GROUP_RUNNER(StructMemory) {
  RUN_TEST_CASE(StructMemory, CorrectLeafNodeMapping);
}
