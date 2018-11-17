#include "structure.h"
#include "unity_fixture.h"

#include "int_sequence.h"

TEST_GROUP(Compressor_wavelet_tree);

#define VAR_IGNORE 1

#ifndef TEST_SEQENCE_LEN
  #define TEST_SEQENCE_LEN 250
#endif

#ifndef TEST_PRINT_SEQUENCES
  #define TEST_PRINT_SEQUENCES false
#endif

uint8_t* sequence;

Graph_Struct Graph;
Graph_Line line;

TEST_SETUP(Compressor_wavelet_tree) {
  sequence = int_sequence_generate_random(TEST_SEQENCE_LEN, 5);
  Graph_Init(&Graph);
}

TEST_TEAR_DOWN(Compressor_wavelet_tree) {
  int_sequence_free(&sequence);
  Graph_Free(&Graph);
}

static void _test_wavelet_tree(GraphRef Graph__, uint8_t* sequence) {
  int32_t i;

  if (TEST_PRINT_SEQUENCES) {
    int_sequence_print(sequence, TEST_SEQENCE_LEN);
    Graph_Print(Graph__);
  }

  for (i = 0; i < TEST_SEQENCE_LEN; i++) {
    GLine_Get(Graph__, i, &line);
    TEST_ASSERT_EQUAL_INT8(int_sequence_get(sequence, TEST_SEQENCE_LEN, i), line.W_);
  }

  for (i = 0; i <= TEST_SEQENCE_LEN; i++) {
    TEST_ASSERT_EQUAL_INT32(int_sequence_rank(sequence, TEST_SEQENCE_LEN, i, VALUE_A),
                            Graph_Rank(Graph__, i, VECTOR_W, VALUE_A));
    TEST_ASSERT_EQUAL_INT32(int_sequence_rank(sequence, TEST_SEQENCE_LEN, i, VALUE_C),
                            Graph_Rank(Graph__, i, VECTOR_W, VALUE_C));
    TEST_ASSERT_EQUAL_INT32(int_sequence_rank(sequence, TEST_SEQENCE_LEN, i, VALUE_G),
                            Graph_Rank(Graph__, i, VECTOR_W, VALUE_G));
    TEST_ASSERT_EQUAL_INT32(int_sequence_rank(sequence, TEST_SEQENCE_LEN, i, VALUE_T),
                            Graph_Rank(Graph__, i, VECTOR_W, VALUE_T));
    TEST_ASSERT_EQUAL_INT32(int_sequence_rank(sequence, TEST_SEQENCE_LEN, i, VALUE_$),
                            Graph_Rank(Graph__, i, VECTOR_W, VALUE_$));

    TEST_ASSERT_EQUAL_INT32(int_sequence_select(sequence, TEST_SEQENCE_LEN, i, VALUE_A),
                            Graph_Select(Graph__, i, VECTOR_W, VALUE_A));
    TEST_ASSERT_EQUAL_INT32(int_sequence_select(sequence, TEST_SEQENCE_LEN, i, VALUE_C),
                            Graph_Select(Graph__, i, VECTOR_W, VALUE_C));
    TEST_ASSERT_EQUAL_INT32(int_sequence_select(sequence, TEST_SEQENCE_LEN, i, VALUE_G),
                            Graph_Select(Graph__, i, VECTOR_W, VALUE_G));
    TEST_ASSERT_EQUAL_INT32(int_sequence_select(sequence, TEST_SEQENCE_LEN, i, VALUE_T),
                            Graph_Select(Graph__, i, VECTOR_W, VALUE_T));
    TEST_ASSERT_EQUAL_INT32(int_sequence_select(sequence, TEST_SEQENCE_LEN, i, VALUE_$),
                            Graph_Select(Graph__, i, VECTOR_W, VALUE_$));
  }
}

TEST(Compressor_wavelet_tree, front_insertion) {
  int32_t i;

  for (i = TEST_SEQENCE_LEN - 1; i >= 0; i--) {
    uint8_t letter = int_sequence_get(sequence, TEST_SEQENCE_LEN, i);

    GLine_Fill(&line, VAR_IGNORE, letter, VAR_IGNORE);
    GLine_Insert(&Graph, 0, &line);
  }
  _test_wavelet_tree(&Graph, sequence);
}

TEST(Compressor_wavelet_tree, rear_insertion) {
  int32_t i;

  for (i = 0; i < TEST_SEQENCE_LEN; i++) {
    uint8_t letter = int_sequence_get(sequence, TEST_SEQENCE_LEN, i);

    GLine_Fill(&line, VAR_IGNORE, letter, VAR_IGNORE);
    GLine_Insert(&Graph, i, &line);
  }
  _test_wavelet_tree(&Graph, sequence);
}

TEST(Compressor_wavelet_tree, symbol_change) {
  int32_t i;

  for (i = 0; i < TEST_SEQENCE_LEN; i++) {
    uint8_t letter = int_sequence_get(sequence, TEST_SEQENCE_LEN, i);

    GLine_Fill(&line, VAR_IGNORE, letter, VAR_IGNORE);
    GLine_Insert(&Graph, i, &line);
  }

  srand(time(NULL));
  for (i = 0; i < TEST_SEQENCE_LEN; i++) {
    switch (rand() % 5) {
      case 0:
        sequence[i] = VALUE_A;
        Graph_Change_symbol(&Graph, i, VALUE_A);
        break;
      case 1:
        sequence[i] = VALUE_C;
        Graph_Change_symbol(&Graph, i, VALUE_C);
        break;
      case 2:
        sequence[i] = VALUE_G;
        Graph_Change_symbol(&Graph, i, VALUE_G);
        break;
      case 3:
        sequence[i] = VALUE_T;
        Graph_Change_symbol(&Graph, i, VALUE_T);
        break;
      case 4:
        sequence[i] = VALUE_$;
        Graph_Change_symbol(&Graph, i, VALUE_$);
        break;
    }
  }
  _test_wavelet_tree(&Graph, sequence);
}

TEST_GROUP_RUNNER(Compressor_wavelet_tree) {
  RUN_TEST_CASE(Compressor_wavelet_tree, front_insertion);
  RUN_TEST_CASE(Compressor_wavelet_tree, rear_insertion);
  RUN_TEST_CASE(Compressor_wavelet_tree, symbol_change);
}
