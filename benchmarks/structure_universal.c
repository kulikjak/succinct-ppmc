#include "structure_common.h"

#include "universal.h"
#include "int_sequence.h"

void benchmark_sequential_insertion() {
  UWT_Struct wt;
  UWT_Init(&wt, 8);

  uint8_t* input_string = int_sequence_generate_random(sample_size, 8);

  START_BENCHMARK;
  for (int32_t i = 0; i < sample_size; i++) {
    UWT_Insert(&wt, i, input_string[i]);
  }
  END_BENCHMARK(seq_insertion);

  free(input_string);
  UWT_Free(&wt);
}

void benchmark_random_insertion() {
  UWT_Struct wt;
  UWT_Init(&wt, 8);

  uint8_t* input_string = int_sequence_generate_random(sample_size, 8);

  START_BENCHMARK;
  for (int32_t i = 0; i < sample_size; i++) {
    UWT_Insert(&wt, (rand() % (i + 1)), input_string[i]);
  }
  END_BENCHMARK(rand_insertion);

  free(input_string);
  UWT_Free(&wt);
}

void benchmark_operations() {
  UWT_Struct wt;
  UWT_Init(&wt, 8);

  uint8_t* input_string = int_sequence_generate_random(sample_size, 8);
  for (int32_t i = 0; i < sample_size; i++)
    UWT_Insert(&wt, i, input_string[i]);
  free(input_string);

  START_BENCHMARK;
  for (int32_t i = 0; i < sample_size; i++) {
    UWT_Rank(&wt, i, rand() % 4);
  }
  END_BENCHMARK(seq_rank);

  START_BENCHMARK;
  for (int32_t i = 0; i < sample_size; i++) {
    UWT_Select(&wt, i, rand() % 4);
  }
  END_BENCHMARK(seq_select);

  START_BENCHMARK;
  for (int32_t i = 0; i < sample_size; i++) {
    UWT_Get(&wt, i);
  }
  END_BENCHMARK(seq_access);

  START_BENCHMARK;
  for (int32_t i = 0; i < secondary_size; i++) {
    UWT_Rank(&wt, rand() % sample_size, rand() % 4);
  }
  END_BENCHMARK(rand_rank);

  START_BENCHMARK;
  for (int32_t i = 0; i < secondary_size; i++) {
    UWT_Select(&wt, rand() % sample_size, rand() % 4);
  }
  END_BENCHMARK(rand_select);

  START_BENCHMARK;
  for (int32_t i = 0; i < secondary_size; i++) {
    UWT_Get(&wt, rand() % sample_size);
  }
  END_BENCHMARK(rand_access);

  UWT_Free(&wt);
}

int main(int argc, char* argv[]) {
  if (argc != 3) {
    printf("Program needs two numerial arguments\n");
    printf("%s <sample_size> <secondary_size>\n", argv[0]);
    return 1;
  }

  sample_size = atoi(argv[1]);
  secondary_size = atoi(argv[2]);

  if (sample_size <= 0) {
    printf("Sample size must be atleast one, got %d\n", sample_size);
    return 1;
  }
  if (secondary_size <= 0) {
    printf("Secondary size must be atleast one, got %d\n", secondary_size);
    return 1;
  }

  init_results();

  benchmark_sequential_insertion();
  benchmark_random_insertion();
  benchmark_operations();

  show_results();
  return EXIT_SUCCESS;
}
