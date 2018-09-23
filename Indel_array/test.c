#include <stdio.h>
#include <stdlib.h>

#include "structure.h"

#define TEST_INDEL_SIZE 20
#define TEST_INDEL_PRINT false

bool simple_static_test(void) {
  int32_t i;

  Indel_Struct IA;
  Indel_Init(&IA);

  for (i = 0; i < TEST_INDEL_SIZE; i++) {
    Indel_Insert(&IA, i, i * 10);
  }

  for (i = 0; i < TEST_INDEL_SIZE; i++) {
    assert((i * 10) == Indel_Get(&IA, i));
  }

#if TEST_INDEL_PRINT
  Indel_Print(&IA);
#endif

  for (i = 0; i < TEST_INDEL_SIZE; i++) {
    if (i % 2) Indel_Set(&IA, i, 2);
  }

  for (i = 0; i < TEST_INDEL_SIZE; i++) {
    if (i % 2)
      assert(2 == Indel_Get(&IA, i));
    else
      assert((i * 10) == Indel_Get(&IA, i));
  }

  for (i = 0; i < TEST_INDEL_SIZE; i++) {
    if (i % 2 == 0) Indel_Set(&IA, i, 4);
  }

  // test insert inside the array
  for (i = 0; i < TEST_INDEL_SIZE; i++) {
    Indel_Insert(&IA, i * 2, 1000);
  }
  for (i = 0; i < TEST_INDEL_SIZE; i++) {
    if (i % 2 == 0)
      assert(1000 == Indel_Get(&IA, i));
    else if ((i - 1) % 4 == 0)
      assert(4 == Indel_Get(&IA, i));
    else
      assert(2 == Indel_Get(&IA, i));
  }

#if TEST_INDEL_PRINT
  printf("-----------------------------------------\n");
  Indel_Print(&IA);
#endif

  assert(Indel_Size(&IA) == TEST_INDEL_SIZE * 2);

  Indel_Free(&IA);
  assert(IA.mem_ == NULL);

  return true;
}

int main(int argc, char* argv[]) {
  UNUSED(argc);
  UNUSED(argv);

  simple_static_test();

  printf("All tests successfull\n");

  return EXIT_SUCCESS;
}
