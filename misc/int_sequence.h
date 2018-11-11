/*
 * Implementation of very simple and inefficient integer sequence structure.
 *
 * Only for testing purposes.
 * Time complexity of both rank and select is O(n).
 */

#ifndef _INTEGER_SEQUENCE__
#define _INTEGER_SEQUENCE__

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

uint8_t* int_sequence_generate_random(int32_t ssize__, int32_t dsize__) {
  uint8_t* sequence = (uint8_t*)malloc(ssize__ * sizeof(uint8_t));

  srand(time(NULL));
  for (int32_t i = 0; i < ssize__; i++)
    sequence[i] = rand() % dsize__;
  return sequence;  
}

void int_sequence_free(uint8_t** seq__) {
  free(*seq__);
  seq__ = NULL;
}

void int_sequence_print(uint8_t* seq__, int32_t len__) {
  for (int32_t i = 0; i < len__; i++)
    printf("%d ", seq__[i]);
  printf("\n");
}

/*
 * Integer sequence get operation.
 * 
 * @param  seq__  Symbol sequence.
 * @param  len__  Length of the sequence.
 * @param  pos__  Query position.
 */
int32_t int_sequence_get(uint8_t* seq__, int32_t len__, int32_t pos__) {
  if (pos__ < 0 || pos__ >= len__) return -1;

  return seq__[pos__];
}

/*
 * Integer sequence rank operation.
 *
 * @param  seq__  Symbol sequence.
 * @param  len__  Length of the sequence.
 * @param  x__  Query position.
 * @param  ch__  Query symbol.
 */
int32_t int_sequence_rank(uint8_t* seq__, int32_t len__, int32_t x__, uint8_t ch__) {
  int32_t rank = 0;

  x__ = (x__ > len__) ? len__ : x__;
  for (int32_t i = 0; i < x__; i++)
    rank += (seq__[i] == ch__) ? 1 : 0;
  return rank;
}

/*
 * Integer sequence select operation.
 *
 * @param  seq__  Symbol sequence.
 * @param  len__  Length of the sequence.
 * @param  x__  Query position.
 * @param  ch__  Query symbol.
 */
int32_t int_sequence_select(uint8_t* seq__, int32_t len__, int32_t x__, uint8_t ch__) {
  if (!x__) return 0;

  for (int32_t i = 0; i < len__; i++) {
    if (seq__[i] == ch__) {
      if (!(--x__)) return i + 1;
    }
  }
  return -1;
}

#endif  /* _INTEGER_SEQUENCE__ */
