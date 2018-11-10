#include "dbv.h"
#include "memory.h"

/* Insert bit into the given leaf on given position with given value. */
#define INSERT_BIT(leaf, pos, value)                                     \
  {                                                                      \
    assert(pos < 32);                                                    \
    uint32_t mask = ((pos) == 0) ? 0 : (uint32_t)(~(0)) << (32 - (pos)); \
    uint32_t vector = 0;                                                 \
                                                                         \
    vector |= (leaf)->vector_ & mask;                                    \
    vector |= (((leaf)->vector_ & ~(mask))) >> 1;                        \
    (leaf)->vector_ = vector;                                            \
    (leaf)->p_ += 1;                                                     \
                                                                         \
    if (value) {                                                         \
      (leaf)->vector_ |= 0x1 << (31 - (pos));                            \
      (leaf)->r_ += 1;                                                   \
    }                                                                    \
  }

/* Perform standard rank on more significant half of a 32 bit vector in given leaf. */
#define RANK16(leaf) \
  (leaf)->r_ = __builtin_popcount((leaf)->vector_ & 0xFFFF0000);

#define NODE_OPERATION_2(r1, r2, op) \
  r1->p_ op r2->p_;                  \
  r1->r_ op r2->r_;
#define NODE_OPERATION_3(r1, r2, r3, op) \
  r1->p_ = r2->p_ op r3->p_;             \
  r1->r_ = r2->r_ op r3->r_;

