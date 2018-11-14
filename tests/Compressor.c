#include "unity_fixture.h"

static void RunAllTests(void) {
  RUN_TEST_GROUP(Compressor_memory);
  RUN_TEST_GROUP(Compressor_binary_vector);
  RUN_TEST_GROUP(Compressor_wavelet_tree);
  RUN_TEST_GROUP(Compressor_int_vector);

  RUN_TEST_GROUP(Compressor_deBruijn);
  RUN_TEST_GROUP(Compressor_main);
}

int main(int argc, const char* argv[]) {
  return UnityMain(argc, argv, RunAllTests);
}
