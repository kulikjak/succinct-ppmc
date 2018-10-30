#include "structure.h"
#include "unity_fixture.h"

#include "misc/bit_sequence.h"

TEST_GROUP(StructBinaryVector);

#define VAR_IGNORE 1
#define BINARY_VECTOR_SAMPLE_SIZE_ 250
#define BINARY_VECTOR_PRINT_SEQUENCES false

uint64_t* sequence;

Graph_Struct Graph;
Graph_Line line;

TEST_SETUP(StructBinaryVector) {
  Graph_Init(&Graph);
  sequence = init_random_bin_sequence(BINARY_VECTOR_SAMPLE_SIZE_);
}

TEST_TEAR_DOWN(StructBinaryVector) {
  Graph_Free(&Graph);
  free(sequence);
}

void _test_binary_vector(GraphRef Graph__, uint64_t* sequence) {
  int32_t i;

  if (BINARY_VECTOR_PRINT_SEQUENCES) {
    print_bit_sequence(sequence, BINARY_VECTOR_SAMPLE_SIZE_);
    Graph_Print(Graph__);
  }

  for (i = 0; i < BINARY_VECTOR_SAMPLE_SIZE_; i++) {
    GLine_Get(Graph__, i, &line);
    TEST_ASSERT_EQUAL_INT32(
        get_bit_sequence(sequence, BINARY_VECTOR_SAMPLE_SIZE_, i), line.L_);
  }

  for (i = 0; i <= BINARY_VECTOR_SAMPLE_SIZE_; i++) {
    TEST_ASSERT_EQUAL_INT32(
        rank_bit_sequence(sequence, BINARY_VECTOR_SAMPLE_SIZE_, i),
        Graph_Rank(Graph__, i, VECTOR_L, VALUE_1));
    TEST_ASSERT_EQUAL_INT32(
        rank0_bit_sequence(sequence, BINARY_VECTOR_SAMPLE_SIZE_, i),
        Graph_Rank(Graph__, i, VECTOR_L, VALUE_0));

    TEST_ASSERT_EQUAL_INT32(
        select_bit_sequence(sequence, BINARY_VECTOR_SAMPLE_SIZE_, i),
        Graph_Select(Graph__, i, VECTOR_L, VALUE_1));
    TEST_ASSERT_EQUAL_INT32(
        select0_bit_sequence(sequence, BINARY_VECTOR_SAMPLE_SIZE_, i),
        Graph_Select(Graph__, i, VECTOR_L, VALUE_0));
  }
}

TEST(StructBinaryVector, CorrectFrontInsertion) {
  int8_t bit;
  int32_t i;

  for (i = 0; i < BINARY_VECTOR_SAMPLE_SIZE_; i++) {
    bit = (sequence[i / 64] >> (63 - (i % 64))) & 0x1;

    GLine_Fill(&line, bit, VAR_IGNORE, VAR_IGNORE);
    GLine_Insert(&Graph, i, &line);
  }
  _test_binary_vector(&Graph, sequence);
}

TEST(StructBinaryVector, CorrectRearInsertion) {
  int8_t bit;
  int32_t i;

  for (i = BINARY_VECTOR_SAMPLE_SIZE_ - 1; i >= 0; i--) {
    bit = (sequence[i / 64] >> (63 - (i % 64))) & 0x1;

    GLine_Fill(&line, bit, VAR_IGNORE, VAR_IGNORE);
    GLine_Insert(&Graph, 0, &line);
  }
  _test_binary_vector(&Graph, sequence);
}

TEST(StructBinaryVector, CorrectFrontRearInsertion) {
  int8_t bit;
  int32_t i;

  for (i = 0; i < BINARY_VECTOR_SAMPLE_SIZE_ / 2; i++) {
    bit = (sequence[i / 64] >> (63 - (i % 64))) & 0x1;

    GLine_Fill(&line, bit, VAR_IGNORE, VAR_IGNORE);
    GLine_Insert(&Graph, i, &line);
  }

  for (i = BINARY_VECTOR_SAMPLE_SIZE_ - 1; i >= BINARY_VECTOR_SAMPLE_SIZE_ / 2; i--) {
    bit = (sequence[i / 64] >> (63 - (i % 64))) & 0x1;

    GLine_Fill(&line, bit, VAR_IGNORE, VAR_IGNORE);
    GLine_Insert(&Graph, BINARY_VECTOR_SAMPLE_SIZE_ / 2, &line);
  }
  _test_binary_vector(&Graph, sequence);
}

