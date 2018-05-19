#ifndef _RAS_GET__
#define _RAS_GET__

#include "memory.h"


int32_t RaS_Get_8_(int32_t root__, int32_t pos__) {
  int32_t temp;

  int32_t current = root__;

  Node_8b* node_ref;
  Leaf_8b* leaf_ref;

  // check correct boundaries
  node_ref = MEMORY_GET_ANY(root__);
  if (pos__ >= node_ref->p_ || pos__ < 0)
    return -1;

  // traverse the tree and enter correct leaf
  while (!IS_LEAF(current)) {
    node_ref = MEMORY_GET_NODE(current);

    // get p_ counter of left child and act accordingly
    temp = MEMORY_GET_ANY(node_ref->left_)->p_;
    if (temp > pos__) {
      current = node_ref->left_;
    } else {
      pos__ -= temp;
      current = node_ref->right_;
    }
  }

  leaf_ref = MEMORY_GET_LEAF(current);

  return (leaf_ref->vector_ >> (7 - pos__)) & 0x1;
}

int32_t RaS_Get_16_(int32_t root__, int32_t num__) {
  UNUSED(root__);
  UNUSED(num__);

  FATAL("Not Implemented");
  return EXIT_FAILURE;
}

int32_t RaS_Get_32_(int32_t root__, int32_t num__) {
  UNUSED(root__);
  UNUSED(num__);

  FATAL("Not Implemented");
  return EXIT_FAILURE;
}


#endif  // _RAS_GET__
