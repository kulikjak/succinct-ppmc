#ifndef _RAS_GET__
#define _RAS_GET__

#include "memory.h"

/*
 * Auxiliary function for 32 bit Get operation.
 *
 * This function should not be called directly,
 * RAS_Get() should be used instead.
 *
 * @param  mem__  Reference to memory object.
 * @param  root__  Number referencing the tree root in the memory.
 * @param  pos__  Position number.
 */
int32_t RaS_Get_32_(memory_32b* mem__, int32_t root__, uint32_t pos__) {
  int32_t temp;

  int32_t current = root__;

  Node_32b* node_ref;
  Leaf_32b* leaf_ref;

  // check correct boundaries
  node_ref = MEMORY_GET_ANY(mem__, root__);
  if (pos__ >= node_ref->p_)
    return -1;

  // traverse the tree and enter correct leaf
  while (!IS_LEAF(current)) {
    node_ref = MEMORY_GET_NODE(mem__, current);

    // get p_ counter of left child and act accordingly
    temp = MEMORY_GET_ANY(mem__, node_ref->left_)->p_;
    if ((uint32_t)temp > pos__) {
      current = node_ref->left_;
    } else {
      pos__ -= temp;
      current = node_ref->right_;
    }
  }

  leaf_ref = MEMORY_GET_LEAF(mem__, current);

  return (leaf_ref->vector_ >> (31 - pos__)) & 0x1;
}


#endif  // _RAS_GET__
