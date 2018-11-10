#include "structure_common.h"

#include "../other/Wavelet_tree/structure_ext.h"

void benchmark_sequential_insertion() {
  WT_Struct wt;
  WT_Init(&wt);

  int8_t* dna_string = gen_dna_vals();

  START_BENCHMARK;
  for (int32_t i = 0; i < sample_size; i++) {
    WT_VInsert(&wt, i, dna_string[i]);
  }
  END_BENCHMARK(seq_insertion);

  free(dna_string);
  WT_Free(&wt);
}

void benchmark_random_insertion() {
  WT_Struct wt;
  WT_Init(&wt);

  int8_t* dna_string = gen_dna_vals();

  START_BENCHMARK;
  for (int32_t i = 0; i < sample_size; i++) {
    WT_VInsert(&wt, (rand() % (i + 1)), dna_string[i]);
  }
  END_BENCHMARK(rand_insertion);

  free(dna_string);
  WT_Free(&wt);
}

void benchmark_operations() {
  WT_Struct wt;
  WT_Init(&wt);

  int8_t* dna_string = gen_dna_vals();
  for (int32_t i = 0; i < sample_size; i++)
    WT_VInsert(&wt, i, dna_string[i]);
  free(dna_string);

  START_BENCHMARK;
  for (int32_t i = 0; i < sample_size; i++) {
    WT_VRank(&wt, i, rand() % 4);
  }
  END_BENCHMARK(seq_rank);

  START_BENCHMARK;
  for (int32_t i = 0; i < sample_size; i++) {
    WT_VSelect(&wt, i, rand() % 4);
  }
  END_BENCHMARK(seq_select);

  START_BENCHMARK;
  for (int32_t i = 0; i < sample_size; i++) {
    WT_VGet(&wt, i);
  }
  END_BENCHMARK(seq_access);

  START_BENCHMARK;
  for (int32_t i = 0; i < secondary_size; i++) {
    WT_VRank(&wt, rand() % sample_size, rand() % 4);
  }
  END_BENCHMARK(rand_rank);

  START_BENCHMARK;
  for (int32_t i = 0; i < secondary_size; i++) {
    WT_VSelect(&wt, rand() % sample_size, rand() % 4);
  }
  END_BENCHMARK(rand_select);

  START_BENCHMARK;
  for (int32_t i = 0; i < secondary_size; i++) {
    WT_VGet(&wt, rand() % sample_size);
  }
  END_BENCHMARK(rand_access);

  WT_Free(&wt);
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
