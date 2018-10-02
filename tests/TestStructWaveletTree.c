#include "structure.h"
#include "unity_fixture.h"

#include "misc/char_sequence.h"

TEST_GROUP(StructWaveletTree);

#define VAR_IGNORE 0
#define WAVELET_TREE_SAMPLE_SIZE_ 250
#define WAVELET_TREE_PRINT_SEQUENCES false

char* sequence;

Graph_Struct Graph;
Graph_Line line;

TEST_SETUP(StructWaveletTree) {
  Graph_Init(&Graph);
  sequence = init_random_char_extdna_sequence(WAVELET_TREE_SAMPLE_SIZE_);
}

TEST_TEAR_DOWN(StructWaveletTree) {
  free_char_sequence(&sequence);
  Graph_Free(&Graph);
}

char char_to_graph_value_(char symb__) {
  switch (symb__) {
    case 'A':
      return VALUE_A;
    case 'C':
      return VALUE_C;
    case 'G':
      return VALUE_G;
    case 'T':
      return VALUE_T;
    case '$':
      return VALUE_$;
  }
  FATAL("Unexpected mask");
  return 0;
}
char graph_value_to_char_(Graph_value mask__) {
  switch (mask__) {
    case VALUE_A:
      return 'A';
    case VALUE_C:
      return 'C';
    case VALUE_G:
      return 'G';
    case VALUE_T:
      return 'T';
    case VALUE_$:
      return '$';
  }
  FATAL("Unexpected mask");
  return 0;
}


void _test_wavelet_tree(GraphRef Graph__, char* sequence) {
  int32_t i;

  if (WAVELET_TREE_PRINT_SEQUENCES) {
    print_char_sequence(sequence, WAVELET_TREE_SAMPLE_SIZE_);
    Graph_Print(Graph__);
  }

  for (i = 0; i < WAVELET_TREE_SAMPLE_SIZE_; i++) {
    GLine_Get(Graph__, i, &line);
    TEST_ASSERT_EQUAL_INT8(
        get_char_sequence(sequence, WAVELET_TREE_SAMPLE_SIZE_, i), graph_value_to_char_(line.W_));
  }

  for (i = 0; i <= WAVELET_TREE_SAMPLE_SIZE_; i++) {
    TEST_ASSERT_EQUAL_INT32(
        rank_char_sequence(sequence, WAVELET_TREE_SAMPLE_SIZE_, i, 'A'),
        Graph_Rank(Graph__, i, VECTOR_W, VALUE_A));
    TEST_ASSERT_EQUAL_INT32(
        rank_char_sequence(sequence, WAVELET_TREE_SAMPLE_SIZE_, i, 'C'),
        Graph_Rank(Graph__, i, VECTOR_W, VALUE_C));
    TEST_ASSERT_EQUAL_INT32(
        rank_char_sequence(sequence, WAVELET_TREE_SAMPLE_SIZE_, i, 'G'),
        Graph_Rank(Graph__, i, VECTOR_W, VALUE_G));
    TEST_ASSERT_EQUAL_INT32(
        rank_char_sequence(sequence, WAVELET_TREE_SAMPLE_SIZE_, i, 'T'),
        Graph_Rank(Graph__, i, VECTOR_W, VALUE_T));
    TEST_ASSERT_EQUAL_INT32(
        rank_char_sequence(sequence, WAVELET_TREE_SAMPLE_SIZE_, i, '$'),
        Graph_Rank(Graph__, i, VECTOR_W, VALUE_$));

    TEST_ASSERT_EQUAL_INT32(
        select_char_sequence(sequence, WAVELET_TREE_SAMPLE_SIZE_, i, 'A'),
        Graph_Select(Graph__, i, VECTOR_W, VALUE_A));
    TEST_ASSERT_EQUAL_INT32(
        select_char_sequence(sequence, WAVELET_TREE_SAMPLE_SIZE_, i, 'C'),
        Graph_Select(Graph__, i, VECTOR_W, VALUE_C));
    TEST_ASSERT_EQUAL_INT32(
        select_char_sequence(sequence, WAVELET_TREE_SAMPLE_SIZE_, i, 'G'),
        Graph_Select(Graph__, i, VECTOR_W, VALUE_G));
    TEST_ASSERT_EQUAL_INT32(
        select_char_sequence(sequence, WAVELET_TREE_SAMPLE_SIZE_, i, 'T'),
        Graph_Select(Graph__, i, VECTOR_W, VALUE_T));
    TEST_ASSERT_EQUAL_INT32(
        select_char_sequence(sequence, WAVELET_TREE_SAMPLE_SIZE_, i, '$'),
        Graph_Select(Graph__, i, VECTOR_W, VALUE_$));
  }
}

TEST(StructWaveletTree, CorrectFrontInsertion) {
  char letter;
  int32_t i;

  for (i = WAVELET_TREE_SAMPLE_SIZE_ - 1; i >= 0; i--) {
    letter = get_char_sequence(sequence, WAVELET_TREE_SAMPLE_SIZE_, i);

    GLine_Fill(&line, VAR_IGNORE, char_to_graph_value_(letter), VAR_IGNORE);
    GLine_Insert(&Graph, 0, &line);
  }
  _test_wavelet_tree(&Graph, sequence);
}

TEST(StructWaveletTree, CorrectRearInsertion) {
  char letter;
  int32_t i;

  for (i = 0; i < WAVELET_TREE_SAMPLE_SIZE_; i++) {
    letter = get_char_sequence(sequence, WAVELET_TREE_SAMPLE_SIZE_, i);

    GLine_Fill(&line, VAR_IGNORE, char_to_graph_value_(letter), VAR_IGNORE);
    GLine_Insert(&Graph, i, &line);
  }
  _test_wavelet_tree(&Graph, sequence);
}

TEST(StructWaveletTree, CorrectSymbolChanging) {
  char letter;
  int32_t i;

  for (i = 0; i < WAVELET_TREE_SAMPLE_SIZE_; i++) {
    letter = get_char_sequence(sequence, WAVELET_TREE_SAMPLE_SIZE_, i);

    GLine_Fill(&line, VAR_IGNORE, char_to_graph_value_(letter), VAR_IGNORE);
    GLine_Insert(&Graph, i, &line);
  }

  srand(time(NULL));
  for (i = 0; i < WAVELET_TREE_SAMPLE_SIZE_; i++) {
    switch(rand() % 5) {
      case 0:
        sequence[i] = 'A';
        Graph_Change_symbol(&Graph, i, VALUE_A);
        break;
      case 1:
        sequence[i] = 'C';
        Graph_Change_symbol(&Graph, i, VALUE_C);
        break;
      case 2:
        sequence[i] = 'G';
        Graph_Change_symbol(&Graph, i, VALUE_G);
        break;
      case 3:
        sequence[i] = 'T';
        Graph_Change_symbol(&Graph, i, VALUE_T);
        break;
      case 4:
        sequence[i] = '$';
        Graph_Change_symbol(&Graph, i, VALUE_$);
        break;
    }
  }
  _test_wavelet_tree(&Graph, sequence);
}

TEST_GROUP_RUNNER(StructWaveletTree) {
  RUN_TEST_CASE(StructWaveletTree, CorrectFrontInsertion);
  RUN_TEST_CASE(StructWaveletTree, CorrectRearInsertion);
  RUN_TEST_CASE(StructWaveletTree, CorrectSymbolChanging);
}
