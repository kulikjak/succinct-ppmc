
#ifndef _RAS_STRUCTURE__
#define _RAS_STRUCTURE__

#include <stdio.h>
#include <stdio.h>
#include <stdio.h>

#include "utils.h"
#include "stack.h"
#include "memory.h"
#include "RaS_Print.h"
#include "RaS_Insert.h"
#include "RaS_Rank.h"
#include "RaS_Get.h"
#include "RaS_Select.h"


int32_t RaS_Init(memory_32b* mem__) {
  int32_t leaf = new_leaf(mem__);
  Leaf_32b* leaf_ref = MEMORY_GET_LEAF(mem__, leaf);

  leaf_ref->p_ = 0;
  leaf_ref->r_ = 0;

  return leaf;
}

void RaS_Print(memory_32b* mem__, int32_t node__) {
  RaS_Print_32_(mem__, node__);
  printf("\n");
}

void RaS_Insert(memory_32b* mem__, int32_t *root__, uint32_t pos__, bool val__) {
  RaS_Insert_32_(mem__, root__, pos__, val__);
}

// Delete is not supported cause it is not needed for deBruijn graphs
void RaS_Delete(memory_32b* mem__, int32_t root__, uint32_t pos__) {
  UNUSED(root__);
  UNUSED(mem__);
  UNUSED(pos__);

  FATAL("Not Implemented");
}

int32_t RaS_Rank(memory_32b* mem__, int32_t root__, uint32_t pos__) {
  return RaS_Rank_32_(mem__, root__, pos__);
}

int32_t RaS_Select(memory_32b* mem__, int32_t root__, uint32_t pos__) {
  return RaS_Select_32_(mem__, root__, pos__);
}

int32_t RaS_Get(memory_32b* mem__, int32_t root__, uint32_t pos__) {
  return RaS_Get_32_(mem__, root__, pos__);
}



#endif  // _RAS_STRUCTURE__
