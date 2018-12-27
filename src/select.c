#include "structure.h"


int32_t graph_select_simple_(Graph_Struct Graph__, uint32_t num__, bool zero__, int32_t type__) {
  int32_t i, temp;
  int32_t select = 0;
  uint32_t local_var;
  MemPtr tmp_node;
  MemPtr current = Graph__.root_;

  NodeRef node_ref;
  LeafRef leaf_ref;

  assert(type__ == VECTOR_L || type__ == VECTOR_W0);

  /* check correct boundaries */
  if (num__ <= 0)
    return 0;

  node_ref = MEMORY_GET_ANY(Graph__.mem_, Graph__.root_);
  local_var = (type__ == VECTOR_L) ? node_ref->rL_ : GET_RVECTOR(node_ref, 0);

  if (zero__) {
    if (num__ > node_ref->p_ - local_var)
      return -1;
  } else {
    if (num__ > local_var)
      return -1;
  }

  /* traverse the tree and enter correct leaf */
  while (!IS_LEAF(current)) {
    node_ref = MEMORY_GET_NODE(Graph__.mem_, current);
    tmp_node = node_ref->left_;

    if (type__ == VECTOR_L)
      local_var = MEMORY_GET_ANY(Graph__.mem_, tmp_node)->rL_;
    else if (type__ == VECTOR_W0)
      local_var = GET_RVECTOR(MEMORY_GET_ANY(Graph__.mem_, tmp_node), 0);

    /* get r_ counter of left child and act accordingly */
    temp = (zero__) ? MEMORY_GET_ANY(Graph__.mem_, tmp_node)->p_ - local_var : local_var;

    if ((uint32_t) temp >= num__) {
      current = tmp_node;
    } else {
      num__ -= temp;
      select += MEMORY_GET_ANY(Graph__.mem_, tmp_node)->p_;
      current = node_ref->right_;
    }
  }

  leaf_ref = MEMORY_GET_LEAF(Graph__.mem_, current);
  local_var = (type__ == VECTOR_L) ? leaf_ref->vectorL_ : leaf_ref->vectorW_[0];

  /* handle last leaf of this query */
  for (i = 0; num__; i++) {
    num__ -= (zero__) ? ((local_var >> (31 - i)) & 0x1) ^ 1 : ((local_var >> (31 - i)) & 0x1);
  }

  return select + i;
}

