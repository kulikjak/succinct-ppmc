#include "universal.h"
#include "utils.h"
#include "unity_fixture.h"

#include "int_sequence.h"

TEST_GROUP(Wavelet_tree_universal);

#ifndef TEST_SEQENCE_LEN
  #define TEST_SEQENCE_LEN 1000
#endif

#ifndef TEST_PRINT_SEQUENCES
  #define TEST_PRINT_SEQUENCES false
#endif

UWT_Struct uwt;
uint8_t* sequence;

TEST_SETUP(Wavelet_tree_universal) {
  sequence = int_sequence_generate_random(TEST_SEQENCE_LEN, 13);
  UWT_Init(&uwt, 13);
}

TEST_TEAR_DOWN(Wavelet_tree_universal) {
  int_sequence_free(&sequence);
  UWT_Free(&uwt);
}

static void _test_wavelet_tree(UWT_Struct* uwt, uint8_t* sequence, uint16_t dsize) {
  int32_t i, j;

  if (TEST_PRINT_SEQUENCES) {
    int_sequence_print(sequence, TEST_SEQENCE_LEN);
    UWT_Print_Symbols(uwt);
  }

  for (i = 0; i < TEST_SEQENCE_LEN; i++) {
    TEST_ASSERT_EQUAL_UINT8(int_sequence_get(sequence, TEST_SEQENCE_LEN, i), UWT_Get(uwt, i));
  }

  for (i = 0; i <= TEST_SEQENCE_LEN; i++) {
    for (j = 0; j < dsize; j++) {
      TEST_ASSERT_EQUAL_INT32(int_sequence_rank(sequence, TEST_SEQENCE_LEN, i, j), UWT_Rank(uwt, i, j));
      TEST_ASSERT_EQUAL_INT32(int_sequence_select(sequence, TEST_SEQENCE_LEN, i, j), UWT_Select(uwt, i, j));
    }
  }
}

TEST(Wavelet_tree_universal, front_insertion) {
  int32_t i;

  for (i = TEST_SEQENCE_LEN - 1; i >= 0; i--) {
    uint8_t letter = int_sequence_get(sequence, TEST_SEQENCE_LEN, i);
    UWT_Insert(&uwt, 0, letter);
  }
  _test_wavelet_tree(&uwt, sequence, 13);
}

TEST(Wavelet_tree_universal, rear_insertion) {
  int32_t i;

  for (i = 0; i < TEST_SEQENCE_LEN; i++) {
    uint8_t letter = int_sequence_get(sequence, TEST_SEQENCE_LEN, i);
    UWT_Insert(&uwt, i, letter);
  }
  _test_wavelet_tree(&uwt, sequence, 13);
}

TEST(Wavelet_tree_universal, deletion) {
  int32_t i;

  int32_t idx = 0;
  int32_t added = 0;
  int32_t pos[TEST_SEQENCE_LEN];

  for (i = 0; i < TEST_SEQENCE_LEN; i++) {
    uint8_t letter = int_sequence_get(sequence, TEST_SEQENCE_LEN, i);

    if (letter == 0 || letter == 3) {
      UWT_Insert(&uwt, i + added, letter);
      pos[idx++] = i + added++;
    }
    UWT_Insert(&uwt, i + added, letter);
  }

  /* delete all additional symbols and check the structure */
  for (i = added - 1; i >= 0; i--)
    UWT_Delete(&uwt, pos[i]);

  _test_wavelet_tree(&uwt, sequence, 13);
}

TEST(Wavelet_tree_universal, symbol_change) {
  int32_t i;
  uint8_t letter;

  uint8_t* seq_one = int_sequence_generate_random(TEST_SEQENCE_LEN, 13);
  uint8_t* seq_two = int_sequence_generate_random(TEST_SEQENCE_LEN, 13);

  for (i = 0; i < TEST_SEQENCE_LEN; i++) {
    letter = int_sequence_get(seq_one, TEST_SEQENCE_LEN, i);
    UWT_Insert(&uwt, i, letter);
  }

  _test_wavelet_tree(&uwt, seq_one, 13);

  /* exchange symbols from first sequence with second sequence symbols */
  for (i = 0; i < TEST_SEQENCE_LEN; i++) {
    UWT_Delete(&uwt, i);

    letter = int_sequence_get(seq_two, TEST_SEQENCE_LEN, i);
    UWT_Insert(&uwt, i, letter);
  }

  _test_wavelet_tree(&uwt, seq_two, 13);

  int_sequence_free(&seq_one);
  int_sequence_free(&seq_two);
}

TEST(Wavelet_tree_universal, random_dict_sizes) {
  int32_t i, s;
  uint8_t* sequence;

  UWT_Struct uwt;

  printf(" ");
  srand(time(NULL));
  for (s = 0; s < 10; s++) {

    printf(".");
    fflush(stdout);
    int16_t size = rand() % 256;

    UWT_Init(&uwt, size);
    sequence = int_sequence_generate_random(TEST_SEQENCE_LEN, size);

    for (i = TEST_SEQENCE_LEN - 1; i >= 0; i--) {
      uint8_t letter = int_sequence_get(sequence, TEST_SEQENCE_LEN, i);
      UWT_Insert(&uwt, 0, letter);
    }
    _test_wavelet_tree(&uwt, sequence, size);

    int_sequence_free(&sequence);
    UWT_Free(&uwt);
  }
}

TEST_GROUP_RUNNER(Wavelet_tree_universal) {
  RUN_TEST_CASE(Wavelet_tree_universal, front_insertion);
  RUN_TEST_CASE(Wavelet_tree_universal, rear_insertion);
  RUN_TEST_CASE(Wavelet_tree_universal, deletion);
  RUN_TEST_CASE(Wavelet_tree_universal, symbol_change);
  RUN_TEST_CASE(Wavelet_tree_universal, random_dict_sizes);
}
