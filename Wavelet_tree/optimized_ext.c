#include "optimized_ext.h"

#define OWTE_MEM_ROOT 0
#define OWTE_MEM_L 1
#define OWTE_MEM_R 2
#define OWTE_MEM_LL 3
#define OWTE_MEM_LR 4
#define OWTE_MEM_RL 5
#define OWTE_MEM_RR 6
#define OWTE_MEM_RRR 7

void OWTE_Init(OWTEStructRef OWTE__) {
  int32_t i;
  for (i = OWTE_MEM_ROOT; i <= OWTE_MEM_RRR; i++)
    DBV_Init(&(OWTE__->DBV_[i]));
}

void OWTE_Free(OWTEStructRef OWTE__) {
  int32_t i;
  for (i = OWTE_MEM_ROOT; i <= OWTE_MEM_RRR; i++)
    DBV_Free(&(OWTE__->DBV_[i]));
}

void OWTE_Delete(OWTEStructRef OWTE__, uint32_t pos__) {  // TODO
  uint32_t tmp, tmp2, bit;

  bit = DBV_Get(&(OWTE__->DBV_[OWTE_MEM_ROOT]), pos__);
  switch (bit) {
    case 0:
      tmp = DBV_Rank0(&(OWTE__->DBV_[OWTE_MEM_ROOT]), pos__);
      DBV_Delete(&(OWTE__->DBV_[OWTE_MEM_ROOT]), pos__);

      if(!DBV_Get(&(OWTE__->DBV_[OWTE_MEM_L]), tmp)) {
        tmp2 = DBV_Rank0(&(OWTE__->DBV_[OWTE_MEM_L]), tmp);
        DBV_Delete(&(OWTE__->DBV_[OWTE_MEM_L]), tmp);
        DBV_Delete(&(OWTE__->DBV_[OWTE_MEM_LL]), tmp2);
      } else {
        tmp2 = DBV_Rank(&(OWTE__->DBV_[OWTE_MEM_L]), tmp);
        DBV_Delete(&(OWTE__->DBV_[OWTE_MEM_L]), tmp);
        DBV_Delete(&(OWTE__->DBV_[OWTE_MEM_LR]), tmp2);
      }
      break;
    case 1:
      tmp = DBV_Rank(&(OWTE__->DBV_[OWTE_MEM_ROOT]), pos__);
      DBV_Delete(&(OWTE__->DBV_[OWTE_MEM_ROOT]), pos__);

      if(!DBV_Get(&(OWTE__->DBV_[OWTE_MEM_R]), tmp)) {
        tmp2 = DBV_Rank0(&(OWTE__->DBV_[OWTE_MEM_R]), tmp);
        DBV_Delete(&(OWTE__->DBV_[OWTE_MEM_R]), tmp);
        DBV_Delete(&(OWTE__->DBV_[OWTE_MEM_RL]), tmp2);
      } else {
        tmp2 = DBV_Rank(&(OWTE__->DBV_[OWTE_MEM_R]), tmp);
        DBV_Delete(&(OWTE__->DBV_[OWTE_MEM_R]), tmp);

        if (DBV_Get(&(OWTE__->DBV_[OWTE_MEM_RR]), tmp2)) {
          tmp = DBV_Rank(&(OWTE__->DBV_[OWTE_MEM_RR]), tmp2);
          DBV_Delete(&(OWTE__->DBV_[OWTE_MEM_RRR]), tmp);
        }
        DBV_Delete(&(OWTE__->DBV_[OWTE_MEM_RR]), tmp2);
      }
      break;
    default:
      UNREACHABLE;
  }
}

