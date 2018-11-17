#include "optimized.h"
#include "utils.h"
#include "unity_fixture.h"

#include "int_sequence.h"

TEST_GROUP(Wavelet_tree_optimized);

#ifndef TEST_SEQENCE_LEN
  #define TEST_SEQENCE_LEN 1000
#endif

#ifndef TEST_PRINT_SEQUENCES
  #define TEST_PRINT_SEQUENCES false
#endif

OWT_Struct owt;
uint8_t* sequence;

TEST_SETUP(Wavelet_tree_optimized) {
  sequence = int_sequence_generate_random(TEST_SEQENCE_LEN, 5);
  OWT_Init(&owt);
}

TEST_TEAR_DOWN(Wavelet_tree_optimized) {
  int_sequence_free(&sequence);
  OWT_Free(&owt);
}

static void _test_wavelet_tree(OWT_Struct* owt, uint8_t* sequence) {
  int32_t i;

  if (TEST_PRINT_SEQUENCES) {
    int_sequence_print(sequence, TEST_SEQENCE_LEN);
    OWT_Print_Symbols(owt);
  }

  for (i = 0; i < TEST_SEQENCE_LEN; i++) {
    TEST_ASSERT_EQUAL_UINT8(int_sequence_get(sequence, TEST_SEQENCE_LEN, i), OWT_Get(owt, i));
  }

  for (i = 0; i <= TEST_SEQENCE_LEN; i++) {
    TEST_ASSERT_EQUAL_INT32(int_sequence_rank(sequence, TEST_SEQENCE_LEN, i, 0),
                            OWT_Rank(owt, i, 0));
    TEST_ASSERT_EQUAL_INT32(int_sequence_rank(sequence, TEST_SEQENCE_LEN, i, 1),
                            OWT_Rank(owt, i, 1));
    TEST_ASSERT_EQUAL_INT32(int_sequence_rank(sequence, TEST_SEQENCE_LEN, i, 2),
                            OWT_Rank(owt, i, 2));
    TEST_ASSERT_EQUAL_INT32(int_sequence_rank(sequence, TEST_SEQENCE_LEN, i, 3),
                            OWT_Rank(owt, i, 3));
    TEST_ASSERT_EQUAL_INT32(int_sequence_rank(sequence, TEST_SEQENCE_LEN, i, 4),
                            OWT_Rank(owt, i, 4));

    TEST_ASSERT_EQUAL_INT32(int_sequence_select(sequence, TEST_SEQENCE_LEN, i, 0),
                            OWT_Select(owt, i, 0));
    TEST_ASSERT_EQUAL_INT32(int_sequence_select(sequence, TEST_SEQENCE_LEN, i, 1),
                            OWT_Select(owt, i, 1));
    TEST_ASSERT_EQUAL_INT32(int_sequence_select(sequence, TEST_SEQENCE_LEN, i, 2),
                            OWT_Select(owt, i, 2));
    TEST_ASSERT_EQUAL_INT32(int_sequence_select(sequence, TEST_SEQENCE_LEN, i, 3),
                            OWT_Select(owt, i, 3));
    TEST_ASSERT_EQUAL_INT32(int_sequence_select(sequence, TEST_SEQENCE_LEN, i, 4),
                            OWT_Select(owt, i, 4));
  }
}

TEST(Wavelet_tree_optimized, front_insertion) {
  int32_t i;

  for (i = TEST_SEQENCE_LEN - 1; i >= 0; i--) {
    uint8_t letter = int_sequence_get(sequence, TEST_SEQENCE_LEN, i);
    OWT_Insert(&owt, 0, letter);
  }
  _test_wavelet_tree(&owt, sequence);
}

TEST(Wavelet_tree_optimized, rear_insertion) {
  int32_t i;

  for (i = 0; i < TEST_SEQENCE_LEN; i++) {
    uint8_t letter = int_sequence_get(sequence, TEST_SEQENCE_LEN, i);
    OWT_Insert(&owt, i, letter);
  }
  _test_wavelet_tree(&owt, sequence);
}

TEST(Wavelet_tree_optimized, deletion) {
  int32_t i;

  int32_t idx = 0;
  int32_t added = 0;
  int32_t pos[TEST_SEQENCE_LEN];

  for (i = 0; i < TEST_SEQENCE_LEN; i++) {
    uint8_t letter = int_sequence_get(sequence, TEST_SEQENCE_LEN, i);

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
}

TEST(Wavelet_tree_optimized, symbol_change) {
  int32_t i;
  uint8_t letter;

  uint8_t* seq_one = int_sequence_generate_random(TEST_SEQENCE_LEN, 5);
  uint8_t* seq_two = int_sequence_generate_random(TEST_SEQENCE_LEN, 5);

  for (i = 0; i < TEST_SEQENCE_LEN; i++) {
    letter = int_sequence_get(seq_one, TEST_SEQENCE_LEN, i);
    OWT_Insert(&owt, i, letter);
  }

  _test_wavelet_tree(&owt, seq_one);

  /* exchange symbols from first sequence with second sequence symbols */
  for (i = 0; i < TEST_SEQENCE_LEN; i++) {
    OWT_Delete(&owt, i);

    letter = int_sequence_get(seq_two, TEST_SEQENCE_LEN, i);
    OWT_Insert(&owt, i, letter);
  }

  _test_wavelet_tree(&owt, seq_two);

  int_sequence_free(&seq_one);
  int_sequence_free(&seq_two);
}

TEST_GROUP_RUNNER(Wavelet_tree_optimized) {
  RUN_TEST_CASE(Wavelet_tree_optimized, front_insertion);
  RUN_TEST_CASE(Wavelet_tree_optimized, rear_insertion);
  RUN_TEST_CASE(Wavelet_tree_optimized, deletion);
  RUN_TEST_CASE(Wavelet_tree_optimized, symbol_change);
}
