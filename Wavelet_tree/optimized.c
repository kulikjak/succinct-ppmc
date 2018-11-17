#include "optimized.h"

#define OWT_MEM_WHOLE 0
#define OWT_MEM_LOWER 1
#define OWT_MEM_HIGHER 2
#define OWT_MEM_EXTRA 3

void OWT_Init(OWTStructRef OWT__) {
  DBV_Init(&(OWT__->DBV_[OWT_MEM_WHOLE]));
  DBV_Init(&(OWT__->DBV_[OWT_MEM_LOWER]));
  DBV_Init(&(OWT__->DBV_[OWT_MEM_HIGHER]));
  DBV_Init(&(OWT__->DBV_[OWT_MEM_EXTRA]));
}

void OWT_Free(OWTStructRef OWT__) {
  DBV_Free(&(OWT__->DBV_[OWT_MEM_WHOLE]));
  DBV_Free(&(OWT__->DBV_[OWT_MEM_LOWER]));
  DBV_Free(&(OWT__->DBV_[OWT_MEM_HIGHER]));
  DBV_Free(&(OWT__->DBV_[OWT_MEM_EXTRA]));
}

void OWT_Delete(OWTStructRef OWT__, uint32_t pos__) {
  uint32_t tmp, bit, tmp2;

  bit = DBV_Get(&(OWT__->DBV_[OWT_MEM_WHOLE]), pos__);
  switch (bit) {
    case 0:
      tmp = DBV_Rank0(&(OWT__->DBV_[OWT_MEM_WHOLE]), pos__);
      DBV_Delete(&(OWT__->DBV_[OWT_MEM_WHOLE]), pos__);
      DBV_Delete(&(OWT__->DBV_[OWT_MEM_LOWER]), tmp);
      break;
    case 1:
      tmp = DBV_Rank(&(OWT__->DBV_[OWT_MEM_WHOLE]), pos__);
      DBV_Delete(&(OWT__->DBV_[OWT_MEM_WHOLE]), pos__);

      bit = DBV_Get(&(OWT__->DBV_[OWT_MEM_HIGHER]), tmp);
      if (bit) {
        tmp2 = DBV_Rank(&(OWT__->DBV_[OWT_MEM_HIGHER]), tmp);
        DBV_Delete(&(OWT__->DBV_[OWT_MEM_EXTRA]), tmp2);
      }
      DBV_Delete(&(OWT__->DBV_[OWT_MEM_HIGHER]), tmp);
      break;
    default:
      UNREACHABLE;
  }
}

void OWT_Insert(OWTStructRef OWT__, uint32_t pos__, int8_t symb__) {
  switch (symb__) {
    case 0:
      DBV_Insert(&(OWT__->DBV_[OWT_MEM_WHOLE]), pos__, 0);
      pos__ = DBV_Rank0(&(OWT__->DBV_[OWT_MEM_WHOLE]), pos__);
      DBV_Insert(&(OWT__->DBV_[OWT_MEM_LOWER]), pos__, 0);
      break;
    case 1:
      DBV_Insert(&(OWT__->DBV_[OWT_MEM_WHOLE]), pos__, 0);
      pos__ = DBV_Rank0(&(OWT__->DBV_[OWT_MEM_WHOLE]), pos__);
      DBV_Insert(&(OWT__->DBV_[OWT_MEM_LOWER]), pos__, 1);
      break;
    case 2:
      DBV_Insert(&(OWT__->DBV_[OWT_MEM_WHOLE]), pos__, 1);
      pos__ = DBV_Rank(&(OWT__->DBV_[OWT_MEM_WHOLE]), pos__);
      DBV_Insert(&(OWT__->DBV_[OWT_MEM_HIGHER]), pos__, 0);
      break;
    case 3:
      DBV_Insert(&(OWT__->DBV_[OWT_MEM_WHOLE]), pos__, 1);
      pos__ = DBV_Rank(&(OWT__->DBV_[OWT_MEM_WHOLE]), pos__);
      DBV_Insert(&(OWT__->DBV_[OWT_MEM_HIGHER]), pos__, 1);
      pos__ = DBV_Rank(&(OWT__->DBV_[OWT_MEM_HIGHER]), pos__);
      DBV_Insert(&(OWT__->DBV_[OWT_MEM_EXTRA]), pos__, 0);
      break;
    case 4:
      DBV_Insert(&(OWT__->DBV_[OWT_MEM_WHOLE]), pos__, 1);
      pos__ = DBV_Rank(&(OWT__->DBV_[OWT_MEM_WHOLE]), pos__);
      DBV_Insert(&(OWT__->DBV_[OWT_MEM_HIGHER]), pos__, 1);
      pos__ = DBV_Rank(&(OWT__->DBV_[OWT_MEM_HIGHER]), pos__);
      DBV_Insert(&(OWT__->DBV_[OWT_MEM_EXTRA]), pos__, 1);
      break;
    default:
      FATAL("Unknown symbol");
  }
}

