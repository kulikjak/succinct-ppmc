#ifndef _TEST_WAVELET_TREE__
#define _TEST_WAVELET_TREE__

#include "../../misc/char_sequence.h"

#define WT_SEQENCE_LEN 32
#define WT_PRINT_SEQUENCES false

void _test_wavelet_tree(GraphRef Graph__, char* sequence) {
  int32_t i;
  Graph_Line line;

  if (WT_PRINT_SEQUENCES) {
    print_char_sequence(sequence, WT_SEQENCE_LEN);
    Graph_Print(Graph__);
  }

  // test get (correct insertion)
  for (i = 0; i < WT_SEQENCE_LEN; i++) {
    GLine_Get(Graph__, i, &line);
    assert(get_char_sequence(sequence, WT_SEQENCE_LEN, i) == line.W_);
  }

  Graph_Print(Graph__);

  // test rank
  for (i = 0; i <= WT_SEQENCE_LEN; i++) {
    assert(rank_char_sequence(sequence, WT_SEQENCE_LEN, i, 'A') == Graph_Rank(Graph__, i, VECTOR_W, VALUE_A));
    assert(rank_char_sequence(sequence, WT_SEQENCE_LEN, i, 'C') == Graph_Rank(Graph__, i, VECTOR_W, VALUE_C));
    assert(rank_char_sequence(sequence, WT_SEQENCE_LEN, i, 'G') == Graph_Rank(Graph__, i, VECTOR_W, VALUE_G));
    assert(rank_char_sequence(sequence, WT_SEQENCE_LEN, i, 'T') == Graph_Rank(Graph__, i, VECTOR_W, VALUE_T));
    assert(rank_char_sequence(sequence, WT_SEQENCE_LEN, i, '$') == Graph_Rank(Graph__, i, VECTOR_W, VALUE_$));
  }
  /*printf("------------------------------------\n");
  for (i = 0; i <= WT_SEQENCE_LEN; i++) {
    printf("%d %d\n", rank_char_sequence(sequence, WT_SEQENCE_LEN, i, 'C'), Graph_Rank(Graph__, i, VECTOR_W, VALUE_C));
  }
  printf("------------------------------------\n");
  for (i = 0; i <= WT_SEQENCE_LEN; i++) {
    printf("%d %d\n", rank_char_sequence(sequence, WT_SEQENCE_LEN, i, 'G'), Graph_Rank(Graph__, i, VECTOR_W, VALUE_G));
  }
  printf("------------------------------------\n");
  for (i = 0; i <= WT_SEQENCE_LEN; i++) {
    printf("%d %d\n", rank_char_sequence(sequence, WT_SEQENCE_LEN, i, 'T'), Graph_Rank(Graph__, i, VECTOR_W, VALUE_T));
  }
  printf("------------------------------------\n");
  for (i = 0; i <= WT_SEQENCE_LEN; i++) {
    printf("%d %d\n", rank_char_sequence(sequence, WT_SEQENCE_LEN, i, '$'), Graph_Rank(Graph__, i, VECTOR_W, VALUE_$));
  }*/

  // test select
  /*for (i = 0; i <= WT_SEQENCE_LEN; i++) {
    assert(select_char_sequence(sequence, WT_SEQENCE_LEN, i, 'A') == WT_Select(wt, i, 'A'));
    assert(select_char_sequence(sequence, WT_SEQENCE_LEN, i, 'C') == WT_Select(wt, i, 'C'));
    assert(select_char_sequence(sequence, WT_SEQENCE_LEN, i, 'G') == WT_Select(wt, i, 'G'));
    assert(select_char_sequence(sequence, WT_SEQENCE_LEN, i, 'T') == WT_Select(wt, i, 'T'));
    assert(select_char_sequence(sequence, WT_SEQENCE_LEN, i, '$') == WT_Select(wt, i, '$'));
  }*/
}

void test_WT_front_insert(void) {
  int32_t i;
  char *sequence, letter;

	Graph_Struct Graph;
  Graph_Line line;

  Graph_Init(&Graph);

  sequence = init_random_char_extdna_sequence(WT_SEQENCE_LEN);
  for (i = WT_SEQENCE_LEN - 1; i >= 0; i--) {
    letter = get_char_sequence(sequence, WT_SEQENCE_LEN, i);

	  GLine_Fill(&line, VAR_IGNORE, letter, VAR_IGNORE);
	  GLine_Insert(&Graph, 0, &line);
  }
  _test_wavelet_tree(&Graph, sequence);

  free_char_sequence(&sequence);
  Graph_Free(&Graph);
}

void test_WT_rear_insert(void) {
  int32_t i;
  char *sequence, letter;

	Graph_Struct Graph;
  Graph_Line line;

  Graph_Init(&Graph);

  sequence = init_random_char_extdna_sequence(WT_SEQENCE_LEN);
  for (i = 0; i < WT_SEQENCE_LEN; i++) {
    letter = get_char_sequence(sequence, WT_SEQENCE_LEN, i);

	  GLine_Fill(&line, VAR_IGNORE, letter, VAR_IGNORE);
	  GLine_Insert(&Graph, i, &line);
  }
  _test_wavelet_tree(&Graph, sequence);

  free_char_sequence(&sequence);
  Graph_Free(&Graph);
}

#endif  // _TEST_WAVELET_TREE__
