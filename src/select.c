#include "structure.h"

/*
 * Code necessary to perform select operations above all structure vectors.
 *
 * Macros are used to reduce code duplication and also to increase performance
 * by reducing number of calls needed to perform selects.
 */

#define GRAPH_SIMPLE_SELECT_EXPAND(type__) {                   \
  int32_t i, temp;                                             \
  int32_t select = 0;                                          \
  uint32_t local_var;                                          \
  MemPtr tmp_node;                                             \
  MemPtr current = Graph__.root_;                              \
                                                               \
  NodeRef node_ref;                                            \
  LeafRef leaf_ref;                                            \
                                                               \
  /* gets optimized away if all expansions are successfull */  \
  if (type__ != EXPAND_L && type__ != EXPAND_WTOP)             \
    FATAL("Wrong type in GRAPH_SIMPLE_SELECT_EXPAND macro");   \
                                                               \
  /* check correct boundaries */                               \
  if (num__ <= 0) return 0;                                    \
                                                               \
  node_ref = MEMORY_GET_ANY(Graph__.mem_, Graph__.root_);      \
  local_var = (type__ == EXPAND_L)                             \
    ? node_ref->rL_                                            \
    : node_ref->rW_;                                           \
                                                               \
  if (zero__) {                                                \
    if (num__ > node_ref->p_ - local_var) return -1;           \
  } else {                                                     \
    if (num__ > local_var) return -1;                          \
  }                                                            \
                                                               \
  /* traverse the tree and enter correct leaf */               \
  while (!IS_LEAF(current)) {                                  \
    node_ref = MEMORY_GET_NODE(Graph__.mem_, current);         \
    tmp_node = node_ref->left_;                                \
                                                               \
    if (type__ == EXPAND_L)                                    \
      local_var = MEMORY_GET_ANY(Graph__.mem_, tmp_node)->rL_; \
    else if (type__ == EXPAND_WTOP)                            \
      local_var = MEMORY_GET_ANY(Graph__.mem_, tmp_node)->rW_; \
                                                               \
    /* get r_ counter of left child and act accordingly */     \
    temp = (zero__)                                            \
      ? MEMORY_GET_ANY(Graph__.mem_, tmp_node)->p_ - local_var \
      : local_var;                                             \
                                                               \
    if ((uint32_t)temp >= num__) {                             \
      current = tmp_node;                                      \
    } else {                                                   \
      num__ -= temp;                                           \
      select += MEMORY_GET_ANY(Graph__.mem_, tmp_node)->p_;    \
      current = node_ref->right_;                              \
    }                                                          \
  }                                                            \
                                                               \
  leaf_ref = MEMORY_GET_LEAF(Graph__.mem_, current);           \
  local_var = (type__ == EXPAND_L)                             \
    ? leaf_ref->vectorL_                                       \
    : leaf_ref->vectorWl0_;                                    \
                                                               \
  /* handle last leaf of this query */                         \
  for (i = 0; num__; i++) {                                    \
    num__ -= (zero__) ? ((local_var >> (31 - i)) & 0x1) ^ 1    \
                      : ((local_var >> (31 - i)) & 0x1);       \
  }                                                            \
                                                               \
  return select + i;                                           \
}

