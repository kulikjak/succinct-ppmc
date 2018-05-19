/*
Implementation very simple and inefficient Rank and Select structure.

It is used only for testing purposes (correct implementation of each other structure).
With time complexity of O(n) for both rank and select, it should not be used on bigger bit sequences.
*/

#ifndef _BIT_SEQUENCE__
#define _BIT_SEQUENCE__

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include <time.h>


void print_bit_sequence32(uint32_t* seq__, int32_t len__) {
  int32_t i;

  for (i = 0; i < len__; i++) {
    printf("%d ", (seq__[i / 32] >> (31 - (i % 32))) & 0x1);
  }
  printf("\n");
}

void print_bit_sequence64(uint64_t* seq__, int32_t len__) {
  int32_t i;

  for (i = 0; i < len__; i++) {
    printf("%ld ", (seq__[i / 64] >> (63 - (i % 64))) & 0x1);
  }
  printf("\n");
}

void print_bit_sequence(uint64_t* seq__, int32_t len__) {
  print_bit_sequence64(seq__, len__);
}


uint64_t* init_random_bin_sequence(int32_t a__) {

  int32_t i;

  // allocate space for new bit sequence
  int32_t limit = (int)ceil(a__ / 64.0);
  uint64_t* sequence = (uint64_t*) calloc (limit, sizeof(uint64_t));

  // seed random number generator
  srand(time(NULL));

  // fill sequence with random bits
  for (i = 0; i < a__; i++) {
    if (rand() % 2) {
      sequence[i / 64] |= ((uint64_t)0x1) << (63 - (i % 64));
    }
  }

  return sequence;
}

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

int32_t get_bit_sequence(uint64_t* seq__, int32_t len__, int32_t pos__) {
  if (pos__ < 0 || pos__ >= len__)
    return -1;

  return (seq__[pos__ / 64] >> (63 - (pos__ % 64))) & 0x1;
}

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
      return i*64 + pos;
    }
    x__ -= rank;
  }
  // if there is not enough 1s, return -1
  return -1;
}


#endif  // _BIT_SEQUENCE__
