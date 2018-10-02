#ifndef _WT_STRUCTURE__
#define _WT_STRUCTURE__

#include <stdio.h>

#include "../RaS_Complex/memory.h"
#include "../RaS_Complex/structure.h"
#include "../shared/utils.h"


#define WT_MEM_WHOLE 0
#define WT_MEM_LOWER 1
#define WT_MEM_HIGHER 2

// #define SYMBOL_UPPER_TRANSLATION_

#define TO_UPPER(symb__) {           \
    if (symb__ >= 'a' && symb__ <= 'z')     \
      symb__ = symb__ - ('a' - 'A'); \
  }

typedef struct {
  RAS_Struct RaS[3];
} WT_Struct;

/*
 * Initialize WT_Struct object given as an argument.
 *
 * Since this is for DNA, this structure is for 4 symbols only and mapping is
 * done automatically. More variable structure can be foud in variable.h
 *
 * @param  WT__  Reference to WT_Struct object.
 */
void WT_Init(WT_Struct* WT__) {
  RAS_Init(&(WT__->RaS[WT_MEM_WHOLE]));
  RAS_Init(&(WT__->RaS[WT_MEM_LOWER]));
  RAS_Init(&(WT__->RaS[WT_MEM_HIGHER]));
}

/*
 * Free all memory associated with WT object.
 *
 * @param  WT__  Reference to WT_Struct object.
 */
void WT_Free(WT_Struct *WT__) {
  RAS_Free(&(WT__->RaS[WT_MEM_WHOLE]));
  RAS_Free(&(WT__->RaS[WT_MEM_LOWER]));
  RAS_Free(&(WT__->RaS[WT_MEM_HIGHER]));
}

/*
 * Delete - this functionality is not implemented.
 *
 * @param  WT__  Reference to WT_Struct object.
 * @param  pos__  Position of to be deleted symbol.
 */
void WT_Delete(WT_Struct* WT__, uint32_t pos__) {
  UNUSED(WT__);
  UNUSED(pos__);

  FATAL("Not Implemented (and likely never will...)");
}

/*
 * Insert new symbol into the WT_Struct.
 *
 * @param  WT__  Reference to WT_Struct object.
 * @param  pos__  Position of newly inserted symbol.
 * @param  symb__  To be inserted symbol.
 */
void WT_Insert(WT_Struct* WT__, uint32_t pos__, char symb__) {

#ifdef SYMBOL_UPPER_TRANSLATION_
  TO_UPPER(symb__);
#endif

  switch(symb__) {
    case 'A':
      RAS_Insert(&(WT__->RaS[WT_MEM_WHOLE]), pos__, 0);
      pos__ = RAS_Rank0(WT__->RaS[WT_MEM_WHOLE], pos__);
      RAS_Insert(&(WT__->RaS[WT_MEM_LOWER]), pos__, 0);
      break;
    case 'C':
      RAS_Insert(&(WT__->RaS[WT_MEM_WHOLE]), pos__, 0);
      pos__ = RAS_Rank0(WT__->RaS[WT_MEM_WHOLE], pos__);
      RAS_Insert(&(WT__->RaS[WT_MEM_LOWER]), pos__, 1);
      break;
    case 'G':
      RAS_Insert(&(WT__->RaS[WT_MEM_WHOLE]), pos__, 1);
      pos__ = RAS_Rank(WT__->RaS[WT_MEM_WHOLE], pos__);
      RAS_Insert(&(WT__->RaS[WT_MEM_HIGHER]), pos__, 0);
      break;
    case 'T':
      RAS_Insert(&(WT__->RaS[WT_MEM_WHOLE]), pos__, 1);
      pos__ = RAS_Rank(WT__->RaS[WT_MEM_WHOLE], pos__);
      RAS_Insert(&(WT__->RaS[WT_MEM_HIGHER]), pos__, 1);
      break;
    default:
      FATAL("Unknown symbol");
  }
}

/*
 * Get symbol from given position.
 *
 * @param  WT__  Reference to WT_Struct object.
 * @param  pos__  Query position.
 */
