#ifndef _RAS_DELETE__
#define _RAS_DELETE__

#include <stdio.h>

#include "memory.h"

#include "../shared/stack.h"
#include "../shared/utils.h"

/*
 * Auxiliary function for 32 bit Delete operation.
 *
 * This function should not be called directly,
 * RAS_Delete() should be used instead.
 *
 * Function never merges leaves and so performance of the structure can be
 * worse after several uses.
 *
 * This function also clears stack by itself because of its implementation.
 *
 * @param  mem__  Reference to memory object.
 * @param  root__  Reference to a number referencing the tree root in the memory.
 * @param  pos__  Delete position index.
 */
void RaS_Delete_32_(memory_32b* mem__, int32_t *root__, uint32_t pos__) {

#ifdef RB_TREE_
  if (pos__ >= MEMORY_GET_ANY(mem__, UNCOLOR_PTR(*root__))->p_)
    FATAL("Index out of range");
#else
  if (pos__ >= MEMORY_GET_ANY(mem__, *root__)->p_)
    FATAL("Index out of range");
#endif

  STACK_CLEAN();

  VERBOSE( fprintf(stderr, "Deleting bit on position %u\n", pos__); )

  // traverse the tree and enter correct leaf
#ifdef RB_TREE_
  int32_t current = UNCOLOR_PTR(*root__);
#else
  int32_t current = *root__;
#endif

  while (!IS_LEAF(current)) {
    STACK_PUSH(current);

    // update p counter as we are traversing the structure
    Node_32b* node = MEMORY_GET_NODE(mem__, current);
    node->p_ -= 1;

#ifdef RB_TREE_
    uint32_t temp = MEMORY_GET_ANY(mem__, UNCOLOR_PTR(node->left_))->p_;
#else
    uint32_t temp = MEMORY_GET_ANY(mem__, node->left_)->p_;
#endif

    if (temp > pos__) {
#ifdef RB_TREE_
      current = UNCOLOR_PTR(node->left_);
#else
      current = node->left_;
#endif
    } else {
      pos__ -= temp;
      current = node->right_;
    }
  }

  // delete bit inside the node
  Leaf_32b* current_ref = MEMORY_GET_LEAF(mem__, current);
  assert(current_ref->p_ > 0);

  uint32_t mask = ((pos__) == 0) ? 0 : (uint32_t)(~(0)) << (32 - (pos__));
  uint32_t vector = 0;

  // contains any nonzero value for set bit (not exactly 1)
  int32_t val = current_ref->vector_ & (0x1 << (31 - pos__));

  vector |= current_ref->vector_ & mask;
  vector |= (current_ref->vector_ & (~(mask) >> 1)) << 1;

  current_ref->vector_ = vector;
  current_ref->p_ -= 1;

  // handle r counters all the way to root in case that deleted value was 1
  if (val) {
    do {
      Node_32b* node = MEMORY_GET_ANY(mem__, current);
      node->r_ -= 1;

      current = STACK_POP();
    } while (current != -1);
  }
}


#endif  // _RAS_DELETE__
