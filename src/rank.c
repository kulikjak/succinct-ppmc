#include "structure.h"

/*
 * Code necessary to perform rank operations above all structure vectors.
 *
 * Macros are used to reduce code duplication and also to increase performance
 * by reducing number of calls needed to perform ranks.
 */

#define GRAPH_SIMPLE_RANK_EXPAND(type__) {                      \
  int32_t i, limit, rank;                                       \
  MemPtr current;                                               \
                                                                \
  current = Graph__.root_;                                      \
  rank = 0;                                                     \
                                                                \
  LeafRef leaf_ref;                                             \
  NodeRef node_ref;                                             \
  NodeRef left_child;                                           \
                                                                \
  /* gets optimized away if all expansions are successfull */   \
  if (type__ != EXPAND_L && type__ != EXPAND_W0)                \
    FATAL("Wrong type in GRAPH_SIMPLE_RANK_EXPAND macro");      \
                                                                \
  node_ref = MEMORY_GET_ANY(Graph__.mem_, current);             \
  if (type__ == EXPAND_L) {                                     \
    if (pos__ >= node_ref->p_) return node_ref->rL_;            \
  } else if (type__ == EXPAND_W0) {                             \
    if (pos__ >= node_ref->p_) return node_ref->rW_[0];         \
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
      else if (type__ == EXPAND_W0)                             \
        rank += left_child->rW_[0];                             \
    }                                                           \
  }                                                             \
  leaf_ref = MEMORY_GET_LEAF(Graph__.mem_, current);            \
                                                                \
  /* handle last leaf of this query */                          \
  limit = (pos__ <= 32) ? pos__ : 32;                           \
  if (type__ == EXPAND_L)                                       \
    for (i = 0; i < limit; i++)                                 \
      rank += (leaf_ref->vectorL_ >> (31 - i)) & 0x1;           \
  else if (type__ == EXPAND_W0)                                 \
    for (i = 0; i < limit; i++)                                 \
      rank += (leaf_ref->vectorW_[0] >> (31 - i)) & 0x1;        \
                                                                \
  return rank;                                                  \
}

