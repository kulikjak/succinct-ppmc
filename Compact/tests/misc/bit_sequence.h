/*
 * Implementation of very simple and inefficient Rank & Select structure.
 *
 * Only for testing purposes (RaS complex tree structures).
 * Time complexity of both rank and select operations is O(n).
 */

#ifndef _BIT_SEQUENCE__
#define _BIT_SEQUENCE__

#include <limits.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/*
 * Generate bit sequence.
 *
 * @param  ssize__  Length of the sequence.
 *
 * @return  Newly created bit sequence (64 bit integer array).
 */
uint64_t* init_random_bin_sequence(int32_t ssize__) {
  int32_t i;

  // allocate space for new bit sequence
  int32_t limit = (int)ceil(ssize__ / 64.0);
  uint64_t* sequence = (uint64_t*)calloc(limit, sizeof(uint64_t));

  // seed random number generator
  srand(time(NULL));

  // fill sequence with random bits
  for (i = 0; i < ssize__; i++) {
    if (rand() % 2) {
      sequence[i / 64] |= ((uint64_t)0x1) << (63 - (i % 64));
    }
  }
  return sequence;
}

void print_bit_sequence(uint64_t* seq__, int32_t len__) {
  int32_t i;

  for (i = 0; i < len__; i++)
    printf("%ld ", (seq__[i / 64] >> (63 - (i % 64))) & 0x1);
  printf("\n");
}

void free_bit_sequence(uint64_t** seq__) {
  free(*seq__);
  seq__ = NULL;
}

/*
 * Rank query bit sequence.
 *
 * @param  seq__  Bit sequence.
 * @param  len__  Length of the sequence.
 * @param  x__  Query position.
 *
 * @return  Result of rank query.
 */
int32_t rank_bit_sequence(uint64_t* seq__, int32_t len__, int32_t x__) {
  int32_t i, limit, rest;
  int32_t rank = 0;

  // check that x__ is not higheer then len of sequence
  x__ = (x__ > len__) ? len__ : x__;

  // get rank from whole integers
  limit = x__ / 64;
  for (i = 0; i < limit; i++) {
    rank += __builtin_popcountl(seq__[i]);
  }

  // get rank from smaller integer
  rest = 63 - (x__ % 64);
  for (i = 63; i > rest; i--) {
    rank += (seq__[limit] >> i) & 0x1;
  }

  return rank;
}

/*
 * Rank zero query bit sequence.
 *
 * @param  seq__  Bit sequence.
 * @param  len__  Length of the sequence.
 * @param  x__  Query position.
 *
 * @return  Result of rank zero query.
 */
int32_t rank0_bit_sequence(uint64_t* seq__, int32_t len__, int32_t x__) {
  int32_t i, limit, rest;
  int32_t rank = 0;

  // check that x__ is not higheer then len of sequence
  x__ = (x__ > len__) ? len__ : x__;

  // get rank from whole integers
  limit = x__ / 64;
  for (i = 0; i < limit; i++) {
    rank += 64 - __builtin_popcountl(seq__[i]);
  }

  // get rank from smaller integer
  rest = 63 - (x__ % 64);
  for (i = 63; i > rest; i--) {
    rank += ((seq__[limit] >> i) & 0x1) ? 0 : 1;
  }

  return rank;
}

/*
 * Get bit from given position.
 *
 * @param  seq__  Symbol sequence.
 * @param  len__  Length of the sequence.
 * @param  pos__  Query position.
 */
int32_t get_bit_sequence(uint64_t* seq__, int32_t len__, int32_t pos__) {
  if (pos__ < 0 || pos__ >= len__) return -1;

  return (seq__[pos__ / 64] >> (63 - (pos__ % 64))) & 0x1;
}

/*
 * Select query bit sequence.
 *
 * @param  seq__  Bit sequence.
 * @param  len__  Length of the sequence.
 * @param  x__  Query position.
 *
 * @return  Result of select query.
 */
int32_t select_bit_sequence(uint64_t* seq__, int32_t len__, int32_t x__) {
  int32_t i, limit, pos;
  int32_t rank = 0;

  limit = (int)((len__ - 1) / 64);
  // run through whole integers
  for (i = 0; i <= limit; i++) {
    rank = __builtin_popcountl(seq__[i]);
    if (rank >= x__) {
      // find correct position in last integer
      for (pos = 0; x__; pos++) {
        x__ -= (seq__[i] >> (63 - pos)) & 0x1;
      }
      return i * 64 + pos;
    }
    x__ -= rank;
  }
  // if there is not enough 1s, return -1
  return -1;
}

/*
 * Select zero query bit sequence.
 *
 * @param  seq__  Bit sequence.
 * @param  len__  Length of the sequence.
 * @param  x__  Query position.
 *
 * @return  Result of select zero query.
 */
int32_t select0_bit_sequence(uint64_t* seq__, int32_t len__, int32_t x__) {
  int32_t i, limit, pos;
  int32_t rank = 0;

  limit = (int)((len__ - 1) / 64);
  // run through whole integers
  for (i = 0; i <= limit; i++) {
    rank = 64 - __builtin_popcountl(seq__[i]);
    if (rank >= x__) {
      // find correct position in last integer
      if (i < limit)
        limit = INT_MAX;
      else
        limit = len__ % 64;
      for (pos = 0; x__; pos++) {
        x__ -= ((seq__[i] >> (63 - pos)) & 0x1) ? 0 : 1;
        if (pos >= limit) return -1;
      }
      return i * 64 + pos;
    }
    x__ -= rank;
  }
  // if there is not enough 1s, return -1
  return -1;
}

#endif  // _BIT_SEQUENCE__
