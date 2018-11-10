#include <stdio.h>
#include <stdlib.h>

#include "memory.h"
#include "dbv.h"

#include "../other/misc/bit_sequence.h"

#define SEQENCE_LEN 25000
#define PRINT_SEQUENCES false


bool _test_itself(uint64_t* sequence, DBVStructRef DBV) {
  int32_t i;

  if (PRINT_SEQUENCES) {
    print_bit_sequence64(sequence, SEQENCE_LEN);
    DBV_Print(DBV);
  }

  // test get (correct insertion)
  for (i = 0; i < SEQENCE_LEN; i++) {
    assert(get_bit_sequence(sequence, SEQENCE_LEN, i) == DBV_Get(DBV, i));
  }

  // test rank
  for (i = 0; i <= SEQENCE_LEN; i++) {
    assert(rank_bit_sequence(sequence, SEQENCE_LEN, i) == DBV_Rank(DBV, i));
  }

  // test rank0
  for (i = 0; i <= SEQENCE_LEN; i++) {
    assert(rank0_bit_sequence(sequence, SEQENCE_LEN, i) == DBV_Rank0(DBV, i));
  }

  // test select
  for (i = 0; i <= SEQENCE_LEN; i++) {
    assert(select_bit_sequence(sequence, SEQENCE_LEN, i) == DBV_Select(DBV, i));
  }

  // test select
  for (i = 0; i <= SEQENCE_LEN; i++) {
    assert(select0_bit_sequence(sequence, SEQENCE_LEN, i) ==
           DBV_Select0(DBV, i));
  }

  return true;
}

bool test_rear_insert(void) {
  int32_t i;
  DBV_Struct DBV;

  DBV_Init(&DBV);

  // init simple bit sequence with random values
  uint64_t* sequence = init_random_bin_sequence(SEQENCE_LEN);

  // insert bits into dynamic DBV
  for (i = 0; i < SEQENCE_LEN; i++) {
    int8_t bit = (sequence[i / 64] >> (63 - (i % 64))) & 0x1;
    DBV_Insert(&DBV, i, bit);
  }

  _test_itself(sequence, &DBV);

  DBV_Free(&DBV);
  free(sequence);

  return true;
}

bool test_front_insert(void) {
  int32_t i;
  DBV_Struct DBV;

  DBV_Init(&DBV);

  // init simple bit sequence with random values
  uint64_t* sequence = init_random_bin_sequence(SEQENCE_LEN);

  // insert bits into dynamic DBV
  for (i = SEQENCE_LEN - 1; i >= 0; i--) {
    int8_t bit = (sequence[i / 64] >> (63 - (i % 64))) & 0x1;
    DBV_Insert(&DBV, 0, bit);
  }

  _test_itself(sequence, &DBV);

  DBV_Free(&DBV);
  free(sequence);

  return true;
}

bool test_deletion_(void) {
  int32_t i;
  DBV_Struct DBV;

  DBV_Init(&DBV);

  // init simple bit sequence with random values
  uint64_t* sequence = init_random_bin_sequence(SEQENCE_LEN);

  int32_t idx = 0;
  int32_t added = 0;
  int32_t pos[SEQENCE_LEN];

  // insert bits into dynamic DBV
  for (i = 0; i < SEQENCE_LEN; i++) {
    int8_t bit = (sequence[i / 64] >> (63 - (i % 64))) & 0x1;

    // insert additional bits for deletion testing purposes
    if (bit) {
      DBV_Insert(&DBV, i + added, bit);
      pos[idx++] = i + added++;
    }
    DBV_Insert(&DBV, i + added, bit);
  }

  // delete all additional bits and check the structure
  for (i = added - 1; i >= 0; i--) {
    DBV_Delete(&DBV, pos[i]);
  }

  _test_itself(sequence, &DBV);

  DBV_Free(&DBV);
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
