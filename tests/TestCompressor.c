#include "compression.h"
#include "unity_fixture.h"

TEST_GROUP(compressor);

#define compressor_PRINT_SEQUENCE false

#define _(symb__) GET_VALUE_FROM_SYMBOL(symb__)

compressor C;

TEST_SETUP(compressor) {
  Compressor_Init(&C);
}

TEST_TEAR_DOWN(compressor) {
  Compressor_Free(&C);
}

TEST(compressor, InsertionStaticTest) {
  int32_t i, size;
  char buffer[CONTEXT_LENGTH + 1];
  Graph_Line line;

  const Graph_value L[] = {0, 0, 0, 1, 0, 1, 1, 1, 0, 0, 1, 1, 0,
                           1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
  const Graph_value W[] = {_('C'), _('G'), _('T'), _('A'), _('G'), _('C'), _('G'), _('$'), _('T'),
                           _('A'), _('C'), _('C'), _('T'), _('A'), _('$'), _('$'), _('C'), _('$'),
                           _('T'), _('T'), _('$'), _('C'), _('$'), _('$'), _('C'), _('$')};
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

  Compressor_Compress_symbol(&C, 0, _('A'));
  Compressor_Compress_symbol(&C, 4, _('C'));
  Compressor_Compress_symbol(&C, 6, _('C'));

  deBruijn_Print(&(C.dB_), true);

  Compressor_Compress_symbol(&C, 8, _('A'));
//  Compressor_Compress_symbol(&C, 7, _('G'));
//  Compressor_Compress_symbol(&C, 17, _('T'));
//  Compressor_Compress_symbol(&C, 21, _('C'));
//  Compressor_Compress_symbol(&C, 16, _('C'));
//  Compressor_Compress_symbol(&C, 15, _('T'));

//  Compressor_Compress_symbol(&C, 0, _('A'));
//  Compressor_Compress_symbol(&C, 1, _('C'));
//  Compressor_Compress_symbol(&C, 4, _('C'));
//
//  Compressor_Compress_symbol(&C, 5, _('A'));
//  Compressor_Compress_symbol(&C, 3, _('G'));
//  Compressor_Compress_symbol(&C, 13, _('T'));
//  Compressor_Compress_symbol(&C, 17, _('C'));
//  Compressor_Compress_symbol(&C, 13, _('C'));
//  Compressor_Compress_symbol(&C, 11, _('T'));

  deBruijn_Print(&(C.dB_), true);

  size = Graph_Size(&(C.dB_.Graph_));
  TEST_ASSERT_EQUAL_INT32(size, 26);

  for (i = 0; i < size; i++) {
    GLine_Get(&(C.dB_.Graph_), i, &line);
    TEST_ASSERT_EQUAL_INT8(line.L_, L[i]);
    TEST_ASSERT_EQUAL_INT8(line.W_, W[i]);
    TEST_ASSERT_EQUAL_INT32(line.P_, P[i]);

    deBruijn_Label(&(C.dB_), i, buffer);
    TEST_ASSERT_EQUAL_STRING(buffer, Labels[i]);
  }

// Old result:
//      F  L  Label  W   P
// ------------------------
//   0: $  0     $   C   4
//   1:    0     $   G   1
//   2:    0     $   T   2
//   3:    1     $   A   2
//   4: A  0    $A   G   1
//   5:    1    $A   C   1
//   6:    1   $CA   G   1
//   7:    1  $CCA   $   0
//   8: C  0    $C   T   1
//   9:    0    $C   A   1
//  10:    1    $C   C   2
//  11:    1   $AC   C   1
//  12:    0   $CC   T   1
//  13:    1   $CC   A   1
//  14:    1  $ACC   $   0
//  15:    1  $TCC   $   0
//  16:    1   $TC   C   1
//  17:    1  $GTC   $   0
//  18: G  1    $G   T   1
//  19:    1   $AG   T   1
//  20:    1  $CAG   $   0
//  21: T  1    $T   C   1
//  22:    1   $CT   $   0
//  23:    1  $CCT   $   0
//  24:    1   $GT   C   1
//  25:    1  $AGT   $   0

#if compressor_PRINT_SEQUENCE
  deBruijn_Print(&(C.dB_), true);
#endif
}


TEST(compressor, TestXXX) {
  int32_t res;

  res = Compressor_Compress_symbol(&C, 0, _('A'));
  deBruijn_Print(&(C.dB_), true);
  printf("%d\n", res);

  res = Compressor_Compress_symbol(&C, res, _('C'));
  deBruijn_Print(&(C.dB_), true);
  printf("%d\n", res);

  res = Compressor_Compress_symbol(&C, res, _('C'));
  deBruijn_Print(&(C.dB_), true);
  printf("%d\n", res);

  res = Compressor_Compress_symbol(&C, res, _('G'));
  deBruijn_Print(&(C.dB_), true);
  printf("%d\n", res);

  res = Compressor_Compress_symbol(&C, res, _('T'));
  deBruijn_Print(&(C.dB_), true);
  printf("%d\n", res);


  res = Compressor_Compress_symbol(&C, res, _('T'));
  deBruijn_Print(&(C.dB_), true);
  printf("%d\n", res);

  res = Compressor_Compress_symbol(&C, res, _('C'));
  deBruijn_Print(&(C.dB_), true);
  printf("%d\n", res);

  res = Compressor_Compress_symbol(&C, res, _('C'));
  deBruijn_Print(&(C.dB_), true);
  printf("%d\n", res);


  res = Compressor_Compress_symbol(&C, res, _('C'));
  deBruijn_Print(&(C.dB_), true);
  printf("%d\n", res);


  res = Compressor_Compress_symbol(&C, res, _('C'));
  deBruijn_Print(&(C.dB_), true);
  printf("%d\n", res);

  res = Compressor_Compress_symbol(&C, res, _('T'));
  deBruijn_Print(&(C.dB_), true);
  printf("%d\n", res);

  res = Compressor_Compress_symbol(&C, res, _('T'));
  deBruijn_Print(&(C.dB_), true);
  printf("%d\n", res);
}

TEST_GROUP_RUNNER(compressor) {
  // FIX ALL THIS
  //RUN_TEST_CASE(compressor, InsertionStaticTest);
  //RUN_TEST_CASE(compressor, TestXXX);
}