void OWTE_Insert(OWTEStructRef OWTE__, uint32_t pos__, int8_t symb__) {
  switch (symb__) {
    case 0:
      DBV_Insert(&(OWTE__->DBV_[OWTE_MEM_ROOT]), pos__, 0);
      pos__ = DBV_Rank0(&(OWTE__->DBV_[OWTE_MEM_ROOT]), pos__);
      DBV_Insert(&(OWTE__->DBV_[OWTE_MEM_L]), pos__, 0);
      pos__ = DBV_Rank0(&(OWTE__->DBV_[OWTE_MEM_L]), pos__);
      DBV_Insert(&(OWTE__->DBV_[OWTE_MEM_LL]), pos__, 0);
      break;
    case 1:
      DBV_Insert(&(OWTE__->DBV_[OWTE_MEM_ROOT]), pos__, 0);
      pos__ = DBV_Rank0(&(OWTE__->DBV_[OWTE_MEM_ROOT]), pos__);
      DBV_Insert(&(OWTE__->DBV_[OWTE_MEM_L]), pos__, 0);
      pos__ = DBV_Rank0(&(OWTE__->DBV_[OWTE_MEM_L]), pos__);
      DBV_Insert(&(OWTE__->DBV_[OWTE_MEM_LL]), pos__, 1);
      break;
    case 2:
      DBV_Insert(&(OWTE__->DBV_[OWTE_MEM_ROOT]), pos__, 0);
      pos__ = DBV_Rank0(&(OWTE__->DBV_[OWTE_MEM_ROOT]), pos__);
      DBV_Insert(&(OWTE__->DBV_[OWTE_MEM_L]), pos__, 1);
      pos__ = DBV_Rank(&(OWTE__->DBV_[OWTE_MEM_L]), pos__);
      DBV_Insert(&(OWTE__->DBV_[OWTE_MEM_LR]), pos__, 0);
      break;
    case 3:
      DBV_Insert(&(OWTE__->DBV_[OWTE_MEM_ROOT]), pos__, 0);
      pos__ = DBV_Rank0(&(OWTE__->DBV_[OWTE_MEM_ROOT]), pos__);
      DBV_Insert(&(OWTE__->DBV_[OWTE_MEM_L]), pos__, 1);
      pos__ = DBV_Rank(&(OWTE__->DBV_[OWTE_MEM_L]), pos__);
      DBV_Insert(&(OWTE__->DBV_[OWTE_MEM_LR]), pos__, 1);
      break;
    case 4:
      DBV_Insert(&(OWTE__->DBV_[OWTE_MEM_ROOT]), pos__, 1);
      pos__ = DBV_Rank(&(OWTE__->DBV_[OWTE_MEM_ROOT]), pos__);
      DBV_Insert(&(OWTE__->DBV_[OWTE_MEM_R]), pos__, 0);
      pos__ = DBV_Rank0(&(OWTE__->DBV_[OWTE_MEM_R]), pos__);
      DBV_Insert(&(OWTE__->DBV_[OWTE_MEM_RL]), pos__, 0);
      break;
    case 5:
      DBV_Insert(&(OWTE__->DBV_[OWTE_MEM_ROOT]), pos__, 1);
      pos__ = DBV_Rank(&(OWTE__->DBV_[OWTE_MEM_ROOT]), pos__);
      DBV_Insert(&(OWTE__->DBV_[OWTE_MEM_R]), pos__, 0);
      pos__ = DBV_Rank0(&(OWTE__->DBV_[OWTE_MEM_R]), pos__);
      DBV_Insert(&(OWTE__->DBV_[OWTE_MEM_RL]), pos__, 1);
      break;
    case 6:
      DBV_Insert(&(OWTE__->DBV_[OWTE_MEM_ROOT]), pos__, 1);
      pos__ = DBV_Rank(&(OWTE__->DBV_[OWTE_MEM_ROOT]), pos__);
      DBV_Insert(&(OWTE__->DBV_[OWTE_MEM_R]), pos__, 1);
      pos__ = DBV_Rank(&(OWTE__->DBV_[OWTE_MEM_R]), pos__);
      DBV_Insert(&(OWTE__->DBV_[OWTE_MEM_RR]), pos__, 0);
      break;
    case 7:
      DBV_Insert(&(OWTE__->DBV_[OWTE_MEM_ROOT]), pos__, 1);
      pos__ = DBV_Rank(&(OWTE__->DBV_[OWTE_MEM_ROOT]), pos__);
      DBV_Insert(&(OWTE__->DBV_[OWTE_MEM_R]), pos__, 1);
      pos__ = DBV_Rank(&(OWTE__->DBV_[OWTE_MEM_R]), pos__);
      DBV_Insert(&(OWTE__->DBV_[OWTE_MEM_RR]), pos__, 1);
      pos__ = DBV_Rank(&(OWTE__->DBV_[OWTE_MEM_RR]), pos__);
      DBV_Insert(&(OWTE__->DBV_[OWTE_MEM_RRR]), pos__, 0);
      break;
    case 8:
      DBV_Insert(&(OWTE__->DBV_[OWTE_MEM_ROOT]), pos__, 1);
      pos__ = DBV_Rank(&(OWTE__->DBV_[OWTE_MEM_ROOT]), pos__);
      DBV_Insert(&(OWTE__->DBV_[OWTE_MEM_R]), pos__, 1);
      pos__ = DBV_Rank(&(OWTE__->DBV_[OWTE_MEM_R]), pos__);
      DBV_Insert(&(OWTE__->DBV_[OWTE_MEM_RR]), pos__, 1);
      pos__ = DBV_Rank(&(OWTE__->DBV_[OWTE_MEM_RR]), pos__);
      DBV_Insert(&(OWTE__->DBV_[OWTE_MEM_RRR]), pos__, 1);
      break;
    default:
      FATAL("Unknown symbol");
  }
}

