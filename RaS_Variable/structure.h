
#ifndef _RAS_STRUCTURE__
#define _RAS_STRUCTURE__

#include <stdio.h>
#include <stdio.h>
#include <stdio.h>

#include "utils.h"
#include "memory.h"
#include "RaS_Print.h"
#include "RaS_Insert.h"
#include "RaS_Rank.h"
#include "RaS_Get.h"
#include "RaS_Select.h"


int32_t RaS_Init() {
  int32_t leaf = new_leaf();
  Leaf_8b* leaf_ref = MEMORY_GET_LEAF(leaf);

  leaf_ref->p_ = 0;
  leaf_ref->r_ = 0;

  return leaf;
}

// Main printing function
// This one should be called instead of other bit sensitive ones.
void RaS_Print(int32_t node__) {
  switch (G_alloc_state) {
    case 8:
      RaS_Print_8_(node__);
      break;
    case 16:
      RaS_Print_16_(node__);
      break;
    case 32:
      RaS_Print_32_(node__);
      break;
    default:
      UNREACHABLE;
  }
  printf("\n");
}

// Main function for insertion.
// This one should be called instead of other bit sensitive ones.
void RaS_Insert(int32_t *root__, uint32_t pos__, bool val__) {
  switch (G_alloc_state) {
    case 8:
      RaS_Insert_8_(root__, pos__, val__);
      break;
    case 16:
      RaS_Insert_16_(root__, pos__, val__);
      break;
    case 32:
      RaS_Insert_32_(root__, pos__, val__);
      break;
    default:
      UNREACHABLE;
  }
}

// Delete is not supported cause it is not needed for deBruijn graphs
void RaS_Delete(int32_t root__, uint32_t pos__) {
  UNUSED(root__);
  UNUSED(pos__);

  FATAL("Not Implemented");
}

// Main Rank function.
// This one should be called instead of Byte specific functions.
int32_t RaS_Rank(int32_t root__, int32_t pos__) {
  switch (G_alloc_state) {
    case 8:
      return RaS_Rank_8_(root__, pos__);
    case 16:
      return RaS_Rank_16_(root__, pos__);
    case 32:
      return RaS_Rank_32_(root__, pos__);
    default:
      UNREACHABLE;
      return EXIT_FAILURE;
  }
}

// Main Select function.
// This one should be called instead of Byte specific functions.
int32_t RaS_Select(int32_t root__, int32_t num__) {
  switch (G_alloc_state) {
    case 8:
      return RaS_Select_8_(root__, num__);
    case 16:
      return RaS_Select_16_(root__, num__);
    case 32:
      return RaS_Select_32_(root__, num__);
    default:
      UNREACHABLE;
      return EXIT_FAILURE;
  }
}

int32_t RaS_Get(int32_t root__, int32_t pos__) {
  switch (G_alloc_state) {
    case 8:
      return RaS_Get_8_(root__, pos__);
    case 16:
      return RaS_Get_16_(root__, pos__);
    case 32:
      return RaS_Get_32_(root__, pos__);
    default:
      UNREACHABLE;
      return EXIT_FAILURE;
  }
}



#endif  // _RAS_STRUCTURE__
