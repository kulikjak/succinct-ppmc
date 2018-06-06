#include <stdio.h>
#include <stdlib.h>

#include "../char_sequence.h"

int main(int argc, char* argv[]) {
  int32_t len = 100;
  char* sequence = init_random_char_dna_sequence(len);

  print_char_dna_sequence(sequence, len);

  printf("T: %d ", rank_char_dna_sequence(sequence, len, 0, 'T'));
  printf("%d ", rank_char_dna_sequence(sequence, len, 1, 'T'));
  printf("%d ", rank_char_dna_sequence(sequence, len, 2, 'T'));
  printf("%d ", rank_char_dna_sequence(sequence, len, 3, 'T'));
  printf("%d\n", rank_char_dna_sequence(sequence, len, 4, 'T'));

  printf("G: %d ", rank_char_dna_sequence(sequence, len, 0, 'G'));
  printf("%d ", rank_char_dna_sequence(sequence, len, 1, 'G'));
  printf("%d ", rank_char_dna_sequence(sequence, len, 2, 'G'));
  printf("%d\n", rank_char_dna_sequence(sequence, len, 3, 'G'));

  printf("C: %d ", rank_char_dna_sequence(sequence, len, 0, 'C'));
  printf("%d ", rank_char_dna_sequence(sequence, len, 1, 'C'));
  printf("%d ", rank_char_dna_sequence(sequence, len, 2, 'C'));
  printf("%d\n", rank_char_dna_sequence(sequence, len, 3, 'C'));

  printf("%c ", get_char_dna_sequence(sequence, len, 0));
  printf("%c ", get_char_dna_sequence(sequence, len, 1));
  printf("%c ", get_char_dna_sequence(sequence, len, 2));
  printf("%c ", get_char_dna_sequence(sequence, len, 3));
  printf("%c ", get_char_dna_sequence(sequence, len, 4));
  printf("%c ", get_char_dna_sequence(sequence, len, 5));
  printf("%c ", get_char_dna_sequence(sequence, len, 6));
  printf("%c ", get_char_dna_sequence(sequence, len, 7));
  printf("%c ", get_char_dna_sequence(sequence, len, 8));

  printf("%c ", get_char_dna_sequence(sequence, len, 99));
  printf("%c ", get_char_dna_sequence(sequence, len, 100));
  printf("%c ", get_char_dna_sequence(sequence, len, 101));
  printf("\n");

  printf("%d ", select_char_dna_sequence(sequence, len, 0, 'T'));
  printf("%d ", select_char_dna_sequence(sequence, len, 1, 'T'));
  printf("%d ", select_char_dna_sequence(sequence, len, 2, 'T'));
  printf("%d ", select_char_dna_sequence(sequence, len, 3, 'T'));
  printf("%d ", select_char_dna_sequence(sequence, len, 4, 'T'));
  printf("%d ", select_char_dna_sequence(sequence, len, 5, 'T'));
  printf("\n");

  free_char_dna_sequence(&sequence);

  return EXIT_SUCCESS;
}