#define GRAPH_MASKED_SELECT_EXPAND(type__) {                                       \
  int32_t i, temp;                                                                 \
  int32_t local_p, vector, mask;                                                   \
  int32_t select = 0;                                                              \
  uint32_t local_var;                                                              \
  MemPtr tmp_node;                                                                 \
  MemPtr current = Graph__.root_;                                                  \
                                                                                   \
  NodeRef node_ref;                                                                \
  LeafRef leaf_ref;                                                                \
                                                                                   \
  /* gets optimized away if all expansions are successfull */                      \
  if (type__ != EXPAND_WLEFT &&                                                    \
      type__ != EXPAND_WRIGHT &&                                                   \
      type__ != EXPAND_WBOTTOM)                                                    \
    FATAL("Wrong type in GRAPH_MASKED_SELECT_EXPAND macro");                       \
                                                                                   \
  /* check correct boundaries */                                                   \
  if (num__ <= 0) return 0;                                                        \
                                                                                   \
  node_ref = MEMORY_GET_ANY(Graph__.mem_, Graph__.root_);                          \
  if (type__ == EXPAND_WLEFT) {                                                    \
    local_p = node_ref->p_ - node_ref->rW_;                                        \
    local_var = node_ref->rWl_;                                                    \
  } else if (type__ == EXPAND_WRIGHT) {                                            \
    local_p = node_ref->rW_;                                                       \
    local_var = node_ref->rWh_;                                                    \
  } else if (type__ == EXPAND_WBOTTOM) {                                           \
    local_p = node_ref->rWh_;                                                      \
    local_var = node_ref->rWs_;                                                    \
  }                                                                                \
                                                                                   \
  if (zero__) {                                                                    \
    if (num__ > local_p - local_var) return -1;                                    \
  } else {                                                                         \
    if (num__ > local_var) return -1;                                              \
  }                                                                                \
                                                                                   \
  /* traverse the tree and enter correct leaf */                                   \
  while (!IS_LEAF(current)) {                                                      \
    node_ref = MEMORY_GET_NODE(Graph__.mem_, current);                             \
    tmp_node = node_ref->left_;                                                    \
                                                                                   \
    /* get rW_ counter of left child and act accordingly */                        \
    if (type__ == EXPAND_WLEFT) {                                                  \
      temp = (zero__)                                                              \
        ? MEMORY_GET_ANY(Graph__.mem_, tmp_node)->p_ -                             \
            MEMORY_GET_ANY(Graph__.mem_, tmp_node)->rW_ -                          \
            MEMORY_GET_ANY(Graph__.mem_, tmp_node)->rWl_                           \
        : MEMORY_GET_ANY(Graph__.mem_, tmp_node)->rWl_;                            \
    } else if (type__ == EXPAND_WRIGHT) {                                          \
      temp = (zero__)                                                              \
        ? MEMORY_GET_ANY(Graph__.mem_, tmp_node)->rW_ -                            \
            MEMORY_GET_ANY(Graph__.mem_, tmp_node)->rWh_                           \
        : MEMORY_GET_ANY(Graph__.mem_, tmp_node)->rWh_;                            \
    } else if (type__ == EXPAND_WBOTTOM) {                                         \
      temp = (zero__)                                                              \
        ? MEMORY_GET_ANY(Graph__.mem_, tmp_node)->rWh_ -                           \
            MEMORY_GET_ANY(Graph__.mem_, tmp_node)->rWs_                           \
        : MEMORY_GET_ANY(Graph__.mem_, tmp_node)->rWs_;                            \
    }                                                                              \
                                                                                   \
    if ((uint32_t)temp >= num__) {                                                 \
      current = tmp_node;                                                          \
    } else {                                                                       \
      num__ -= temp;                                                               \
      current = node_ref->right_;                                                  \
                                                                                   \
      if (type__ == EXPAND_WLEFT) {                                                \
        select += (MEMORY_GET_ANY(Graph__.mem_, tmp_node)->p_ -                    \
                   MEMORY_GET_ANY(Graph__.mem_, tmp_node)->rW_);                   \
      } else if (type__ == EXPAND_WRIGHT) {                                        \
        select += MEMORY_GET_ANY(Graph__.mem_, tmp_node)->rW_;                     \
      } else if (type__ == EXPAND_WBOTTOM) {                                       \
        select += MEMORY_GET_ANY(Graph__.mem_, tmp_node)->rWh_;                    \
      }                                                                            \
    }                                                                              \
  }                                                                                \
                                                                                   \
  leaf_ref = MEMORY_GET_LEAF(Graph__.mem_, current);                               \
                                                                                   \
  /* handle last leaf of this query */                                             \
  if (type__ == EXPAND_WLEFT) {                                                    \
    vector = ~(leaf_ref->vectorWl0_) & leaf_ref->vectorWl1_;                       \
    mask = ~(leaf_ref->vectorWl0_);                                                \
  } else if (type__ == EXPAND_WRIGHT) {                                            \
    vector = (leaf_ref->vectorWl0_) & leaf_ref->vectorWl1_;                        \
    mask = leaf_ref->vectorWl0_;                                                   \
  } else if (type__ == EXPAND_WBOTTOM) {                                           \
    vector = (leaf_ref->vectorWl0_) & leaf_ref->vectorWl1_ & leaf_ref->vectorWl2_; \
    mask = leaf_ref->vectorWl0_ & leaf_ref->vectorWl1_;                            \
  }                                                                                \
                                                                                   \
  for (i = 0; num__; i++) {                                                        \
    if ((mask >> (31 - i)) & 0x1) {                                                \
      select++;                                                                    \
      num__ -= (zero__) ? ((vector >> (31 - i)) & 0x1) ^ 1                         \
                        : ((vector >> (31 - i)) & 0x1);                            \
    }                                                                              \
  }                                                                                \
  return select;                                                                   \
}