int8_t OWTE_Get(OWTEStructRef OWTE__, uint32_t pos__) {
  int32_t bit = DBV_Get(&(OWTE__->DBV_[OWTE_MEM_ROOT]), pos__);

  switch (bit) {
    case 0:
      pos__ = DBV_Rank0(&(OWTE__->DBV_[OWTE_MEM_ROOT]), pos__);
      if(!DBV_Get(&(OWTE__->DBV_[OWTE_MEM_L]), pos__))
        return DBV_Get(&(OWTE__->DBV_[OWTE_MEM_LL]),
                DBV_Rank0(&(OWTE__->DBV_[OWTE_MEM_L]), pos__));
      return DBV_Get(&(OWTE__->DBV_[OWTE_MEM_LR]),
              DBV_Rank(&(OWTE__->DBV_[OWTE_MEM_L]), pos__)) + 2;
    case 1:
      pos__ = DBV_Rank(&(OWTE__->DBV_[OWTE_MEM_ROOT]), pos__);
      if(!DBV_Get(&(OWTE__->DBV_[OWTE_MEM_R]), pos__))
        return DBV_Get(&(OWTE__->DBV_[OWTE_MEM_RL]),
                DBV_Rank0(&(OWTE__->DBV_[OWTE_MEM_R]), pos__)) + 4;
      pos__ = DBV_Rank(&(OWTE__->DBV_[OWTE_MEM_R]), pos__);
      if (!DBV_Get(&(OWTE__->DBV_[OWTE_MEM_RR]), pos__))
        return 6;
      return DBV_Get(&(OWTE__->DBV_[OWTE_MEM_RRR]),
              DBV_Rank(&(OWTE__->DBV_[OWTE_MEM_RR]), pos__)) + 7;
  }
  UNREACHABLE
  return -1;
}

void OWTE_Print_BitVectors(OWTEStructRef OWTE__) {
  printf("Root sequence:\n");
  DBV_Print(&(OWTE__->DBV_[OWTE_MEM_ROOT]));
  printf("\nLeft sequence:\n");
  DBV_Print(&(OWTE__->DBV_[OWTE_MEM_L]));
  printf("\nRight sequence:\n");
  DBV_Print(&(OWTE__->DBV_[OWTE_MEM_R]));
  printf("\nLeft Left sequence:\n");
  DBV_Print(&(OWTE__->DBV_[OWTE_MEM_LL]));
  printf("\nLeft Right sequence:\n");
  DBV_Print(&(OWTE__->DBV_[OWTE_MEM_LR]));
  printf("\nRight Left sequence:\n");
  DBV_Print(&(OWTE__->DBV_[OWTE_MEM_RL]));
  printf("\nRight Right sequence:\n");
  DBV_Print(&(OWTE__->DBV_[OWTE_MEM_RR]));
  printf("\nRight Right Right sequence:\n");
  DBV_Print(&(OWTE__->DBV_[OWTE_MEM_RRR]));
}

void OWTE_Print_Symbols(OWTEStructRef OWTE__) {
  int32_t size = DBV_Size(&(OWTE__->DBV_[OWTE_MEM_ROOT]));

  for (int32_t i = 0; i < size; i++)
    printf("%d ", OWTE_Get(OWTE__, i));
  printf("\n");
}

