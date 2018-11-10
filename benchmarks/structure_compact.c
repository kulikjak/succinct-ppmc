#include "structure_common.h"

#include "../src/structure.h"

void benchmark_sequential_insertion() {
  Graph_Struct Graph;
  Graph_Line line;
  Graph_Init(&Graph);

  int8_t* dna_string = gen_dna_vals();

  START_BENCHMARK;
  for (int32_t i = 0; i < sample_size; i++) {
    GLine_Fill(&line, VAR_IGNORE, dna_string[i], VAR_IGNORE);
    GLine_Insert(&Graph, i, &line);
  }
  END_BENCHMARK(seq_insertion);

  free(dna_string);
  Graph_Free(&Graph);
}

void benchmark_random_insertion() {
  Graph_Struct Graph;
  Graph_Line line;
  Graph_Init(&Graph);

  int8_t *dna_string = gen_dna_vals();

  START_BENCHMARK;
  for (int32_t i = 0; i < sample_size; i++) {
    GLine_Fill(&line, VAR_IGNORE, dna_string[i], VAR_IGNORE);
    GLine_Insert(&Graph, (rand() % (i + 1)), &line);
  }
  END_BENCHMARK(rand_insertion);

  free(dna_string);
  Graph_Free(&Graph);
}

void benchmark_operations() {
  Graph_Struct Graph;
  Graph_Line line;
  Graph_Init(&Graph);

  int8_t *dna_string = gen_dna_vals();
  for (int32_t i = 0; i < sample_size; i++) {
    GLine_Fill(&line, VAR_IGNORE, dna_string[i], VAR_IGNORE);
    GLine_Insert(&Graph, i, &line);
  }
  free(dna_string);

  START_BENCHMARK;
  for (int32_t i = 0; i < sample_size; i++) {
    Graph_Rank(&Graph, i, VECTOR_W, rand() % 4);
  }
  END_BENCHMARK(seq_rank);

  START_BENCHMARK;
  for (int32_t i = 0; i < sample_size; i++) {
    Graph_Select(&Graph, i, VECTOR_W, rand() % 4);
  }
  END_BENCHMARK(seq_select);

  START_BENCHMARK;
  for (int32_t i = 0; i < sample_size; i++) {
    GLine_Get(&Graph, i, &line);
  }
  END_BENCHMARK(seq_access);

  START_BENCHMARK;
  for (int32_t i = 0; i < secondary_size; i++) {
    Graph_Rank(&Graph, rand() % sample_size, VECTOR_W, rand() % 4);
  }
  END_BENCHMARK(rand_rank);

  START_BENCHMARK;
  for (int32_t i = 0; i < secondary_size; i++) {
    Graph_Select(&Graph, rand() % sample_size, VECTOR_W, rand() % 4);
  }
  END_BENCHMARK(rand_select);

  START_BENCHMARK;
  for (int32_t i = 0; i < secondary_size; i++) {
    GLine_Get(&Graph, rand() % sample_size, &line);
  }
  END_BENCHMARK(rand_access);

  Graph_Free(&Graph);
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
