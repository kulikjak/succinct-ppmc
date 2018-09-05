#include <stdio.h>
#include <stdlib.h>

#include "memory.h"
#include "structure.h"

#include "../misc/bit_sequence.h"

#define SEQENCE_LEN 250
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

bool test_deletion_(void) {
  int32_t i;
  RAS_Struct RaS;

  RAS_Init(&RaS);

  // init simple bit sequence with random values
  uint64_t* sequence = init_random_bin_sequence(SEQENCE_LEN);

  int32_t idx = 0;
  int32_t added = 0;
  int32_t pos[SEQENCE_LEN];

  // insert bits into dynamic RaS
  for (i = 0; i < SEQENCE_LEN; i++) {
    int8_t bit = (sequence[i / 64] >> (63 - (i % 64))) & 0x1;

    // insert additional bits for deletion testing purposes
    if (bit) {
      RAS_Insert(&RaS, i + added, bit);
      pos[idx++] = i + added++;
    }
    RAS_Insert(&RaS, i + added, bit);
  }

  // delete all additional bits and check the structure
  for (i = added - 1; i >= 0; i--) {
    RAS_Delete(&RaS, pos[i]);
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

  test_deletion_();

  printf("All tests successfull\n");

  return EXIT_SUCCESS;
}