#define GRAPH_MASKED_RANK_EXPAND(type__) {                                                        \
    int32_t i, temp, rank;                                                                        \
    int32_t vector, local_p, mask;                                                                \
                                                                                                  \
    MemPtr current = Graph__.root_;                                                               \
    rank = 0;                                                                                     \
                                                                                                  \
    LeafRef leaf_ref;                                                                             \
    NodeRef node_ref;                                                                             \
    NodeRef left_child;                                                                           \
                                                                                                  \
    /* gets optimized away if all expansions are successfull */                                   \
    if (type__ != EXPAND_W1 && type__ != EXPAND_W2 && type__ != EXPAND_W3 &&                      \
        type__ != EXPAND_W4 && type__ != EXPAND_W5 && type__ != EXPAND_W6 && type__ != EXPAND_W7) \
      FATAL("Wrong type in GRAPH_MASKED_RANK_EXPAND macro");                                      \
                                                                                                  \
    node_ref = MEMORY_GET_ANY(Graph__.mem_, current);                                             \
    if (type__ == EXPAND_W1) {                                                                    \
      local_p = node_ref->p_ - node_ref->rW_[0];                                                  \
      if (pos__ >= (uint32_t) local_p)                                                            \
        return node_ref->rW_[1];                                                                  \
    } else if (type__ == EXPAND_W2) {                                                             \
      local_p = node_ref->rW_[0];                                                                 \
      if (pos__ >= (uint32_t) local_p)                                                            \
        return node_ref->rW_[2];                                                                  \
    } else if (type__ == EXPAND_W3) {                                                             \
      local_p = node_ref->p_ - node_ref->rW_[0] - node_ref->rW_[1];                               \
      if (pos__ >= (uint32_t) local_p)                                                            \
        return node_ref->rW_[3];                                                                  \
    } else if (type__ == EXPAND_W4) {                                                             \
      local_p = node_ref->rW_[1];                                                                 \
      if (pos__ >= (uint32_t) local_p)                                                            \
        return node_ref->rW_[4];                                                                  \
    } else if (type__ == EXPAND_W5) {                                                             \
      local_p = node_ref->rW_[0] - node_ref->rW_[2];                                              \
      if (pos__ >= (uint32_t) local_p)                                                            \
        return node_ref->rW_[5];                                                                  \
    } else if (type__ == EXPAND_W6) {                                                             \
      local_p = node_ref->rW_[2];                                                                 \
      if (pos__ >= (uint32_t) local_p)                                                            \
        return node_ref->rW_[6];                                                                  \
    } else if (type__ == EXPAND_W7) {                                                             \
      local_p = node_ref->rW_[6];                                                                 \
      if (pos__ >= (uint32_t) local_p)                                                            \
        return node_ref->rW_[7];                                                                  \
    }                                                                                             \
                                                                                                  \
    /* traverse the tree and enter correct leaf */                                                \
    while (!IS_LEAF(current)) {                                                                   \
      node_ref = MEMORY_GET_NODE(Graph__.mem_, current);                                          \
      left_child = MEMORY_GET_ANY(Graph__.mem_, node_ref->left_);                                 \
                                                                                                  \
      /* get p_ counter of left child and act accordingly */                                      \
      if (type__ == EXPAND_W1) {                                                                  \
        temp = left_child->p_ - left_child->rW_[0];                                               \
      } else if (type__ == EXPAND_W2) {                                                           \
        temp = left_child->rW_[0];                                                                \
      } else if (type__ == EXPAND_W3) {                                                           \
        temp = left_child->p_ - left_child->rW_[0] - left_child->rW_[1];                          \
      } else if (type__ == EXPAND_W4) {                                                           \
        temp = left_child->rW_[1];                                                                \
      } else if (type__ == EXPAND_W5) {                                                           \
        temp = left_child->rW_[0] - left_child->rW_[2];                                           \
      } else if (type__ == EXPAND_W6) {                                                           \
        temp = left_child->rW_[2];                                                                \
      } else if (type__ == EXPAND_W7) {                                                           \
        temp = left_child->rW_[6];                                                                \
      }                                                                                           \
                                                                                                  \
      if ((uint32_t) temp >= pos__) {                                                             \
        current = node_ref->left_;                                                                \
      } else {                                                                                    \
        pos__ -= temp;                                                                            \
        current = node_ref->right_;                                                               \
                                                                                                  \
        if (type__ == EXPAND_W1) {                                                                \
          rank += left_child->rW_[1];                                                             \
        } else if (type__ == EXPAND_W2) {                                                         \
          rank += left_child->rW_[2];                                                             \
        } else if (type__ == EXPAND_W3) {                                                         \
          rank += left_child->rW_[3];                                                             \
        } else if (type__ == EXPAND_W4) {                                                         \
          rank += left_child->rW_[4];                                                             \
        } else if (type__ == EXPAND_W5) {                                                         \
          rank += left_child->rW_[5];                                                             \
        } else if (type__ == EXPAND_W6) {                                                         \
          rank += left_child->rW_[6];                                                             \
        } else if (type__ == EXPAND_W7) {                                                         \
          rank += left_child->rW_[7];                                                             \
        }                                                                                         \
      }                                                                                           \
    }                                                                                             \
    leaf_ref = MEMORY_GET_LEAF(Graph__.mem_, current);                                            \
                                                                                                  \
    /* handle last leaf of this query */                                                          \
    if (type__ == EXPAND_W1) {                                                                    \
      vector = ~(leaf_ref->vectorW_[0]) & leaf_ref->vectorW_[1];                                  \
      mask = ~(leaf_ref->vectorW_[0]);                                                            \
    } else if (type__ == EXPAND_W2) {                                                             \
      vector = leaf_ref->vectorW_[0] & leaf_ref->vectorW_[1];                                     \
      mask = leaf_ref->vectorW_[0];                                                               \
    } else if (type__ == EXPAND_W3) {                                                             \
      vector = ~(leaf_ref->vectorW_[0]) & ~(leaf_ref->vectorW_[1]) & leaf_ref->vectorW_[2];       \
      mask = ~(leaf_ref->vectorW_[0]) & ~(leaf_ref->vectorW_[1]);                                 \
    } else if (type__ == EXPAND_W4) {                                                             \
      vector = ~(leaf_ref->vectorW_[0]) & leaf_ref->vectorW_[1] & leaf_ref->vectorW_[2];          \
      mask = ~(leaf_ref->vectorW_[0]) & leaf_ref->vectorW_[1];                                    \
    } else if (type__ == EXPAND_W5) {                                                             \
      vector = leaf_ref->vectorW_[0] & ~(leaf_ref->vectorW_[1]) & leaf_ref->vectorW_[2];          \
      mask = leaf_ref->vectorW_[0] & ~(leaf_ref->vectorW_[1]);                                    \
    } else if (type__ == EXPAND_W6) {                                                             \
      vector = leaf_ref->vectorW_[0] & leaf_ref->vectorW_[1] & leaf_ref->vectorW_[2];             \
      mask = leaf_ref->vectorW_[0] & leaf_ref->vectorW_[1];                                       \
    } else if (type__ == EXPAND_W7) {                                                             \
      vector = leaf_ref->vectorW_[0] & leaf_ref->vectorW_[1] & leaf_ref->vectorW_[2] &            \
          leaf_ref->vectorW_[3];                                                                  \
      mask = leaf_ref->vectorW_[0] & leaf_ref->vectorW_[1] & leaf_ref->vectorW_[2];               \
    }                                                                                             \
    for (i = 0; pos__; i++) {                                                                     \
      if ((mask >> (31 - i)) & 0x1) {                                                             \
        rank += (vector >> (31 - i)) & 0x1;                                                       \
        pos__--;                                                                                  \
      }                                                                                           \
    }                                                                                             \
    return rank;                                                                                  \
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
 * Auxiliary functions for 32 bit rank operation above different W vectors.
 *
 * @param  Graph__  Reference to graph structure.
 * @param  pos__  Rank query position.
 */