int8_t OWT_Get(OWTStructRef OWT__, uint32_t pos__) {
  int32_t bit = DBV_Get(&(OWT__->DBV_[OWT_MEM_WHOLE]), pos__);

  switch (bit) {
    case 0:
      pos__ = DBV_Rank0(&(OWT__->DBV_[OWT_MEM_WHOLE]), pos__);
      return DBV_Get(&(OWT__->DBV_[OWT_MEM_LOWER]), pos__);
    case 1:
      pos__ = DBV_Rank(&(OWT__->DBV_[OWT_MEM_WHOLE]), pos__);
      bit = DBV_Get(&(OWT__->DBV_[OWT_MEM_HIGHER]), pos__);
      switch (bit) {
        case 0:
          return 2;
        case 1:
          pos__ = DBV_Rank(&(OWT__->DBV_[OWT_MEM_HIGHER]), pos__);
          return DBV_Get(&(OWT__->DBV_[OWT_MEM_EXTRA]), pos__) + 3;
      }
      break;
  }
  return -1;
}

void OWT_Print_BitVectors(OWTStructRef OWT__) {
  printf("Main sequence:\n");
  DBV_Print(&(OWT__->DBV_[OWT_MEM_WHOLE]));
  printf("Left sequence:\n");
  DBV_Print(&(OWT__->DBV_[OWT_MEM_LOWER]));
  printf("Right sequence:\n");
  DBV_Print(&(OWT__->DBV_[OWT_MEM_HIGHER]));
  printf("Extra sequence:\n");
  DBV_Print(&(OWT__->DBV_[OWT_MEM_EXTRA]));
}

void OWT_Print_Symbols(OWTStructRef OWT__) {
  int32_t size = DBV_Size(&(OWT__->DBV_[OWT_MEM_WHOLE]));

  for (int32_t i = 0; i < size; i++)
    printf("%d ", OWT_Get(OWT__, i));
  printf("\n");
}

int32_t OWT_Rank(OWTStructRef OWT__, uint32_t pos__, int8_t symb__) {
  switch (symb__) {
    case 0:
      return DBV_Rank0(&(OWT__->DBV_[OWT_MEM_LOWER]),
                       DBV_Rank0(&(OWT__->DBV_[OWT_MEM_WHOLE]), pos__));
    case 1:
      return DBV_Rank(&(OWT__->DBV_[OWT_MEM_LOWER]),
                      DBV_Rank0(&(OWT__->DBV_[OWT_MEM_WHOLE]), pos__));
    case 2:
      return DBV_Rank0(&(OWT__->DBV_[OWT_MEM_HIGHER]),
                       DBV_Rank(&(OWT__->DBV_[OWT_MEM_WHOLE]), pos__));
    case 3:
      return DBV_Rank0(&(OWT__->DBV_[OWT_MEM_EXTRA]),
                       DBV_Rank(&(OWT__->DBV_[OWT_MEM_HIGHER]),
                                DBV_Rank(&(OWT__->DBV_[OWT_MEM_WHOLE]), pos__)));
    case 4:
      return DBV_Rank(&(OWT__->DBV_[OWT_MEM_EXTRA]),
                      DBV_Rank(&(OWT__->DBV_[OWT_MEM_HIGHER]),
                               DBV_Rank(&(OWT__->DBV_[OWT_MEM_WHOLE]), pos__)));
  }
  FATAL("Unknown symbol");
  return 0;
}

int32_t OWT_Select(OWTStructRef OWT__, uint32_t num__, int8_t symb__) {
  switch (symb__) {
    case 0:
      return DBV_Select0(&(OWT__->DBV_[OWT_MEM_WHOLE]),
                         DBV_Select0(&(OWT__->DBV_[OWT_MEM_LOWER]), num__));
    case 1:
      return DBV_Select0(&(OWT__->DBV_[OWT_MEM_WHOLE]),
                         DBV_Select(&(OWT__->DBV_[OWT_MEM_LOWER]), num__));
    case 2:
      return DBV_Select(&(OWT__->DBV_[OWT_MEM_WHOLE]),
                        DBV_Select0(&(OWT__->DBV_[OWT_MEM_HIGHER]), num__));
    case 3:
      return DBV_Select(&(OWT__->DBV_[OWT_MEM_WHOLE]),
                        DBV_Select(&(OWT__->DBV_[OWT_MEM_HIGHER]),
                                   DBV_Select0(&(OWT__->DBV_[OWT_MEM_EXTRA]), num__)));
    case 4:
      return DBV_Select(&(OWT__->DBV_[OWT_MEM_WHOLE]),
                        DBV_Select(&(OWT__->DBV_[OWT_MEM_HIGHER]),
                                   DBV_Select(&(OWT__->DBV_[OWT_MEM_EXTRA]), num__)));
  }
  FATAL("Unknown symbol");
  return 0;
}