int32_t graph_select_masked_(Graph_Struct Graph__, uint32_t num__, bool zero__, int32_t type__) {
  int32_t i, temp;
  int32_t local_p, vector, mask;
  int32_t select = 0;
  uint32_t local_var;
  MemPtr tmp_node;
  MemPtr current = Graph__.root_;

  NodeRef node_ref;
  LeafRef leaf_ref;

  assert(type__ >= VECTOR_W1 && type__ <= VECTOR_W7);

  /* check correct boundaries */
  if (num__ <= 0) return 0;

  node_ref = MEMORY_GET_ANY(Graph__.mem_, Graph__.root_);
  if (type__ == VECTOR_W1) {
    local_p = node_ref->p_ - GET_RVECTOR(node_ref, 0);
    local_var = GET_RVECTOR(node_ref, 1);
  } else if (type__ == VECTOR_W2) {
    local_p = GET_RVECTOR(node_ref, 0);
    local_var = GET_RVECTOR(node_ref, 2);
  } else if (type__ == VECTOR_W3) {
    local_p = node_ref->p_ - GET_RVECTOR(node_ref, 0) - GET_RVECTOR(node_ref, 1);
    local_var = GET_RVECTOR(node_ref, 3);
  } else if (type__ == VECTOR_W4) {
    local_p = GET_RVECTOR(node_ref, 1);
    local_var = GET_RVECTOR(node_ref, 4);
  } else if (type__ == VECTOR_W5) {
    local_p = GET_RVECTOR(node_ref, 0) - GET_RVECTOR(node_ref, 2);
    local_var = GET_RVECTOR(node_ref, 5);
  } else if (type__ == VECTOR_W6) {
    local_p = GET_RVECTOR(node_ref, 2);
    local_var = GET_RVECTOR(node_ref, 6);
  } else if (type__ == VECTOR_W7) {
    local_p = GET_RVECTOR(node_ref, 6);
    local_var = GET_RVECTOR(node_ref, 7);
  }

  if (zero__) {
    if (num__ > local_p - local_var)
      return -1;
  } else {
    if (num__ > local_var)
      return -1;
  }

  /* traverse the tree and enter correct leaf */
  while (!IS_LEAF(current)) {
    node_ref = MEMORY_GET_NODE(Graph__.mem_, current);
    tmp_node = node_ref->left_;

    /* get rW_ counter of left child and act accordingly */
    if (type__ == VECTOR_W1) {
      temp = (zero__) ? MEMORY_GET_ANY(Graph__.mem_, tmp_node)->p_ -
              GET_RVECTOR(MEMORY_GET_ANY(Graph__.mem_, tmp_node), 0) -
              GET_RVECTOR(MEMORY_GET_ANY(Graph__.mem_, tmp_node), 1)
                      : GET_RVECTOR(MEMORY_GET_ANY(Graph__.mem_, tmp_node), 1);
    } else if (type__ == VECTOR_W2) {
      temp = (zero__) ? GET_RVECTOR(MEMORY_GET_ANY(Graph__.mem_, tmp_node), 0) -
              GET_RVECTOR(MEMORY_GET_ANY(Graph__.mem_, tmp_node), 2)
                      : GET_RVECTOR(MEMORY_GET_ANY(Graph__.mem_, tmp_node), 2);
    } else if (type__ == VECTOR_W3) {
      temp = (zero__) ? MEMORY_GET_ANY(Graph__.mem_, tmp_node)->p_ -
              GET_RVECTOR(MEMORY_GET_ANY(Graph__.mem_, tmp_node), 0) -
              GET_RVECTOR(MEMORY_GET_ANY(Graph__.mem_, tmp_node), 1) -
              GET_RVECTOR(MEMORY_GET_ANY(Graph__.mem_, tmp_node), 3)
                      : GET_RVECTOR(MEMORY_GET_ANY(Graph__.mem_, tmp_node), 3);
    } else if (type__ == VECTOR_W4) {
      temp = (zero__) ? GET_RVECTOR(MEMORY_GET_ANY(Graph__.mem_, tmp_node), 1) -
              GET_RVECTOR(MEMORY_GET_ANY(Graph__.mem_, tmp_node), 4)
                      : GET_RVECTOR(MEMORY_GET_ANY(Graph__.mem_, tmp_node), 4);
    } else if (type__ == VECTOR_W5) {
      temp = (zero__) ? GET_RVECTOR(MEMORY_GET_ANY(Graph__.mem_, tmp_node), 0) -
              GET_RVECTOR(MEMORY_GET_ANY(Graph__.mem_, tmp_node), 2) -
              GET_RVECTOR(MEMORY_GET_ANY(Graph__.mem_, tmp_node), 5)
                      : GET_RVECTOR(MEMORY_GET_ANY(Graph__.mem_, tmp_node), 5);
    } else if (type__ == VECTOR_W6) {
      temp = (zero__) ? GET_RVECTOR(MEMORY_GET_ANY(Graph__.mem_, tmp_node), 2) -
              GET_RVECTOR(MEMORY_GET_ANY(Graph__.mem_, tmp_node), 6)
                      : GET_RVECTOR(MEMORY_GET_ANY(Graph__.mem_, tmp_node), 6);
    } else if (type__ == VECTOR_W7) {
      temp = (zero__) ? GET_RVECTOR(MEMORY_GET_ANY(Graph__.mem_, tmp_node), 6) -
              GET_RVECTOR(MEMORY_GET_ANY(Graph__.mem_, tmp_node), 7)
                      : GET_RVECTOR(MEMORY_GET_ANY(Graph__.mem_, tmp_node), 7);
    }

    if ((uint32_t) temp >= num__) {
      current = tmp_node;
    } else {
      num__ -= temp;
      current = node_ref->right_;

#ifdef FAST_SELECT
      select += MEMORY_GET_ANY(Graph__.mem_, tmp_node)->p_;
#else
      if (type__ == VECTOR_W1) {
        select += (MEMORY_GET_ANY(Graph__.mem_, tmp_node)->p_ -
                   GET_RVECTOR(MEMORY_GET_ANY(Graph__.mem_, tmp_node), 0));
      } else if (type__ == VECTOR_W2) {
        select += GET_RVECTOR(MEMORY_GET_ANY(Graph__.mem_, tmp_node), 0);
      } else if (type__ == VECTOR_W3) {
        select += (MEMORY_GET_ANY(Graph__.mem_, tmp_node)->p_ -
                   GET_RVECTOR(MEMORY_GET_ANY(Graph__.mem_, tmp_node), 0) -
                   GET_RVECTOR(MEMORY_GET_ANY(Graph__.mem_, tmp_node), 1));
      } else if (type__ == VECTOR_W4) {
        select += GET_RVECTOR(MEMORY_GET_ANY(Graph__.mem_, tmp_node), 1);
      } else if (type__ == VECTOR_W5) {
        select += GET_RVECTOR(MEMORY_GET_ANY(Graph__.mem_, tmp_node), 0) -
            GET_RVECTOR(MEMORY_GET_ANY(Graph__.mem_, tmp_node), 2);
      } else if (type__ == VECTOR_W6) {
        select += GET_RVECTOR(MEMORY_GET_ANY(Graph__.mem_, tmp_node), 2);
      } else if (type__ == VECTOR_W7) {
        select += GET_RVECTOR(MEMORY_GET_ANY(Graph__.mem_, tmp_node), 6);
      }
#endif
    }
  }

  leaf_ref = MEMORY_GET_LEAF(Graph__.mem_, current);

  /* handle last leaf of this query */
  if (type__ == VECTOR_W1) {
    vector = leaf_ref->vectorW_[1];
    mask = ~(leaf_ref->vectorW_[0]);
  } else if (type__ == VECTOR_W2) {
    vector = leaf_ref->vectorW_[1];
    mask = leaf_ref->vectorW_[0];
  } else if (type__ == VECTOR_W3) {
    vector = leaf_ref->vectorW_[2];
    mask = ~(leaf_ref->vectorW_[0]) & ~(leaf_ref->vectorW_[1]);
  } else if (type__ == VECTOR_W4) {
    vector = leaf_ref->vectorW_[2];
    mask = ~(leaf_ref->vectorW_[0]) & leaf_ref->vectorW_[1];
  } else if (type__ == VECTOR_W5) {
    vector = leaf_ref->vectorW_[2];
    mask = leaf_ref->vectorW_[0] & ~(leaf_ref->vectorW_[1]);
  } else if (type__ == VECTOR_W6) {
    vector = leaf_ref->vectorW_[2];
    mask = leaf_ref->vectorW_[0] & leaf_ref->vectorW_[1];
  } else if (type__ == VECTOR_W7) {
    vector = leaf_ref->vectorW_[3];
    mask = leaf_ref->vectorW_[0] & leaf_ref->vectorW_[1] & leaf_ref->vectorW_[2];
  }

  for (i = 0; num__; i++) {
#ifdef FAST_SELECT
    select++;
    if ((mask >> (31 - i)) & 0x1) {
      num__ -= (zero__) ? ((vector >> (31 - i)) & 0x1) ^ 1 : ((vector >> (31 - i)) & 0x1);
    }
#else
    if ((mask >> (31 - i)) & 0x1) {
      select++;
      num__ -= (zero__) ? ((vector >> (31 - i)) & 0x1) ^ 1 : ((vector >> (31 - i)) & 0x1);
    }
#endif
  }

  return select;
}