int32_t OWTE_Rank(OWTEStructRef OWTE__, uint32_t pos__, int8_t symb__) {
  switch (symb__) {
    case 0:
      return DBV_Rank0(&(OWTE__->DBV_[OWTE_MEM_LL]),
             DBV_Rank0(&(OWTE__->DBV_[OWTE_MEM_L]),
             DBV_Rank0(&(OWTE__->DBV_[OWTE_MEM_ROOT]), pos__)));
    case 1:
      return DBV_Rank(&(OWTE__->DBV_[OWTE_MEM_LL]),
             DBV_Rank0(&(OWTE__->DBV_[OWTE_MEM_L]),
             DBV_Rank0(&(OWTE__->DBV_[OWTE_MEM_ROOT]), pos__)));
    case 2:
      return DBV_Rank0(&(OWTE__->DBV_[OWTE_MEM_LR]),
             DBV_Rank(&(OWTE__->DBV_[OWTE_MEM_L]),
             DBV_Rank0(&(OWTE__->DBV_[OWTE_MEM_ROOT]), pos__)));
    case 3:
      return DBV_Rank(&(OWTE__->DBV_[OWTE_MEM_LR]),
             DBV_Rank(&(OWTE__->DBV_[OWTE_MEM_L]),
             DBV_Rank0(&(OWTE__->DBV_[OWTE_MEM_ROOT]), pos__)));
    case 4:
      return DBV_Rank0(&(OWTE__->DBV_[OWTE_MEM_RL]),
             DBV_Rank0(&(OWTE__->DBV_[OWTE_MEM_R]),
             DBV_Rank(&(OWTE__->DBV_[OWTE_MEM_ROOT]), pos__)));
    case 5:
      return DBV_Rank(&(OWTE__->DBV_[OWTE_MEM_RL]),
             DBV_Rank0(&(OWTE__->DBV_[OWTE_MEM_R]),
             DBV_Rank(&(OWTE__->DBV_[OWTE_MEM_ROOT]), pos__)));
    case 6:
      return DBV_Rank0(&(OWTE__->DBV_[OWTE_MEM_RR]),
             DBV_Rank(&(OWTE__->DBV_[OWTE_MEM_R]),
             DBV_Rank(&(OWTE__->DBV_[OWTE_MEM_ROOT]), pos__)));
    case 7:
      return DBV_Rank0(&(OWTE__->DBV_[OWTE_MEM_RRR]),
             DBV_Rank(&(OWTE__->DBV_[OWTE_MEM_RR]),
             DBV_Rank(&(OWTE__->DBV_[OWTE_MEM_R]),
             DBV_Rank(&(OWTE__->DBV_[OWTE_MEM_ROOT]), pos__))));
    case 8:
      return DBV_Rank(&(OWTE__->DBV_[OWTE_MEM_RRR]),
             DBV_Rank(&(OWTE__->DBV_[OWTE_MEM_RR]),
             DBV_Rank(&(OWTE__->DBV_[OWTE_MEM_R]),
             DBV_Rank(&(OWTE__->DBV_[OWTE_MEM_ROOT]), pos__))));
  }
  FATAL("Unknown symbol");
  return 0;
}

int32_t OWTE_Select(OWTEStructRef OWTE__, uint32_t num__, int8_t symb__) {
  switch (symb__) {
    case 0:
      return DBV_Select0(&(OWTE__->DBV_[OWTE_MEM_ROOT]),
             DBV_Select0(&(OWTE__->DBV_[OWTE_MEM_L]),
             DBV_Select0(&(OWTE__->DBV_[OWTE_MEM_LL]), num__)));
    case 1:
      return DBV_Select0(&(OWTE__->DBV_[OWTE_MEM_ROOT]),
             DBV_Select0(&(OWTE__->DBV_[OWTE_MEM_L]),
             DBV_Select(&(OWTE__->DBV_[OWTE_MEM_LL]), num__)));
    case 2:
      return DBV_Select0(&(OWTE__->DBV_[OWTE_MEM_ROOT]),
             DBV_Select(&(OWTE__->DBV_[OWTE_MEM_L]),
             DBV_Select0(&(OWTE__->DBV_[OWTE_MEM_LR]), num__)));
    case 3:
      return DBV_Select0(&(OWTE__->DBV_[OWTE_MEM_ROOT]),
             DBV_Select(&(OWTE__->DBV_[OWTE_MEM_L]),
             DBV_Select(&(OWTE__->DBV_[OWTE_MEM_LR]), num__)));
    case 4:
      return DBV_Select(&(OWTE__->DBV_[OWTE_MEM_ROOT]),
             DBV_Select0(&(OWTE__->DBV_[OWTE_MEM_R]),
             DBV_Select0(&(OWTE__->DBV_[OWTE_MEM_RL]), num__)));
    case 5:
      return DBV_Select(&(OWTE__->DBV_[OWTE_MEM_ROOT]),
             DBV_Select0(&(OWTE__->DBV_[OWTE_MEM_R]),
             DBV_Select(&(OWTE__->DBV_[OWTE_MEM_RL]), num__)));
    case 6:
      return DBV_Select(&(OWTE__->DBV_[OWTE_MEM_ROOT]),
             DBV_Select(&(OWTE__->DBV_[OWTE_MEM_R]),
             DBV_Select0(&(OWTE__->DBV_[OWTE_MEM_RR]), num__)));
    case 7:
      return DBV_Select(&(OWTE__->DBV_[OWTE_MEM_ROOT]),
             DBV_Select(&(OWTE__->DBV_[OWTE_MEM_R]),
             DBV_Select(&(OWTE__->DBV_[OWTE_MEM_RR]),
             DBV_Select0(&(OWTE__->DBV_[OWTE_MEM_RRR]), num__))));
    case 8:
      return DBV_Select(&(OWTE__->DBV_[OWTE_MEM_ROOT]),
             DBV_Select(&(OWTE__->DBV_[OWTE_MEM_R]),
             DBV_Select(&(OWTE__->DBV_[OWTE_MEM_RR]),
             DBV_Select(&(OWTE__->DBV_[OWTE_MEM_RRR]), num__))));
  }
  FATAL("Unknown symbol");
  return 0;
}