TEST(StructBinaryVector, CorrectRearFrontInsertion) {
  int8_t bit;
  int32_t i;

  for (i = (BINARY_VECTOR_SAMPLE_SIZE_ / 2) - 1; i >= 0; i--) {
    bit = (sequence[i / 64] >> (63 - (i % 64))) & 0x1;

    GLine_Fill(&line, bit, VAR_IGNORE, VAR_IGNORE);
    GLine_Insert(&Graph, 0, &line);
  }

  for (i = BINARY_VECTOR_SAMPLE_SIZE_ / 2; i < BINARY_VECTOR_SAMPLE_SIZE_; i++) {
    bit = (sequence[i / 64] >> (63 - (i % 64))) & 0x1;

    GLine_Fill(&line, bit, VAR_IGNORE, VAR_IGNORE);
    GLine_Insert(&Graph, i, &line);
  }
  _test_binary_vector(&Graph, sequence);
}

TEST(StructBinaryVector, CorrectMixedInsertion) {
  int8_t bit;
  int32_t i;

  for (i = 0; i < BINARY_VECTOR_SAMPLE_SIZE_ / 4; i++) {
    bit = (sequence[i / 64] >> (63 - (i % 64))) & 0x1;

    GLine_Fill(&line, bit, VAR_IGNORE, VAR_IGNORE);
    GLine_Insert(&Graph, i, &line);
  }

  for (i = BINARY_VECTOR_SAMPLE_SIZE_ - 1; i >= (BINARY_VECTOR_SAMPLE_SIZE_ / 4 ) * 3; i--) {
    bit = (sequence[i / 64] >> (63 - (i % 64))) & 0x1;

    GLine_Fill(&line, bit, VAR_IGNORE, VAR_IGNORE);
    GLine_Insert(&Graph, BINARY_VECTOR_SAMPLE_SIZE_ / 4, &line);
  }

  for (i = (BINARY_VECTOR_SAMPLE_SIZE_ / 2) - 1; i >= BINARY_VECTOR_SAMPLE_SIZE_ / 4; i--) {
    bit = (sequence[i / 64] >> (63 - (i % 64))) & 0x1;

    GLine_Fill(&line, bit, VAR_IGNORE, VAR_IGNORE);
    GLine_Insert(&Graph, BINARY_VECTOR_SAMPLE_SIZE_ / 4, &line);
  }

  for (i = BINARY_VECTOR_SAMPLE_SIZE_ / 2; i < (BINARY_VECTOR_SAMPLE_SIZE_ / 4 ) * 3; i++) {
    bit = (sequence[i / 64] >> (63 - (i % 64))) & 0x1;

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

TEST(StructBinaryVector, CorrectCleverNodeSplit) {
  int8_t bit;
  int32_t i;

  for (i = BINARY_VECTOR_SAMPLE_SIZE_ - 1; i >= 0; i--) {
    bit = (sequence[i / 64] >> (63 - (i % 64))) & 0x1;

    GLine_Fill(&line, bit, VAR_IGNORE, VAR_IGNORE);
    GLine_Insert(&Graph, 0, &line);
  }

  test_node_split_(Graph.root_);
}
#endif

TEST_GROUP_RUNNER(StructBinaryVector) {
  RUN_TEST_CASE(StructBinaryVector, CorrectFrontInsertion);
  RUN_TEST_CASE(StructBinaryVector, CorrectRearInsertion);
  RUN_TEST_CASE(StructBinaryVector, CorrectFrontRearInsertion);
  RUN_TEST_CASE(StructBinaryVector, CorrectRearFrontInsertion);
  RUN_TEST_CASE(StructBinaryVector, CorrectMixedInsertion);

#ifdef ENABLE_CLEVER_NODE_SPLIT
  RUN_TEST_CASE(StructBinaryVector, CorrectCleverNodeSplit);
#endif
}
