#ifndef _RAS_PRINT__
#define _RAS_PRINT__

#include <stdio.h>
#include "memory.h"


void RaS_Print_8_(int32_t node__) {
  uint8_t i;

  Leaf_8b* leaf_ref;
  Node_8b* node_ref;

  if (IS_LEAF(node__)) {
    // print leaf and return to previous node
    leaf_ref = MEMORY_GET_LEAF(node__);

    for (i = 0; i < leaf_ref->p_; i++)
      printf("%d ", (leaf_ref->vector_ >> (7 - i)) & 0x1);

    return;
  }

  // handle child nodes
  node_ref = MEMORY_GET_NODE(node__);
  RaS_Print_8_(node_ref->left_);
  RaS_Print_8_(node_ref->right_);
}

void RaS_Print_16_(int32_t node__) {
  UNUSED(node__);
  FATAL("Not Implemented");
/*  uint16_t i;

  Leaf_16b* leaf_ref;
  Node_16b* node_ref;

  if (IS_LEAF(node__)) {
    // print leaf and return to previous node
    leaf_ref = MEMORY_GET_LEAF(node__);

    for (i = 0; i < leaf_ref->p_; i++)
      printf("%d ", (leaf_ref->vector_ >> (15 - i)) & 0x1);

    return;
  }

  // handle child nodes
  node_ref = MEMORY_GET_NODE(node__);
  RaS_Print_16_(node_ref->left_);
  RaS_Print_16_(node_ref->right_); */
}

void RaS_Print_32_(int32_t node__) {
  UNUSED(node__);
  FATAL("Not Implemented");
/*  uint32_t i;

  Leaf_32b* leaf_ref;
  Node_32b* node_ref;

  if (IS_LEAF(node__)) {
    // print leaf and return to previous node
    leaf_ref = MEMORY_GET_LEAF(node__);

    for (i = 0; i < leaf_ref->p_; i++)
      printf("%d ", (leaf_ref->vector_ >> (31 - i)) & 0x1);

    return;
  }

  // handle child nodes
  node_ref = MEMORY_GET_NODE(node__);
  RaS_Print_32_(node_ref->left_);
  RaS_Print_32_(node_ref->right_); */
}


#endif  // _RAS_PRINT__
