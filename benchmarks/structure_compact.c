#include "structure_common.h"

#include "../src/structure.h"
#include "int_sequence.h"

void benchmark_sequential_insertion() {
  Graph_Struct Graph;
  Graph_Line line;
  Graph_Init(&Graph);

  uint8_t* input_string = int_sequence_generate_random(sample_size, 8);

  START_BENCHMARK;
  for (int32_t i = 0; i < sample_size; i++) {
    GLine_Fill(&line, VAR_IGNORE, input_string[i], VAR_IGNORE);
    GLine_Insert(&Graph, i, &line);
  }
  END_BENCHMARK(seq_insertion);

  free(input_string);
  Graph_Free(&Graph);
}

void benchmark_random_insertion() {
  Graph_Struct Graph;
  Graph_Line line;
  Graph_Init(&Graph);

  uint8_t* input_string = int_sequence_generate_random(sample_size, 8);

  START_BENCHMARK;
  for (int32_t i = 0; i < sample_size; i++) {
    GLine_Fill(&line, VAR_IGNORE, input_string[i], VAR_IGNORE);
    GLine_Insert(&Graph, (rand() % (i + 1)), &line);
  }
  END_BENCHMARK(rand_insertion);

  free(input_string);
  Graph_Free(&Graph);
}

void benchmark_operations() {
  Graph_Struct Graph;
  Graph_Line line;
  Graph_Init(&Graph);

  uint8_t* input_string = int_sequence_generate_random(sample_size, 8);
  for (int32_t i = 0; i < sample_size; i++) {
    GLine_Fill(&line, VAR_IGNORE, input_string[i], VAR_IGNORE);
    GLine_Insert(&Graph, i, &line);
  }
  free(input_string);

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
    fprintf(stderr, "Program needs two numerial arguments\n");
    fprintf(stderr, "%s <sample_size> <secondary_size>\n", argv[0]);
    return 1;
  }

  sample_size = atoi(argv[1]);
  secondary_size = atoi(argv[2]);

  if (sample_size <= 0) {
    fprintf(stderr, "Sample size must be atleast one, got %d\n", sample_size);
    return 1;
  }
  if (secondary_size <= 0) {
    fprintf(stderr, "Secondary size must be atleast one, got %d\n", secondary_size);
    return 1;
  }

  init_results();

  benchmark_sequential_insertion();
  benchmark_random_insertion();
  benchmark_operations();

  show_results();
  return EXIT_SUCCESS;
}
