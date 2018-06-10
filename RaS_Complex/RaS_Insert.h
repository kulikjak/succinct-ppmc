#ifndef _RAS_INSERT__
#define _RAS_INSERT__

#include <stdio.h>

#include "utils.h"
#include "memory.h"
#include "stack.h"

// TODO RB Tree

/* Insert bit into the given leaf on given position with given value. */
#define INSERT_BIT(leaf, pos, value)                         \
  {                                                          \
    uint32_t mask = ((pos) == 0) ? 0 : ~(0) << (32 - (pos)); \
    uint32_t vector = 0;                                     \
                                                             \
    vector |= (leaf)->vector_ & mask;                        \
    vector |= (((leaf)->vector_ & ~(mask))) >> 1;            \
    (leaf)->vector_ = vector;                                \
    (leaf)->p_ += 1;                                         \
                                                             \
    if (value) {                                             \
      (leaf)->vector_ |= 0x1 << (31 - (pos));                \
      (leaf)->r_ += 1;                                       \
    }                                                        \
  }

/* Perform standard rank on more significant half of a 32 bit vector in given leaf. */
#define RANK16(leaf)                           \
  (leaf)->r_ += ((leaf)->vector_ >> 31) & 0x1; \
  (leaf)->r_ += ((leaf)->vector_ >> 30) & 0x1; \
  (leaf)->r_ += ((leaf)->vector_ >> 29) & 0x1; \
  (leaf)->r_ += ((leaf)->vector_ >> 28) & 0x1; \
  (leaf)->r_ += ((leaf)->vector_ >> 27) & 0x1; \
  (leaf)->r_ += ((leaf)->vector_ >> 26) & 0x1; \
  (leaf)->r_ += ((leaf)->vector_ >> 25) & 0x1; \
  (leaf)->r_ += ((leaf)->vector_ >> 24) & 0x1; \
  (leaf)->r_ += ((leaf)->vector_ >> 23) & 0x1; \
  (leaf)->r_ += ((leaf)->vector_ >> 22) & 0x1; \
  (leaf)->r_ += ((leaf)->vector_ >> 21) & 0x1; \
  (leaf)->r_ += ((leaf)->vector_ >> 20) & 0x1; \
  (leaf)->r_ += ((leaf)->vector_ >> 19) & 0x1; \
  (leaf)->r_ += ((leaf)->vector_ >> 18) & 0x1; \
  (leaf)->r_ += ((leaf)->vector_ >> 17) & 0x1; \
  (leaf)->r_ += ((leaf)->vector_ >> 16) & 0x1;


/*
 * Auxiliary function for 32 bit Insert operation.
 *
 * This function should not be called directly,
 * RAS_Insert() should be used instead.
 *
 * @param  mem__  Reference to memory object.
 * @param  root__  Reference to a number referencing the tree root in the memory.
 * @param  pos__  Insert position number.
 * @param  val__  Value of inserted bit.
 */
void RaS_Insert_32_(memory_32b* mem__, int32_t *root__, uint32_t pos__, bool val__) {

#ifdef RB_TREE_
  if (pos__ > MEMORY_GET_ANY(mem__, UNCOLOR_PTR(*root__))->p_)
    FATAL("Index out of range");
#else
  if (pos__ > MEMORY_GET_ANY(mem__, *root__)->p_)
    FATAL("Index out of range");
#endif


  VERBOSE( fprintf(stderr, "Inserting value %d on position %u\n", val__, pos__); )

  // traverse the tree and enter correct leaf
  int32_t current = *root__;
  STACK_PUSH(current);

  while (!IS_LEAF(current)) {
    // update p and r counters as we are traversing the structure
    Node_32b* node = MEMORY_GET_NODE(mem__, current);
    node->p_ += 1;
    node->r_ += val__;

    uint32_t temp = MEMORY_GET_ANY(mem__, node->left_)->p_;
    if (temp >= pos__) {
      current = node->left_;
    } else {
      pos__ -= temp;
      current = node->right_;
    }

    STACK_PUSH(current);
  }

  Leaf_32b* current_ref = MEMORY_GET_LEAF(mem__, current);
  if (current_ref->p_ < 32) {
    INSERT_BIT(current_ref, pos__, val__);

  } else {
    VERBOSE( fprintf(stderr, "Splitting full node\n"); )

    int32_t node = new_node(mem__);
    Node_32b* node_ref = MEMORY_GET_NODE(mem__, node);

    node_ref->p_ = 33;
    node_ref->r_ = current_ref->r_ + val__;

    // allocate new right leaf and reuse current as left leaf
    node_ref->right_ = new_leaf(mem__);
    node_ref->left_ = current;

    // initialize new right node
    Leaf_32b* right_ref = MEMORY_GET_LEAF(mem__, node_ref->right_);
    right_ref->vector_ = (current_ref->vector_ & (0xFFFF)) << 16;
    right_ref->p_ = 16;
    right_ref->r_ = 0;

    // calculate rank on newly created node
    RANK16(right_ref)

    // initialize (reuse) old leaf as left one
    current_ref->vector_ = current_ref->vector_ & (0xFFFF0000);
    current_ref->p_ = 16;
    current_ref->r_ -= right_ref->r_;

    // now insert bit into correct leaf
    if (pos__ <= 16) {
      INSERT_BIT(current_ref, pos__, val__);
    } else {
      INSERT_BIT(right_ref, pos__ - 16, val__);
    }

    // finally exchange pointers to new node
    if (STACK_GET_PARENT() == -1) {
      *root__ = node;
    } else {
      Node_32b* parent = MEMORY_GET_NODE(mem__, STACK_GET_PARENT());
      if (parent->left_ == current)
        parent->left_ = node;
      else
        parent->right_ = node;
    }
  }
  STACK_CLEAN();
}


#endif  // _RAS_INSERT__
