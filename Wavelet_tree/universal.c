#include "universal.h"

void UWT_Init(UWTStructRef UWT__, int32_t scount__) {
  int32_t ncount;

  /* calculate number of WT nodes */
  ncount = scount__ / 2;
  ncount *= 2;
  if (scount__ % 2 == 0)
    ncount -= 1;

  /* allocate and initialize the structure */
  UWT__->DBV_ = (DBVStructRef) malloc (ncount * sizeof(DBV_Struct));
  UWT__->ncount_ = ncount;
  UWT__->scount_ = scount__;

  for (int32_t i = 0; i < ncount; i++)
    DBV_Init(&(UWT__->DBV_[i]));
}

void UWT_Free(UWTStructRef UWT__) {
  for (int32_t i = 0; i < UWT__->ncount_; i++)
    DBV_Free(&(UWT__->DBV_[i]));

  free(UWT__->DBV_);

  UWT__->ncount_ = 0;
  UWT__->scount_ = 0;
}

void UWT_Delete(UWTStructRef UWT__, uint32_t pos__) {
  int32_t curr, level, bit, temp;

  curr = 0;
  level = 0;
  do {
    bit = DBV_Get(&(UWT__->DBV_[curr]), pos__);
    if (bit == 0) {
      temp = DBV_Rank0(&(UWT__->DBV_[curr]), pos__);
      DBV_Delete(&(UWT__->DBV_[curr]), pos__);
      curr = curr * 2 + 1;
    } else {
      temp = DBV_Rank(&(UWT__->DBV_[curr]), pos__);
      DBV_Delete(&(UWT__->DBV_[curr]), pos__);
      curr = (curr + 1) * 2;
    }
    pos__ = temp;
    level++;

  } while (curr < UWT__->ncount_);
}

void UWT_Insert(UWTStructRef UWT__, uint32_t pos__, uint8_t symb__) {
  int32_t curr, level, bit;

  curr = 0;
  level = 0;
  do {
    bit = (universal_map[symb__] >> level) & 0x1;
    if (bit == 0) {
      DBV_Insert(&(UWT__->DBV_[curr]), pos__, 0);
      pos__ = DBV_Rank0(&(UWT__->DBV_[curr]), pos__);

      curr = curr * 2 + 1;
    } else {
      DBV_Insert(&(UWT__->DBV_[curr]), pos__, 1);
      pos__ = DBV_Rank(&(UWT__->DBV_[curr]), pos__);

      curr = (curr + 1) * 2;
    }
    level++;

  } while (curr < UWT__->ncount_);
}

uint8_t UWT_Get(UWTStructRef UWT__, uint32_t pos__) {
  int32_t curr, level, bit, val;

  val = 0;
  curr = 0;
  level = 0;
  do {
    bit = DBV_Get(&(UWT__->DBV_[curr]), pos__);
    if (bit == 0) {
      pos__ = DBV_Rank0(&(UWT__->DBV_[curr]), pos__);
      curr = curr * 2 + 1;
    } else {
      val |= (1 << level);
      pos__ = DBV_Rank(&(UWT__->DBV_[curr]), pos__);
      curr = (curr + 1) * 2;
    }
    level++;

  } while (curr < UWT__->ncount_);

  return universal_map[val];
}

void UWT_Print_BitVectors(UWTStructRef UWT__) {
  printf("Bit vectors in level-order\n");
  for (int32_t i = 0; i < UWT__->ncount_; i++) {
    printf("%d: ", i);
    DBV_Print(&(UWT__->DBV_[i]));
  }
}

void UWT_Print_Symbols(UWTStructRef UWT__) {
  int32_t size = DBV_Size(&(UWT__->DBV_[0]));

  for (int32_t i = 0; i < size; i++) {
    printf("%d ", UWT_Get(UWT__, i));
  }
  printf("\n");
}

int32_t UWT_Rank(UWTStructRef UWT__, uint32_t pos__, uint8_t symb__) {
  int32_t curr, level, bit;

  curr = 0;
  level = 0;
  do {
    bit = (universal_map[symb__] >> level) & 0x1;
    if (bit == 0) {
      pos__ = DBV_Rank0(&(UWT__->DBV_[curr]), pos__);

      curr = curr * 2 + 1;
    } else {
      pos__ = DBV_Rank(&(UWT__->DBV_[curr]), pos__);

      curr = (curr + 1) * 2;
    }
    level++;

  } while (curr < UWT__->ncount_);

  return pos__;
}

int32_t UWT_Select(UWTStructRef UWT__, uint32_t num__, uint8_t symb__) {
  int32_t curr, level, bit, temp;

  temp = 0;
  level = 0;
  do {
    curr = temp;
    bit = (universal_map[symb__] >> level) & 0x1;
    if (bit == 0) {
      temp = curr * 2 + 1;
    } else {
      temp = (curr + 1) * 2;
    }
    level++;

  } while (temp < UWT__->ncount_);

  do {
    level --;

    bit = (universal_map[symb__] >> level) & 0x1;
    if (bit == 0) {
      num__ = DBV_Select0(&(UWT__->DBV_[curr]), num__);
      curr = (curr - 1) / 2;
    } else {
      num__ = DBV_Select(&(UWT__->DBV_[curr]), num__);
      curr = (curr - 1) / 2;
    }
  } while (level);

  return num__;
}

