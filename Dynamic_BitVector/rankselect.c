#include "dbv.h"
#include "dbv_memory.h"

int32_t DBV_Rank(DBVStructRef DBV__, uint32_t pos__) {
  int32_t i, temp, limit, tmp_node;

  int32_t rank = 0;
  DBVMemObj mem = DBV__->mem_;
  DBVMemPtr current = DBV__->root_;

  DBVNodeRef node_ref;
  DBVLeafRef leaf_ref;

  node_ref = DBV_MEMORY_GET_ANY(mem, current);
  if (pos__ >= node_ref->p_)
    return node_ref->r_;

  /* traverse the tree and enter correct leaf */
  while (!DBV_IS_LEAF(current)) {
    node_ref = DBV_MEMORY_GET_NODE(mem, current);
    tmp_node = node_ref->left_;

    /* get p_ counter of left child and act accordingly */
    temp = DBV_MEMORY_GET_ANY(mem, tmp_node)->p_;
    if ((uint32_t) temp >= pos__) {
      current = tmp_node;
    } else {
      pos__ -= temp;
      rank += DBV_MEMORY_GET_ANY(mem, tmp_node)->r_;
      current = node_ref->right_;
    }
  }

  leaf_ref = DBV_MEMORY_GET_LEAF(mem, current);

  /* handle last leaf of this query */
  limit = (pos__ <= 32) ? pos__ : 32;
  for (i = 0; i < limit; i++)
    rank += (leaf_ref->vector_ >> (31 - i)) & 0x1;

  return rank;
}

int32_t DBV_Rank0(DBVStructRef DBV__, uint32_t pos__) {
  return pos__ - DBV_Rank(DBV__, pos__);
}

static int32_t DBV_Select_aux(DBVStructRef DBV__, uint32_t pos__, bool zero__) {
  int32_t i, temp, tmp_node;

  int32_t select = 0;
  DBVMemObj mem = DBV__->mem_;
  DBVMemPtr current = DBV__->root_;

  DBVNodeRef node_ref;
  DBVLeafRef leaf_ref;

  /* check correct boundaries */
  if (pos__ <= 0) return 0;

  node_ref = DBV_MEMORY_GET_ANY(mem, DBV__->root_);
  if (zero__) {
    if (pos__ > node_ref->p_ - node_ref->r_) return -1;
  } else {
    if (pos__ > node_ref->r_) return -1;
  }

  /* traverse the tree and enter correct leaf */
  while (!DBV_IS_LEAF(current)) {
    node_ref = DBV_MEMORY_GET_NODE(mem, current);
    tmp_node = node_ref->left_;

    /* get r_ counter of left child and act accordingly */
    DBVNodeRef temp_ref = DBV_MEMORY_GET_ANY(mem, tmp_node);
    temp = (zero__) ? temp_ref->p_ - temp_ref->r_ : temp_ref->r_;
    if ((uint32_t) temp >= pos__) {
      current = tmp_node;
    } else {
      pos__ -= temp;
      select += DBV_MEMORY_GET_ANY(mem, tmp_node)->p_;
      current = node_ref->right_;
    }
  }

  leaf_ref = DBV_MEMORY_GET_LEAF(mem, current);

  /* handle last leaf of this query */
  for (i = 0; pos__; i++) {
    pos__ -= (zero__) ? ((leaf_ref->vector_ >> (31 - i)) & 0x1) ^ 1
                      : ((leaf_ref->vector_ >> (31 - i)) & 0x1);
  }
  return select + i;
}

int32_t DBV_Select(DBVStructRef DBV__, uint32_t pos__) {
  return DBV_Select_aux(DBV__, pos__, false);
}

int32_t DBV_Select0(DBVStructRef DBV__, uint32_t pos__) {
  return DBV_Select_aux(DBV__, pos__, true);
}
