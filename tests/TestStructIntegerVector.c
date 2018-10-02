#include "structure.h"
#include "unity_fixture.h"

TEST_GROUP(StructIntegerVector);

#define VAR_IGNORE 0
#define INTEGER_VECTOR_SAMPLE_SIZE_ 250
#define INTEGER_VECTOR_PRINT_SEQUENCES false

Graph_Struct Graph;
Graph_Line line;

TEST_SETUP(StructIntegerVector) { Graph_Init(&Graph); }

TEST_TEAR_DOWN(StructIntegerVector) { Graph_Free(&Graph); }

TEST(StructIntegerVector, CorrectFrontInsertion) {
  int32_t i, sequence[INTEGER_VECTOR_SAMPLE_SIZE_];

  for (i = 0; i < INTEGER_VECTOR_SAMPLE_SIZE_; i++) {
    sequence[i] = rand();

    GLine_Fill(&line, VAR_IGNORE, VAR_IGNORE, sequence[i]);
    GLine_Insert(&Graph, 0, &line);
  }

  for (i = 0; i < INTEGER_VECTOR_SAMPLE_SIZE_; i++) {
    GLine_Get(&Graph, i, &line);
    TEST_ASSERT_EQUAL_UINT32(sequence[INTEGER_VECTOR_SAMPLE_SIZE_ - i - 1],
                             line.P_);
  }
}

TEST(StructIntegerVector, CorrectRearInsertion) {
  int32_t i, sequence[INTEGER_VECTOR_SAMPLE_SIZE_];

  for (i = 0; i < INTEGER_VECTOR_SAMPLE_SIZE_; i++) {
    sequence[i] = rand();

    GLine_Fill(&line, VAR_IGNORE, VAR_IGNORE, sequence[i]);
    GLine_Insert(&Graph, i, &line);
  }

  for (i = 0; i < INTEGER_VECTOR_SAMPLE_SIZE_; i++) {
    GLine_Get(&Graph, i, &line);
    TEST_ASSERT_EQUAL_UINT32(sequence[i], line.P_);
  }
}

TEST(StructIntegerVector, FrequencyIncrease) {
  int32_t i, pos, sequence[INTEGER_VECTOR_SAMPLE_SIZE_];

  for (i = 0; i < INTEGER_VECTOR_SAMPLE_SIZE_; i++) {
    sequence[i] = rand();

    GLine_Fill(&line, VAR_IGNORE, VAR_IGNORE, sequence[i]);
    GLine_Insert(&Graph, i, &line);
  }

  srand(time(NULL));
  for (i = 0; i < INTEGER_VECTOR_SAMPLE_SIZE_ * 4; i++) {
    pos = rand() % INTEGER_VECTOR_SAMPLE_SIZE_;

    sequence[pos]++;
    Graph_Increase_frequency(&Graph, pos);
  }

  for (i = 0; i < INTEGER_VECTOR_SAMPLE_SIZE_; i++) {
    GLine_Get(&Graph, i, &line);
    TEST_ASSERT_EQUAL_UINT32(sequence[i], line.P_);
  }
}

TEST_GROUP_RUNNER(StructIntegerVector) {
  RUN_TEST_CASE(StructIntegerVector, CorrectFrontInsertion);
  RUN_TEST_CASE(StructIntegerVector, CorrectRearInsertion);
  RUN_TEST_CASE(StructIntegerVector, FrequencyIncrease);
}
