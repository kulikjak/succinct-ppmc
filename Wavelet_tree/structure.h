
#ifndef _WT_STRUCTURE__
#define _WT_STRUCTURE__

#include <stdio.h>

#include "../RaS_Complex/memory.h"
#include "../RaS_Complex/structure.h"


typedef struct WT_DNA {
  memory_32b* mem_whole;
  memory_32b* mem_lower;
  memory_32b* mem_higher;

  int32_t RaS_Root_whole;
  int32_t RaS_Root_lower;
  int32_t RaS_Root_higher;
} WT_DNA;


void WT_Init(WT_DNA* tree__) {
  tree__->mem_whole = init_memory();
  tree__->mem_lower = init_memory();
  tree__->mem_higher = init_memory();

  STACK_CLEAN();
  tree__->RaS_Root_whole = RaS_Init(tree__->mem_whole);
  tree__->RaS_Root_lower = RaS_Init(tree__->mem_lower);
  tree__->RaS_Root_higher = RaS_Init(tree__->mem_higher);
}

void WT_Free(WT_DNA** tree__) {
  clean_memory(&((*tree__)->mem_whole));
  clean_memory(&((*tree__)->mem_lower));
  clean_memory(&((*tree__)->mem_higher));

  tree__ = NULL;
}

void WT_Delete(WT_DNA* tree__, uint32_t pos__) {
  UNUSED(tree__);
  UNUSED(pos__);

  FATAL("Not Implemented (and never will...)");
}

void WT_Insert(WT_DNA* tree__, uint32_t pos__, char symb__) {
  int newa;
  switch (symb__) {
    case 'A':
      RaS_Insert(tree__->mem_whole, &tree__->RaS_Root_whole, pos__, 0);
      newa = RaS_Rank0(tree__->mem_whole, tree__->RaS_Root_whole, pos__);
      RaS_Insert(tree__->mem_lower, &tree__->RaS_Root_lower, newa, 0);
      break;
    case 'C':
      RaS_Insert(tree__->mem_whole, &tree__->RaS_Root_whole, pos__, 0);
      newa = RaS_Rank0(tree__->mem_whole, tree__->RaS_Root_whole, pos__);
      RaS_Insert(tree__->mem_lower, &tree__->RaS_Root_lower, newa, 1);
      break;
    case 'G':
      RaS_Insert(tree__->mem_whole, &tree__->RaS_Root_whole, pos__, 1);
      newa = RaS_Rank(tree__->mem_whole, tree__->RaS_Root_whole, pos__);
      RaS_Insert(tree__->mem_higher, &tree__->RaS_Root_higher, newa, 0);
      break;
    case 'T':
      RaS_Insert(tree__->mem_whole, &tree__->RaS_Root_whole, pos__, 1);
      newa = RaS_Rank(tree__->mem_whole, tree__->RaS_Root_whole, pos__);
      RaS_Insert(tree__->mem_higher, &tree__->RaS_Root_higher, newa, 1);
      break;
    default:
      FATAL("Unknown symbol");
  }
}

int32_t WT_Rank(WT_DNA* tree__, uint32_t pos__, char symb__) {
  switch (symb__) {
    case 'A':
      return RaS_Rank0(
          tree__->mem_lower, tree__->RaS_Root_lower,
          RaS_Rank0(tree__->mem_whole, tree__->RaS_Root_whole, pos__));
    case 'C':
      return RaS_Rank(
          tree__->mem_lower, tree__->RaS_Root_lower,
          RaS_Rank0(tree__->mem_whole, tree__->RaS_Root_whole, pos__));
    case 'G':
      return RaS_Rank0(
          tree__->mem_higher, tree__->RaS_Root_higher,
          RaS_Rank(tree__->mem_whole, tree__->RaS_Root_whole, pos__));
    case 'T':
      return RaS_Rank(
          tree__->mem_higher, tree__->RaS_Root_higher,
          RaS_Rank(tree__->mem_whole, tree__->RaS_Root_whole, pos__));
  }
  FATAL("Unknown symbol");
  return 0;
}

int32_t WT_Select(WT_DNA* tree__, uint32_t num__, char symb__) {
  switch (symb__) {
    case 'A':
      return RaS_Select0(
          tree__->mem_whole, tree__->RaS_Root_whole,
          RaS_Select0(tree__->mem_lower, tree__->RaS_Root_lower, num__));
    case 'C':
      return RaS_Select0(
          tree__->mem_whole, tree__->RaS_Root_whole,
          RaS_Select(tree__->mem_lower, tree__->RaS_Root_lower, num__));
    case 'G':
      return RaS_Select(
          tree__->mem_whole, tree__->RaS_Root_whole,
          RaS_Select0(tree__->mem_higher, tree__->RaS_Root_higher, num__));
    case 'T':
      return RaS_Select(
          tree__->mem_whole, tree__->RaS_Root_whole,
          RaS_Select(tree__->mem_higher, tree__->RaS_Root_higher, num__));
  }
  FATAL("Unknown symbol");
  return 0;
}

char WT_Get(WT_DNA* tree__, uint32_t pos__) {
  int32_t pos2;
  int32_t bit = RaS_Get(tree__->mem_whole, tree__->RaS_Root_whole, pos__);

  switch (bit) {
    case -1:
      return -1;
    case 0:
      pos2 = RaS_Rank0(tree__->mem_whole, tree__->RaS_Root_whole, pos__);
      bit = RaS_Get(tree__->mem_lower, tree__->RaS_Root_lower, pos2);
      switch (bit) {
        case -1:
          return -1;
        case 0:
          return 'A';
        case 1:
          return 'C';
      }
    case 1:
      pos2 = RaS_Rank(tree__->mem_whole, tree__->RaS_Root_whole, pos__);
      bit = RaS_Get(tree__->mem_higher, tree__->RaS_Root_higher, pos2);
      switch (bit) {
        case -1:
          return -1;
        case 0:
          return 'G';
        case 1:
          return 'T';
      }
  }

  return 0;
}

void WT_Print_BitVectors(WT_DNA* tree__) {
  printf("Main sequence:\n");
  RaS_Print(tree__->mem_whole, tree__->RaS_Root_whole);
  printf("Left sequence:\n");
  RaS_Print(tree__->mem_lower, tree__->RaS_Root_lower);
  printf("Right sequence:\n");
  RaS_Print(tree__->mem_higher, tree__->RaS_Root_higher);
}

// not very effective but not that important
void WT_Print_Letters(WT_DNA* tree__) {
  int32_t i;
  int32_t size = RaS_Size(tree__->mem_whole, tree__->RaS_Root_whole);

  for (i = 0; i < size; i++) {
    printf("%c ", WT_Get(tree__, i));
  }
  printf("\n");
}


#endif  // _WT_STRUCTURE__
