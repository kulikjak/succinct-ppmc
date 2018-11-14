/**
 * Calculate mapping order for Wavelett tree symbol to number mapping. To make
 * things faster and more importantly better readable, this array is
 * precalculated and directly inside the code.
 */

#include <stdio.h>
#include <stdlib.h>

void calculate_mapping_order() {
  int32_t i, order[256];
  int32_t ptr, counter, next;

  order[0] = 0;
  order[1] = 1;

  counter = ptr = 1;
  next = 2;

  do {
    for (i = 0; i < counter; i++) {
      order[next++] = order[ptr++] * 2;
    }
    for (i = 0; i < counter; i++) {
      order[next++] = order[ptr + i] + 1;
    }

    counter *= 2;
  } while (next < 256);

  printf("{");
  for (i = 0; i < 256; i++) {
    printf("%d", order[i]);
    if (i < 255) printf(", ");
  }
  printf("}\n");
}

int main() {
  calculate_mapping_order();

  return 0;
}