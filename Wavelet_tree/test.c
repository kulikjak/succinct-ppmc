#include <stdio.h>
#include <stdlib.h>

#include "structure.h"

#include "../misc/char_sequence.h"

#define SEQENCE_LEN 100
#define PRINT_SEQUENCES true


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
  WT_Init(&wt);

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
  WT_Init(&wt);

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


int main(int argc, char* argv[]) {
  UNUSED(argc);
  UNUSED(argv);

  test_front_insert();
  test_rear_insert();

  printf("All tests successfull\n");

  return EXIT_SUCCESS;
}
