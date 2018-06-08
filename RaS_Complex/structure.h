
#ifndef _RAS_STRUCTURE__
#define _RAS_STRUCTURE__

#include <stdio.h>

#include "RaS_Get.h"
#include "RaS_Insert.h"
#include "RaS_Print.h"
#include "RaS_Rank.h"
#include "RaS_Select.h"
#include "memory.h"
#include "stack.h"
#include "utils.h"


typedef struct {
  int32_t root_;
  memory_32b* mem_;
} RAS_Struct;



void RAS_Init(RAS_Struct *RAS__) {
  STACK_CLEAN();

  RAS__->mem_ = init_memory();
  RAS__->root_ = new_leaf(RAS__->mem_);

  Leaf_32b* leaf_ref = MEMORY_GET_LEAF(RAS__->mem_, RAS__->root_);

  leaf_ref->p_ = 0;
  leaf_ref->r_ = 0;
}

void RAS_Free(RAS_Struct *RAS__) {
  RAS__->root_ = -1;
  clean_memory(&(RAS__->mem_));
}

void RAS_Print(RAS_Struct RAS__) {
  RaS_Print_32_(RAS__.mem_, RAS__.root_);
  printf("\n");
}

void RAS_Insert(RAS_Struct *RAS__, uint32_t pos__, bool val__) {
  RaS_Insert_32_(RAS__->mem_, &(RAS__->root_), pos__, val__);
}

// Delete is not supported cause it is not needed for deBruijn graphs
void RAS_Delete(RAS_Struct RAS__, uint32_t pos__) {
  UNUSED(RAS__);
  UNUSED(pos__);

  FATAL("Not Implemented");
}

int32_t RAS_Rank(RAS_Struct RAS__, uint32_t pos__) {
  return RaS_Rank_32_(RAS__.mem_, RAS__.root_, pos__);
}

int32_t RAS_Rank0(RAS_Struct RAS__, uint32_t pos__) {
  return pos__ - RaS_Rank_32_(RAS__.mem_, RAS__.root_, pos__);
}

int32_t RAS_Select(RAS_Struct RAS__, uint32_t pos__) {
  return RaS_Select_32_(RAS__.mem_, RAS__.root_, pos__, false);
}

int32_t RAS_Select0(RAS_Struct RAS__, uint32_t pos__) {
  return RaS_Select_32_(RAS__.mem_, RAS__.root_, pos__, true);
}

int32_t RAS_Get(RAS_Struct RAS__, uint32_t pos__) {
  return RaS_Get_32_(RAS__.mem_, RAS__.root_, pos__);
}

int32_t RAS_Size(RAS_Struct RAS__) {
  // check correct boundaries

  Node_32b* node_ref = MEMORY_GET_ANY(RAS__.mem_, RAS__.root_);
  return node_ref->p_;
}


#endif  // _RAS_STRUCTURE__
