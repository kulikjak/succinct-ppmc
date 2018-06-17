#include <stdio.h>
#include <stdlib.h>

#include "variable.h"

#include "../misc/char_sequence.h"

#define SEQENCE_LEN 100
#define PRINT_SEQUENCES false

int32_t dna_map(uchar symb__) {
  switch (symb__) {
    case 'A':
      return 0;
    case 'C':
      return 1;
    case 'G':
      return 2;
    case 'T':
      return 3;
  }
  FATAL("Unknown symbol");
  return -1;
}

uchar dna_rmap(int32_t num__) {
  switch (num__) {
    case 0:
      return 'A';
    case 1:
      return 'C';
    case 2:
      return 'G';
    case 3:
      return 'T';
  }
  FATAL("Unknown symbol");
  return -1;
}

bool _test_wavelet_tree(WT_Struct* wt, char* sequence) {
  int32_t i;

  if (PRINT_SEQUENCES) {
    print_char_dna_sequence(sequence, SEQENCE_LEN);
    WT_Print_Symbols(wt);
  }

  // test get (correct insertion)
  for (i = 0; i < SEQENCE_LEN; i++) {
    assert(get_char_dna_sequence(sequence, SEQENCE_LEN, i) == WT_Get(wt, i));
  }

  // test rank
  for (i = 0; i <= SEQENCE_LEN; i++) {
    assert(rank_char_dna_sequence(sequence, SEQENCE_LEN, i, 'A') ==
           WT_Rank(wt, i, 'A'));
    assert(rank_char_dna_sequence(sequence, SEQENCE_LEN, i, 'C') ==
           WT_Rank(wt, i, 'C'));
    assert(rank_char_dna_sequence(sequence, SEQENCE_LEN, i, 'G') ==
           WT_Rank(wt, i, 'G'));
    assert(rank_char_dna_sequence(sequence, SEQENCE_LEN, i, 'T') ==
           WT_Rank(wt, i, 'T'));
  }

  // test select
  for (i = 0; i <= SEQENCE_LEN; i++) {
    assert(select_char_dna_sequence(sequence, SEQENCE_LEN, i, 'A') ==
    WT_Select(wt, i, 'A'));
    assert(select_char_dna_sequence(sequence, SEQENCE_LEN, i, 'C') ==
    WT_Select(wt, i, 'C'));
    assert(select_char_dna_sequence(sequence, SEQENCE_LEN, i, 'G') ==
    WT_Select(wt, i, 'G'));
    assert(select_char_dna_sequence(sequence, SEQENCE_LEN, i, 'T') ==
    WT_Select(wt, i, 'T'));
  }

  return true;
}

bool test_front_insert(void) {
  int32_t i;

  WT_Struct wt;
  WT_Init(&wt, 4, dna_map, dna_rmap);

  char* sequence = init_random_char_dna_sequence(SEQENCE_LEN);

  // insert chars into dynamic WT
  for (i = SEQENCE_LEN - 1; i >= 0; i--) {
    char letter = get_char_dna_sequence(sequence, SEQENCE_LEN, i);
    WT_Insert(&wt, 0, letter);
  }

  _test_wavelet_tree(&wt, sequence);

  free_char_dna_sequence(&sequence);
  WT_Free(&wt);

  return true;
}

bool test_rear_insert(void) {
  int32_t i;

  WT_Struct wt;
  WT_Init(&wt, WT_DEFAULT_SIZE, WT_DEFAULT_MAP, WT_DEFAULT_RMAP);

  char* sequence = init_random_char_dna_sequence(SEQENCE_LEN);

  // insert chars into dynamic WT
  for (i = 0; i < SEQENCE_LEN; i++) {
    char letter = get_char_dna_sequence(sequence, SEQENCE_LEN, i);
    WT_Insert(&wt, i, letter);
  }

  _test_wavelet_tree(&wt, sequence);

  free_char_dna_sequence(&sequence);
  WT_Free(&wt);

  return true;
}

bool simple_visual_test_(void) {
  WT_Struct wt;
  WT_Init(&wt, 4, dna_map, dna_rmap);

  WT_Insert(&wt, 0, 'A');
  WT_Print_BitVectors(&wt);

  WT_Insert(&wt, 0, 'C');
  WT_Print_BitVectors(&wt);

  WT_Insert(&wt, 0, 'G');
  WT_Print_BitVectors(&wt);

  WT_Insert(&wt, 0, 'T');
  WT_Print_BitVectors(&wt);

  printf("%c\n", WT_Get(&wt, 0));
  printf("%c\n", WT_Get(&wt, 1));
  printf("%c\n", WT_Get(&wt, 2));
  printf("%c\n", WT_Get(&wt, 3));

  printf("%d\n", WT_Select(&wt, 1, 'A'));
  printf("%d\n", WT_Select(&wt, 1, 'C'));
  printf("%d\n", WT_Select(&wt, 1, 'G'));
  printf("%d\n", WT_Select(&wt, 1, 'T'));

  WT_Free(&wt);

  return true;
}

int main(int argc, char* argv[]) {
  UNUSED(argc);
  UNUSED(argv);

  // simple_visual_test_();
  test_front_insert();
  test_rear_insert();

  printf("All tests successfull\n");

  return EXIT_SUCCESS;
}
