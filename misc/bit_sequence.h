/*
 * Implementation of very simple and inefficient dynamic Rank & Select structure.
 *
 * Only for testing purposes.
 * Time complexity of both rank and select is O(n).
 */

#ifndef _BIT_SEQUENCE__
#define _BIT_SEQUENCE__

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define BitSeqType uint64_t
#define BitSeqSize 64

static BitSeqType* bit_sequence_generate_random(int32_t ssize__) {
  int32_t i, bit;

  int32_t limit = (int)ceil(ssize__ / (double)BitSeqSize);
  BitSeqType* sequence = (BitSeqType*)calloc(limit, sizeof(BitSeqType));

  srand(time(NULL));
  for (i = 0; i < ssize__; i++) {
    if (rand() % 2) {
      bit = ((BitSeqType)0x1) << ((BitSeqSize - 1) - (i % BitSeqSize));
      sequence[i / BitSeqSize] |= bit;
    }
  }
  return sequence;
}

static void bit_sequence_print(BitSeqType* seq__, int32_t len__) {
  int32_t i;

  for (i = 0; i < len__; i++)
    printf("%ld ", (seq__[i / BitSeqSize] >> ((BitSeqSize - 1) - (i % BitSeqSize))) & 0x1);
  printf("\n");
}

static void bit_sequence_free(BitSeqType** seq__) {
  free(*seq__);
  seq__ = NULL;
}

/*
 * Bit sequence get operation.
 *
 * @param  seq__  Symbol sequence.
 * @param  len__  Length of the sequence.
 * @param  pos__  Query position.
 */
static int32_t bit_sequence_get(BitSeqType* seq__, int32_t len__, int32_t pos__) {
  if (pos__ < 0 || pos__ >= len__) return -1;

  return (seq__[pos__ / BitSeqSize] >> ((BitSeqSize - 1) - (pos__ % BitSeqSize))) & 0x1;
}


/*
 * Bit sequence rank operation.
 *
 * @param  seq__  Symbol sequence.
 * @param  len__  Length of the sequence.
 * @param  x__  Query position.
 */
static int32_t bit_sequence_rank(BitSeqType* seq__, int32_t len__, int32_t x__) {
  int32_t i, limit, rest;
  int32_t rank = 0;

  x__ = (x__ > len__) ? len__ : x__;

  limit = x__ / BitSeqSize;
  for (i = 0; i < limit; i++) {
    rank += __builtin_popcountl(seq__[i]);
  }

  rest = (BitSeqSize - 1) - (x__ % BitSeqSize);
  for (i = BitSeqSize-1; i > rest; i--) {
    rank += (seq__[limit] >> i) & 0x1;
  }

  return rank;
}

static int32_t bit_sequence_rank0(BitSeqType* seq__, int32_t len__, int32_t x__) {
  int32_t i, limit, rest;
  int32_t rank = 0;

  x__ = (x__ > len__) ? len__ : x__;

  limit = x__ / BitSeqSize;
  for (i = 0; i < limit; i++) {
    rank += BitSeqSize - __builtin_popcountl(seq__[i]);
  }

  rest = (BitSeqSize - 1) - (x__ % BitSeqSize);
  for (i = (BitSeqSize - 1); i > rest; i--) {
    rank += ((seq__[limit] >> i) & 0x1) ? 0 : 1;
  }

  return rank;
}

/*
 * Bit sequence select operation.
 *
 * @param  seq__  Symbol sequence.
 * @param  len__  Length of the sequence.
 * @param  x__  Query position.
 * @param  ch__  Query symbol.
 */
static int32_t bit_sequence_select(BitSeqType* seq__, int32_t len__, int32_t x__) {
  int32_t i, limit, pos;
  int32_t rank = 0;

  limit = (int)((len__ - 1) / BitSeqSize);
  for (i = 0; i <= limit; i++) {
    rank = __builtin_popcountl(seq__[i]);
    if (rank >= x__) {
      for (pos = 0; x__; pos++) {
        x__ -= (seq__[i] >> ((BitSeqSize - 1) - pos)) & 0x1;
      }
      return i * BitSeqSize + pos;
    }
    x__ -= rank;
  }
  /* if there is not enough 1s, return -1 */
  return -1;
}

static int32_t bit_sequence_select0(BitSeqType* seq__, int32_t len__, int32_t x__) {
  int32_t i, limit, pos;
  int32_t rank = 0;

  limit = (int)((len__ - 1) / BitSeqSize);
  for (i = 0; i <= limit; i++) {
    rank = BitSeqSize - __builtin_popcountl(seq__[i]);
    if (rank >= x__) {
      if (i < limit)
        limit = INT_MAX;
      else
        limit = len__ % BitSeqSize;
      for (pos = 0; x__; pos++) {
        x__ -= ((seq__[i] >> ((BitSeqSize - 1) - pos)) & 0x1) ? 0 : 1;
        if (pos >= limit) return -1;
      }
      return i * BitSeqSize + pos;
    }
    x__ -= rank;
  }
  /* if there is not enough 1s, return -1 */
  return -1;
}

#endif
