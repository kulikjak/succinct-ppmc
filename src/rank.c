#include "structure.h"


int32_t graph_rank_simple_(Graph_Struct Graph__, uint32_t pos__, int32_t type__) {
  int32_t i, limit, rank;
  MemPtr current;

  current = Graph__.root_;
  rank = 0;

  LeafRef leaf_ref;
  NodeRef node_ref;
  NodeRef left_child;

  assert(type__ == VECTOR_L || type__ == VECTOR_W0);

  node_ref = MEMORY_GET_ANY(Graph__.mem_, current);
  if (type__ == VECTOR_L) {
    if (pos__ >= node_ref->p_)
      return node_ref->rL_;
  } else if (type__ == VECTOR_W0) {
    if (pos__ >= node_ref->p_)
      return GET_RVECTOR(node_ref, 0);
  }

  /* traverse the tree and enter correct leaf */
  while (!IS_LEAF(current)) {
    node_ref = MEMORY_GET_NODE(Graph__.mem_, current);
    left_child = MEMORY_GET_ANY(Graph__.mem_, node_ref->left_);

    /* get p_ counter of left child and act accordingly */
    if ((uint32_t) left_child->p_ >= pos__) {
      current = node_ref->left_;
    } else {
      pos__ -= left_child->p_;
      current = node_ref->right_;

      if (type__ == VECTOR_L)
        rank += left_child->rL_;
      else if (type__ == VECTOR_W0)
        rank += GET_RVECTOR(left_child, 0);
    }
  }
  leaf_ref = MEMORY_GET_LEAF(Graph__.mem_, current);

  /* handle last leaf of this query */
  limit = (pos__ <= 32) ? pos__ : 32;
  if (type__ == VECTOR_L)
    for (i = 0; i < limit; i++)
      rank += (leaf_ref->vectorL_ >> (31 - i)) & 0x1;
  else if (type__ == VECTOR_W0)
    for (i = 0; i < limit; i++)
      rank += (leaf_ref->vectorW_[0] >> (31 - i)) & 0x1;

  return rank;
}

