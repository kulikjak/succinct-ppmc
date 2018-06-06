#ifndef _RAS_SELECT__
#define _RAS_SELECT__

#include "memory.h"

int32_t RaS_Select_32_(memory_32b* mem__, int32_t root__, uint32_t num__, bool zero__) {
  int32_t i, temp;

  int32_t current = root__;
  int32_t select = 0;

  Node_32b* node_ref;
  Leaf_32b* leaf_ref;

  // check correct boundaries
  if (num__ <= 0) return 0;

  node_ref = MEMORY_GET_ANY(mem__, root__);
  if (zero__) {
    if (num__ > node_ref->p_ - node_ref->r_) return -1;
  } else {
    if (num__ > node_ref->r_) return -1;
  }

  // traverse the tree and enter correct leaf
  while (!IS_LEAF(current)) {
    node_ref = MEMORY_GET_NODE(mem__, current);

    // get r_ counter of left child and act accordingly
    temp = (zero__) ? MEMORY_GET_ANY(mem__, node_ref->left_)->p_ - MEMORY_GET_ANY(mem__, node_ref->left_)->r_
                    : MEMORY_GET_ANY(mem__, node_ref->left_)->r_;
    if ((uint32_t)temp >= num__) {
      current = node_ref->left_;
    } else {
      num__ -= temp;
      select += MEMORY_GET_ANY(mem__, node_ref->left_)->p_;
      current = node_ref->right_;
    }
  }

  leaf_ref = MEMORY_GET_LEAF(mem__, current);

  // handle last leaf of this query
  for (i = 0; num__; i++) {
    num__ -= (zero__) ? ((leaf_ref->vector_ >> (31 - i)) & 0x1) ^ 1
                      : ((leaf_ref->vector_ >> (31 - i)) & 0x1);
  }

  return select + i;
}

#endif  // _RAS_SELECT__
