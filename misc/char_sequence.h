/*
 * Implementation of very simple and inefficient symbol sequence structure.
 *
 * Only for testing purposes (Wavelet tree structures).
 * Time complexity of both rank and select is O(n).
 */

#ifndef _CHAR_SEQUENCE__
#define _CHAR_SEQUENCE__

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/* whether generated sequences should be random or seeded with static value */
#define SEQUENCE_RANDOM_

/*
 * Generate symbol sequence.
 *
 * @param  ssize__  Length of the sequence.
 * @param  dict__  Dictionary with possible sequence symbols.
 * @param  dsize__  Size of the dictionary.
 *
 * @return  Newly created symbol sequence.
 */
char* init_random_char_sequence_aux_(int32_t ssize__, char* dict__, int32_t dsize__) {
  int32_t i, ri;

  char* sequence = (char*)malloc(ssize__ * sizeof(char));

#ifdef SEQUENCE_RANDOM_
  srand(time(NULL));
#else
  srand(0);
#endif

  for (i = 0; i < ssize__; i++) {
    ri = rand() % dsize__;
    sequence[i] = dict__[ri];
  }
  return sequence;
}

/*
 * Generate sequence with dna symbols (A, C, G, T).
 *
 * @param  ssize__  Length of the sequence.
 *
 * @return  Newly created symbol sequence.
 */
char* init_random_char_dna_sequence(int32_t ssize__) {
  char dict[4] = {'A', 'C', 'G', 'T'};
  return init_random_char_sequence_aux_(ssize__, dict, 4);
}

/*
 * Generate sequence with dna symbols (A, C, G, T) and dolar symbol.
 *
 * These symbols are used for deBuijn graphs.
 *
 * @param  ssize__  Length of the sequence.
 *
 * @return  Newly created symbol sequence.
 */
char* init_random_char_extdna_sequence(int32_t ssize__) {
  char dict[5] = {'A', 'C', 'G', 'T', '$'};
  return init_random_char_sequence_aux_(ssize__, dict, 5);
}

/*
 * Generate sequence with full ASCII (256 values).
 *
 * @param  ssize__  Length of the sequence.
 *
 * @return  Newly created symbol sequence.
 */
char* init_random_char_sequence(int32_t ssize__) {
  int32_t i;
  char dict[256];

  for (i = 0; i < 256; i++) dict[i] = (char)i;

  return init_random_char_sequence_aux_(ssize__, dict, 256);
}

void print_char_sequence(char* seq__, int32_t len__) {
  int32_t i;

  for (i = 0; i < len__; i++)
    printf("%c ", seq__[i]);
  printf("\n");
}

void free_char_sequence(char** seq__) {
  free(*seq__);
  seq__ = NULL;
}

/*
 * Rank query symbol sequence.
 *
 * @param  seq__  Symbol sequence.
 * @param  len__  Length of the sequence.
 * @param  x__  Query position.
 * @param  ch__  Query symbol.
 *
 * @return  Result of rank query.
 */
int32_t rank_char_sequence(char* seq__, int32_t len__, int32_t x__, char ch__) {
  int32_t i, rank = 0;

  // check that x__ is not higheer then len of sequence
  x__ = (x__ > len__) ? len__ : x__;
  for (i = 0; i < x__; i++) {
    rank += (seq__[i] == ch__) ? 1 : 0;
  }

  return rank;
}

/*
 * Get symbol from given position.
 * 
 * @param  seq__  Symbol sequence.
 * @param  len__  Length of the sequence.
 * @param  pos__  Query position.
 */
char get_char_sequence(char* seq__, int32_t len__, int32_t pos__) {
  if (pos__ < 0 || pos__ >= len__) return ' ';

  return seq__[pos__];
}

/*
 * Select query symbol sequence.
 *
 * @param  seq__  Symbol sequence.
 * @param  len__  Length of the sequence.
 * @param  x__  Query position.
 * @param  ch__  Query symbol.
 *
 * @return  Result of select query.
 */
int32_t select_char_sequence(char* seq__, int32_t len__, int32_t x__, char ch__) {
  int32_t i;

  if (!x__) return 0;

  for (i = 0; i < len__; i++) {
    if (seq__[i] == ch__) {
      if (!(--x__)) return i + 1;
    }
  }

  return -1;
}


#endif  // _CHAR_SEQUENCE__