char WT_Get(WT_Struct* WT__, uint32_t pos__) {
  int32_t bit = RAS_Get(WT__->RaS[WT_MEM_WHOLE], pos__);

  switch (bit) {
    case 0:
      pos__ = RAS_Rank0(WT__->RaS[WT_MEM_WHOLE], pos__);
      bit = RAS_Get(WT__->RaS[WT_MEM_LOWER], pos__);
      switch (bit) {
        case 0:
          return 'A';
        case 1:
          return 'C';
      }
      break;
    case 1:
      pos__ = RAS_Rank(WT__->RaS[WT_MEM_WHOLE], pos__);
      bit = RAS_Get(WT__->RaS[WT_MEM_HIGHER], pos__);
      switch (bit) {
        case 0:
          return 'G';
        case 1:
          return 'T';
      }
      break;
  }
  return -1;
}

/*
 * Print underlying bit vectors.
 *
 * @param  WT__  Reference to WT_Struct object.
 */
void WT_Print_BitVectors(WT_Struct* WT__) {
  printf("Main sequence:\n");
  RAS_Print(WT__->RaS[WT_MEM_WHOLE]);
  printf("Left sequence:\n");
  RAS_Print(WT__->RaS[WT_MEM_LOWER]);
  printf("Right sequence:\n");
  RAS_Print(WT__->RaS[WT_MEM_HIGHER]);
}

/*
 * Print symbols saved in the structure.
 *
 * This function is not very effective and should be only used for testing
 * purposes. Here it is used only for testing purposes.
 *
 * @param  WT__  Reference to WT_Struct object.
 */
void WT_Print_Symbols(WT_Struct* WT__) {
  int32_t i;
  int32_t size = RAS_Size(WT__->RaS[WT_MEM_WHOLE]);

  for (i = 0; i < size; i++) {
    printf("%c ", WT_Get(WT__, i));
  }
  printf("\n");
}

/*
 * Rank given symbol in WT_Struct.
 *
 * @param  WT__  Reference to WT_Struct object.
 * @param  pos__  Query position.
 * @param  symb__  Query symbol.
 */
int32_t WT_Rank(WT_Struct* WT__, uint32_t pos__, char symb__) {

#ifdef SYMBOL_UPPER_TRANSLATION_
  TO_UPPER(symb__);
#endif

  switch (symb__) {
    case 'A':
      return RAS_Rank0(WT__->RaS[WT_MEM_LOWER],
                       RAS_Rank0(WT__->RaS[WT_MEM_WHOLE], pos__));
    case 'C':
      return RAS_Rank(WT__->RaS[WT_MEM_LOWER],
                      RAS_Rank0(WT__->RaS[WT_MEM_WHOLE], pos__));
    case 'G':
      return RAS_Rank0(WT__->RaS[WT_MEM_HIGHER],
                       RAS_Rank(WT__->RaS[WT_MEM_WHOLE], pos__));
    case 'T':
      return RAS_Rank(WT__->RaS[WT_MEM_HIGHER],
                      RAS_Rank(WT__->RaS[WT_MEM_WHOLE], pos__));
  }
  FATAL("Unknown symbol");
  return 0;
}

/*
 * Select given symbol in WT_Struct.
 *
 * @param  WT__  Reference to WT_Struct object.
 * @param  num__  Query count.
 * @param  symb__  Query symbol.
 */
int32_t WT_Select(WT_Struct* WT__, uint32_t num__, char symb__) {

#ifdef SYMBOL_UPPER_TRANSLATION_
  TO_UPPER(symb__);
#endif

  switch (symb__) {
    case 'A':
      return RAS_Select0(WT__->RaS[WT_MEM_WHOLE],
                         RAS_Select0(WT__->RaS[WT_MEM_LOWER], num__));
    case 'C':
      return RAS_Select0(WT__->RaS[WT_MEM_WHOLE],
                         RAS_Select(WT__->RaS[WT_MEM_LOWER], num__));
    case 'G':
      return RAS_Select(WT__->RaS[WT_MEM_WHOLE],
                        RAS_Select0(WT__->RaS[WT_MEM_HIGHER], num__));
    case 'T':
      return RAS_Select(WT__->RaS[WT_MEM_WHOLE],
                        RAS_Select(WT__->RaS[WT_MEM_HIGHER], num__));
  }
  FATAL("Unknown symbol");
  return 0;
}


#endif  // _WT_STRUCTURE__
