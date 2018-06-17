#ifndef _VARIABLE_WT_UTILS__
#define _VARIABLE_WT_UTILS__

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#define uchar unsigned char
#define EXTENDED_TESTS_


/*
 * Finds first higher power of two of given number.
 * If number is some power of two, returned value is that number.
 *
 * Other tested can be found in higher_power.c file.
 */
int32_t higher_pow(int32_t num__) {

  __asm__ __volatile__(
      "sub $0x1, %%eax;"
      "bsr %%eax, %%ecx;"
      "jz ZERO;"
      "mov $0x2, %%eax;"
      "shl %%cl, %%eax;"
      "jmp DONE;"
      "ZERO: xor %%eax, %%eax;"
      "DONE:"
      : "=a"(num__)
      : "a"(num__));

  return num__;
}

/*
 * Finds first lower power of two of given number.
 * If number is some power of two, returned value is that number.
 */
int32_t lower_pow(int32_t num__) {

  __asm__ __volatile__(
      "bsr %%eax, %%ecx;"
      "jz ZERO2;"
      "mov $0x1, %%eax;"
      "shl %%cl, %%eax;"
      "jmp DONE2;"
      "ZERO2: xor %%eax, %%eax;"
      "DONE2:"
      : "=a"(num__)
      : "a"(num__));

  return num__;
}

/* Default symbol map with simple ASCII mapping. */
int32_t default_map_(uchar symb__) {
	return (int32_t)symb__;
}

/* Default reverse map with simple ASCII mapping. */
uchar default_rmap_(int32_t num__) {
	return (uchar)num__;
}

#define WT_DEFAULT_MAP default_map_
#define WT_DEFAULT_RMAP default_rmap_
#define WT_DEFAULT_SIZE 256

#ifdef EXTENDED_TESTS_

/*
 * Test symbol map and reverse map.
 *
 * Both functions should be injective and surjective.
 * Each symbol must have its own number and vice versa.
 *
 * If number of symbols n is equal to some power of two, than you should simply
 * map each symbol to numbers between 0 and n-1. If n is not equal to power of
 * two, than map symbols to first lower power of two as described before. Than
 * take all numbers in interval <lower power, higher power) and sort them by
 * their bit value from right to left (reverse bit order). Then assign them to
 * symbols in ascending order.
 *
 * @Example: n = 13
 *  first lower power is 8 -> we can map some symbols to numbers from 0 to 7
 *  all numbers in higher interval <8, 16) and their reversed values:
 *    8  -> 1000 -> 0001
 *    9  -> 1001 -> 1001
 *    10 -> 1010 -> 0101
 *    11 -> 1011 -> 1101
 *    12 -> 1100 -> 0011
 *    13 -> 1101 -> 1011
 *    14 -> 1110 -> 0111
 *    15 -> 1111 -> 1111
 *  sorted:
 *    8, 12, 10, 14, 9, 13, 11, 15
 *  first five numbers will be used for mapping function (13 - 8 = 5).
 *
 * You can also check mapping in an precalculated array in this function.
 *
 * @param  scount__  Number of registered symbols.
 * @param  map__  Function mapping symbols to numbers.
 * @param  rmap__  Reverse mapping function (numbers to symbols).
 *
 * @return  Maps are working correctly or not (in that case assert will fail).
 */
bool test_maps(int32_t scount__, int32_t (*map__)(uchar), uchar (*rmap__)(int32_t)) {
	int32_t i;
	char letters[256] = {0};

  int32_t order[256] = {0, 1, 2, 3, 4, 6, 5, 7, 8, 12, 10, 14, 9, 13, 11, 15, 16, 24,
    20, 28, 18, 26, 22, 30, 17, 25, 21, 29, 19, 27, 23, 31, 32, 48, 40, 56, 36, 52, 44,
    60, 34, 50, 42, 58, 38, 54, 46, 62, 33, 49, 41, 57, 37, 53, 45, 61, 35, 51, 43, 59,
    39, 55, 47, 63, 64, 96, 80, 112, 72, 104, 88, 120, 68, 100, 84, 116, 76, 108, 92,
    124, 66, 98, 82, 114, 74, 106, 90, 122, 70, 102, 86, 118, 78, 110, 94, 126, 65, 97,
    81, 113, 73, 105, 89, 121, 69, 101, 85, 117, 77, 109, 93, 125, 67, 99, 83, 115, 75,
    107, 91, 123, 71, 103, 87, 119, 79, 111, 95, 127, 128, 192, 160, 224, 144, 208, 176,
    240, 136, 200, 168, 232, 152, 216, 184, 248, 132, 196, 164, 228, 148, 212, 180, 244,
    140, 204, 172, 236, 156, 220, 188, 252, 130, 194, 162, 226, 146, 210, 178, 242, 138,
    202, 170, 234, 154, 218, 186, 250, 134, 198, 166, 230, 150, 214, 182, 246, 142, 206,
    174, 238, 158, 222, 190, 254, 129, 193, 161, 225, 145, 209, 177, 241, 137, 201, 169,
    233, 153, 217, 185, 249, 133, 197, 165, 229, 149, 213, 181, 245, 141, 205, 173, 237,
    157, 221, 189, 253, 131, 195, 163, 227, 147, 211, 179, 243, 139, 203, 171, 235, 155,
    219, 187, 251, 135, 199, 167, 231, 151, 215, 183, 247, 143, 207, 175, 239, 159, 223,
    191, 255};

  if (scount__ > 256) {
    printf("Mapping test function only works with up to 256 symbols.\n");
    assert(0);
  }

  // check injectivity and surjectivity
	for (i = 0; i < scount__; i++) {
		uint8_t letter = (uint8_t)rmap__(order[i]);
		letters[letter]++;

		assert(letters[letter] == 1);
		assert(order[i] == map__((uchar)letter));
	}

  return true;
}

#endif  // EXTENDED_TESTS_


#endif  // _VARIABLE_WT_UTILS__