void DBV_Insert(DBVStructRef DBV__, uint32_t pos__, bool val__) {

  if (pos__ > DBV_MEMORY_GET_ANY(DBV__->mem_, DBV__->root_)->p_)
    FATAL("Index out of range");

  DBV_STACK_CLEAN();

  /* traverse the tree and enter correct leaf */
  DBVMemObj mem = DBV__->mem_;
  DBVMemPtr current = DBV__->root_;
  DBV_STACK_PUSH(current);

  while (!DBV_IS_LEAF(current)) {
    /* update p and r counters as we are traversing the structure */
    DBVNodeRef node = DBV_MEMORY_GET_NODE(mem, current);
    node->p_ += 1;
    node->r_ += val__;

    uint32_t temp = DBV_MEMORY_GET_ANY(mem, node->left_)->p_;
    if (temp >= pos__) {
      current = node->left_;
    } else {
      pos__ -= temp;
      current = node->right_;
    }

    DBV_STACK_PUSH(current);
  }

  DBVLeafRef current_ref = DBV_MEMORY_GET_LEAF(mem, current);
  if (current_ref->p_ < 32) {
    INSERT_BIT(current_ref, pos__, val__);

  } else {
  
    DBVMemPtr node = DBV_Memory_new_node(mem);
    DBVNodeRef node_ref = DBV_MEMORY_GET_NODE(mem, node);

    node_ref->p_ = 33;
    node_ref->r_ = current_ref->r_ + val__;

    /* allocate new right leaf and reuse current as left leaf */
    node_ref->right_ = DBV_Memory_new_leaf(mem);
    node_ref->left_ = current;

    /* initialize new right node */
    DBVLeafRef right_ref = DBV_MEMORY_GET_LEAF(mem, node_ref->right_);
    right_ref->vector_ = (current_ref->vector_ & (0xFFFF)) << 16;
    right_ref->p_ = 16;

    /* calculate rank on newly created node */
    RANK16(right_ref)

    /* initialize (reuse) old leaf as left one */
    current_ref->vector_ = current_ref->vector_ & (0xFFFF0000);
    current_ref->p_ = 16;
    current_ref->r_ -= right_ref->r_;

    /* now insert bit into correct leaf */
    if (pos__ <= 16) {
      INSERT_BIT(current_ref, pos__, val__);
    } else {
      INSERT_BIT(right_ref, pos__ - 16, val__);
    }

    /* finally exchange pointers to new node */
    if (DBV_STACK_GET_PARENT() == -1) {
      DBV__->root_ = node;
    } else {
      DBVNodeRef parent = DBV_MEMORY_GET_NODE(mem, DBV_STACK_GET_PARENT());
      if (parent->left_ == current)
        parent->left_ = node;
      else
        parent->right_ = node;
    }

#ifdef DBV_ENABLE_RED_BLACK_BALANCING
    bool parent_left, grandparent_left;

    /* red black balancing */
    node_ref->rb_flag_ = true;

    do {
      /* current node is the root - change it to black and end */
      if (DBV_STACK_GET_PARENT() == -1) {
        node_ref->rb_flag_ = false;
        return;
      }

      DBVMemPtr parent_idx = DBV_STACK_GET_PARENT();
      DBVNodeRef parent = DBV_MEMORY_GET_NODE(mem, parent_idx);
      /* parent node is a black node - do nothing */
      if (parent->rb_flag_ == false) {
        return;
      }

      DBVMemPtr grandparent_idx = DBV_STACK_GET_GRANDPARENT();
      DBVNodeRef grandparent = DBV_MEMORY_GET_NODE(mem, grandparent_idx);

      /* get uncle */
      DBVMemPtr uncle_idx = grandparent->left_;
      grandparent_left = false;
      if (grandparent->left_ == DBV_STACK_GET_PARENT()) {
        uncle_idx = grandparent->right_;
        grandparent_left = true;
      }

      DBVNodeRef uncle = DBV_MEMORY_GET_ANY(mem, uncle_idx);

      /* uncle is red - change colors and go to start */
      if (!DBV_IS_LEAF(uncle_idx) && uncle->rb_flag_ == true) {
        parent->rb_flag_ = false;
        uncle->rb_flag_ = false;
        grandparent->rb_flag_ = true;

        node = grandparent_idx;
        node_ref = grandparent;
        DBV_STACK_POP();
        DBV_STACK_POP();
        continue;
      }

      /* uncle is black - time for rotations */
      parent_left = true;
      if (node == parent->right_) {
        parent_left = false;
      }

      DBVMemPtr newroot = 0;
      if (parent_left && grandparent_left) {
        grandparent->rb_flag_ = true;
        parent->rb_flag_ = false;

        grandparent->left_ = parent->right_;
        parent->right_ = grandparent_idx;

        NODE_OPERATION_2(grandparent, node_ref, -=);
        NODE_OPERATION_2(parent, uncle, +=);

        newroot = parent_idx;
      } else if (!parent_left && !grandparent_left) {
        grandparent->rb_flag_ = true;
        parent->rb_flag_ = false;

        grandparent->right_ = parent->left_;
        parent->left_ = grandparent_idx;

        NODE_OPERATION_2(grandparent, node_ref, -=);
        NODE_OPERATION_2(parent, uncle, +=);

        newroot = parent_idx;
      } else if (!parent_left && grandparent_left) {
        grandparent->rb_flag_ = true;
        node_ref->rb_flag_ = false;

        grandparent->left_ = node_ref->right_;
        parent->right_ = node_ref->left_;

        node_ref->left_ = parent_idx;
        node_ref->right_ = grandparent_idx;

        DBVNodeRef temp = DBV_MEMORY_GET_ANY(mem, grandparent->left_);

        NODE_OPERATION_2(parent, temp, -=);
        NODE_OPERATION_2(grandparent, parent, -=);
        NODE_OPERATION_3(node_ref, parent, grandparent, +);

        newroot = node;
      } else if (parent_left && !grandparent_left) {
        grandparent->rb_flag_ = true;
        node_ref->rb_flag_ = false;

        grandparent->right_ = node_ref->left_;
        parent->left_ = node_ref->right_;

        node_ref->left_ = grandparent_idx;
        node_ref->right_ = parent_idx;

        DBVNodeRef temp = DBV_MEMORY_GET_ANY(mem, grandparent->right_);

        NODE_OPERATION_2(parent, temp, -=);
        NODE_OPERATION_2(grandparent, parent, -=);
        NODE_OPERATION_3(node_ref, parent, grandparent, +);

        newroot = node;
      }

      /* finally exchange pointers to new node */
      if (DBV_STACK_GET_GRANDGRANDPARENT() == -1) {
        DBV__->root_ = newroot;
      } else {
        DBVNodeRef grandgrandparent = DBV_MEMORY_GET_NODE(mem, DBV_STACK_GET_GRANDGRANDPARENT());
        if (grandgrandparent->left_ == grandparent_idx)
          grandgrandparent->left_ = newroot;
        else
          grandgrandparent->right_ = newroot;
      }
      break;
    } while (true);

#endif  /* DBV_ENABLE_RED_BLACK_BALANCING */

  }
}
