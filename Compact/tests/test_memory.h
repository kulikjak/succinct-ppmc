#ifndef _TEST_MEMORY__
#define _TEST_MEMORY__

#include "memory.h"

#define MEMORY_SAMPLE_SIZE_ 1000

void test_memory() {
  int32_t i, idx;
  MemObj mem;

  mem = Memory_init();

  for (i = 0; i < MEMORY_SAMPLE_SIZE_; i++) {
    idx = Memory_new_node(mem);

    assert(idx == i * 2);
    assert(!IS_LEAF(idx));
  }
  for (i = 0; i < MEMORY_SAMPLE_SIZE_; i++) {
    idx = Memory_new_leaf(mem);

    assert(idx == (i * 2) + 1);
    assert(IS_LEAF(idx));
  }

  Memory_free(&mem);
}

#endif  // _TEST_MEMORY__