int32_t graph_rank_masked_(Graph_Struct Graph__, uint32_t pos__, int32_t type__) {

  int32_t i, temp, rank;
  int32_t vector, local_p, mask;

  MemPtr current = Graph__.root_;
  rank = 0;

  LeafRef leaf_ref;
  NodeRef node_ref;
  NodeRef left_child;

  assert(type__ >= VECTOR_W1 && type__ <= VECTOR_W7);

  node_ref = MEMORY_GET_ANY(Graph__.mem_, current);
  if (type__ == VECTOR_W1) {
    local_p = node_ref->p_ - GET_RVECTOR(node_ref, 0);
    if (pos__ >= (uint32_t) local_p)
      return GET_RVECTOR(node_ref, 1);
  } else if (type__ == VECTOR_W2) {
    local_p = GET_RVECTOR(node_ref, 0);
    if (pos__ >= (uint32_t) local_p)
      return GET_RVECTOR(node_ref, 2);
  } else if (type__ == VECTOR_W3) {
    local_p = node_ref->p_ - GET_RVECTOR(node_ref, 0) - GET_RVECTOR(node_ref, 1);
    if (pos__ >= (uint32_t) local_p)
      return GET_RVECTOR(node_ref, 3);
  } else if (type__ == VECTOR_W4) {
    local_p = GET_RVECTOR(node_ref, 1);
    if (pos__ >= (uint32_t) local_p)
      return GET_RVECTOR(node_ref, 4);
  } else if (type__ == VECTOR_W5) {
    local_p = GET_RVECTOR(node_ref, 0) - GET_RVECTOR(node_ref, 2);
    if (pos__ >= (uint32_t) local_p)
      return GET_RVECTOR(node_ref, 5);
  } else if (type__ == VECTOR_W6) {
    local_p = GET_RVECTOR(node_ref, 2);
    if (pos__ >= (uint32_t) local_p)
      return GET_RVECTOR(node_ref, 6);
  } else if (type__ == VECTOR_W7) {
    local_p = GET_RVECTOR(node_ref, 6);
    if (pos__ >= (uint32_t) local_p)
      return GET_RVECTOR(node_ref, 7);
  }

  /* traverse the tree and enter correct leaf */
  while (!IS_LEAF(current)) {
    node_ref = MEMORY_GET_NODE(Graph__.mem_, current);
    left_child = MEMORY_GET_ANY(Graph__.mem_, node_ref->left_);

    /* get p_ counter of left child and act accordingly */
    if (type__ == VECTOR_W1) {
      temp = left_child->p_ - GET_RVECTOR(left_child, 0);
    } else if (type__ == VECTOR_W2) {
      temp = GET_RVECTOR(left_child, 0);
    } else if (type__ == VECTOR_W3) {
      temp = left_child->p_ - GET_RVECTOR(left_child, 0) - GET_RVECTOR(left_child, 1);
    } else if (type__ == VECTOR_W4) {
      temp = GET_RVECTOR(left_child, 1);
    } else if (type__ == VECTOR_W5) {
      temp = GET_RVECTOR(left_child, 0) - GET_RVECTOR(left_child, 2);
    } else if (type__ == VECTOR_W6) {
      temp = GET_RVECTOR(left_child, 2);
    } else if (type__ == VECTOR_W7) {
      temp = GET_RVECTOR(left_child, 6);
    }

    if ((uint32_t) temp >= pos__) {
      current = node_ref->left_;
    } else {
      pos__ -= temp;
      current = node_ref->right_;

      if (type__ == VECTOR_W1) {
        rank += GET_RVECTOR(left_child, 1);
      } else if (type__ == VECTOR_W2) {
        rank += GET_RVECTOR(left_child, 2);
      } else if (type__ == VECTOR_W3) {
        rank += GET_RVECTOR(left_child, 3);
      } else if (type__ == VECTOR_W4) {
        rank += GET_RVECTOR(left_child, 4);
      } else if (type__ == VECTOR_W5) {
        rank += GET_RVECTOR(left_child, 5);
      } else if (type__ == VECTOR_W6) {
        rank += GET_RVECTOR(left_child, 6);
      } else if (type__ == VECTOR_W7) {
        rank += GET_RVECTOR(left_child, 7);
      }
    }
  }
  leaf_ref = MEMORY_GET_LEAF(Graph__.mem_, current);

  /* handle last leaf of this query */
  if (type__ == VECTOR_W1) {
    vector = ~(leaf_ref->vectorW_[0]) & leaf_ref->vectorW_[1];
    mask = ~(leaf_ref->vectorW_[0]);
  } else if (type__ == VECTOR_W2) {
    vector = leaf_ref->vectorW_[0] & leaf_ref->vectorW_[1];
    mask = leaf_ref->vectorW_[0];
  } else if (type__ == VECTOR_W3) {
    vector = ~(leaf_ref->vectorW_[0]) & ~(leaf_ref->vectorW_[1]) & leaf_ref->vectorW_[2];
    mask = ~(leaf_ref->vectorW_[0]) & ~(leaf_ref->vectorW_[1]);
  } else if (type__ == VECTOR_W4) {
    vector = ~(leaf_ref->vectorW_[0]) & leaf_ref->vectorW_[1] & leaf_ref->vectorW_[2];
    mask = ~(leaf_ref->vectorW_[0]) & leaf_ref->vectorW_[1];
  } else if (type__ == VECTOR_W5) {
    vector = leaf_ref->vectorW_[0] & ~(leaf_ref->vectorW_[1]) & leaf_ref->vectorW_[2];
    mask = leaf_ref->vectorW_[0] & ~(leaf_ref->vectorW_[1]);
  } else if (type__ == VECTOR_W6) {
    vector = leaf_ref->vectorW_[0] & leaf_ref->vectorW_[1] & leaf_ref->vectorW_[2];
    mask = leaf_ref->vectorW_[0] & leaf_ref->vectorW_[1];
  } else if (type__ == VECTOR_W7) {
    vector = leaf_ref->vectorW_[0] & leaf_ref->vectorW_[1] & leaf_ref->vectorW_[2] &
        leaf_ref->vectorW_[3];
    mask = leaf_ref->vectorW_[0] & leaf_ref->vectorW_[1] & leaf_ref->vectorW_[2];
  }
  for (i = 0; pos__; i++) {
    if ((mask >> (31 - i)) & 0x1) {
      rank += (vector >> (31 - i)) & 0x1;
      pos__--;
    }
  }
  return rank;
}

