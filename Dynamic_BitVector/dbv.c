#include <stdio.h>

#include "dbv.h"

void DBV_Init(DBVStructRef DBV__) {
  DBV__->mem_ = DBV_Memory_Init();
  DBV__->root_ = DBV_Memory_new_leaf(DBV__->mem_);

  DBVLeafRef leaf_ref = DBV_MEMORY_GET_LEAF(DBV__->mem_, DBV__->root_);

  leaf_ref->p_ = 0;
  leaf_ref->r_ = 0;
}

void DBV_Free(DBVStructRef DBV__) {
  DBV__->root_ = -1;
  DBV_Memory_Free(&(DBV__->mem_));
}

int32_t DBV_Size(DBVStructRef DBV__) {
  DBVNodeRef node_ref = DBV_MEMORY_GET_ANY(DBV__->mem_, DBV__->root_);
  return node_ref->p_;
}

int32_t DBV_Get(DBVStructRef DBV__, uint32_t pos__) {
  int32_t temp;

  DBVMemObj mem = DBV__->mem_;
  DBVMemPtr current = DBV__->root_;

  DBVNodeRef node_ref;
  DBVLeafRef leaf_ref;

  /* check correct boundaries */
  node_ref = DBV_MEMORY_GET_ANY(mem, DBV__->root_);
  if (pos__ >= node_ref->p_) return -1;

  /* traverse the tree and enter correct leaf */
  while (!DBV_IS_LEAF(current)) {
    node_ref = DBV_MEMORY_GET_NODE(mem, current);

    /* get p_ counter of left child and act accordingly */
    temp = DBV_MEMORY_GET_ANY(mem, node_ref->left_)->p_;
    if ((uint32_t)temp > pos__) {
      current = node_ref->left_;
    } else {
      pos__ -= temp;
      current = node_ref->right_;
    }
  }

  leaf_ref = DBV_MEMORY_GET_LEAF(mem, current);

  return (leaf_ref->vector_ >> (31 - pos__)) & 0x1;
}

static void DBV_Print_aux(DBVMemObj mem__, DBVMemPtr node__) {
  uint32_t i;

  DBVNodeRef node_ref;
  DBVLeafRef leaf_ref;

  if (DBV_IS_LEAF(node__)) {
    /* print leaf and return to previous node */
    leaf_ref = DBV_MEMORY_GET_LEAF(mem__, node__);

    for (i = 0; i < leaf_ref->p_; i++)
      printf("%d ", (leaf_ref->vector_ >> (31 - i)) & 0x1);

    return;
  }

  /* handle child nodes */
  node_ref = DBV_MEMORY_GET_NODE(mem__, node__);
  DBV_Print_aux(mem__, node_ref->left_);
  DBV_Print_aux(mem__, node_ref->right_);
}

void DBV_Print(DBVStructRef DBV__) {
  DBV_Print_aux(DBV__->mem_, DBV__->root_);
}
