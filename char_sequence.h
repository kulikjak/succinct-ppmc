/*
Implementation of very simple and inefficient Wavelet tree structure.

It is used only for testing purposes (correct implementation of each other
structure).
With time complexity of O(n) for both rank and select, it should not be used on
bigger bit sequences.
*/

#ifndef _CHAR_SEQUENCE__
#define _CHAR_SEQUENCE__

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


void print_char_dna_sequence(char* seq__, int32_t len__) {
  int32_t i;

  for (i = 0; i < len__; i++)
    printf("%c ", seq__[i]);
  printf("\n");
}

char* init_random_char_dna_sequence(int32_t a__) {
  int32_t i, temp;

  char* sequence = (char*)malloc(a__ * sizeof(char));
  srand(0);

  for (i = 0; i < a__; i++) {
    temp = rand() % 4;
    switch (temp) {
      case 0:
        sequence[i] = 'A';
        break;
      case 1:
        sequence[i] = 'C';
        break;
      case 2:
        sequence[i] = 'G';
        break;
      case 3:
        sequence[i] = 'T';
        break;
    }
  }
  return sequence;
}

void free_char_dna_sequence(char** seq__) {
  free(*seq__);
  seq__ = NULL;
}

int32_t rank_char_dna_sequence(char* seq__, int32_t len__, int32_t x__, char ch__) {
  int32_t i, rank = 0;

  // check that x__ is not higheer then len of sequence
  x__ = (x__ > len__) ? len__ : x__;
  for (i = 0; i < x__; i++) {
    rank += (seq__[i] == ch__) ? 1 : 0;
  }

  return rank;
}

char get_char_dna_sequence(char* seq__, int32_t len__, int32_t pos__) {
  if (pos__ < 0 || pos__ >= len__) return ' ';

  return seq__[pos__];
}

int32_t select_char_dna_sequence(char* seq__, int32_t len__, int32_t x__, char ch__) {
  int32_t i;

  if (!x__) return 0;

  for (i = 0; i < len__; i++) {
    if (seq__[i] == ch__) {
      if (!(--x__)) return i + 1;
    }
  }

  // if there is not enough 1s, return -1
  return -1;
}


#endif  // _CHAR_SEQUENCE__
