#include "deBruijn.h"
#include "unity_fixture.h"

TEST_GROUP(deBruijn);

#define DEBRUIJN_PRINT_SEQUENCE false

deBruijn_graph dB;
Graph_Line line;

TEST_SETUP(deBruijn) { deBruijn_Init(&dB); }

TEST_TEAR_DOWN(deBruijn) { deBruijn_Free(&dB); }

TEST(deBruijn, BasicStaticTest) {
  const int8_t L[] = {0, 0, 1, 0, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1};
  const char W[] = {'A', 'C', 'G', 'C', 'G', 'C', '$', '$', '$', '$', 'A', 'G', 'G', 'A', '$'};
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

    TEST_ASSERT_EQUAL_INT32(resOutgoingA[i], deBruijn_Outgoing(&dB, i, 'A'));
    TEST_ASSERT_EQUAL_INT32(resOutgoingC[i], deBruijn_Outgoing(&dB, i, 'C'));
    TEST_ASSERT_EQUAL_INT32(resOutgoingG[i], deBruijn_Outgoing(&dB, i, 'G'));
    TEST_ASSERT_EQUAL_INT32(-1, deBruijn_Outgoing(&dB, i, 'T'));
    TEST_ASSERT_EQUAL_INT32(-1, deBruijn_Outgoing(&dB, i, '$'));

    TEST_ASSERT_EQUAL_INT32(resIndegree[i], deBruijn_Indegree(&dB, i));

    TEST_ASSERT_EQUAL_INT32(resShorten1[i], deBruijn_Shorten_context(&dB, i, 1));
    TEST_ASSERT_EQUAL_INT32(resShorten2[i], deBruijn_Shorten_context(&dB, i, 2));

    TEST_ASSERT_EQUAL_INT32(resContextLen[i], deBruijn_get_context_len_(&dB, i));
  }
}

TEST(deBruijn, InsertionStaticTest) {
  int32_t i, size;
  char buffer[CONTEXT_LENGTH + 1];

  const int8_t L[] = {0, 0, 0, 1, 0, 1, 1, 1, 0, 0, 1, 1, 0,
                      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
  const char W[] = {'C', 'G', 'T', 'A', 'G', 'C', 'G', '$', 'T',
                    'A', 'C', 'C', 'T', 'A', '$', '$', 'C', '$',
                    'T', 'T', '$', 'C', '$', '$', 'C', '$'};
  const int32_t P[] = {4, 1, 2, 2, 1, 1, 1, 0, 1, 1, 2, 1, 1,
                       1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 0};

  // this works only with CONTEXT_LENGTH 4
#ifdef OMIT_EXCESSIVE_DOLLAR_SIGNS_
  const char *Labels[] = {
      "   $", "   $", "   $", "   $", "  $A", "  $A", " $CA", "$CCA", "  $C",
      "  $C", "  $C", " $AC", " $CC", " $CC", "$ACC", "$TCC", " $TC", "$GTC",
      "  $G", " $AG", "$CAG", "  $T", " $CT", "$CCT", " $GT", "$AGT"};
#else
  const char *Labels[] = {
      "$$$$", "$$$$", "$$$$", "$$$$", "$$$A", "$$$A", "$$CA", "$CCA", "$$$C",
      "$$$C", "$$$C", "$$AC", "$$CC", "$$CC", "$ACC", "$TCC", "$$TC", "$GTC",
      "$$$G", "$$AG", "$CAG", "$$$T", "$$CT", "$CCT", "$$GT", "$AGT"};
#endif

  deBruijn_Add_context_symbol(&dB, 0, 'A');
  deBruijn_Add_context_symbol(&dB, 1, 'C');
  deBruijn_Add_context_symbol(&dB, 4, 'C');

  deBruijn_Add_context_symbol(&dB, 5, 'A');
  deBruijn_Add_context_symbol(&dB, 3, 'G');
  deBruijn_Add_context_symbol(&dB, 13, 'T');
  deBruijn_Add_context_symbol(&dB, 17, 'C');
  deBruijn_Add_context_symbol(&dB, 13, 'C');
  deBruijn_Add_context_symbol(&dB, 11, 'T');

  size = Graph_Size(&(dB.Graph_));
  TEST_ASSERT_EQUAL_INT32(size, 26);

  for (i = 0; i < size; i++) {
    GLine_Get(&(dB.Graph_), i, &line);
    TEST_ASSERT_EQUAL_INT8(line.L_, L[i]);
    TEST_ASSERT_EQUAL_INT8(line.W_, W[i]);
    TEST_ASSERT_EQUAL_INT32(line.P_, P[i]);

    deBruijn_Label(&dB, i, buffer);
    TEST_ASSERT_EQUAL_STRING(buffer, Labels[i]);
  }

#if DEBRUIJN_PRINT_SEQUENCE
  deBruijn_Print(&dB, true);
#endif
}

TEST_GROUP_RUNNER(deBruijn) {
  RUN_TEST_CASE(deBruijn, BasicStaticTest);
  RUN_TEST_CASE(deBruijn, InsertionStaticTest);
}
