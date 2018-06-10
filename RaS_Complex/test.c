#include <stdio.h>
#include <stdlib.h>

#include "memory.h"
#include "structure.h"

#include "../bit_sequence.h"

#define SEQENCE_LEN 240
#define PRINT_SEQUENCES false


bool _test_itself(uint64_t* sequence, RAS_Struct RaS) {
  int32_t i;

  if (PRINT_SEQUENCES) {
    print_bit_sequence64(sequence, SEQENCE_LEN);
    RAS_Print(RaS);
  }

  // test get (correct insertion)
  for (i = 0; i < SEQENCE_LEN; i++) {
    assert(get_bit_sequence(sequence, SEQENCE_LEN, i) == RAS_Get(RaS, i));
  }

  // test rank
  for (i = 0; i <= SEQENCE_LEN; i++) {
    assert(rank_bit_sequence(sequence, SEQENCE_LEN, i) == RAS_Rank(RaS, i));
  }

  // test rank0
  for (i = 0; i <= SEQENCE_LEN; i++) {
    assert(rank0_bit_sequence(sequence, SEQENCE_LEN, i) == RAS_Rank0(RaS, i));
  }

  // test select
  for (i = 0; i <= SEQENCE_LEN; i++) {
    assert(select_bit_sequence(sequence, SEQENCE_LEN, i) == RAS_Select(RaS, i));
  }

  // test select
  for (i = 0; i <= SEQENCE_LEN; i++) {
    assert(select0_bit_sequence(sequence, SEQENCE_LEN, i) ==
           RAS_Select0(RaS, i));
  }

  return true;
}

bool test_rear_insert(void) {
  int32_t i;
  RAS_Struct RaS;

  RAS_Init(&RaS);

  // init simple bit sequence with random values
  uint64_t* sequence = init_random_bin_sequence(SEQENCE_LEN);

  // insert bits into dynamic RaS
  for (i = 0; i < SEQENCE_LEN; i++) {
    int8_t bit = (sequence[i / 64] >> (63 - (i % 64))) & 0x1;
    RAS_Insert(&RaS, i, bit);
  }

  _test_itself(sequence, RaS);

  RAS_Free(&RaS);
  free(sequence);

  return true;
}

bool test_front_insert(void) {
  int32_t i;
  RAS_Struct RaS;

  RAS_Init(&RaS);

  // init simple bit sequence with random values
  uint64_t* sequence = init_random_bin_sequence(SEQENCE_LEN);

  // insert bits into dynamic RaS
  for (i = SEQENCE_LEN - 1; i >= 0; i--) {
    int8_t bit = (sequence[i / 64] >> (63 - (i % 64))) & 0x1;
    RAS_Insert(&RaS, 0, bit);
  }

  _test_itself(sequence, RaS);

  RAS_Free(&RaS);
  free(sequence);

  return true;
}

int main(int argc, char* argv[]) {
  UNUSED(argc);
  UNUSED(argv);

  test_rear_insert();
  test_front_insert();

  printf("All tests successfull\n");

  return EXIT_SUCCESS;
}
