#include "dbv.h"
#include "utils.h"
#include "unity_fixture.h"

#include "bit_sequence.h"

TEST_GROUP(Dynamic_BitVector);

#ifndef TEST_SEQENCE_LEN
  #define TEST_SEQENCE_LEN 25000
#endif

#ifndef TEST_PRINT_SEQUENCES
  #define TEST_PRINT_SEQUENCES false
#endif

DBV_Struct DBV;
BitSeqType* sequence;

TEST_SETUP(Dynamic_BitVector) {
  sequence = bit_sequence_generate_random(TEST_SEQENCE_LEN);
  DBV_Init(&DBV);
}

TEST_TEAR_DOWN(Dynamic_BitVector) {
  bit_sequence_free(&sequence);
  DBV_Free(&DBV);
}

void _test_binary_vector(DBVStructRef DBV, BitSeqType* sequence) {
  int32_t i;

  if (TEST_PRINT_SEQUENCES) {
    bit_sequence_print(sequence, TEST_SEQENCE_LEN);
    DBV_Print(DBV);
  }

  for (i = 0; i < TEST_SEQENCE_LEN; i++) {
    TEST_ASSERT_EQUAL_INT32(bit_sequence_get(sequence, TEST_SEQENCE_LEN, i), DBV_Get(DBV, i));
  }

  for (i = 0; i <= TEST_SEQENCE_LEN; i++) {
    TEST_ASSERT_EQUAL_INT32(bit_sequence_rank(sequence, TEST_SEQENCE_LEN, i), DBV_Rank(DBV, i));
    TEST_ASSERT_EQUAL_INT32(bit_sequence_rank0(sequence, TEST_SEQENCE_LEN, i), DBV_Rank0(DBV, i));
    TEST_ASSERT_EQUAL_INT32(bit_sequence_select(sequence, TEST_SEQENCE_LEN, i), DBV_Select(DBV, i));
    TEST_ASSERT_EQUAL_INT32(bit_sequence_select0(sequence, TEST_SEQENCE_LEN, i),
                            DBV_Select0(DBV, i));
  }
}

TEST(Dynamic_BitVector, front_insertion) {
  int8_t bit;
  int32_t i;

  for (i = TEST_SEQENCE_LEN - 1; i >= 0; i--) {
    bit = (sequence[i / BitSeqSize] >> ((BitSeqSize - 1) - (i % BitSeqSize))) & 0x1;
    DBV_Insert(&DBV, 0, bit);
  }
  _test_binary_vector(&DBV, sequence);
}

TEST(Dynamic_BitVector, rear_insertion) {
  int8_t bit;
  int32_t i;

  for (i = 0; i < TEST_SEQENCE_LEN; i++) {
    bit = (sequence[i / BitSeqSize] >> ((BitSeqSize - 1) - (i % BitSeqSize))) & 0x1;
    DBV_Insert(&DBV, i, bit);
  }
  _test_binary_vector(&DBV, sequence);
}

TEST(Dynamic_BitVector, front_rear_insertion) {
  int8_t bit;
  int32_t i;

  for (i = 0; i < TEST_SEQENCE_LEN / 2; i++) {
    bit = (sequence[i / BitSeqSize] >> ((BitSeqSize - 1) - (i % BitSeqSize))) & 0x1;
    DBV_Insert(&DBV, i, bit);
  }

  for (i = TEST_SEQENCE_LEN - 1; i >= TEST_SEQENCE_LEN / 2; i--) {
    bit = (sequence[i / BitSeqSize] >> ((BitSeqSize - 1) - (i % BitSeqSize))) & 0x1;
    DBV_Insert(&DBV, TEST_SEQENCE_LEN / 2, bit);
  }
  _test_binary_vector(&DBV, sequence);
}

TEST(Dynamic_BitVector, rear_front_insertion) {
  int8_t bit;
  int32_t i;

  for (i = (TEST_SEQENCE_LEN / 2) - 1; i >= 0; i--) {
    bit = (sequence[i / BitSeqSize] >> ((BitSeqSize - 1) - (i % BitSeqSize))) & 0x1;
    DBV_Insert(&DBV, 0, bit);
  }

  for (i = TEST_SEQENCE_LEN / 2; i < TEST_SEQENCE_LEN; i++) {
    bit = (sequence[i / BitSeqSize] >> ((BitSeqSize - 1) - (i % BitSeqSize))) & 0x1;
    DBV_Insert(&DBV, i, bit);
  }
  _test_binary_vector(&DBV, sequence);
}

TEST(Dynamic_BitVector, mixed_insertion) {
  int8_t bit;
  int32_t i;

  for (i = 0; i < TEST_SEQENCE_LEN / 4; i++) {
    bit = (sequence[i / BitSeqSize] >> ((BitSeqSize - 1) - (i % BitSeqSize))) & 0x1;
    DBV_Insert(&DBV, i, bit);
  }

  for (i = TEST_SEQENCE_LEN - 1; i >= (TEST_SEQENCE_LEN / 4) * 3; i--) {
    bit = (sequence[i / BitSeqSize] >> ((BitSeqSize - 1) - (i % BitSeqSize))) & 0x1;
    DBV_Insert(&DBV, (TEST_SEQENCE_LEN / 4), bit);
  }

  for (i = (TEST_SEQENCE_LEN / 2) - 1; i >= TEST_SEQENCE_LEN / 4; i--) {
    bit = (sequence[i / BitSeqSize] >> ((BitSeqSize - 1) - (i % BitSeqSize))) & 0x1;
    DBV_Insert(&DBV, (TEST_SEQENCE_LEN / 4), bit);
  }

  for (i = TEST_SEQENCE_LEN / 2; i < (TEST_SEQENCE_LEN / 4) * 3; i++) {
    bit = (sequence[i / BitSeqSize] >> ((BitSeqSize - 1) - (i % BitSeqSize))) & 0x1;
    DBV_Insert(&DBV, i, bit);
  }
  _test_binary_vector(&DBV, sequence);
}

TEST(Dynamic_BitVector, deletion) {
  int8_t bit;
  int32_t i;

  int32_t idx = 0;
  int32_t added = 0;
  int32_t pos[TEST_SEQENCE_LEN];

  for (i = 0; i < TEST_SEQENCE_LEN; i++) {
    bit = (sequence[i / BitSeqSize] >> ((BitSeqSize - 1) - (i % BitSeqSize))) & 0x1;

    /* insert additional bits for deletion testing purposes */
    if (bit) {
      DBV_Insert(&DBV, i + added, bit);
      pos[idx++] = i + added++;
    }
    DBV_Insert(&DBV, i + added, bit);
  }

  /* delete all additional bits and check the structure */
  for (i = added - 1; i >= 0; i--) {
    DBV_Delete(&DBV, pos[i]);
  }
  _test_binary_vector(&DBV, sequence);
}

TEST_GROUP_RUNNER(Dynamic_BitVector) {
  RUN_TEST_CASE(Dynamic_BitVector, front_insertion);
  RUN_TEST_CASE(Dynamic_BitVector, rear_insertion);
  RUN_TEST_CASE(Dynamic_BitVector, front_rear_insertion);
  RUN_TEST_CASE(Dynamic_BitVector, rear_front_insertion);
  RUN_TEST_CASE(Dynamic_BitVector, mixed_insertion);
  RUN_TEST_CASE(Dynamic_BitVector, deletion);
}
