#ifndef _RAS_PRINT__
#define _RAS_PRINT__

#include <stdio.h>
#include "memory.h"


void RaS_Print_32_(memory_32b* mem__, int32_t node__) {
  uint32_t i;

  Leaf_32b* leaf_ref;
  Node_32b* node_ref;

#ifdef RB_TREE_
  node__ = UNCOLOR_PTR(node__);
#endif

  if (IS_LEAF(node__)) {
    // print leaf and return to previous node
    leaf_ref = MEMORY_GET_LEAF(mem__, node__);

    for (i = 0; i < leaf_ref->p_; i++)
      printf("%d ", (leaf_ref->vector_ >> (31 - i)) & 0x1);

    return;
  }

  // handle child nodes
  node_ref = MEMORY_GET_NODE(mem__, node__);
  RaS_Print_32_(mem__, node_ref->left_);
  RaS_Print_32_(mem__, node_ref->right_);
}


#endif  // _RAS_PRINT__
