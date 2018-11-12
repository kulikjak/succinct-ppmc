#include <stdio.h>
#include <stdlib.h>

#include "optimized.h"
#include "utils.h"
#include "../misc/int_sequence.h"

#define SEQENCE_LEN 100
#define PRINT_SEQUENCES true

bool _test_wavelet_tree(OWT_Struct* owt, uint8_t* sequence) {
  int32_t i;

  if (PRINT_SEQUENCES) {
    int_sequence_print(sequence, SEQENCE_LEN);
    OWT_Print_Symbols(owt);
  }

  for (i = 0; i < SEQENCE_LEN; i++) {
    assert(int_sequence_get(sequence, SEQENCE_LEN, i) == OWT_Get(owt, i));
  }

  for (i = 0; i <= SEQENCE_LEN; i++) {
    assert(int_sequence_rank(sequence, SEQENCE_LEN, i, 0) ==
           OWT_Rank(owt, i, 0));
    assert(int_sequence_rank(sequence, SEQENCE_LEN, i, 1) ==
           OWT_Rank(owt, i, 1));
    assert(int_sequence_rank(sequence, SEQENCE_LEN, i, 2) ==
           OWT_Rank(owt, i, 2));
    assert(int_sequence_rank(sequence, SEQENCE_LEN, i, 3) ==
           OWT_Rank(owt, i, 3));
    assert(int_sequence_rank(sequence, SEQENCE_LEN, i, 4) ==
           OWT_Rank(owt, i, 4));
  }

  for (i = 0; i <= SEQENCE_LEN; i++) {
    assert(int_sequence_select(sequence, SEQENCE_LEN, i, 0) ==
           OWT_Select(owt, i, 0));
    assert(int_sequence_select(sequence, SEQENCE_LEN, i, 1) ==
           OWT_Select(owt, i, 1));
    assert(int_sequence_select(sequence, SEQENCE_LEN, i, 2) ==
           OWT_Select(owt, i, 2));
    assert(int_sequence_select(sequence, SEQENCE_LEN, i, 3) ==
           OWT_Select(owt, i, 3));
    assert(int_sequence_select(sequence, SEQENCE_LEN, i, 4) ==
           OWT_Select(owt, i, 4));
  }

  return true;
}

bool test_front_insert(void) {
  int32_t i;

  OWT_Struct owt;
  OWT_Init(&owt);

  uint8_t* sequence = int_sequence_generate_random(SEQENCE_LEN, 5);

  for (i = SEQENCE_LEN - 1; i >= 0; i--) {
    uint8_t letter = int_sequence_get(sequence, SEQENCE_LEN, i);
    OWT_Insert(&owt, 0, letter);
  }

  _test_wavelet_tree(&owt, sequence);

  int_sequence_free(&sequence);
  OWT_Free(&owt);

  return true;
}

bool test_rear_insert(void) {
  int32_t i;

  OWT_Struct owt;
  OWT_Init(&owt);

  uint8_t* sequence = int_sequence_generate_random(SEQENCE_LEN, 5);

  for (i = 0; i < SEQENCE_LEN; i++) {
    uint8_t letter = int_sequence_get(sequence, SEQENCE_LEN, i);
    OWT_Insert(&owt, i, letter);
  }

  _test_wavelet_tree(&owt, sequence);

  int_sequence_free(&sequence);
  OWT_Free(&owt);

  return true;
}

bool test_deletion(void) {
  int32_t i;

  OWT_Struct owt;
  OWT_Init(&owt);

  uint8_t* sequence = int_sequence_generate_random(SEQENCE_LEN, 5);

  int32_t idx = 0;
  int32_t added = 0;
  int32_t pos[SEQENCE_LEN];

  for (i = 0; i < SEQENCE_LEN; i++) {
    uint8_t letter = int_sequence_get(sequence, SEQENCE_LEN, i);

    if (letter == 0 || letter == 3) {
      OWT_Insert(&owt, i + added, letter);
      pos[idx++] = i + added++;
    }
    OWT_Insert(&owt, i + added, letter);
  }

  /* delete all additional symbols and check the structure */
  for (i = added - 1; i >= 0; i--)
    OWT_Delete(&owt, pos[i]);

  _test_wavelet_tree(&owt, sequence);

  int_sequence_free(&sequence);
  OWT_Free(&owt);

  return true;
}

bool test_symbol_change(void) {
  int32_t i;
  uint8_t letter;

  OWT_Struct owt;
  OWT_Init(&owt);

  uint8_t* seq_one = int_sequence_generate_random(SEQENCE_LEN, 5);
  uint8_t* seq_two = int_sequence_generate_random(SEQENCE_LEN, 5);

  for (i = 0; i < SEQENCE_LEN; i++) {
    letter = int_sequence_get(seq_one, SEQENCE_LEN, i);
    OWT_Insert(&owt, i, letter);
  }

  _test_wavelet_tree(&owt, seq_one);

  /* exchange symbols from first sequence with second sequence symbols */
  for (i = 0; i < SEQENCE_LEN; i++) {
    OWT_Delete(&owt, i);

    letter = int_sequence_get(seq_two, SEQENCE_LEN, i);
    OWT_Insert(&owt, i, letter);
  }

  _test_wavelet_tree(&owt, seq_two);

  int_sequence_free(&seq_one);
  int_sequence_free(&seq_two);
  OWT_Free(&owt);

  return true;
}

int main(int argc, char* argv[]) {
  UNUSED(argc);
  UNUSED(argv);

  srand(time(NULL));

  test_front_insert();
  test_rear_insert();
  test_deletion();
  test_symbol_change();

  printf("All tests successfull\n");

  return EXIT_SUCCESS;
}
