#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

const char dna[] = {'A', 'C', 'G', 'T'};

void generate_dna_string(int64_t length__) {
  int64_t i;

  srand(time(NULL));
  for (i = 0; i < length__; i++) {
    fprintf(stdout, "%c", dna[rand() % 4]);
  }
}

void usage(const char* name__) {
  printf("Usage: %s <len>\n", name__);
  printf("Generate DNA like sequence of 4 symbols (A,C,G,T).\n\n");
  printf("  <len> length of the sequence/number of letters\n\n");
  printf("Sequence is generated on the stdout\n");
}

int main(int argc, char* argv[]) {
  int64_t length;

  if (argc != 2) {
    usage(argv[0]);
    return EXIT_FAILURE;
  }

  if (!strncmp(argv[1], "--help", 6)) {
    usage(argv[0]);
    return EXIT_SUCCESS;
  }

  length = strtoll(argv[1], NULL, 10);

  if (length <= 0) {
    printf("Argument must be a positive whole number\n\n");
    usage(argv[0]);
    return EXIT_FAILURE;
  }

  generate_dna_string(length);

  return EXIT_SUCCESS;
}
