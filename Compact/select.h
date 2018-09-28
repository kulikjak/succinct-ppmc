#ifndef _COMPRESSION_STRUCT_SELECT__
#define _COMPRESSION_STRUCT_SELECT__

#include "memory.h"

/* TODO fix this and somehow merge these function into more compact source code
 */

/*
 * Auxiliary function for 32 bit select operation above L vector.
 *
 * @param  Graph__  Reference to graph structure.
 * @param  pos__  Select query position.
 * @param  zero__  Whether this should select ones or zeroes (true for zero).
 */
int32_t graph_Lselect_(Graph_Struct Graph__, uint32_t num__, bool zero__) {
  int32_t i, temp, tmp_node;

  int32_t current = Graph__.root_;
  int32_t select = 0;

  NodeRef node_ref;
  LeafRef leaf_ref;

  // check correct boundaries
  if (num__ <= 0) return 0;

  node_ref = MEMORY_GET_ANY(Graph__.mem_, Graph__.root_);
  if (zero__) {
    if (num__ > node_ref->p_ - node_ref->rL_) return -1;
  } else {
    if (num__ > node_ref->rL_) return -1;
  }

  // traverse the tree and enter correct leaf
  while (!IS_LEAF(current)) {
    node_ref = MEMORY_GET_NODE(Graph__.mem_, current);

    tmp_node = node_ref->left_;

    // get rL_ counter of left child and act accordingly
    temp = (zero__)
               ? MEMORY_GET_ANY(Graph__.mem_, tmp_node)->p_ -
                     MEMORY_GET_ANY(Graph__.mem_, tmp_node)->rL_
               : MEMORY_GET_ANY(Graph__.mem_, tmp_node)->rL_;
    if ((uint32_t)temp >= num__) {
      current = tmp_node;
    } else {
      num__ -= temp;
      select += MEMORY_GET_ANY(Graph__.mem_, tmp_node)->p_;
      current = node_ref->right_;
    }
  }

  leaf_ref = MEMORY_GET_LEAF(Graph__.mem_, current);

  // handle last leaf of this query
  for (i = 0; num__; i++) {
    num__ -= (zero__) ? ((leaf_ref->vectorL_ >> (31 - i)) & 0x1) ^ 1
                      : ((leaf_ref->vectorL_ >> (31 - i)) & 0x1);
  }
  return select + i;
}

/*
 * Auxiliary function for 32 bit select operation above top W vector.
 *
 * @param  Graph__  Reference to graph structure.
 * @param  pos__  Select query position.
 * @param  zero__  Whether this should select ones or zeroes (true for zero).
 */
int32_t graph_Wselect_top_(Graph_Struct Graph__, uint32_t num__, bool zero__) {
  int32_t i, temp, tmp_node;

  int32_t current = Graph__.root_;
  int32_t select = 0;

  NodeRef node_ref;
  LeafRef leaf_ref;

  // check correct boundaries
  if (num__ <= 0) return 0;

  node_ref = MEMORY_GET_ANY(Graph__.mem_, Graph__.root_);
  if (zero__) {
    if (num__ > node_ref->p_ - node_ref->rW_) return -1;
  } else {
    if (num__ > node_ref->rW_) return -1;
  }

  // traverse the tree and enter correct leaf
  while (!IS_LEAF(current)) {
    node_ref = MEMORY_GET_NODE(Graph__.mem_, current);
    tmp_node = node_ref->left_;

    // get rW_ counter of left child and act accordingly
    temp = (zero__)
               ? MEMORY_GET_ANY(Graph__.mem_, tmp_node)->p_ -
                     MEMORY_GET_ANY(Graph__.mem_, tmp_node)->rW_
               : MEMORY_GET_ANY(Graph__.mem_, tmp_node)->rW_;
    if ((uint32_t)temp >= num__) {
      current = tmp_node;
    } else {
      num__ -= temp;
      select += MEMORY_GET_ANY(Graph__.mem_, tmp_node)->p_;
      current = node_ref->right_;
    }
  }

  leaf_ref = MEMORY_GET_LEAF(Graph__.mem_, current);

  // handle last leaf of this query
  for (i = 0; num__; i++) {
    num__ -= (zero__) ? ((leaf_ref->vectorWl0_ >> (31 - i)) & 0x1) ^ 1
                      : ((leaf_ref->vectorWl0_ >> (31 - i)) & 0x1);
  }
  return select + i;
}

