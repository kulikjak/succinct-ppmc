#include <stdlib.h>
#include <time.h>

#include "compressor.h"
#include "unity_fixture.h"

TEST_GROUP(Compressor_main);

#define COMPRESSOR_LABEL_TEST_SIZE 1000
#define COMPRESSOR_RANDOM_TEST_MAX_SEQ_SIZE 300
#define COMPRESSOR_RANDOM_TEST_POOL_SIZE 300

#define _(symb__) GET_VALUE_FROM_SYMBOL(symb__)

compressor C;

FILE *ofp, *ifp;

void start_compressor(const char* filename__) {
  ofp = fopen(filename__, "wb");
  if (ofp == NULL) {
    fprintf(stderr, "Can't open file %s!\n", filename__);
    exit(1);
  }
  Process_Init(&C);
  Compression_Start(ofp);
}
void start_decompressor(const char* filename__) {
  ifp = fopen(filename__, "rb");
  if (ifp == NULL) {
    fprintf(stderr, "Can't open file %s!\n", filename__);
    exit(1);
  }
  Process_Init(&C);
  Decompression_Start(ifp);
}

void end_compressor() {
  Process_Free(&C);
  Compression_Finalize();
  fclose(ofp);
}
void end_decompressor() {
  Process_Free(&C);
  Decompression_Finalize();
  fclose(ifp);
}

char* generate_dna_string(int64_t length__) {
  int64_t i;
  char* res;

  res = (char*) malloc(length__ * sizeof(*res));
  for (i = 0; i < length__; i++)
    res[i] = (rand() % 4) * 2;
  return res;
}

TEST_SETUP(Compressor_main) {}

TEST_TEAR_DOWN(Compressor_main) {}

TEST(Compressor_main, LabelTest) {
  int32_t i, j;
  char* dna;
  char label[CONTEXT_LENGTH + 2];

  start_compressor("tmp/astatic_test.bin");

  srand(time(NULL));
  dna = generate_dna_string(COMPRESSOR_LABEL_TEST_SIZE);

  for (i = 0; i < COMPRESSOR_LABEL_TEST_SIZE; i++) {
    Compressor_Compress_symbol(&C, dna[i]);

    if ((i % (CONTEXT_LENGTH / 2) == 0) & (i > CONTEXT_LENGTH)) {
      deBruijn_Label(&(C.dB_), C.state_, label);
      for (j = 1; j <= CONTEXT_LENGTH; j++) {
        assert(GET_VALUE_FROM_SYMBOL(label[j]) == dna[i - CONTEXT_LENGTH + j]);
      }
    }
  }
  free(dna);

  end_compressor();
}

TEST(Compressor_main, StaticTest) {
  start_compressor("tmp/static_test.bin");

  Compressor_Compress_symbol(&C, VALUE_A);
  Compressor_Compress_symbol(&C, VALUE_C);
  Compressor_Compress_symbol(&C, VALUE_A);
  Compressor_Compress_symbol(&C, VALUE_A);
  Compressor_Compress_symbol(&C, VALUE_C);
  Compressor_Compress_symbol(&C, VALUE_C);
  Compressor_Compress_symbol(&C, VALUE_G);
  Compressor_Compress_symbol(&C, VALUE_T);
  Compressor_Compress_symbol(&C, VALUE_A);

  end_compressor();
  start_decompressor("tmp/static_test.bin");

  Graph_value val;
  Decompressor_Decompress_symbol(&C, &val);
  TEST_ASSERT_EQUAL_INT32(VALUE_A, val);
  Decompressor_Decompress_symbol(&C, &val);
  TEST_ASSERT_EQUAL_INT32(VALUE_C, val);
  Decompressor_Decompress_symbol(&C, &val);
  TEST_ASSERT_EQUAL_INT32(VALUE_A, val);
  Decompressor_Decompress_symbol(&C, &val);
  TEST_ASSERT_EQUAL_INT32(VALUE_A, val);
  Decompressor_Decompress_symbol(&C, &val);
  TEST_ASSERT_EQUAL_INT32(VALUE_C, val);
  Decompressor_Decompress_symbol(&C, &val);
  TEST_ASSERT_EQUAL_INT32(VALUE_C, val);
  Decompressor_Decompress_symbol(&C, &val);
  TEST_ASSERT_EQUAL_INT32(VALUE_G, val);
  Decompressor_Decompress_symbol(&C, &val);
  TEST_ASSERT_EQUAL_INT32(VALUE_T, val);
  Decompressor_Decompress_symbol(&C, &val);
  TEST_ASSERT_EQUAL_INT32(VALUE_A, val);

  end_decompressor();
}

TEST(Compressor_main, RandomTest) {
  char filename[255] = {0};
  int32_t i, run, len;
  Graph_value val;

  srand(0);
  printf(" ");

  for (run = 0; run < COMPRESSOR_RANDOM_TEST_POOL_SIZE; run++) {
    /* print dot for each running test */
    printf(".");
    fflush(stdout);

    /* generate filename based on test number */
    snprintf(filename, sizeof(filename), "tmp/random_test_%03d.bin", run);
    start_compressor(filename);

    /* generate dna sequence */
    len = rand() % COMPRESSOR_RANDOM_TEST_MAX_SEQ_SIZE;
    char* dna = generate_dna_string(len);

    for (i = 0; i < len; i++) {
      Compressor_Compress_symbol(&C, dna[i]);
    }

    end_compressor();
    start_decompressor(filename);

    for (i = 0; i < len; i++) {
      Decompressor_Decompress_symbol(&C, &val);
      TEST_ASSERT_EQUAL_INT32(dna[i], val);
    }

    end_decompressor();
    free(dna);
  }
}

TEST_GROUP_RUNNER(Compressor_main) {
  RUN_TEST_CASE(Compressor_main, LabelTest);
  RUN_TEST_CASE(Compressor_main, StaticTest);
  RUN_TEST_CASE(Compressor_main, RandomTest);
}
