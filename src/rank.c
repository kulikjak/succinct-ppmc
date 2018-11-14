#include "structure.h"

/*
 * Code necessary to perform rank operations above all structure vectors.
 *
 * Macros are used to reduce code duplication and also to increase performance
 * by reducing number of calls needed to perform ranks.
 */

#define GRAPH_SIMPLE_RANK_EXPAND(type__) {                      \
  int32_t i, limit, rank;                                       \
  mem_ptr current;                                              \
                                                                \
  current = Graph__.root_;                                      \
  rank = 0;                                                     \
                                                                \
  LeafRef leaf_ref;                                             \
  NodeRef node_ref;                                             \
  NodeRef left_child;                                           \
                                                                \
  /* gets optimized away if all expansions are successfull */   \
  if (type__ != EXPAND_L && type__ != EXPAND_WTOP)              \
    FATAL("Wrong type in GRAPH_SIMPLE_RANK_EXPAND macro");      \
                                                                \
  node_ref = MEMORY_GET_ANY(Graph__.mem_, current);             \
  if (type__ == EXPAND_L) {                                     \
    if (pos__ >= node_ref->p_) return node_ref->rL_;            \
  } else if (type__ == EXPAND_WTOP) {                           \
    if (pos__ >= node_ref->p_) return node_ref->rW_;            \
  }                                                             \
                                                                \
  /* traverse the tree and enter correct leaf */                \
  while (!IS_LEAF(current)) {                                   \
    node_ref = MEMORY_GET_NODE(Graph__.mem_, current);          \
    left_child = MEMORY_GET_ANY(Graph__.mem_, node_ref->left_); \
                                                                \
    /* get p_ counter of left child and act accordingly */      \
    if ((uint32_t)left_child->p_ >= pos__) {                    \
      current = node_ref->left_;                                \
    } else {                                                    \
      pos__ -= left_child->p_;                                  \
      current = node_ref->right_;                               \
                                                                \
      if (type__ == EXPAND_L)                                   \
        rank += left_child->rL_;                                \
      else if (type__ == EXPAND_WTOP)                           \
        rank += left_child->rW_;                                \
    }                                                           \
  }                                                             \
  leaf_ref = MEMORY_GET_LEAF(Graph__.mem_, current);            \
                                                                \
  /* handle last leaf of this query */                          \
  limit = (pos__ <= 32) ? pos__ : 32;                           \
  if (type__ == EXPAND_L)                                       \
    for (i = 0; i < limit; i++)                                 \
      rank += (leaf_ref->vectorL_ >> (31 - i)) & 0x1;           \
  else if (type__ == EXPAND_WTOP)                               \
    for (i = 0; i < limit; i++)                                 \
      rank += (leaf_ref->vectorWl0_ >> (31 - i)) & 0x1;         \
                                                                \
  return rank;                                                  \
}

#define GRAPH_MASKED_RANK_EXPAND(type__) {                                  \
  int32_t i, temp, rank;                                                    \
  int32_t vector, local_p, mask;                                            \
                                                                            \
  mem_ptr current = Graph__.root_;                                          \
  rank = 0;                                                                 \
                                                                            \
  LeafRef leaf_ref;                                                         \
  NodeRef node_ref;                                                         \
  NodeRef left_child;                                                       \
                                                                            \
  /* gets optimized away if all expansions are successfull */               \
  if (type__ != EXPAND_WLEFT &&                                             \
      type__ != EXPAND_WRIGHT &&                                            \
      type__ != EXPAND_WBOTTOM)                                             \
    FATAL("Wrong type in GRAPH_MASKED_RANK_EXPAND macro");                  \
                                                                            \
  node_ref = MEMORY_GET_ANY(Graph__.mem_, current);                         \
                                                                            \
  if (type__ == EXPAND_WLEFT) {                                             \
    local_p = node_ref->p_ - node_ref->rW_;                                 \
    if (pos__ >= (uint32_t)local_p) return node_ref->rWl_;                  \
  } else if (type__ == EXPAND_WRIGHT) {                                     \
    local_p = node_ref->rW_;                                                \
    if (pos__ >= (uint32_t)local_p) return node_ref->rWh_;                  \
  } else if (type__ == EXPAND_WBOTTOM) {                                    \
    local_p = node_ref->rWh_;                                               \
    if (pos__ >= (uint32_t)local_p) return node_ref->rWs_;                  \
  }                                                                         \
                                                                            \
  /* traverse the tree and enter correct leaf */                            \
  while (!IS_LEAF(current)) {                                               \
    node_ref = MEMORY_GET_NODE(Graph__.mem_, current);                      \
    left_child = MEMORY_GET_ANY(Graph__.mem_, node_ref->left_);             \
                                                                            \
    /* get p_ counter of left child and act accordingly */                  \
    if (type__ == EXPAND_WLEFT) {                                           \
      temp = left_child->p_ - left_child->rW_;                              \
    } else if (type__ == EXPAND_WRIGHT) {                                   \
      temp = left_child->rW_;                                               \
    } else if (type__ == EXPAND_WBOTTOM) {                                  \
      temp = left_child->rWh_;                                              \
    }                                                                       \
    if ((uint32_t)temp >= pos__) {                                          \
      current = node_ref->left_;                                            \
    } else {                                                                \
      pos__ -= temp;                                                        \
      current = node_ref->right_;                                           \
                                                                            \
      if (type__ == EXPAND_WLEFT) {                                         \
        rank += left_child->rWl_;                                           \
      } else if (type__ == EXPAND_WRIGHT) {                                 \
        rank += left_child->rWh_;                                           \
      } else if (type__ == EXPAND_WBOTTOM) {                                \
        rank += left_child->rWs_;                                           \
      }                                                                     \
    }                                                                       \
  }                                                                         \
  leaf_ref = MEMORY_GET_LEAF(Graph__.mem_, current);                        \
                                                                            \
  /* handle last leaf of this query */                                      \
  if (type__ == EXPAND_WLEFT) {                                             \
    vector = ~(leaf_ref->vectorWl0_) & leaf_ref->vectorWl1_;                \
    mask = ~(leaf_ref->vectorWl0_);                                         \
  } else if (type__ == EXPAND_WRIGHT) {                                     \
    vector = leaf_ref->vectorWl0_ & leaf_ref->vectorWl1_;                   \
    mask = leaf_ref->vectorWl0_;                                            \
  } else if (type__ == EXPAND_WBOTTOM) {                                    \
    vector =                                                                \
        leaf_ref->vectorWl0_ & leaf_ref->vectorWl1_ & leaf_ref->vectorWl2_; \
    mask = leaf_ref->vectorWl0_ & leaf_ref->vectorWl1_;                     \
  }                                                                         \
                                                                            \
  for (i = 0; pos__; i++) {                                                 \
    if ((mask >> (31 - i)) & 0x1) {                                         \
      rank += (vector >> (31 - i)) & 0x1;                                   \
      pos__--;                                                              \
    }                                                                       \
  }                                                                         \
  return rank;                                                              \
}

