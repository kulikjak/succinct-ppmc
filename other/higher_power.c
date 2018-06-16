/**
 * This file contains several algorithms for "higher power of two" 
 * and more code to test their speed.
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/* Test sample size */
#define SIZE_TRESHOLD_ 100000000

/* Testing number */
#define TEST_NUMBER_ 1025

//#define PRINT_RESULT_


int main(int argc, char* argv[]) {
  int32_t i;
  int32_t num, res;
  time_t ticks, tickse;

  /* A basic version of "higher power of two" code. */

  ticks = clock();
  for (i = 0; i < SIZE_TRESHOLD_; i++) {
    num = TEST_NUMBER_;

    num--;
    num |= num >> 1;
    num |= num >> 2;
    num |= num >> 4;
    num |= num >> 8;
    num |= num >> 16;
    num++;

#ifdef PRINT_RESULT_
    if (!i) printf("%d\n", num);
#endif
  }
  tickse = clock();
  printf("OR basic version:\t%ld\n", tickse - ticks);

  /*
   * Same as the previous version but written in assembly.
   * All operations are now done in the CPU registers.
   */

  ticks = clock();
  for (i = 0; i < SIZE_TRESHOLD_; i++) {
    num = TEST_NUMBER_;

    __asm__ __volatile__(
        "sub $0x1, %%ecx;"
        "mov %%ecx, %%eax;"
        "shr %%eax;"
        "or %%eax, %%ecx;"
        "mov %%ecx, %%eax;"
        "shr $0x2, %%eax;"
        "or %%eax, %%ecx;"
        "mov %%ecx, %%eax;"
        "shr $0x4, %%eax;"
        "or %%eax, %%ecx;"
        "mov %%ecx, %%eax;"
        "shr $0x8, %%eax;"
        "or %%eax, %%ecx;"
        "mov %%ecx, %%eax;"
        "shr $0x10, %%eax;"
        "or %%eax, %%ecx;"
        "add $0x1, %%ecx;"
        : "=c"(res)
        : "c"(num));

#ifdef PRINT_RESULT_
    if (!i) printf("%d\n", res);
#endif
  }
  tickse = clock();
  printf("OR ASM version: \t%ld\n", tickse - ticks);

  /* More advanced assembly code with bsr instruction. */  

  ticks = clock();
  for (i = 0; i < SIZE_TRESHOLD_; i++) {
    num = TEST_NUMBER_;

    __asm__ __volatile__(
        "sub $0x1, %%eax;"
        "bsr %%eax, %%ecx;"
        "jz ZERO;"
        "mov $0x2, %%eax;"
        "shl %%cl, %%eax;"
        "jmp DONE;"
        "ZERO: xor %%eax, %%eax;"
        "DONE:"
        : "=a"(res)
        : "a"(num));

#ifdef PRINT_RESULT_
    if (!i) printf("%d\n", res);
#endif
  }
  tickse = clock();
  printf("BSR ASM version:\t%ld\n", tickse - ticks);

  return EXIT_SUCCESS;
}
