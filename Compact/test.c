#include <stdio.h>
#include <stdlib.h>

#include "memory.h"
#include "structure.h"

#define VAR_IGNORE 0
#define CHAR_IGNORE '$'

#include "tests/test_wavelet_tree.h"
#include "tests/test_binary_vector.h"

#define SEQENCE_LEN 250
#define PRINT_SEQUENCES false


bool test_ext(void) {
  int32_t i;
  Graph_Struct MS;
  Graph_Line line;

  Graph_Init(&MS);

  GLine_Fill(&line, 0, 'A', 100);
  GLine_Insert(&MS, 0, &line);

  GLine_Fill(&line, 1, 'C', 200);
  GLine_Insert(&MS, 0, &line);

  GLine_Fill(&line, 1, 'G', 300);
  GLine_Insert(&MS, 0, &line);

  GLine_Fill(&line, 0, 'T', 400);
  GLine_Insert(&MS, 0, &line);

  for (i = 0; i < 20; i++) {
    GLine_Fill(&line, 0, 'A', 100);
    GLine_Insert(&MS, 0, &line);
  }

  //printf("%d\n", Graph_WRank(&MS, 200, 'A'));

  GLine_Get(&MS, 2, &line);
  printf("%d %c %d\n", line.L_, line.W_, line.P_);

  Graph_Print(&MS);

  Graph_Free(&MS);

  return true;  
}

int main(int argc, char* argv[]) {
  UNUSED(argc);
  UNUSED(argv);

  // binary tree rank & select tests
  //test_BV_front_insert();
  //test_BV_rear_insert();

  // wavelet tree tests
  //test_WT_front_insert();
  test_WT_rear_insert();

  printf("All tests successfull\n");

  return EXIT_SUCCESS;
}
