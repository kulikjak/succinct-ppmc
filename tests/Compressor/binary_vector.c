#include "structure.h"
#include "unity_fixture.h"

#include "bit_sequence.h"

TEST_GROUP(Compressor_binary_vector);

#define VAR_IGNORE 1

#ifndef TEST_SEQENCE_LEN
  #define TEST_SEQENCE_LEN 1000
#endif

#ifndef TEST_PRINT_SEQUENCES
  #define TEST_PRINT_SEQUENCES false
#endif

BitSeqType* sequence;

Graph_Struct Graph;
Graph_Line line;

TEST_SETUP(Compressor_binary_vector) {
  Graph_Init(&Graph);
  sequence = bit_sequence_generate_random(TEST_SEQENCE_LEN);
}

TEST_TEAR_DOWN(Compressor_binary_vector) {
  Graph_Free(&Graph);
  bit_sequence_free(&sequence);
}

void _test_binary_vector(GraphRef Graph__, BitSeqType* sequence) {
  int32_t i;

  if (TEST_PRINT_SEQUENCES) {
    bit_sequence_print(sequence, TEST_SEQENCE_LEN);
    Graph_Print(Graph__);
  }

  for (i = 0; i < TEST_SEQENCE_LEN; i++) {
    GLine_Get(Graph__, i, &line);
    TEST_ASSERT_EQUAL_INT32(
        bit_sequence_get(sequence, TEST_SEQENCE_LEN, i), line.L_);
  }

  for (i = 0; i <= TEST_SEQENCE_LEN; i++) {
    TEST_ASSERT_EQUAL_INT32(bit_sequence_rank(sequence, TEST_SEQENCE_LEN, i),
                            Graph_Rank(Graph__, i, VECTOR_L, VALUE_1));
    TEST_ASSERT_EQUAL_INT32(bit_sequence_rank0(sequence, TEST_SEQENCE_LEN, i),
                            Graph_Rank(Graph__, i, VECTOR_L, VALUE_0));
    TEST_ASSERT_EQUAL_INT32(bit_sequence_select(sequence, TEST_SEQENCE_LEN, i),
                            Graph_Select(Graph__, i, VECTOR_L, VALUE_1));
    TEST_ASSERT_EQUAL_INT32(bit_sequence_select0(sequence, TEST_SEQENCE_LEN, i),
                            Graph_Select(Graph__, i, VECTOR_L, VALUE_0));
  }
}

TEST(Compressor_binary_vector, front_insertion) {
  int8_t bit;
  int32_t i;

  for (i = 0; i < TEST_SEQENCE_LEN; i++) {
    bit = (sequence[i / BitSeqSize] >> ((BitSeqSize - 1) - (i % BitSeqSize))) & 0x1;

    GLine_Fill(&line, bit, VAR_IGNORE, VAR_IGNORE);
    GLine_Insert(&Graph, i, &line);
  }
  _test_binary_vector(&Graph, sequence);
}

TEST(Compressor_binary_vector, rear_insertion) {
  int8_t bit;
  int32_t i;

  for (i = TEST_SEQENCE_LEN - 1; i >= 0; i--) {
    bit = (sequence[i / BitSeqSize] >> ((BitSeqSize - 1) - (i % BitSeqSize))) & 0x1;

    GLine_Fill(&line, bit, VAR_IGNORE, VAR_IGNORE);
    GLine_Insert(&Graph, 0, &line);
  }
  _test_binary_vector(&Graph, sequence);
}

TEST(Compressor_binary_vector, front_rear_insertion) {
  int8_t bit;
  int32_t i;

  for (i = 0; i < TEST_SEQENCE_LEN / 2; i++) {
    bit = (sequence[i / BitSeqSize] >> ((BitSeqSize - 1) - (i % BitSeqSize))) & 0x1;

    GLine_Fill(&line, bit, VAR_IGNORE, VAR_IGNORE);
    GLine_Insert(&Graph, i, &line);
  }

  for (i = TEST_SEQENCE_LEN - 1; i >= TEST_SEQENCE_LEN / 2; i--) {
    bit = (sequence[i / BitSeqSize] >> ((BitSeqSize - 1) - (i % BitSeqSize))) & 0x1;

    GLine_Fill(&line, bit, VAR_IGNORE, VAR_IGNORE);
    GLine_Insert(&Graph, TEST_SEQENCE_LEN / 2, &line);
  }
  _test_binary_vector(&Graph, sequence);
}