int32_t graph_fast_rank_masked_(Graph_Struct Graph__, uint32_t pos__, bool zero__, int32_t type__) {
  int32_t i, rank, total;
  int32_t vector, local_p, mask;

  MemPtr current = Graph__.root_;
  rank = 0;
  total = 0;

  LeafRef leaf_ref;
  NodeRef node_ref;
  NodeRef left_child;

  assert(type__ >= VECTOR_W1 && type__ <= VECTOR_W7);

  node_ref = MEMORY_GET_ANY(Graph__.mem_, current);
  if (type__ == VECTOR_W1) {
    if (pos__ >= (uint32_t) node_ref->p_) {
      local_p = node_ref->p_ - GET_RVECTOR(node_ref, 0);
      return (zero__) ? local_p - GET_RVECTOR(node_ref, 1) : GET_RVECTOR(node_ref, 1);
    }
  } else if (type__ == VECTOR_W2) {
    if (pos__ >= (uint32_t) node_ref->p_) {
      local_p = GET_RVECTOR(node_ref, 0);
      return (zero__) ? local_p - GET_RVECTOR(node_ref, 2) : GET_RVECTOR(node_ref, 2);
    }
  } else if (type__ == VECTOR_W3) {
    if (pos__ >= (uint32_t) node_ref->p_) {
      local_p = node_ref->p_ - GET_RVECTOR(node_ref, 0) - GET_RVECTOR(node_ref, 1);
      return (zero__) ? local_p - GET_RVECTOR(node_ref, 3) : GET_RVECTOR(node_ref, 3);
    }
  } else if (type__ == VECTOR_W4) {
    if (pos__ >= (uint32_t) node_ref->p_) {
      local_p = GET_RVECTOR(node_ref, 1);
      return (zero__) ? local_p - GET_RVECTOR(node_ref, 4) : GET_RVECTOR(node_ref, 4);
    }
  } else if (type__ == VECTOR_W5) {
    if (pos__ >= (uint32_t) node_ref->p_) {
      local_p = GET_RVECTOR(node_ref, 0) - GET_RVECTOR(node_ref, 2);
      return (zero__) ? local_p - GET_RVECTOR(node_ref, 5) : GET_RVECTOR(node_ref, 5);
    }
  } else if (type__ == VECTOR_W6) {
    if (pos__ >= (uint32_t) node_ref->p_) {
      local_p = GET_RVECTOR(node_ref, 2);
      return (zero__) ? local_p - GET_RVECTOR(node_ref, 6) : GET_RVECTOR(node_ref, 6);
    }
  } else if (type__ == VECTOR_W7) {
    if (pos__ >= (uint32_t) node_ref->p_) {
      local_p = GET_RVECTOR(node_ref, 6);
      return (zero__) ? local_p - GET_RVECTOR(node_ref, 7) : GET_RVECTOR(node_ref, 7);
    }
  }

  /* traverse the tree and enter correct leaf */
  while (!IS_LEAF(current)) {
    node_ref = MEMORY_GET_NODE(Graph__.mem_, current);
    left_child = MEMORY_GET_ANY(Graph__.mem_, node_ref->left_);

    if ((uint32_t)left_child->p_ >= pos__) {
      current = node_ref->left_;
    } else {
      pos__ -= left_child->p_;
      current = node_ref->right_;

      if (type__ == VECTOR_W1) {
        rank += GET_RVECTOR(left_child, 1);
        total += left_child->p_ - GET_RVECTOR(left_child, 0);
      } else if (type__ == VECTOR_W2) {
        rank += GET_RVECTOR(left_child, 2);
        total += GET_RVECTOR(left_child, 0);
      } else if (type__ == VECTOR_W3) {
        rank += GET_RVECTOR(left_child, 3);
        total += left_child->p_ - GET_RVECTOR(left_child, 0) - GET_RVECTOR(left_child, 1);
      } else if (type__ == VECTOR_W4) {
        rank += GET_RVECTOR(left_child, 4);
        total += GET_RVECTOR(left_child, 1);
      } else if (type__ == VECTOR_W5) {
        rank += GET_RVECTOR(left_child, 5);
        total += GET_RVECTOR(left_child, 0) - GET_RVECTOR(left_child, 2);
      } else if (type__ == VECTOR_W6) {
        rank += GET_RVECTOR(left_child, 6);
        total += GET_RVECTOR(left_child, 2);
      } else if (type__ == VECTOR_W7) {
        rank += GET_RVECTOR(left_child, 7);
        total += GET_RVECTOR(left_child, 6);
      }
    }
  }
  leaf_ref = MEMORY_GET_LEAF(Graph__.mem_, current);

  /* handle last leaf of this query */
  if (type__ == VECTOR_W1) {
    vector = ~(leaf_ref->vectorW_[0]) & leaf_ref->vectorW_[1];
    mask = ~(leaf_ref->vectorW_[0]);
  } else if (type__ == VECTOR_W2) {
    vector = leaf_ref->vectorW_[0] & leaf_ref->vectorW_[1];
    mask = leaf_ref->vectorW_[0];
  } else if (type__ == VECTOR_W3) {
    vector = ~(leaf_ref->vectorW_[0]) & ~(leaf_ref->vectorW_[1]) & leaf_ref->vectorW_[2];
    mask = ~(leaf_ref->vectorW_[0]) & ~(leaf_ref->vectorW_[1]);
  } else if (type__ == VECTOR_W4) {
    vector = ~(leaf_ref->vectorW_[0]) & leaf_ref->vectorW_[1] & leaf_ref->vectorW_[2];
    mask = ~(leaf_ref->vectorW_[0]) & leaf_ref->vectorW_[1];
  } else if (type__ == VECTOR_W5) {
    vector = leaf_ref->vectorW_[0] & ~(leaf_ref->vectorW_[1]) & leaf_ref->vectorW_[2];
    mask = leaf_ref->vectorW_[0] & ~(leaf_ref->vectorW_[1]);
  } else if (type__ == VECTOR_W6) {
    vector = leaf_ref->vectorW_[0] & leaf_ref->vectorW_[1] & leaf_ref->vectorW_[2];
    mask = leaf_ref->vectorW_[0] & leaf_ref->vectorW_[1];
  } else if (type__ == VECTOR_W7) {
    vector = leaf_ref->vectorW_[0] & leaf_ref->vectorW_[1] & leaf_ref->vectorW_[2] &
        leaf_ref->vectorW_[3];
    mask = leaf_ref->vectorW_[0] & leaf_ref->vectorW_[1] & leaf_ref->vectorW_[2];
  }
  for (i = 0; pos__; i++) {
    pos__--;
    if ((mask >> (31 - i)) & 0x1) {
      total++;
      rank += (vector >> (31 - i)) & 0x1;
    }
  }
  return (zero__) ? total - rank : rank;
}

