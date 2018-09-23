#include <stdio.h>
#include <stdlib.h>

#include "memory.h"

#include "../shared/utils.h"

#define TEST_NAME_ "Memory manager"

#define TEST_SAMPLE_SIZE_ 1000

int main(int argc, char* argv[]) {
  UNUSED(argc);
  UNUSED(argv);

  int32_t i, idx;
  MemObj mem;

  mem = Memory_init();

  for (i = 0; i < TEST_SAMPLE_SIZE_; i++) {
    idx = Memory_new_node(mem);

    assert(idx == i * 2);
    assert(!IS_LEAF(idx));
  }
  for (i = 0; i < TEST_SAMPLE_SIZE_; i++) {
    idx = Memory_new_leaf(mem);

    assert(idx == (i * 2) + 1);
    assert(IS_LEAF(idx));
  }

  Memory_free(&mem);

  printf("[%s] All tests successfull\n", TEST_NAME_);

  return EXIT_SUCCESS;
}
