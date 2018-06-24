#ifndef _WT_EXT_STRUCTURE__
#define _WT_EXT_STRUCTURE__

#include <stdio.h>

#include "../RaS_Complex/memory.h"
#include "../RaS_Complex/structure.h"
#include "../shared/utils.h"


#define WT_MEM_WHOLE 0
#define WT_MEM_LOWER 1
#define WT_MEM_HIGHER 2
#define WT_MEM_EXTRA 3

// #define SYMBOL_UPPER_TRANSLATION_

#define TO_UPPER(symb__) {              \
    if (symb__ >= 'a' && symb__ <= 'z') \
      symb__ = symb__ - ('a' - 'A');    \
  }

#define GET_VALUE(val__, symb__) {  \
  switch (symb__) {                 \
    case 'A': val__ = 0; break;     \
    case 'C': val__ = 1; break;     \
    case 'G': val__ = 2; break;     \
    case 'T': val__ = 3; break;     \
    case '$': val__ = 4; break;     \
    default:                        \
      FATAL("Unknown symbol");      \
  }                                 \
}

typedef struct {
  RAS_Struct RaS[4];
} WT_Struct;

/*
 * Initialize WT_Struct object given as an argument.
 *
 * Since this is for DNA, this structure is for 5 symbols only and mapping is
 * done automatically (last symbol being special $ symbol used in deBurijn
 * graphs). More variable structure can be foud in variable.h
 *
 * @param  WT__  Reference to WT_Struct object.
 */
