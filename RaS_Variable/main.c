#include <stdlib.h>
#include <stdio.h>

#include "memory.h"
#include "structure.h"

#include "../bit_sequence.h"


#define SEQENCE_LEN 100

bool test_rear_insert() {
  int32_t i;

  init_stack();
  init_memory();

  // init simple bit sequence with random values
  uint64_t* sequence = init_random_bin_sequence(SEQENCE_LEN);

  // init our awesome data structure
  int32_t RaS_root = RaS_Init();

  // insert bits into dynamic RaS
  for (i = 0; i < SEQENCE_LEN; i++) {
    int8_t bit = (sequence[i / 64] >> (63 - (i % 64))) & 0x1;
    RaS_Insert(&RaS_root, i, bit);
  }

  print_bit_sequence64(sequence, SEQENCE_LEN);
  RaS_Print(RaS_root);

  // test get (correct insertion)
  for (i = 0; i < SEQENCE_LEN; i++) {
    // printf("%d %d\n", get_bit_sequence(sequence, SEQENCE_LEN, i), RaS_Get(RaS_root, i));
    assert(get_bit_sequence(sequence, SEQENCE_LEN, i) == RaS_Get(RaS_root, i));
  }

  // test rank
  for (i = 0; i <= SEQENCE_LEN; i++) {
    assert(rank_bit_sequence(sequence, SEQENCE_LEN, i) == RaS_Rank(RaS_root, i));
  }

  // test select
  for (i = 0; i <= SEQENCE_LEN; i++) {
    assert(select_bit_sequence(sequence, SEQENCE_LEN, i) == RaS_Select(RaS_root, i));
  }

  free(sequence);

  return true;
}

bool test_front_insert() {
  int32_t i;

  init_stack();
  init_memory();

  // init simple bit sequence with random values
  uint64_t* sequence = init_random_bin_sequence(SEQENCE_LEN);

  // init our awesome data structure
  int32_t RaS_root = RaS_Init();

  // insert bits into dynamic RaS
  for (i = SEQENCE_LEN - 1; i >= 0; i--) {
    int8_t bit = (sequence[i / 64] >> (63 - (i % 64))) & 0x1;
    RaS_Insert(&RaS_root, 0, bit);
  }

  print_bit_sequence64(sequence, SEQENCE_LEN);
  RaS_Print(RaS_root);

  // test get (correct insertion)
  for (i = 0; i < SEQENCE_LEN; i++) {
    // printf("%d %d\n", get_bit_sequence(sequence, SEQENCE_LEN, i), RaS_Get(RaS_root, i));
    assert(get_bit_sequence(sequence, SEQENCE_LEN, i) == RaS_Get(RaS_root, i));
  }

  // test rank
  for (i = 0; i <= SEQENCE_LEN; i++) {
    assert(rank_bit_sequence(sequence, SEQENCE_LEN, i) == RaS_Rank(RaS_root, i));
  }

  // test select
  for (i = 0; i <= SEQENCE_LEN; i++) {
    assert(select_bit_sequence(sequence, SEQENCE_LEN, i) == RaS_Select(RaS_root, i));
  }

  free(sequence);

  return true;
}


int main (int argc, char* argv[]) {
  UNUSED(argc);
  UNUSED(argv);

  test_rear_insert();
  test_front_insert();

  return EXIT_SUCCESS;
}
