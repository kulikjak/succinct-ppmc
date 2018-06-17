
#ifndef _VARIABLE_WT_STRUCTURE__
#define _VARIABLE_WT_STRUCTURE__

#include <stdio.h>

#include "../RaS_Complex/memory.h"
#include "../RaS_Complex/structure.h"
#include "variable_utils.h"


typedef struct {
  int32_t ncount_;
  int32_t scount_;
  RAS_Struct *RaS_;

  int32_t (*map_)(uchar);
  uchar (*rmap_)(int32_t);
} WT_Struct;

/*
 * Initialize WT_Struct object given as an argument.
 *
 * To use normal ASCII mapping you can pass WT_DEFAULT_SIZE, WT_DEFAULT_MAP
 * and WT_DEFAULT_RMAP as arguments, however, this is not recommended
 * to use with a small number of symbols as the struct can lose performance.
 *
 * To see how mapping function should work, check test_maps() function in utils.
 *
 * @example WT_Init(&WT, WT_DEFAULT_SIZE, WT_DEFAULT_MAP, WT_DEFAULT_RMAP);
 *
 * @param  WT__  Reference to WT_Struct object.
 * @param  scount__  Number of symbols.
 * @param  map__  Function mapping symbols to numbers.
 * @param  rmap__  Reverse mapping function (numbers to symbols).
 */
void WT_Init(WT_Struct* WT__, int32_t scount__, int32_t (*map__)(uchar), uchar (*rmap__)(int32_t)) {
  int32_t i, ncount;

  // calculate number of WT nodes
  ncount = scount__ / 2;
  ncount *= 2;
  if (scount__ % 2 == 0)
    ncount -= 1;

  // allocate memory
  WT__->RaS_ = (RAS_Struct*) malloc (ncount * sizeof(RAS_Struct));

  // initialize WT structure
  WT__->ncount_ = ncount;
  WT__->scount_ = scount__;

  WT__->map_ = map__;
  WT__->rmap_ = rmap__;

  for (i = 0; i < ncount; i++)
    RAS_Init(&(WT__->RaS_[i]));

#ifdef EXTENDED_TESTS_
	assert(test_maps(scount__, map__, rmap__));
#endif

}

/*
 * Free all memory associated with WT object.
 * 
 * @param  WT__  Reference to WT_Struct object.
 */
void WT_Free(WT_Struct *WT__) {
  int32_t i;

  for (i = 0; i < WT__->ncount_; i++)
    RAS_Free(&(WT__->RaS_[i]));

  free(WT__->RaS_);

  WT__->ncount_ = 0;
  WT__->scount_ = 0;
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
  int32_t curr, level, bit, symb;

  symb = WT__->map_(symb__);

  curr = 0;
  level = 0;
  do {
    bit = (symb >> level) & 0x1;
    if (bit == 0) {
      RAS_Insert(&(WT__->RaS_[curr]), pos__, 0);
      pos__ = RAS_Rank0(WT__->RaS_[curr], pos__);

      curr = curr * 2 + 1;
    } else {
      RAS_Insert(&(WT__->RaS_[curr]), pos__, 1);
      pos__ = RAS_Rank(WT__->RaS_[curr], pos__);

      curr = (curr + 1) * 2;
    }
    level++;

  } while (curr < WT__->ncount_);
}

/*
 * Get symbol from given position.
 * 
 * @param  WT__  Reference to WT_Struct object.
 * @param  pos__  Query position.
 */
char WT_Get(WT_Struct* WT__, uint32_t pos__) {
  int32_t curr, level, bit, val;

  val = 0;
  curr = 0;
  level = 0;
  do {
    bit = RAS_Get(WT__->RaS_[curr], pos__);
    if (bit == 0) {
      pos__ = RAS_Rank0(WT__->RaS_[curr], pos__);
      curr = curr * 2 + 1;
    } else {
      val |= (1 << level);
      pos__ = RAS_Rank(WT__->RaS_[curr], pos__);
      curr = (curr + 1) * 2;
    }
    level++;

  } while (curr < WT__->ncount_);

  return WT__->rmap_(val);
}

/*
 * Print underlying bit vectors.
 *
 * @param  WT__  Reference to WT_Struct object.
 */
void WT_Print_BitVectors(WT_Struct* WT__) {
  int32_t i;

  printf("Bit vectors in level-order\n");
  for (i = 0; i < WT__->ncount_; i++) {
    printf("%d: ", i);
    RAS_Print(WT__->RaS_[i]);
  }
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
  int32_t size = RAS_Size(WT__->RaS_[0]);

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
  int32_t curr, level, bit, symb;

  symb = WT__->map_(symb__);

  curr = 0;
  level = 0;
  do {
    bit = (symb >> level) & 0x1;
    if (bit == 0) {
      pos__ = RAS_Rank0(WT__->RaS_[curr], pos__);

      curr = curr * 2 + 1;
    } else {
      pos__ = RAS_Rank(WT__->RaS_[curr], pos__);

      curr = (curr + 1) * 2;
    }
    level++;

  } while (curr < WT__->ncount_);

  return pos__;
}

/*
 * Select given symbol in WT_Struct.
 *
 * @node This function uses division which is slower than mulitplication and
 * therefore it is slower than corresponding rank function.
 *
 * @param  WT__  Reference to WT_Struct object.
 * @param  num__  Query count.
 * @param  symb__  Query symbol.
 */
int32_t WT_Select(WT_Struct* WT__, uint32_t num__, char symb__) {
  int32_t curr, level, bit, symb, temp;

  symb = WT__->map_(symb__);

  temp = 0;
  level = 0;
  do {
    curr = temp;
    bit = (symb >> level) & 0x1;
    if (bit == 0) {
      temp = curr * 2 + 1;
    } else {
      temp = (curr + 1) * 2;
    }
    level++;

  } while (temp < WT__->ncount_);

  do {
    level --;

    bit = (symb >> level) & 0x1;
    if (bit == 0) {
      num__ = RAS_Select0(WT__->RaS_[curr], num__);
      curr = (curr - 1) / 2;
    } else {
      num__ = RAS_Select(WT__->RaS_[curr], num__);
      curr = (curr - 1) / 2;
    }
  } while (level);

  return num__;
}


#endif  // _VARIABLE_WT_STRUCTURE__