/*
 * Auxiliary function for 32 bit select operation above L vector.
 *
 * @param  Graph__  Reference to graph structure.
 * @param  pos__  Select query position.
 * @param  zero__  Whether this should select ones or zeroes (true for zero).
 */
int32_t graph_Lselect_(Graph_Struct Graph__, uint32_t num__, bool zero__) {
  GRAPH_SIMPLE_SELECT_EXPAND(EXPAND_L);
}

/*
 * Auxiliary function for 32 bit select operation above top W vector.
 *
 * @param  Graph__  Reference to graph structure.
 * @param  pos__  Select query position.
 * @param  zero__  Whether this should select ones or zeroes (true for zero).
 */
int32_t graph_Wselect_top_(Graph_Struct Graph__, uint32_t num__, bool zero__) {
  GRAPH_SIMPLE_SELECT_EXPAND(EXPAND_WTOP);
}

/*
 * Auxiliary function for 32 bit select operation above left W vector.
 *
 * @param  Graph__  Reference to graph structure.
 * @param  pos__  Select query position.
 * @param  zero__  Whether this should select ones or zeroes (true for zero).
 */
int32_t graph_Wselect_left_(Graph_Struct Graph__, uint32_t num__, bool zero__) {
  GRAPH_MASKED_SELECT_EXPAND(EXPAND_WLEFT);
}

/*
 * Auxiliary function for 32 bit select operation above right W vector.
 *
 * @param  Graph__  Reference to graph structure.
 * @param  pos__  Select query position.
 * @param  zero__  Whether this should select ones or zeroes (true for zero).
 */
int32_t graph_Wselect_right_(Graph_Struct Graph__, uint32_t num__, bool zero__) {
  GRAPH_MASKED_SELECT_EXPAND(EXPAND_WRIGHT);
}

/*
 * Auxiliary function for 32 bit select operation above bottom W vector.
 *
 * @param  Graph__  Reference to graph structure.
 * @param  pos__  Select query position.
 * @param  zero__  Whether this should select ones or zeroes (true for zero).
 */
int32_t graph_Wselect_bottom_(Graph_Struct Graph__, uint32_t num__, bool zero__) {
  GRAPH_MASKED_SELECT_EXPAND(EXPAND_WBOTTOM);
}

int32_t Graph_Select_L(GraphRef Graph__, uint32_t pos__, Graph_value val__) {
  if (val__ == VALUE_0)
    return graph_Lselect_(*Graph__, pos__, true);
  else if (val__ == VALUE_1)
    return graph_Lselect_(*Graph__, pos__, false);

  FATAL("VECTOR_L does not support this query value.");
  return 0;
}

int32_t Graph_Select_W(GraphRef Graph__, uint32_t pos__, Graph_value val__) {
  int32_t temp;

  switch (val__) {
    case VALUE_A:
      temp = graph_Wselect_left_(*Graph__, pos__, true);
      return graph_Wselect_top_(*Graph__, temp, true);
    case VALUE_C:
      temp = graph_Wselect_left_(*Graph__, pos__, false);
      return graph_Wselect_top_(*Graph__, temp, true);
    case VALUE_G:
      temp = graph_Wselect_right_(*Graph__, pos__, true);
      return graph_Wselect_top_(*Graph__, temp, false);
    case VALUE_T:
      temp = graph_Wselect_bottom_(*Graph__, pos__, true);
      temp = graph_Wselect_right_(*Graph__, temp, false);
      return graph_Wselect_top_(*Graph__, temp, false);
    case VALUE_$:
      temp = graph_Wselect_bottom_(*Graph__, pos__, false);
      temp = graph_Wselect_right_(*Graph__, temp, false);
      return graph_Wselect_top_(*Graph__, temp, false);
  }

  FATAL("VECTOR_W does not support this query value.");
  return 0;
}