TEST(Compressor_binary_vector, rear_front_insertion) {
  int8_t bit;
  int32_t i;

  for (i = (TEST_SEQENCE_LEN / 2) - 1; i >= 0; i--) {
    bit = (sequence[i / BitSeqSize] >> ((BitSeqSize - 1) - (i % BitSeqSize))) & 0x1;

    GLine_Fill(&line, bit, VAR_IGNORE, VAR_IGNORE);
    GLine_Insert(&Graph, 0, &line);
  }

  for (i = TEST_SEQENCE_LEN / 2; i < TEST_SEQENCE_LEN; i++) {
    bit = (sequence[i / BitSeqSize] >> ((BitSeqSize - 1) - (i % BitSeqSize))) & 0x1;

    GLine_Fill(&line, bit, VAR_IGNORE, VAR_IGNORE);
    GLine_Insert(&Graph, i, &line);
  }
  _test_binary_vector(&Graph, sequence);
}

TEST(Compressor_binary_vector, mixed_insertion) {
  int8_t bit;
  int32_t i;

  for (i = 0; i < TEST_SEQENCE_LEN / 4; i++) {
    bit = (sequence[i / BitSeqSize] >> ((BitSeqSize - 1) - (i % BitSeqSize))) & 0x1;

    GLine_Fill(&line, bit, VAR_IGNORE, VAR_IGNORE);
    GLine_Insert(&Graph, i, &line);
  }

  for (i = TEST_SEQENCE_LEN - 1; i >= (TEST_SEQENCE_LEN / 4 ) * 3; i--) {
    bit = (sequence[i / BitSeqSize] >> ((BitSeqSize - 1) - (i % BitSeqSize))) & 0x1;

    GLine_Fill(&line, bit, VAR_IGNORE, VAR_IGNORE);
    GLine_Insert(&Graph, TEST_SEQENCE_LEN / 4, &line);
  }

  for (i = (TEST_SEQENCE_LEN / 2) - 1; i >= TEST_SEQENCE_LEN / 4; i--) {
    bit = (sequence[i / BitSeqSize] >> ((BitSeqSize - 1) - (i % BitSeqSize))) & 0x1;

    GLine_Fill(&line, bit, VAR_IGNORE, VAR_IGNORE);
    GLine_Insert(&Graph, TEST_SEQENCE_LEN / 4, &line);
  }

  for (i = TEST_SEQENCE_LEN / 2; i < (TEST_SEQENCE_LEN / 4 ) * 3; i++) {
    bit = (sequence[i / BitSeqSize] >> ((BitSeqSize - 1) - (i % BitSeqSize))) & 0x1;

    GLine_Fill(&line, bit, VAR_IGNORE, VAR_IGNORE);
    GLine_Insert(&Graph, i, &line);
  }
  _test_binary_vector(&Graph, sequence);
}

#ifdef ENABLE_CLEVER_NODE_SPLIT

void test_node_split_(mem_ptr ptr__) {
  LeafRef leaf;
  NodeRef node;

  if (IS_LEAF(ptr__)) {
    leaf = MEMORY_GET_LEAF(Graph.mem_, ptr__);
    TEST_ASSERT_TRUE((leaf->vectorL_ >> (32 - leaf->p_) & 0x1))

  } else {
    node = MEMORY_GET_NODE(Graph.mem_, ptr__);
    test_node_split_(node->left_);
    test_node_split_(node->right_);
  }
}

TEST(Compressor_binary_vector, clever_node_split) {
  int8_t bit;
  int32_t i, limiter = 0;

  for (i = 0; i < TEST_SEQENCE_LEN; i++) {
    if (!limiter) {
      sequence[i / BitSeqSize] |= ((BitSeqType)0x1) << ((BitSeqSize - 1) - (i % BitSeqSize));
      limiter = 3;
    } else {
      limiter--;
    }
  }
  sequence[(TEST_SEQENCE_LEN-1) / BitSeqSize] |=
    ((BitSeqType)0x1) << ((BitSeqSize - 1) - ((TEST_SEQENCE_LEN-1) % BitSeqSize));

  for (i = TEST_SEQENCE_LEN - 1; i >= 0; i--) {
    bit = (sequence[i / BitSeqSize] >> ((BitSeqSize - 1) - (i % BitSeqSize))) & 0x1;

    GLine_Fill(&line, bit, VAR_IGNORE, VAR_IGNORE);
    GLine_Insert(&Graph, 0, &line);
  }

  test_node_split_(Graph.root_);
}
#endif

TEST_GROUP_RUNNER(Compressor_binary_vector) {
  RUN_TEST_CASE(Compressor_binary_vector, front_insertion);
  RUN_TEST_CASE(Compressor_binary_vector, rear_insertion);
  RUN_TEST_CASE(Compressor_binary_vector, front_rear_insertion);
  RUN_TEST_CASE(Compressor_binary_vector, rear_front_insertion);
  RUN_TEST_CASE(Compressor_binary_vector, mixed_insertion);

#ifdef ENABLE_CLEVER_NODE_SPLIT
  RUN_TEST_CASE(Compressor_binary_vector, clever_node_split);
#endif
}