/*
 * Auxiliary function for 32 bit select operation above left W vector.
 *
 * @param  Graph__  Reference to graph structure.
 * @param  pos__  Select query position.
 * @param  zero__  Whether this should select ones or zeroes (true for zero).
 */
int32_t graph_Wselect_left_(Graph_Struct Graph__, uint32_t num__, bool zero__) {
  int32_t i, temp, tmp_node;
  int32_t local_p, vector, mask;

  int32_t current = Graph__.root_;
  int32_t select = 0;

  NodeRef node_ref;
  LeafRef leaf_ref;

  // check correct boundaries
  if (num__ <= 0) return 0;

  node_ref = MEMORY_GET_ANY(Graph__.mem_, Graph__.root_);
  local_p = node_ref->p_ - node_ref->rW_;
  if (zero__) {
    if (num__ > local_p - node_ref->rWl_) return -1;
  } else {
    if (num__ > node_ref->rWl_) return -1;
  }

  // traverse the tree and enter correct leaf
  while (!IS_LEAF(current)) {
    node_ref = MEMORY_GET_NODE(Graph__.mem_, current);
    tmp_node = node_ref->left_;

    // get rW_ counter of left child and act accordingly
    temp = (zero__)
               ? MEMORY_GET_ANY(Graph__.mem_, tmp_node)->p_ -
                     MEMORY_GET_ANY(Graph__.mem_, tmp_node)->rW_ -
                     MEMORY_GET_ANY(Graph__.mem_, tmp_node)->rWl_
               : MEMORY_GET_ANY(Graph__.mem_, tmp_node)->rWl_;
    if ((uint32_t)temp >= num__) {
      current = tmp_node;
    } else {
      num__ -= temp;
      select += (MEMORY_GET_ANY(Graph__.mem_, tmp_node)->p_ -
                 MEMORY_GET_ANY(Graph__.mem_, tmp_node)->rW_);
      current = node_ref->right_;
    }
  }

  leaf_ref = MEMORY_GET_LEAF(Graph__.mem_, current);

  // handle last leaf of this query
  vector = ~(leaf_ref->vectorWl0_) & leaf_ref->vectorWl1_;
  mask = ~(leaf_ref->vectorWl0_);

  for (i = 0; num__; i++) {
    if ((mask >> (31 - i)) & 0x1) {
      select++;
      num__ -= (zero__) ? ((vector >> (31 - i)) & 0x1) ^ 1
                        : ((vector >> (31 - i)) & 0x1);
    }
  }
  return select;
}

/*
 * Auxiliary function for 32 bit select operation above right W vector.
 *
 * @param  Graph__  Reference to graph structure.
 * @param  pos__  Select query position.
 * @param  zero__  Whether this should select ones or zeroes (true for zero).
 */
