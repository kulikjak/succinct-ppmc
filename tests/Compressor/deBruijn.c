#include "deBruijn.h"
#include "unity_fixture.h"

TEST_GROUP(Compressor_deBruijn);

#ifndef TEST_PRINT_SEQUENCES
#define TEST_PRINT_SEQUENCES false
#endif

#define DEBRUIJN_PRINT_SEQUENCE false

#define _(symb__) GET_VALUE_FROM_SYMBOL(symb__)

#define GET_VALUE_FROM_SYMBOL(symb__)        \
      (((symb__) == 'A') ? VALUE_A           \
    : ((symb__) == 'C') ? VALUE_C            \
    : ((symb__) == 'G') ? VALUE_G            \
    : ((symb__) == 'T') ? VALUE_T : VALUE_$)


deBruijn_graph dB;
Graph_Line line;

TEST_SETUP(Compressor_deBruijn) {
  deBruijn_Init(&dB);
}

TEST_TEAR_DOWN(Compressor_deBruijn) {
  deBruijn_Free(&dB);
}

TEST(Compressor_deBruijn, static_test) {
  const Graph_value L[] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0,
                           1, 1, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
  const Graph_value W[] = {_('A'), _('C'), _('C'), _('C'), _('A'), _('A'), _('C'), _('C'), _('$'),
                           _('A'), _('T'), _('C'), _('G'), _('C'), _('C') + 1, _('C'), _('C') + 1,
                           _('A'), _('A'), _('G'), _('G') + 1, _('G'), _('A'), _('T'), _('G'),
                           _('A'), _('T'), _('G'), _('A'), _('A') + 1, _('C'), _('C') + 1, _('G'),
                           _('G'), _('G'), _('A'), _('G')};

  const int32_t P[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  const int32_t F[] = {1, 12, 23, 34};

  const int32_t resOutdegree[] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 1, 1, 1, 1, 1, 1, 1, 2,
                                  2, 1, 2, 2, 1, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
  const int32_t resOutgoingA[] = {1, -1, -1, -1, 2, 3, -1, -1, -1, 4, 4, -1, -1, -1, -1, -1, -1, 5,
                                  6, 6, -1, 7, 7, -1, 8, 8, 8, -1, 10, 10, -1, -1, -1, -1, -1, 11, -1};
  const int32_t resOutgoingC[] = {-1, 12, 13, 14, -1, -1, 15, 16, -1, -1, -1, 17, -1, 19, 19, 20, 20,
                                  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 22, 22, -1, -1, -1, -1, -1};
  const int32_t resOutgoingG[] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 23, -1, -1, -1, -1,
                                  -1, 26, 26, 26, 27, 27, -1, 28, 28, 28, 29, -1, -1, -1, -1, 30, 31, 32, -1, 33};
  const int32_t resOutgoingT[] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, 34, 34, -1, -1, -1, -1, -1, -1,
                                  -1, -1, -1, -1, -1, -1, 35, 36, 36, 36, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};

  deBruijn_Free(&dB);
  deBruijn_Insert_test_data(&dB, L, W, P, F, 37);

  for (int32_t i = 0; i < 37; i++) {
    TEST_ASSERT_EQUAL_INT32(resOutdegree[i], deBruijn_Outdegree(&dB, i));

    TEST_ASSERT_EQUAL_INT32(resOutgoingA[i], deBruijn_Outgoing(&dB, i, _('A')));
    TEST_ASSERT_EQUAL_INT32(resOutgoingC[i], deBruijn_Outgoing(&dB, i, _('C')));
    TEST_ASSERT_EQUAL_INT32(resOutgoingG[i], deBruijn_Outgoing(&dB, i, _('G')));
    TEST_ASSERT_EQUAL_INT32(resOutgoingT[i], deBruijn_Outgoing(&dB, i, _('T')));
    TEST_ASSERT_EQUAL_INT32(-1, deBruijn_Outgoing(&dB, i, _('$')));
  }
}

TEST(Compressor_deBruijn, cummulative_frequency) {
  cfreq freq;

  const Graph_value L[] = {0, 0, 0, 1, 1, 1, 0, 1, 0, 0, 1, 0, 0, 1, 1, 1};
  const Graph_value W[] = {_('A'), _('C'), _('G'), _('T'), _('$'), _('G'), _('C'), _('G'),
                           _('A'), _('C'), _('G'), _('A'), _('G'), _('T'), _('A'), _('C')};
  const int32_t P[] = {12, 3, 3, 5, 0, 8, 1, 2, 5, 4, 6, 6, 1, 4, 5, 0};
  const int32_t F[] = {1, 2, 4, 8};  /* not important for this test */

  const int32_t resA[] = {12, 12, 12, 12, 0, 0, 0, 0, 5, 5, 5, 6, 6, 6, 5, 0};
  const int32_t resC[] = {3, 3, 3, 3, 0, 0, 1, 1, 4, 4, 4, 0, 0, 0, 0, 0};
  const int32_t resG[] = {3, 3, 3, 3, 0, 8, 2, 2, 6, 6, 6, 1, 1, 1, 0, 0};
  const int32_t resT[] = {5, 5, 5, 5, 0, 0, 0, 0, 0, 0, 0, 4, 4, 4, 0, 0};
  const int32_t resEsc[] = {4, 4, 4, 4, 0, 1, 2, 2, 3, 3, 3, 3, 3, 3, 1, 1};
  const int32_t resTotal[] = {27, 27, 27, 27, 0, 9, 5, 5, 18, 18, 18, 14, 14, 14, 6, 1};

  deBruijn_Free(&dB);
  deBruijn_Insert_test_data(&dB, L, W, P, F, 16);

  for (int32_t i = 0; i < 16; i++) {
    deBruijn_Get_symbol_frequency(&dB, i, &freq);

    TEST_ASSERT_EQUAL_INT32(resA[i], freq.symbol_[VALUE_A >> 0x1]);
    TEST_ASSERT_EQUAL_INT32(resC[i], freq.symbol_[VALUE_C >> 0x1]);
    TEST_ASSERT_EQUAL_INT32(resG[i], freq.symbol_[VALUE_G >> 0x1]);
    TEST_ASSERT_EQUAL_INT32(resT[i], freq.symbol_[VALUE_T >> 0x1]);
    TEST_ASSERT_EQUAL_INT32(resEsc[i], freq.symbol_[VALUE_ESC >> 0x1]);
    TEST_ASSERT_EQUAL_INT32(resTotal[i], freq.total_);
  }
}

TEST_GROUP_RUNNER(Compressor_deBruijn) {
  RUN_TEST_CASE(Compressor_deBruijn, static_test);
  RUN_TEST_CASE(Compressor_deBruijn, cummulative_frequency);
}