/*
 * Auxiliary function for 32 bit rank operation above L vector.
 *
 * @param  Graph__  Reference to graph structure.
 * @param  pos__  Rank query position.
 */
int32_t graph_Lrank_(Graph_Struct Graph__, uint32_t pos__) {
  GRAPH_SIMPLE_RANK_EXPAND(EXPAND_L);
}

/*
 * Auxiliary function for 32 bit rank operation above top W vector.
 *
 * @param  Graph__  Reference to graph structure.
 * @param  pos__  Rank query position.
 */
int32_t graph_Wrank_top_(Graph_Struct Graph__, uint32_t pos__) {
  GRAPH_SIMPLE_RANK_EXPAND(EXPAND_WTOP);
}

/*
 * Auxiliary function for 32 bit rank operation above left W vector.
 *
 * @param  Graph__  Reference to graph structure.
 * @param  pos__  Rank query position.
 */
int32_t graph_Wrank_left_(Graph_Struct Graph__, uint32_t pos__) {
  GRAPH_MASKED_RANK_EXPAND(EXPAND_WLEFT);
}

/*
 * Auxiliary function for 32 bit rank operation above right W vector.
 *
 * @param  Graph__  Reference to graph structure.
 * @param  pos__  Rank query position.
 */
int32_t graph_Wrank_right_(Graph_Struct Graph__, uint32_t pos__) {
  GRAPH_MASKED_RANK_EXPAND(EXPAND_WRIGHT);
}

/*
 * Auxiliary function for 32 bit rank operation above bottom W vector.
 *
 * @param  Graph__  Reference to graph structure.
 * @param  pos__  Rank query position.
 */
int32_t graph_Wrank_bottom_(Graph_Struct Graph__, uint32_t pos__) {
  GRAPH_MASKED_RANK_EXPAND(EXPAND_WBOTTOM);
}

int32_t Graph_Rank_L(GraphRef Graph__, uint32_t pos__, Graph_value val__) {
  if (val__ == VALUE_0)
    return pos__ - graph_Lrank_(*Graph__, pos__);
  else if (val__ == VALUE_1)
    return graph_Lrank_(*Graph__, pos__);

  FATAL("VECTOR_L does not support this query value.");
  return 0;
}

int32_t Graph_Rank_W(GraphRef Graph__, uint32_t pos__, Graph_value val__) {
  int32_t temp;

  switch (val__) {
    case VALUE_A:
      temp = pos__ - graph_Wrank_top_(*Graph__, pos__);
      return temp - graph_Wrank_left_(*Graph__, temp);
    case VALUE_C:
      temp = pos__ - graph_Wrank_top_(*Graph__, pos__);
      return graph_Wrank_left_(*Graph__, temp);
    case VALUE_G:
      temp = graph_Wrank_top_(*Graph__, pos__);
      return temp - graph_Wrank_right_(*Graph__, temp);
    case VALUE_T:
      temp = graph_Wrank_top_(*Graph__, pos__);
      temp = graph_Wrank_right_(*Graph__, temp);
      return temp - graph_Wrank_bottom_(*Graph__, temp);
    case VALUE_$:
      temp = graph_Wrank_top_(*Graph__, pos__);
      temp = graph_Wrank_right_(*Graph__, temp);
      return graph_Wrank_bottom_(*Graph__, temp);
  }

  FATAL("VECTOR_W does not support this query value.");
  return 0;
}