int32_t graph_Wselect_right_(Graph_Struct Graph__, uint32_t num__,
                             bool zero__) {
  int32_t i, temp, tmp_node;
  int32_t local_p, vector, mask;

  int32_t current = Graph__.root_;
  int32_t select = 0;

  NodeRef node_ref;
  LeafRef leaf_ref;

  // check correct boundaries
  if (num__ <= 0) return 0;

  node_ref = MEMORY_GET_ANY(Graph__.mem_, Graph__.root_);
  local_p = node_ref->rW_;
  if (zero__) {
    if (num__ > local_p - node_ref->rWh_) return -1;
  } else {
    if (num__ > node_ref->rWh_) return -1;
  }

  // traverse the tree and enter correct leaf
  while (!IS_LEAF(current)) {
    node_ref = MEMORY_GET_NODE(Graph__.mem_, current);
    tmp_node = node_ref->left_;

    // get rW_ counter of left child and act accordingly
    temp = (zero__)
               ? MEMORY_GET_ANY(Graph__.mem_, tmp_node)->rW_ -
                     MEMORY_GET_ANY(Graph__.mem_, tmp_node)->rWh_
               : MEMORY_GET_ANY(Graph__.mem_, tmp_node)->rWh_;
    if ((uint32_t)temp >= num__) {
      current = tmp_node;
    } else {
      num__ -= temp;
      select += MEMORY_GET_ANY(Graph__.mem_, tmp_node)->rW_;
      current = node_ref->right_;
    }
  }

  leaf_ref = MEMORY_GET_LEAF(Graph__.mem_, current);

  // handle last leaf of this query
  vector = (leaf_ref->vectorWl0_) & leaf_ref->vectorWl1_;
  mask = leaf_ref->vectorWl0_;

  for (i = 0; num__; i++) {
    if ((mask >> (31 - i)) & 0x1) {
      select++;
      num__ -= (zero__) ? ((vector >> (31 - i)) & 0x1) ^ 1
                        : ((vector >> (31 - i)) & 0x1);
    }
  }
  return select;
}

/*
 * Auxiliary function for 32 bit select operation above bottom W vector.
 *
 * @param  Graph__  Reference to graph structure.
 * @param  pos__  Select query position.
 * @param  zero__  Whether this should select ones or zeroes (true for zero).
 */
int32_t graph_Wselect_bottom_(Graph_Struct Graph__, uint32_t num__,
                              bool zero__) {
  int32_t i, temp, tmp_node;
  int32_t local_p, vector, mask;

  int32_t current = Graph__.root_;
  int32_t select = 0;

  NodeRef node_ref;
  LeafRef leaf_ref;

  // check correct boundaries
  if (num__ <= 0) return 0;

  node_ref = MEMORY_GET_ANY(Graph__.mem_, Graph__.root_);
  local_p = node_ref->rWh_;
  if (zero__) {
    if (num__ > local_p - node_ref->rWs_) return -1;
  } else {
    if (num__ > node_ref->rWs_) return -1;
  }

  // traverse the tree and enter correct leaf
  while (!IS_LEAF(current)) {
    node_ref = MEMORY_GET_NODE(Graph__.mem_, current);
    tmp_node = node_ref->left_;

    // get rW_ counter of left child and act accordingly
    temp = (zero__)
               ? MEMORY_GET_ANY(Graph__.mem_, tmp_node)->rWh_ -
                     MEMORY_GET_ANY(Graph__.mem_, tmp_node)->rWs_
               : MEMORY_GET_ANY(Graph__.mem_, tmp_node)->rWs_;
    if ((uint32_t)temp >= num__) {
      current = tmp_node;
    } else {
      num__ -= temp;
      select += MEMORY_GET_ANY(Graph__.mem_, tmp_node)->rWh_;
      current = node_ref->right_;
    }
  }

  leaf_ref = MEMORY_GET_LEAF(Graph__.mem_, current);

  // handle last leaf of this query
  vector = (leaf_ref->vectorWl0_) & leaf_ref->vectorWl1_ & leaf_ref->vectorWl2_;
  mask = leaf_ref->vectorWl0_ & leaf_ref->vectorWl1_;

  for (i = 0; num__; i++) {
    if ((mask >> (31 - i)) & 0x1) {
      select++;
      num__ -= (zero__) ? ((vector >> (31 - i)) & 0x1) ^ 1
                        : ((vector >> (31 - i)) & 0x1);
    }
  }
  return select;
}

#endif  // _COMPRESSION_STRUCT_SELECT__