int32_t graph_Wrank_0_(Graph_Struct Graph__, uint32_t pos__) {
  GRAPH_SIMPLE_RANK_EXPAND(EXPAND_W0);
}
int32_t graph_Wrank_1_(Graph_Struct Graph__, uint32_t pos__) {
  GRAPH_MASKED_RANK_EXPAND(EXPAND_W1);
}
int32_t graph_Wrank_2_(Graph_Struct Graph__, uint32_t pos__) {
  GRAPH_MASKED_RANK_EXPAND(EXPAND_W2);
}
int32_t graph_Wrank_3_(Graph_Struct Graph__, uint32_t pos__) {
  GRAPH_MASKED_RANK_EXPAND(EXPAND_W3);
}
int32_t graph_Wrank_4_(Graph_Struct Graph__, uint32_t pos__) {
  GRAPH_MASKED_RANK_EXPAND(EXPAND_W4);
}
int32_t graph_Wrank_5_(Graph_Struct Graph__, uint32_t pos__) {
  GRAPH_MASKED_RANK_EXPAND(EXPAND_W5);
}
int32_t graph_Wrank_6_(Graph_Struct Graph__, uint32_t pos__) {
  GRAPH_MASKED_RANK_EXPAND(EXPAND_W6);
}
int32_t graph_Wrank_7_(Graph_Struct Graph__, uint32_t pos__) {
  GRAPH_MASKED_RANK_EXPAND(EXPAND_W7);
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
      temp = pos__ - graph_Wrank_0_(*Graph__, pos__);
      temp = temp - graph_Wrank_1_(*Graph__, temp);
      return temp - graph_Wrank_3_(*Graph__, temp);
    case VALUE_Ax:
      temp = pos__ - graph_Wrank_0_(*Graph__, pos__);
      temp = temp - graph_Wrank_1_(*Graph__, temp);
      return graph_Wrank_3_(*Graph__, temp);
    case VALUE_C:
      temp = pos__ - graph_Wrank_0_(*Graph__, pos__);
      temp = graph_Wrank_1_(*Graph__, temp);
      return temp - graph_Wrank_4_(*Graph__, temp);
    case VALUE_Cx:
      temp = pos__ - graph_Wrank_0_(*Graph__, pos__);
      temp = graph_Wrank_1_(*Graph__, temp);
      return graph_Wrank_4_(*Graph__, temp);
    case VALUE_G:
      temp = graph_Wrank_0_(*Graph__, pos__);
      temp = temp - graph_Wrank_2_(*Graph__, temp);
      return temp - graph_Wrank_5_(*Graph__, temp);
    case VALUE_Gx:
      temp = graph_Wrank_0_(*Graph__, pos__);
      temp = temp - graph_Wrank_2_(*Graph__, temp);
      return graph_Wrank_5_(*Graph__, temp);
    case VALUE_T:
      temp = graph_Wrank_0_(*Graph__, pos__);
      temp = graph_Wrank_2_(*Graph__, temp);
      return temp - graph_Wrank_6_(*Graph__, temp);
    case VALUE_Tx:
      temp = graph_Wrank_0_(*Graph__, pos__);
      temp = graph_Wrank_2_(*Graph__, temp);
      temp = graph_Wrank_6_(*Graph__, temp);
      return temp - graph_Wrank_7_(*Graph__, temp);
    case VALUE_$:
      temp = graph_Wrank_0_(*Graph__, pos__);
      temp = graph_Wrank_2_(*Graph__, temp);
      temp = graph_Wrank_6_(*Graph__, temp);
      return graph_Wrank_7_(*Graph__, temp);
    case VALUE_As:
      temp = pos__ - graph_Wrank_0_(*Graph__, pos__);
      return temp - graph_Wrank_1_(*Graph__, temp);
    case VALUE_Cs:
      temp = pos__ - graph_Wrank_0_(*Graph__, pos__);
      return graph_Wrank_1_(*Graph__, temp);
    case VALUE_Gs:
      temp = graph_Wrank_0_(*Graph__, pos__);
      return temp - graph_Wrank_2_(*Graph__, temp);
    case VALUE_Ts:
      temp = graph_Wrank_0_(*Graph__, pos__);
      temp = graph_Wrank_2_(*Graph__, temp);
      return temp - Graph_Rank_W(Graph__, pos__, VALUE_$);
  }

  FATAL("VECTOR_W does not support this query value.");
  return 0;
}
