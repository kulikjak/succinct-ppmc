#include <stdio.h>
#include <stdlib.h>

#include "universal.h"
#include "utils.h"
#include "../misc/int_sequence.h"

#define SEQENCE_LEN 100
#define PRINT_SEQUENCES true

bool _test_wavelet_tree(UWT_Struct* uwt, uint8_t* sequence, uint16_t dsize) {
  int32_t i, j;

  if (PRINT_SEQUENCES) {
    int_sequence_print(sequence, SEQENCE_LEN);
    UWT_Print_Symbols(uwt);
  }

  for (i = 0; i < SEQENCE_LEN; i++) {
    assert(int_sequence_get(sequence, SEQENCE_LEN, i) == UWT_Get(uwt, i));
  }

  for (i = 0; i <= SEQENCE_LEN; i++) {
    for (j = 0; j < dsize; j++) {
      assert(int_sequence_rank(sequence, SEQENCE_LEN, i, j) ==
             UWT_Rank(uwt, i, j));
    }
  }

  for (i = 0; i <= SEQENCE_LEN; i++) {
    for (j = 0; j < dsize; j++) {
      assert(int_sequence_select(sequence, SEQENCE_LEN, i, j) ==
             UWT_Select(uwt, i, j));
    }
  }

  return true;
}

bool test_front_insert(void) {
  int32_t i;

  UWT_Struct uwt;
  UWT_Init(&uwt, 4);

  uint8_t* sequence = int_sequence_generate_random(SEQENCE_LEN, 4);

  for (i = SEQENCE_LEN - 1; i >= 0; i--) {
    uint8_t letter = int_sequence_get(sequence, SEQENCE_LEN, i);
    UWT_Insert(&uwt, 0, letter);
  }

  _test_wavelet_tree(&uwt, sequence, 4);

  int_sequence_free(&sequence);
  UWT_Free(&uwt);

  return true;
}

bool test_rear_insert(void) {
  int32_t i;

  UWT_Struct uwt;
  UWT_Init(&uwt, 4);

  uint8_t* sequence = int_sequence_generate_random(SEQENCE_LEN, 4);

  for (i = 0; i < SEQENCE_LEN; i++) {
    uint8_t letter = int_sequence_get(sequence, SEQENCE_LEN, i);
    UWT_Insert(&uwt, i, letter);
  }

  _test_wavelet_tree(&uwt, sequence, 4);

  int_sequence_free(&sequence);
  UWT_Free(&uwt);

  return true;
}

bool test_deletion(void) {
  int32_t i;

  UWT_Struct uwt;
  UWT_Init(&uwt, 6);

  uint8_t* sequence = int_sequence_generate_random(SEQENCE_LEN, 6);

  int32_t idx = 0;
  int32_t added = 0;
  int32_t pos[SEQENCE_LEN];

  for (i = 0; i < SEQENCE_LEN; i++) {
    uint8_t letter = int_sequence_get(sequence, SEQENCE_LEN, i);

    if (letter % 6) {
      UWT_Insert(&uwt, i + added, letter);
      pos[idx++] = i + added++;
    }
    UWT_Insert(&uwt, i + added, letter);
  }

  UWT_Print_Symbols(&uwt);

  /* delete all additional symbols and check the structure */
  for (i = added - 1; i >= 0; i--)
    UWT_Delete(&uwt, pos[i]);

  _test_wavelet_tree(&uwt, sequence, 6);

  int_sequence_free(&sequence);
  UWT_Free(&uwt);

  return true;
}

bool test_full_ascii(void) {
  int32_t i;

  UWT_Struct uwt;
  UWT_Init(&uwt, 256);

  uint8_t* sequence = int_sequence_generate_random(SEQENCE_LEN, 256);

  for (i = SEQENCE_LEN - 1; i >= 0; i--) {
    uint8_t letter = int_sequence_get(sequence, SEQENCE_LEN, i);
    UWT_Insert(&uwt, 0, letter);
  }

  _test_wavelet_tree(&uwt, sequence, 256);

  int_sequence_free(&sequence);
  UWT_Free(&uwt);

  return true;
}

int main(int argc, char* argv[]) {
  UNUSED(argc);
  UNUSED(argv);

  srand(time(NULL));

  test_front_insert();
  test_rear_insert();
  test_deletion();
  test_full_ascii();

  printf("All tests successfull\n");

  return EXIT_SUCCESS;
}
