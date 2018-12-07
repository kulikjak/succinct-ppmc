#include "unity_fixture.h"

static void RunAllTests(void) {
  RUN_TEST_GROUP(Wavelet_tree_optimized);
  RUN_TEST_GROUP(Wavelet_tree_optimized_ext);
  RUN_TEST_GROUP(Wavelet_tree_universal);
}

int main(int argc, const char* argv[]) {
  return UnityMain(argc, argv, RunAllTests);
}
