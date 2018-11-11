#include <stdio.h>
#include <stdlib.h>

#include "optimized.h"

#include "../other/shared/utils.h"
#include "../misc/int_sequence.h"

#define SEQENCE_LEN 100
#define PRINT_SEQUENCES true


bool _test_wavelet_tree(OWT_Struct* uwt, int8_t* sequence) {
  int32_t i;

  if (PRINT_SEQUENCES) {
    int_sequence_print(sequence, SEQENCE_LEN);
    OWT_Print_Symbols(uwt);
  }

  // test get (correct insertion)
  for (i = 0; i < SEQENCE_LEN; i++) {
    assert(int_sequence_get(sequence, SEQENCE_LEN, i) == OWT_Get(uwt, i));
  }

  // test rank
  for (i = 0; i <= SEQENCE_LEN; i++) {
    assert(int_sequence_rank(sequence, SEQENCE_LEN, i, 0) ==
           OWT_Rank(uwt, i, 0));
    assert(int_sequence_rank(sequence, SEQENCE_LEN, i, 1) ==
           OWT_Rank(uwt, i, 1));
    assert(int_sequence_rank(sequence, SEQENCE_LEN, i, 2) ==
           OWT_Rank(uwt, i, 2));
    assert(int_sequence_rank(sequence, SEQENCE_LEN, i, 3) ==
           OWT_Rank(uwt, i, 3));
    assert(int_sequence_rank(sequence, SEQENCE_LEN, i, 4) ==
           OWT_Rank(uwt, i, 4));
  }

  // test select
  for (i = 0; i <= SEQENCE_LEN; i++) {
    assert(int_sequence_select(sequence, SEQENCE_LEN, i, 0) ==
           OWT_Select(uwt, i, 0));
    assert(int_sequence_select(sequence, SEQENCE_LEN, i, 1) ==
           OWT_Select(uwt, i, 1));
    assert(int_sequence_select(sequence, SEQENCE_LEN, i, 2) ==
           OWT_Select(uwt, i, 2));
    assert(int_sequence_select(sequence, SEQENCE_LEN, i, 3) ==
           OWT_Select(uwt, i, 3));
    assert(int_sequence_select(sequence, SEQENCE_LEN, i, 4) ==
           OWT_Select(uwt, i, 4));
  }

  return true;
}

bool test_front_insert(void) {
  int32_t i;

  OWT_Struct uwt;
  OWT_Init(&uwt);

  int8_t* sequence = int_sequence_generate_random(SEQENCE_LEN, 5);

  for (i = SEQENCE_LEN - 1; i >= 0; i--) {
    int8_t letter = int_sequence_get(sequence, SEQENCE_LEN, i);
    OWT_Insert(&uwt, 0, letter);
  }

  _test_wavelet_tree(&uwt, sequence);

  int_sequence_free(&sequence);
  OWT_Free(&uwt);

  return true;
}

bool test_rear_insert(void) {
  int32_t i;

  OWT_Struct uwt;
  OWT_Init(&uwt);

  int8_t* sequence = int_sequence_generate_random(SEQENCE_LEN, 5);

  for (i = 0; i < SEQENCE_LEN; i++) {
    int8_t letter = int_sequence_get(sequence, SEQENCE_LEN, i);
    OWT_Insert(&uwt, i, letter);
  }

  _test_wavelet_tree(&uwt, sequence);

  int_sequence_free(&sequence);
  OWT_Free(&uwt);

  return true;
}

bool test_deletion(void) {
  int32_t i;

  OWT_Struct uwt;
  OWT_Init(&uwt);

  int8_t* sequence = int_sequence_generate_random(SEQENCE_LEN, 5);

  int32_t idx = 0;
  int32_t added = 0;
  int32_t pos[SEQENCE_LEN];

  for (i = 0; i < SEQENCE_LEN; i++) {
    int8_t letter = int_sequence_get(sequence, SEQENCE_LEN, i);

    if (letter == 'A' || letter == 'T') {
      OWT_Insert(&uwt, i + added, letter);
      pos[idx++] = i + added++;
    }
    OWT_Insert(&uwt, i + added, letter);
  }

  /* delete all additional symbols and check the structure */
  for (i = added - 1; i >= 0; i--)
    OWT_Delete(&uwt, pos[i]);

  _test_wavelet_tree(&uwt, sequence);

  int_sequence_free(&sequence);
  OWT_Free(&uwt);

  return true;
}

bool test_symbol_change(void) {
  int32_t i;
  int8_t letter;

  OWT_Struct uwt;
  OWT_Init(&uwt);

  int8_t* seq_one = int_sequence_generate_random(SEQENCE_LEN, 5);
  int8_t* seq_two = int_sequence_generate_random(SEQENCE_LEN, 5);

  for (i = 0; i < SEQENCE_LEN; i++) {
    letter = int_sequence_get(seq_one, SEQENCE_LEN, i);
    OWT_Insert(&uwt, i, letter);
  }

  _test_wavelet_tree(&uwt, seq_one);

  /* exchange symbols from first sequence with second sequence symbols */
  for (i = 0; i < SEQENCE_LEN; i++) {
    OWT_Delete(&uwt, i);

    letter = int_sequence_get(seq_two, SEQENCE_LEN, i);
    OWT_Insert(&uwt, i, letter);
  }

  _test_wavelet_tree(&uwt, seq_two);

  int_sequence_free(&seq_one);
  int_sequence_free(&seq_two);
  OWT_Free(&uwt);

  return true;
}

int main(int argc, char* argv[]) {
  UNUSED(argc);
  UNUSED(argv);

  // seed the random string generator
  srand(time(NULL));

  test_front_insert();
  test_rear_insert();

  test_deletion();
  test_symbol_change();

  printf("All tests successfull\n");

  return EXIT_SUCCESS;
}
