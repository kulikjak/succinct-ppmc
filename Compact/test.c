#include <stdio.h>
#include <stdlib.h>

#include "memory.h"
#include "structure.h"

#define VAR_IGNORE 0
#define CHAR_IGNORE '$'

#include "tests/test_memory.h"
#include "tests/test_wavelet_tree.h"
#include "tests/test_binary_vector.h"

int main(int argc, char* argv[]) {
  UNUSED(argc);
  UNUSED(argv);

  // memory tests
  test_memory();
  printf("All memory tests successfull [1]\n");

  // binary tree rank & select tests
  test_BV_front_insert();
  test_BV_rear_insert();
  printf("All binary vector tests successfull [2]\n");

  // wavelet tree tests
  test_WT_front_insert();
  test_WT_rear_insert();
  printf("All wavelet tree tests successfull [2]\n");

  printf("All tests successfull\n");

  return EXIT_SUCCESS;
}
