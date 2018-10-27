#include "deBruijn.h"
#include "unity_fixture.h"

TEST_GROUP(deBruijn);

#define DEBRUIJN_PRINT_SEQUENCE false

#define _(symb__) GET_VALUE_FROM_SYMBOL(symb__)

deBruijn_graph dB;
Graph_Line line;

TEST_SETUP(deBruijn) { deBruijn_Init(&dB); }

TEST_TEAR_DOWN(deBruijn) { deBruijn_Free(&dB); }

TEST(deBruijn, BasicStaticTest) {
  const Graph_value L[] = {0, 0, 1, 0, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1};
  const Graph_value W[] = {_('A'), _('C'), _('G'), _('C'), _('G'), _('C'), _('$'), _('$'), _('$'), _('$'), _('A'), _('G'), _('G'), _('A'), _('$')};
  const int32_t P[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  const int32_t F[] = {3, 7, 10, 15};

  const int32_t resOutdegree[] = {3, 3, 3, 2, 2, 1, 1, 1, 1, 1, 2, 2, 1, 1, 1};
  const int32_t resOutgoingA[] = {4,  4,  4, -1, -1, -1, -1, -1, -1, -1, 5, 5,  -1, 6,  -1};
  const int32_t resOutgoingC[] = {7,  7,  7,  8,  8,  9,  -1, -1, -1, -1, -1, -1, -1, -1, -1};
  const int32_t resOutgoingG[] = {11, 11, 11, 12, 12, -1, -1, -1, -1, -1, 13, 13, 14, -1, -1};

  const int32_t resIndegree[] = {0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};

  const int32_t resShorten1[] = {-1, -1, -1, -1, 3,  3,  3, -1, 7,  7,  -1, 10, 10, 10, 10};
  const int32_t resShorten2[] = {-1, -1, -1, -1, -1, -1, 5, -1, -1, 8,  -1, -1, -1, -1, 13};

  const int32_t resContextLen[] = {0, 0, 0, 1, 1, 2, 3, 1, 2, 3, 1, 1, 2, 2, 3};

  deBruijn_Free(&dB);
  deBruijn_Insert_test_data(&dB, L, W, P, F, 15);

  for (int32_t i = 0; i < 15; i++) {
    TEST_ASSERT_EQUAL_INT32(resOutdegree[i], deBruijn_Outdegree(&dB, i));

    TEST_ASSERT_EQUAL_INT32(resOutgoingA[i], deBruijn_Outgoing(&dB, i, _('A')));
    TEST_ASSERT_EQUAL_INT32(resOutgoingC[i], deBruijn_Outgoing(&dB, i, _('C')));
    TEST_ASSERT_EQUAL_INT32(resOutgoingG[i], deBruijn_Outgoing(&dB, i, _('G')));
    TEST_ASSERT_EQUAL_INT32(-1, deBruijn_Outgoing(&dB, i, _('T')));
    TEST_ASSERT_EQUAL_INT32(-1, deBruijn_Outgoing(&dB, i, _('$')));

    TEST_ASSERT_EQUAL_INT32(resIndegree[i], deBruijn_Indegree(&dB, i));

    TEST_ASSERT_EQUAL_INT32(resShorten1[i], deBruijn_Shorten_context(&dB, i, 1));
    TEST_ASSERT_EQUAL_INT32(resShorten2[i], deBruijn_Shorten_context(&dB, i, 2));

    TEST_ASSERT_EQUAL_INT32(resContextLen[i], deBruijn_get_context_len_(&dB, i));
  }
}

TEST(deBruijn, CummulativeFrequencyTest) {
  cfreq freq;

  const Graph_value L[] = {0, 0, 0, 1, 1, 1, 0, 1, 0, 0, 1, 0, 0, 1, 1, 1};
  const Graph_value W[] = {_('A'), _('C'), _('G'), _('T'), _('$'), _('G'),
                           _('C'), _('G'), _('A'), _('C'), _('G'), _('A'),
                           _('G'), _('T'), _('A'), _('C')};
  const int32_t P[] = {12, 3, 3, 5, 0, 8, 1, 2, 5, 4, 6, 6, 1, 4, 5, 0};

  // not important for this test
  const int32_t F[] = {1, 2, 4, 8};

  // result arrays
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

    TEST_ASSERT_EQUAL_INT32(resA[i], freq.symbol_[VALUE_A]);
    TEST_ASSERT_EQUAL_INT32(resC[i], freq.symbol_[VALUE_C]);
    TEST_ASSERT_EQUAL_INT32(resG[i], freq.symbol_[VALUE_G]);
    TEST_ASSERT_EQUAL_INT32(resT[i], freq.symbol_[VALUE_T]);
    TEST_ASSERT_EQUAL_INT32(resEsc[i], freq.symbol_[VALUE_ESC]);
    TEST_ASSERT_EQUAL_INT32(resTotal[i], freq.total_);
  }
}

TEST_GROUP_RUNNER(deBruijn) {
  RUN_TEST_CASE(deBruijn, BasicStaticTest);
  RUN_TEST_CASE(deBruijn, CummulativeFrequencyTest);
}
