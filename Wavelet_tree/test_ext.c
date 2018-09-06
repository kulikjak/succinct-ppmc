#include <stdio.h>
#include <stdlib.h>

#include "structure_ext.h"

#include "../misc/char_sequence.h"
#include "../shared/utils.h"

#define SEQENCE_LEN 100
#define PRINT_SEQUENCES true


bool _test_wavelet_tree(WT_Struct* wt, char* sequence) {
  int32_t i;

  if (PRINT_SEQUENCES) {
    print_char_sequence(sequence, SEQENCE_LEN);
    WT_Print_Symbols(wt);
  }

  // test get (correct insertion)
  for (i = 0; i < SEQENCE_LEN; i++) {
    assert(get_char_sequence(sequence, SEQENCE_LEN, i) == WT_Get(wt, i));
  }

  // test rank
  for (i = 0; i <= SEQENCE_LEN; i++) {
    assert(rank_char_sequence(sequence, SEQENCE_LEN, i, 'A') ==
           WT_Rank(wt, i, 'A'));
    assert(rank_char_sequence(sequence, SEQENCE_LEN, i, 'C') ==
           WT_Rank(wt, i, 'C'));
    assert(rank_char_sequence(sequence, SEQENCE_LEN, i, 'G') ==
           WT_Rank(wt, i, 'G'));
    assert(rank_char_sequence(sequence, SEQENCE_LEN, i, 'T') ==
           WT_Rank(wt, i, 'T'));
    assert(rank_char_sequence(sequence, SEQENCE_LEN, i, '$') ==
           WT_Rank(wt, i, '$'));
  }

  // test select
  for (i = 0; i <= SEQENCE_LEN; i++) {
    assert(select_char_sequence(sequence, SEQENCE_LEN, i, 'A') ==
           WT_Select(wt, i, 'A'));
    assert(select_char_sequence(sequence, SEQENCE_LEN, i, 'C') ==
           WT_Select(wt, i, 'C'));
    assert(select_char_sequence(sequence, SEQENCE_LEN, i, 'G') ==
           WT_Select(wt, i, 'G'));
    assert(select_char_sequence(sequence, SEQENCE_LEN, i, 'T') ==
           WT_Select(wt, i, 'T'));
    assert(select_char_sequence(sequence, SEQENCE_LEN, i, '$') ==
           WT_Select(wt, i, '$'));
  }

  return true;
}

bool test_front_insert(void) {
  int32_t i;

  WT_Struct wt;
  WT_Init(&wt);

  char* sequence = init_random_char_extdna_sequence(SEQENCE_LEN);

  // insert chars into dynamic WT
  for (i = SEQENCE_LEN - 1; i >= 0; i--) {
    char letter = get_char_sequence(sequence, SEQENCE_LEN, i);
    WT_Insert(&wt, 0, letter);
  }

  _test_wavelet_tree(&wt, sequence);

  free_char_sequence(&sequence);
  WT_Free(&wt);

  return true;
}

bool test_rear_insert(void) {
  int32_t i;

  WT_Struct wt;
  WT_Init(&wt);

  char* sequence = init_random_char_extdna_sequence(SEQENCE_LEN);

  // insert chars into dynamic WT
  for (i = 0; i < SEQENCE_LEN; i++) {
    char letter = get_char_sequence(sequence, SEQENCE_LEN, i);
    WT_Insert(&wt, i, letter);
  }

  _test_wavelet_tree(&wt, sequence);

  free_char_sequence(&sequence);
  WT_Free(&wt);

  return true;
}

bool test_deletion(void) {
  int32_t i;

  WT_Struct wt;
  WT_Init(&wt);

  char* sequence = init_random_char_extdna_sequence(SEQENCE_LEN);

  int32_t idx = 0;
  int32_t added = 0;
  int32_t pos[SEQENCE_LEN];

  // insert chars into dynamic WT
  for (i = 0; i < SEQENCE_LEN; i++) {
    char letter = get_char_sequence(sequence, SEQENCE_LEN, i);

    if (letter == 'A' || letter == 'T') {
      WT_Insert(&wt, i + added, letter);
      pos[idx++] = i + added++;
    }
    WT_Insert(&wt, i + added, letter);
  }

  // delete all additional symbols and check the structure
  for (i = added - 1; i >= 0; i--)
    WT_Delete(&wt, pos[i]);

  _test_wavelet_tree(&wt, sequence);

  free_char_sequence(&sequence);
  WT_Free(&wt);

  return true;
}

bool test_symbol_change(void) {
  int32_t i;
  char letter;

  WT_Struct wt;
  WT_Init(&wt);

  char* seq_one = init_random_char_extdna_sequence(SEQENCE_LEN);
  char* seq_two = init_random_char_extdna_sequence(SEQENCE_LEN);

  // insert chars into dynamic WT
  for (i = 0; i < SEQENCE_LEN; i++) {
    letter = get_char_sequence(seq_one, SEQENCE_LEN, i);
    WT_Insert(&wt, i, letter);
  }

  _test_wavelet_tree(&wt, seq_one);

  // exchange symbols from first sequence with second sequence symbols
  for (i = 0; i < SEQENCE_LEN; i++) {
    WT_Delete(&wt, i);

    letter = get_char_sequence(seq_two, SEQENCE_LEN, i);
    WT_Insert(&wt, i, letter);
  }

  _test_wavelet_tree(&wt, seq_two);

  free_char_sequence(&seq_one);
  free_char_sequence(&seq_two);
  WT_Free(&wt);

  return true;
}

int main(int argc, char* argv[]) {
  UNUSED(argc);
  UNUSED(argv);

  // seed the random string generator
  srand(time(NULL));

  test_front_insert();
  test_rear_insert();

  test_deletion();
  test_symbol_change();

  printf("All tests successfull\n");

  return EXIT_SUCCESS;
}