/*
 * Auxiliary function for 32 bit select operation above L vector.
 *
 * @param  Graph__  Reference to graph structure.
 * @param  pos__  Select query position.
 * @param  zero__  Whether this should select ones or zeroes (true for zero).
 */
int32_t graph_Lselect_(Graph_Struct Graph__, uint32_t num__, bool zero__) {
  return graph_select_simple_(Graph__, num__, zero__, VECTOR_L);
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
  int32_t temp, temp2;

#ifdef FAST_SELECT
  switch (val__) {
    case VALUE_A:
      return graph_select_masked_(*Graph__, pos__, true, VECTOR_W3);
    case VALUE_Ax:
      return graph_select_masked_(*Graph__, pos__, false, VECTOR_W3);
    case VALUE_C:
      return graph_select_masked_(*Graph__, pos__, true, VECTOR_W4);
    case VALUE_Cx:
      return graph_select_masked_(*Graph__, pos__, false, VECTOR_W4);
    case VALUE_G:
      return graph_select_masked_(*Graph__, pos__, true, VECTOR_W5);
    case VALUE_Gx:
      return graph_select_masked_(*Graph__, pos__, false, VECTOR_W5);
    case VALUE_T:
      return graph_select_masked_(*Graph__, pos__, true, VECTOR_W6);
    case VALUE_Tx:
      return graph_select_masked_(*Graph__, pos__, true, VECTOR_W7);
    case VALUE_$:
      return graph_select_masked_(*Graph__, pos__, false, VECTOR_W7);
    case VALUE_As:
      return graph_select_masked_(*Graph__, pos__, true, VECTOR_W1);
    case VALUE_Cs:
      return graph_select_masked_(*Graph__, pos__, false, VECTOR_W1);
    case VALUE_Gs:
      return graph_select_masked_(*Graph__, pos__, true, VECTOR_W2);
    case VALUE_Ts:
      /* This is not very optimized way of doing this! */
      temp = graph_select_masked_(*Graph__, pos__, false, VECTOR_W2);

      if (temp == -1)
        return temp;

      temp2 = Graph_Select_W(Graph__, 1, VALUE_$);
      if (temp2 == -1 || temp < temp2) {
        return temp;
      }

      return graph_select_masked_(*Graph__, pos__ + 1, false, VECTOR_W2);
  }
#else
    switch (val__) {
    case VALUE_A:
      temp = graph_select_masked_(*Graph__, pos__, true, VECTOR_W3);
      temp = graph_select_masked_(*Graph__, temp, true, VECTOR_W1);
      return graph_select_simple_(*Graph__, temp, true, VECTOR_W0);
    case VALUE_Ax:
      temp = graph_select_masked_(*Graph__, pos__, false, VECTOR_W3);
      temp = graph_select_masked_(*Graph__, temp, true, VECTOR_W1);
      return graph_select_simple_(*Graph__, temp, true, VECTOR_W0);
    case VALUE_C:
      temp = graph_select_masked_(*Graph__, pos__, true, VECTOR_W4);
      temp = graph_select_masked_(*Graph__, temp, false, VECTOR_W1);
      return graph_select_simple_(*Graph__, temp, true, VECTOR_W0);
    case VALUE_Cx:
      temp = graph_select_masked_(*Graph__, pos__, false, VECTOR_W4);
      temp = graph_select_masked_(*Graph__, temp, false, VECTOR_W1);
      return graph_select_simple_(*Graph__, temp, true, VECTOR_W0);
    case VALUE_G:
      temp = graph_select_masked_(*Graph__, pos__, true, VECTOR_W5);
      temp = graph_select_masked_(*Graph__, temp, true, VECTOR_W2);
      return graph_select_simple_(*Graph__, temp, false, VECTOR_W0);
    case VALUE_Gx:
      temp = graph_select_masked_(*Graph__, pos__, false, VECTOR_W5);
      temp = graph_select_masked_(*Graph__, temp, true, VECTOR_W2);
      return graph_select_simple_(*Graph__, temp, false, VECTOR_W0);
    case VALUE_T:
      temp = graph_select_masked_(*Graph__, pos__, true, VECTOR_W6);
      temp = graph_select_masked_(*Graph__, temp, false, VECTOR_W2);
      return graph_select_simple_(*Graph__, temp, false, VECTOR_W0);
    case VALUE_Tx:
      temp = graph_select_masked_(*Graph__, pos__, true, VECTOR_W7);
      temp = graph_select_masked_(*Graph__, temp, false, VECTOR_W6);
      temp = graph_select_masked_(*Graph__, temp, false, VECTOR_W2);
      return graph_select_simple_(*Graph__, temp, false, VECTOR_W0);
    case VALUE_$:
      temp = graph_select_masked_(*Graph__, pos__, false, VECTOR_W7);
      temp = graph_select_masked_(*Graph__, temp, false, VECTOR_W6);
      temp = graph_select_masked_(*Graph__, temp, false, VECTOR_W2);
      return graph_select_simple_(*Graph__, temp, false, VECTOR_W0);
    case VALUE_As:
      temp = graph_select_masked_(*Graph__, pos__, true, VECTOR_W1);
      return graph_select_simple_(*Graph__, temp, true, VECTOR_W0);
    case VALUE_Cs:
      temp = graph_select_masked_(*Graph__, pos__, false, VECTOR_W1);
      return graph_select_simple_(*Graph__, temp, true, VECTOR_W0);
    case VALUE_Gs:
      temp = graph_select_masked_(*Graph__, pos__, true, VECTOR_W2);
      return graph_select_simple_(*Graph__, temp, false, VECTOR_W0);
    case VALUE_Ts:
      /* This is not very optimized way of doing this! */
      temp = graph_select_masked_(*Graph__, pos__, false, VECTOR_W2);
      temp = graph_select_simple_(*Graph__, temp, false, VECTOR_W0);

      if (temp == -1)
        return temp;

      temp2 = Graph_Select_W(Graph__, 1, VALUE_$);
      if (temp2 == -1 || temp < temp2) {
        return temp;
      }

      temp = graph_select_masked_(*Graph__, pos__ + 1, false, VECTOR_W2);
      return graph_select_simple_(*Graph__, temp, false, VECTOR_W0);
  }
#endif

  FATAL("VECTOR_W does not support this query value.");
  return 0;
}
