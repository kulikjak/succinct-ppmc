#ifndef _RAS_INSERT__
#define _RAS_INSERT__

#include <stdio.h>

#include "utils.h"
#include "memory.h"


#define INSERT_BIT(leaf, pos, value)                         \
  {                                                          \
    uint32_t mask = ((pos) == 0) ? 0 : ~(0) << (8 - (pos));  \
    uint32_t vector = 0;                                     \
                                                             \
    vector |= (leaf)->vector_& mask;                         \
    vector |= (((leaf)->vector_ & ~(mask))) >> 1;            \
    (leaf)->vector_ = vector;                                \
    (leaf)->p_ += 1;                                         \
                                                             \
    if (value) {                                             \
      (leaf)->vector_ |= 0x1 << (7 - (pos));                 \
      (leaf)->r_ += 1;                                       \
    }                                                        \
  }

#define RANK4(leaf)                           \
  (leaf)->r_ += ((leaf)->vector_ >> 7) & 0x1; \
  (leaf)->r_ += ((leaf)->vector_ >> 6) & 0x1; \
  (leaf)->r_ += ((leaf)->vector_ >> 5) & 0x1; \
  (leaf)->r_ += ((leaf)->vector_ >> 4) & 0x1;



void RaS_Insert_8_(int32_t *root__, uint32_t pos__, bool val__) {
  if (pos__ > MEMORY_GET_ANY(*root__)->p_)
    FATAL("Index out of range");

  VERBOSE( fprintf(stderr, "Inserting value %d on position %u\n", val__, pos__); )


  // traverse the tree and enter correct leaf
  int32_t current = *root__;
  STACK_PUSH(current);

  while (!IS_LEAF(current)) {
    // update p and r counters as we are traversing the structure
    Node_8b* node = MEMORY_GET_NODE(current);
    node->p_ += 1;
    node->r_ += val__;

    uint32_t temp = MEMORY_GET_ANY(node->left_)->p_;
    if (temp >= pos__) {
      current = node->left_;
    } else {
      pos__ -= temp;
      current = node->right_;
    }

    STACK_PUSH(current);
  }

  Leaf_8b* current_ref = MEMORY_GET_LEAF(current);
  if (current_ref->p_ < 8) {
    INSERT_BIT(current_ref, pos__, val__);

  } else {
    VERBOSE( fprintf(stderr, "Splitting full node\n"); )

    int32_t node = new_node();
    Node_8b* node_ref = MEMORY_GET_NODE(node);

    node_ref->p_ = 9;
    node_ref->r_ = current_ref->r_ + val__;

    // allocate new right leaf and reuse current as left leaf
    node_ref->right_ = new_leaf();
    node_ref->left_ = current;

    // initialize new right node
    Leaf_8b* right_ref = MEMORY_GET_LEAF(node_ref->right_);
    right_ref->vector_ = (current_ref->vector_ & (0x0F)) << 4;
    right_ref->p_ = 4;
    right_ref->r_ = 0;

    // calculate rank on newly created node
    RANK4(right_ref)

    // initialize (reuse) old leaf as left one
    current_ref->vector_ = current_ref->vector_ & (0xF0);
    current_ref->p_ = 4;
    current_ref->r_ -= right_ref->r_;

    // now insert bit into correct leaf
    if (pos__ <= 4) {
      INSERT_BIT(current_ref, pos__, val__);
    } else {
      INSERT_BIT(right_ref, pos__ - 4, val__);
    }

    // finally exchange pointers to new node
    if (STACK_GET_PARENT() == -1) {
      *root__ = node;
    } else {
      Node_8b* parent = MEMORY_GET_NODE(STACK_GET_PARENT());
      if (parent->left_ == current)
        parent->left_ = node;
      else
        parent->right_ = node;
    }
  }
  STACK_CLEAN();
}

void RaS_Insert_16_(int32_t *root__, uint32_t pos__, bool val__) {
  UNUSED(root__);
  UNUSED(pos__);
  UNUSED(val__);

  FATAL("Not Implemented");
}

void RaS_Insert_32_(int32_t *root__, uint32_t pos__, bool val__) {
  UNUSED(root__);
  UNUSED(pos__);
  UNUSED(val__);

  FATAL("Not Implemented");
}


#endif  // _RAS_INSERT__
