#include <time.h>
#include <stdlib.h>

#include "compressor.h"
#include "unity_fixture.h"

TEST_GROUP(compressor);

#define compressor_PRINT_SEQUENCE false

#define compressor_RANDOM_TEST_VERBOSE false
#define compressor_RANDOM_TEST_MAX_SEQ_SIZE 300
#define compressor_RANDOM_TEST_POOL_SIZE 1

#define _(symb__) GET_VALUE_FROM_SYMBOL(symb__)

compressor C;
decompressor D;

FILE *ofp, *ifp;

void start_compressor(const char* filename__) {
  ofp = fopen(filename__, "wb");
  if (ofp == NULL) {
    fprintf(stderr, "Can't open file %s!\n", filename__);
    exit(1);
  }
  Compressor_Init(&C, ofp);
}
void start_decompressor(const char* filename__) {
  ifp = fopen(filename__, "rb");
  if (ifp == NULL) {
    fprintf(stderr, "Can't open file %s!\n", filename__);
    exit(1);
  }
  Decompressor_Init(&D, ifp);
}

void end_compressor() {
  Compressor_Finalize(&C);
  fclose(ofp);
}
void end_decompressor() {
  Decompressor_Finalize(&D);
  fclose(ifp);
}

char* generate_dna_string(int64_t length__) {
  int64_t i;
  char *res;

  res = (char*) malloc (length__ * sizeof(*res));
  for (i = 0; i < length__; i++)
    res[i] = rand() % 4;
  return res;
}

TEST_SETUP(compressor) {}

TEST_TEAR_DOWN(compressor) {}

TEST(compressor, StaticTest) {
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
  Decompressor_Decompress_symbol(&D, &val);
  TEST_ASSERT_EQUAL_INT32(val, VALUE_A);
  Decompressor_Decompress_symbol(&D, &val);
  TEST_ASSERT_EQUAL_INT32(val, VALUE_C);
  Decompressor_Decompress_symbol(&D, &val);
  TEST_ASSERT_EQUAL_INT32(val, VALUE_A);
  Decompressor_Decompress_symbol(&D, &val);
  TEST_ASSERT_EQUAL_INT32(val, VALUE_A);
  Decompressor_Decompress_symbol(&D, &val);
  TEST_ASSERT_EQUAL_INT32(val, VALUE_C);
  Decompressor_Decompress_symbol(&D, &val);
  TEST_ASSERT_EQUAL_INT32(val, VALUE_C);
  Decompressor_Decompress_symbol(&D, &val);
  TEST_ASSERT_EQUAL_INT32(val, VALUE_G);
  Decompressor_Decompress_symbol(&D, &val);
  TEST_ASSERT_EQUAL_INT32(val, VALUE_T);
  Decompressor_Decompress_symbol(&D, &val);
  TEST_ASSERT_EQUAL_INT32(val, VALUE_A);

  end_decompressor();
}

TEST(compressor, RandomTest) {
  char filename[255] = {0};
  int32_t i, run, len;
  Graph_value val;

  srand(0);
  printf(" ");

  for (run = 0; run < compressor_RANDOM_TEST_POOL_SIZE; run++) {
    // print dot for each running test
    printf(".");
    fflush(stdout);

    // generate filename based on test number
    sprintf(filename, "tmp/random_test_%03d.bin", run);
    start_compressor(filename);

    // generate dna sequence
    len = rand() % compressor_RANDOM_TEST_MAX_SEQ_SIZE;
    char* dna = generate_dna_string(len);

    for (i = 0; i < len; i++) {
      Compressor_Compress_symbol(&C, dna[i]);
    }

    deBruijn_Print(&(C.dB_), true);

    end_compressor();
    start_decompressor(filename);

    for (i = 0; i < len; i++) {
      Decompressor_Decompress_symbol(&D, &val);
      TEST_ASSERT_EQUAL_INT32(dna[i], val);
    }

    end_decompressor();
    free(dna);
  }
}

TEST_GROUP_RUNNER(compressor) {
  RUN_TEST_CASE(compressor, StaticTest);
  RUN_TEST_CASE(compressor, RandomTest);
}
