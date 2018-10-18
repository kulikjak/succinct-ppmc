#include "unity_fixture.h"

static void RunAllTests(void) {
  RUN_TEST_GROUP(StructMemory);
  RUN_TEST_GROUP(StructBinaryVector);
  RUN_TEST_GROUP(StructWaveletTree);
  RUN_TEST_GROUP(StructIntegerVector);

  RUN_TEST_GROUP(deBruijn);
  RUN_TEST_GROUP(compressor);
}

int main(int argc, const char* argv[]) {
  return UnityMain(argc, argv, RunAllTests);
}