void WT_Init(WT_Struct* WT__) {
  RAS_Init(&(WT__->RaS[WT_MEM_WHOLE]));
  RAS_Init(&(WT__->RaS[WT_MEM_LOWER]));
  RAS_Init(&(WT__->RaS[WT_MEM_HIGHER]));
  RAS_Init(&(WT__->RaS[WT_MEM_EXTRA]));
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
  RAS_Free(&(WT__->RaS[WT_MEM_EXTRA]));
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
 * @param  symb__  To be inserted symbol value.
 */
void WT_VInsert(WT_Struct* WT__, uint32_t pos__, int8_t symb__) {
  switch(symb__) {
    case 0:
      RAS_Insert(&(WT__->RaS[WT_MEM_WHOLE]), pos__, 0);
      pos__ = RAS_Rank0(WT__->RaS[WT_MEM_WHOLE], pos__);
      RAS_Insert(&(WT__->RaS[WT_MEM_LOWER]), pos__, 0);
      break;
    case 1:
      RAS_Insert(&(WT__->RaS[WT_MEM_WHOLE]), pos__, 0);
      pos__ = RAS_Rank0(WT__->RaS[WT_MEM_WHOLE], pos__);
      RAS_Insert(&(WT__->RaS[WT_MEM_LOWER]), pos__, 1);
      break;
    case 2:
      RAS_Insert(&(WT__->RaS[WT_MEM_WHOLE]), pos__, 1);
      pos__ = RAS_Rank(WT__->RaS[WT_MEM_WHOLE], pos__);
      RAS_Insert(&(WT__->RaS[WT_MEM_HIGHER]), pos__, 0);
      break;
    case 3:
      RAS_Insert(&(WT__->RaS[WT_MEM_WHOLE]), pos__, 1);
      pos__ = RAS_Rank(WT__->RaS[WT_MEM_WHOLE], pos__);
      RAS_Insert(&(WT__->RaS[WT_MEM_HIGHER]), pos__, 1);
      pos__ = RAS_Rank(WT__->RaS[WT_MEM_HIGHER], pos__);
      RAS_Insert(&(WT__->RaS[WT_MEM_EXTRA]), pos__, 0);
      break;
    case 4:
      RAS_Insert(&(WT__->RaS[WT_MEM_WHOLE]), pos__, 1);
      pos__ = RAS_Rank(WT__->RaS[WT_MEM_WHOLE], pos__);
      RAS_Insert(&(WT__->RaS[WT_MEM_HIGHER]), pos__, 1);
      pos__ = RAS_Rank(WT__->RaS[WT_MEM_HIGHER], pos__);
      RAS_Insert(&(WT__->RaS[WT_MEM_EXTRA]), pos__, 1);
      break;
    default:
      FATAL("Unknown symbol");
  }
}

/*
 * Insert new symbol into the WT_Struct.
 *
 * @param  WT__  Reference to WT_Struct object.
 * @param  pos__  Position of newly inserted symbol.
 * @param  symb__  To be inserted symbol.
 */
void WT_Insert(WT_Struct* WT__, uint32_t pos__, char symb__) {
  int8_t val;

#ifdef SYMBOL_UPPER_TRANSLATION_
  TO_UPPER(symb__);
#endif

  GET_VALUE(val, symb__)
  WT_VInsert(WT__, pos__, val);
}

/*
 * Get symbol value from given position.
 *
 * @param  WT__  Reference to WT_Struct object.
 * @param  pos__  Query position.
 */
int8_t WT_VGet(WT_Struct* WT__, uint32_t pos__) {
  int32_t bit = RAS_Get(WT__->RaS[WT_MEM_WHOLE], pos__);

  switch (bit) {
    case 0:
      pos__ = RAS_Rank0(WT__->RaS[WT_MEM_WHOLE], pos__);
      return RAS_Get(WT__->RaS[WT_MEM_LOWER], pos__);
    case 1:
      pos__ = RAS_Rank(WT__->RaS[WT_MEM_WHOLE], pos__);
      bit = RAS_Get(WT__->RaS[WT_MEM_HIGHER], pos__);
      switch (bit) {
        case 0:
          return 2;
        case 1:
          pos__ = RAS_Rank(WT__->RaS[WT_MEM_HIGHER], pos__);
          return RAS_Get(WT__->RaS[WT_MEM_EXTRA], pos__) + 3;
      }
      break;
  }
  return -1;
}

/*
 * Get symbol from given position.
 *
 * @param  WT__  Reference to WT_Struct object.
 * @param  pos__  Query position.
 */
char WT_Get(WT_Struct* WT__, uint32_t pos__) {
  int8_t val = WT_VGet(WT__, pos__);
  char map[] = {'A', 'C', 'G', 'T', '$'};

  return (val == -1) ? -1 : map[val];
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
  printf("Extra sequence:\n");
  RAS_Print(WT__->RaS[WT_MEM_EXTRA]);
}

/*
 * Print symbols saved in the structure.
 *
 * This function is not very effecient and should be only used for testing
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
 * Rank given symbol value in WT_Struct.
 *
 * @param  WT__  Reference to WT_Struct object.
 * @param  pos__  Query position.
 * @param  symb__  Query symbol value.
 */
int32_t WT_VRank(WT_Struct* WT__, uint32_t pos__, int8_t symb__) {
  switch (symb__) {
    case 0:
      return RAS_Rank0(WT__->RaS[WT_MEM_LOWER],
                       RAS_Rank0(WT__->RaS[WT_MEM_WHOLE], pos__));
    case 1:
      return RAS_Rank(WT__->RaS[WT_MEM_LOWER],
                      RAS_Rank0(WT__->RaS[WT_MEM_WHOLE], pos__));
    case 2:
      return RAS_Rank0(WT__->RaS[WT_MEM_HIGHER],
                       RAS_Rank(WT__->RaS[WT_MEM_WHOLE], pos__));
    case 3:
      return RAS_Rank0(WT__->RaS[WT_MEM_EXTRA],
                       RAS_Rank(WT__->RaS[WT_MEM_HIGHER],
                                RAS_Rank(WT__->RaS[WT_MEM_WHOLE], pos__)));
    case 4:
      return RAS_Rank(WT__->RaS[WT_MEM_EXTRA],
                      RAS_Rank(WT__->RaS[WT_MEM_HIGHER],
                               RAS_Rank(WT__->RaS[WT_MEM_WHOLE], pos__)));
  }
  FATAL("Unknown symbol");
  return 0;  // Unreachable
}

/*
 * Rank given symbol in WT_Struct.
 *
 * @param  WT__  Reference to WT_Struct object.
 * @param  pos__  Query position.
 * @param  symb__  Query symbol.
 */
int32_t WT_Rank(WT_Struct* WT__, uint32_t pos__, char symb__) {
  int8_t val;

#ifdef SYMBOL_UPPER_TRANSLATION_
  TO_UPPER(symb__);
#endif

  GET_VALUE(val, symb__)
  return WT_VRank(WT__, pos__, val);
}

/*
 * Select given symbol value in WT_Struct.
 *
 * @param  WT__  Reference to WT_Struct object.
 * @param  num__  Query count.
 * @param  symb__  Query symbol value.
 */
int32_t WT_VSelect(WT_Struct* WT__, uint32_t num__, int8_t symb__) {
  switch (symb__) {
    case 0:
      return RAS_Select0(WT__->RaS[WT_MEM_WHOLE],
                         RAS_Select0(WT__->RaS[WT_MEM_LOWER], num__));
    case 1:
      return RAS_Select0(WT__->RaS[WT_MEM_WHOLE],
                         RAS_Select(WT__->RaS[WT_MEM_LOWER], num__));
    case 2:
      return RAS_Select(WT__->RaS[WT_MEM_WHOLE],
                        RAS_Select0(WT__->RaS[WT_MEM_HIGHER], num__));
    case 3:
      return RAS_Select(
          WT__->RaS[WT_MEM_WHOLE],
          RAS_Select(WT__->RaS[WT_MEM_HIGHER],
                     RAS_Select0(WT__->RaS[WT_MEM_EXTRA], num__)));
    case 4:
      return RAS_Select(WT__->RaS[WT_MEM_WHOLE],
                        RAS_Select(WT__->RaS[WT_MEM_HIGHER],
                                   RAS_Select(WT__->RaS[WT_MEM_EXTRA], num__)));
  }
  FATAL("Unknown symbol");
  return 0;  // Unreachable
}

/*
 * Select given symbol in WT_Struct.
 *
 * @param  WT__  Reference to WT_Struct object.
 * @param  num__  Query count.
 * @param  symb__  Query symbol.
 */
int32_t WT_Select(WT_Struct* WT__, uint32_t num__, char symb__) {
  int8_t val;

#ifdef SYMBOL_UPPER_TRANSLATION_
  TO_UPPER(symb__);
#endif

  GET_VALUE(val, symb__)
  return WT_VSelect(WT__, num__, val);
}

#endif  // _WT_EXT_STRUCTURE__