/*
 * Auxiliary function for 32 bit rank operation above L vector.
 *
 * @param  Graph__  Reference to graph structure.
 * @param  pos__  Rank query position.
 */
int32_t graph_Lrank_(Graph_Struct Graph__, uint32_t pos__) {
  return graph_rank_simple_(Graph__, pos__, VECTOR_L);
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

#ifdef FAST_RANK
  switch (val__) {
    case VALUE_A:
      return graph_fast_rank_masked_(*Graph__, pos__, true, VECTOR_W3);
    case VALUE_Ax:
      return graph_fast_rank_masked_(*Graph__, pos__, false, VECTOR_W3);
    case VALUE_C:
      return graph_fast_rank_masked_(*Graph__, pos__, true, VECTOR_W4);
    case VALUE_Cx:
      return graph_fast_rank_masked_(*Graph__, pos__, false, VECTOR_W4);
    case VALUE_G:
      return graph_fast_rank_masked_(*Graph__, pos__, true, VECTOR_W5);
    case VALUE_Gx:
      return graph_fast_rank_masked_(*Graph__, pos__, false, VECTOR_W5);
    case VALUE_T:
      return graph_fast_rank_masked_(*Graph__, pos__, true, VECTOR_W6);
    case VALUE_Tx:
      return graph_fast_rank_masked_(*Graph__, pos__, true, VECTOR_W7);
    case VALUE_$:
      return graph_fast_rank_masked_(*Graph__, pos__, false, VECTOR_W7);
    case VALUE_As:
      return graph_fast_rank_masked_(*Graph__, pos__, true, VECTOR_W1);
    case VALUE_Cs:
      return graph_fast_rank_masked_(*Graph__, pos__, false, VECTOR_W1);
    case VALUE_Gs:
      return graph_fast_rank_masked_(*Graph__, pos__, true, VECTOR_W2);
    case VALUE_Ts:
      temp = graph_fast_rank_masked_(*Graph__, pos__, false, VECTOR_W2);
      return temp - Graph_Rank_W(Graph__, pos__, VALUE_$);
  }
#else
  switch (val__) {
    case VALUE_A:
      temp = pos__ - graph_rank_simple_(*Graph__, pos__, VECTOR_W0);
      temp = temp - graph_rank_masked_(*Graph__, temp, VECTOR_W1);
      return temp - graph_rank_masked_(*Graph__, temp, VECTOR_W3);
    case VALUE_Ax:
      temp = pos__ - graph_rank_simple_(*Graph__, pos__, VECTOR_W0);
      temp = temp - graph_rank_masked_(*Graph__, temp, VECTOR_W1);
      return graph_rank_masked_(*Graph__, temp, VECTOR_W3);
    case VALUE_C:
      temp = pos__ - graph_rank_simple_(*Graph__, pos__, VECTOR_W0);
      temp = graph_rank_masked_(*Graph__, temp, VECTOR_W1);
      return temp - graph_rank_masked_(*Graph__, temp, VECTOR_W4);
    case VALUE_Cx:
      temp = pos__ - graph_rank_simple_(*Graph__, pos__, VECTOR_W0);
      temp = graph_rank_masked_(*Graph__, temp, VECTOR_W1);
      return graph_rank_masked_(*Graph__, temp, VECTOR_W4);
    case VALUE_G:
      temp = graph_rank_simple_(*Graph__, pos__, VECTOR_W0);
      temp = temp - graph_rank_masked_(*Graph__, temp, VECTOR_W2);
      return temp - graph_rank_masked_(*Graph__, temp, VECTOR_W5);
    case VALUE_Gx:
      temp = graph_rank_simple_(*Graph__, pos__, VECTOR_W0);
      temp = temp - graph_rank_masked_(*Graph__, temp, VECTOR_W2);
      return graph_rank_masked_(*Graph__, temp, VECTOR_W5);
    case VALUE_T:
      temp = graph_rank_simple_(*Graph__, pos__, VECTOR_W0);
      temp = graph_rank_masked_(*Graph__, temp, VECTOR_W2);
      return temp - graph_rank_masked_(*Graph__, temp, VECTOR_W6);
    case VALUE_Tx:
      temp = graph_rank_simple_(*Graph__, pos__, VECTOR_W0);
      temp = graph_rank_masked_(*Graph__, temp, VECTOR_W2);
      temp = graph_rank_masked_(*Graph__, temp, VECTOR_W6);
      return temp - graph_rank_masked_(*Graph__, temp, VECTOR_W7);
    case VALUE_$:
      temp = graph_rank_simple_(*Graph__, pos__, VECTOR_W0);
      temp = graph_rank_masked_(*Graph__, temp, VECTOR_W2);
      temp = graph_rank_masked_(*Graph__, temp, VECTOR_W6);
      return graph_rank_masked_(*Graph__, temp, VECTOR_W7);
    case VALUE_As:
      temp = pos__ - graph_rank_simple_(*Graph__, pos__, VECTOR_W0);
      return temp - graph_rank_masked_(*Graph__, temp, VECTOR_W1);
    case VALUE_Cs:
      temp = pos__ - graph_rank_simple_(*Graph__, pos__, VECTOR_W0);
      return graph_rank_masked_(*Graph__, temp, VECTOR_W1);
    case VALUE_Gs:
      temp = graph_rank_simple_(*Graph__, pos__, VECTOR_W0);
      return temp - graph_rank_masked_(*Graph__, temp, VECTOR_W2);
    case VALUE_Ts:
      temp = graph_rank_simple_(*Graph__, pos__, VECTOR_W0);
      temp = graph_rank_masked_(*Graph__, temp, VECTOR_W2);
      return temp - Graph_Rank_W(Graph__, pos__, VALUE_$);
  }
#endif

  FATAL("VECTOR_W does not support this query value.");
  return 0;
}
