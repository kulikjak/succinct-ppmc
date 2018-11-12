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
