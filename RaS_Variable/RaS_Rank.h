#ifndef _RAS_RANK__
#define _RAS_RANK__

#include "memory.h"


int32_t RaS_Rank_8_(int32_t root__, int32_t pos__) {
  int32_t i, temp, limit;

  int32_t current = root__;
  int32_t rank = 0;

  Node_8b* node_ref;
  Leaf_8b* leaf_ref;

  // check correct boundaries
  if (pos__ < 0) return 0;

  node_ref = MEMORY_GET_ANY(root__);
  if (pos__ >= node_ref->p_)
    return node_ref->r_;

  // traverse the tree and enter correct leaf
  while (!IS_LEAF(current)) {
  	node_ref = MEMORY_GET_NODE(current);

  	// get p_ counter of left child and act accordingly
		temp = MEMORY_GET_ANY(node_ref->left_)->p_;
    if (temp >= pos__) {
      current = node_ref->left_;
    } else {
      pos__ -= temp;
      rank += MEMORY_GET_ANY(node_ref->left_)->r_;
      current = node_ref->right_;
    }
  }

  leaf_ref = MEMORY_GET_LEAF(current);

  // handle last leaf of this query
	limit = (pos__ <= 8) ? pos__ : 8;
  for (i = 0; i < limit; i++)
  	rank += (leaf_ref->vector_ >> (7 - i)) & 0x1;

  return rank;
}

int32_t RaS_Rank_16_(int32_t root__, int32_t pos__) {
  UNUSED(root__);
  UNUSED(pos__);

	FATAL("Not Implemented");
  return EXIT_FAILURE;
}

int32_t RaS_Rank_32_(int32_t root__, int32_t pos__) {
  UNUSED(root__);
  UNUSED(pos__);

	FATAL("Not Implemented");
  return EXIT_FAILURE;
}


#endif  // _RAS_RANK__
