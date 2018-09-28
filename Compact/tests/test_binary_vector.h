#ifndef _TEST_BINARY_VECTOR__
#define _TEST_BINARY_VECTOR__

#include "../../misc/bit_sequence.h"

#define BV_SEQENCE_LEN 100
#define BV_PRINT_SEQUENCES false

void _test_binary_vector(GraphRef Graph__, uint64_t* sequence) {
  int32_t i;
  Graph_Line line;

  if (BV_PRINT_SEQUENCES) {
    print_bit_sequence64(sequence, BV_SEQENCE_LEN);
    Graph_Print(Graph__);
  }

  // test get (correct insertion)
  for (i = 0; i < BV_SEQENCE_LEN; i++) {
    GLine_Get(Graph__, i, &line);
    assert(get_bit_sequence(sequence, BV_SEQENCE_LEN, i) == line.L_);
  }

  // test rank
  for (i = 0; i <= BV_SEQENCE_LEN; i++) {
    assert(rank_bit_sequence(sequence, BV_SEQENCE_LEN, i) == Graph_Rank(Graph__, i, VECTOR_L, VALUE_1));
  }

  // test rank0
  for (i = 0; i <= BV_SEQENCE_LEN; i++) {
    assert(rank0_bit_sequence(sequence, BV_SEQENCE_LEN, i) == Graph_Rank(Graph__, i, VECTOR_L, VALUE_0));
  }

  // test select
  for (i = 0; i <= BV_SEQENCE_LEN; i++) {
    //printf("%d %d\n", select_bit_sequence(sequence, BV_SEQENCE_LEN, i), Graph_Select(Graph__, i, VECTOR_L, VALUE_1));
    assert(select_bit_sequence(sequence, BV_SEQENCE_LEN, i) == Graph_Select(Graph__, i, VECTOR_L, VALUE_1));
  }

  // test select0
  for (i = 0; i <= BV_SEQENCE_LEN; i++) {
    assert(select0_bit_sequence(sequence, BV_SEQENCE_LEN, i) == Graph_Select(Graph__, i, VECTOR_L, VALUE_0));
  }
}

void test_BV_rear_insert(void) {
  int32_t i;

  Graph_Struct Graph;
  Graph_Line line;

  Graph_Init(&Graph);

  uint64_t* sequence = init_random_bin_sequence(BV_SEQENCE_LEN);
  for (i = 0; i < BV_SEQENCE_LEN; i++) {
    int8_t bit = (sequence[i / 64] >> (63 - (i % 64))) & 0x1;

    GLine_Fill(&line, bit, CHAR_IGNORE, VAR_IGNORE);
    GLine_Insert(&Graph, i, &line);
  }
  _test_binary_vector(&Graph, sequence);

  Graph_Free(&Graph);
  free(sequence);
}

void test_BV_front_insert(void) {
  int32_t i;

  Graph_Struct Graph;
  Graph_Line line;

  Graph_Init(&Graph);

  uint64_t* sequence = init_random_bin_sequence(BV_SEQENCE_LEN);
  for (i = BV_SEQENCE_LEN - 1; i >= 0; i--) {
    int8_t bit = (sequence[i / 64] >> (63 - (i % 64))) & 0x1;

    GLine_Fill(&line, bit, CHAR_IGNORE, VAR_IGNORE);
    GLine_Insert(&Graph, 0, &line);
  }
  _test_binary_vector(&Graph, sequence);

  Graph_Free(&Graph);
  free(sequence);
}

#endif   // _TEST_BINARY_VECTOR__
