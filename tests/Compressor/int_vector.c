#include "structure.h"
#include "unity_fixture.h"

TEST_GROUP(Compressor_int_vector);

#define VAR_IGNORE 1

#ifndef TEST_SEQENCE_LEN
  #define TEST_SEQENCE_LEN 250
#endif

#ifndef TEST_PRINT_SEQUENCES
  #define TEST_PRINT_SEQUENCES false
#endif

Graph_Struct Graph;
Graph_Line line;

TEST_SETUP(Compressor_int_vector) {
  Graph_Init(&Graph);
}

TEST_TEAR_DOWN(Compressor_int_vector) {
  Graph_Free(&Graph);
}

TEST(Compressor_int_vector, front_insertion) {
  int32_t i, sequence[TEST_SEQENCE_LEN];

  for (i = 0; i < TEST_SEQENCE_LEN; i++) {
    sequence[i] = rand();

    GLine_Fill(&line, VAR_IGNORE, VAR_IGNORE, sequence[i]);
    GLine_Insert(&Graph, 0, &line);
  }

  for (i = 0; i < TEST_SEQENCE_LEN; i++) {
    GLine_Get(&Graph, i, &line);
    TEST_ASSERT_EQUAL_UINT32(sequence[TEST_SEQENCE_LEN - i - 1], line.P_);
  }
}

TEST(Compressor_int_vector, rear_insertion) {
  int32_t i, sequence[TEST_SEQENCE_LEN];

  for (i = 0; i < TEST_SEQENCE_LEN; i++) {
    sequence[i] = rand();

    GLine_Fill(&line, VAR_IGNORE, VAR_IGNORE, sequence[i]);
    GLine_Insert(&Graph, i, &line);
  }

  for (i = 0; i < TEST_SEQENCE_LEN; i++) {
    GLine_Get(&Graph, i, &line);
    TEST_ASSERT_EQUAL_UINT32(sequence[i], line.P_);
  }
}

TEST(Compressor_int_vector, frequency_increase) {
  int32_t i, pos, sequence[TEST_SEQENCE_LEN];

  for (i = 0; i < TEST_SEQENCE_LEN; i++) {
    sequence[i] = rand();

    GLine_Fill(&line, VAR_IGNORE, VAR_IGNORE, sequence[i]);
    GLine_Insert(&Graph, i, &line);
  }

  srand(time(NULL));
  for (i = 0; i < TEST_SEQENCE_LEN * 4; i++) {
    pos = rand() % TEST_SEQENCE_LEN;

    sequence[pos]++;
    Graph_Increase_frequency(&Graph, pos, 1);
  }

  for (i = 0; i < TEST_SEQENCE_LEN; i++) {
    GLine_Get(&Graph, i, &line);
    TEST_ASSERT_EQUAL_UINT32(sequence[i], line.P_);
  }
}

TEST_GROUP_RUNNER(Compressor_int_vector) {
  RUN_TEST_CASE(Compressor_int_vector, front_insertion);
  RUN_TEST_CASE(Compressor_int_vector, rear_insertion);
  RUN_TEST_CASE(Compressor_int_vector, frequency_increase);
}
