#include "deBruijn.h"

void deBruijn_Init(deBruijn_graph *dB__) {
  Graph_Line line;

  // initialize all structures
  Graph_Init(&(dB__->Graph_));

  // insert root node with all four transitions
  GLine_Fill(&line, VALUE_0, VALUE_A, 1);
  GLine_Insert(&(dB__->Graph_), 0, &line);
  GLine_Fill(&line, VALUE_0, VALUE_C, 1);
  GLine_Insert(&(dB__->Graph_), 1, &line);
  GLine_Fill(&line, VALUE_0, VALUE_G, 1);
  GLine_Insert(&(dB__->Graph_), 2, &line);
  GLine_Fill(&line, VALUE_1, VALUE_T, 1);
  GLine_Insert(&(dB__->Graph_), 3, &line);

  // insert target null nodes
  GLine_Fill(&line, VALUE_1, VALUE_$, 0);
  GLine_Insert(&(dB__->Graph_), 4, &line);
  GLine_Fill(&line, VALUE_1, VALUE_$, 0);
  GLine_Insert(&(dB__->Graph_), 5, &line);
  GLine_Fill(&line, VALUE_1, VALUE_$, 0);
  GLine_Insert(&(dB__->Graph_), 6, &line);
  GLine_Fill(&line, VALUE_1, VALUE_$, 0);
  GLine_Insert(&(dB__->Graph_), 7, &line);

  dB__->F_[0] = 4;
  dB__->F_[1] = 5;
  dB__->F_[2] = 6;
  dB__->F_[3] = 7;

  /* calculate csl for all inserted lines */
  deBruijn_update_csl(dB__, 1);
  deBruijn_update_csl(dB__, 3);
  deBruijn_update_csl(dB__, 5);
  deBruijn_update_csl(dB__, 7);
}

void deBruijn_Free(deBruijn_graph *dB__) {
  Graph_Free(&(dB__->Graph_));
}

int32_t deBruijn_Forward_(deBruijn_graph *dB__, int32_t idx__) {
  int32_t rank, spos, temp;
  Graph_Line line;

  DEBRUIJN_VERBOSE(
    printf("[deBruijn]: Calling Forward on index %d\n", idx__);
  )

  // find edge label of given edge (outgoing edge symbol)
  GLine_Get(&(dB__->Graph_), idx__, &line);

  // if edge label is dollar, there is nowhere to go
  if (line.W_ == VALUE_$) return -1;

  // calculate rank of edge label in the W array
  rank = Graph_Rank(&(dB__->Graph_), idx__ + 1, VECTOR_W, line.W_);

  // get starting position of edge label TODO CAN BE OPTIMIZED (cannot be $)
  spos = dB__->F_[line.W_];

  // get index of the last edge of the node pointed to by given edge
  temp = Graph_Rank(&(dB__->Graph_), spos, VECTOR_L, VALUE_1);
  return Graph_Select(&(dB__->Graph_), temp + rank, VECTOR_L, VALUE_1) - 1;
}

int32_t deBruijn_Backward_(deBruijn_graph *dB__, int32_t idx__) {
  int32_t base, temp;
  Graph_value symbol;
  Graph_Line line;

  DEBRUIJN_VERBOSE(
    printf("[deBruijn]: Calling Backward on index %d\n", idx__);
  )

  assert(idx__ < Graph_Size(&(dB__->Graph_)) && idx__ >= 0);

  // find last symbol of this node
  symbol = GET_VALUE_FROM_IDX(idx__, dB__);

  // if last symbol is dollar, there is nowhere to go
  if (symbol == VALUE_$) return -1;

  // rank to current base
  base = Graph_Rank(&(dB__->Graph_), dB__->F_[symbol], VECTOR_L, VALUE_1);

  // rank to given line (including it)
  temp = Graph_Rank(&(dB__->Graph_), idx__ + 1, VECTOR_L, VALUE_1);

  // if given line is not last edge of the node, add that node
  GLine_Get(&(dB__->Graph_), (uint32_t)idx__, &line);
  temp += (line.L_) ? 0 : 1;

  // get index of the edge leading to given node
  return Graph_Select(&(dB__->Graph_), temp - base, VECTOR_W, symbol) - 1;
}

int32_t deBruijn_Outdegree(deBruijn_graph *dB__, int32_t idx__) {
  int32_t node_id;

  DEBRUIJN_VERBOSE(
    printf("[deBruijn]: Calling Outdegree on index %d\n", idx__);
  )

  assert(idx__ < Graph_Size(&(dB__->Graph_)) && idx__ >= 0);

  // get node index
  node_id = Graph_Rank(&(dB__->Graph_), idx__, VECTOR_L, VALUE_1);

  // calculate outdegree itself
  return Graph_Select(&(dB__->Graph_), node_id + 1, VECTOR_L, VALUE_1) -
         Graph_Select(&(dB__->Graph_), node_id, VECTOR_L, VALUE_1);
}

int32_t deBruijn_Find_Edge(deBruijn_graph *dB__, int32_t idx__, Graph_value gval__) {
#ifdef ENABLE_CLEVER_NODE_SPLIT
  return Graph_Find_Edge(&(dB__->Graph_), idx__, gval__);
#else
  int32_t node_id, last_pos, range, select, temp;

  DEBRUIJN_VERBOSE(
    printf("[deBruijn]: Calling Find_Edge on index %d and value %d\n", idx__, gval__);
  )

  // get last edge index for this node
  node_id = Graph_Rank(&(dB__->Graph_), idx__, VECTOR_L, VALUE_1);
  last_pos = Graph_Select(&(dB__->Graph_), node_id + 1, VECTOR_L, VALUE_1);

  // find range (number of edges leaving current node)
  range = last_pos - Graph_Select(&(dB__->Graph_), node_id, VECTOR_L, VALUE_1);

  // get position of last 'symbol' up to this node
  temp = Graph_Rank(&(dB__->Graph_), last_pos, VECTOR_W, gval__);
  select = Graph_Select(&(dB__->Graph_), temp, VECTOR_W, gval__) - 1;

  // check if this position is in the range (transition exist)
  if (last_pos - range <= select) return select;
  return -1;
#endif
}

int32_t deBruijn_Outgoing(deBruijn_graph *dB__, int32_t idx__, Graph_value gval__) {
  int32_t edge_idx;

  DEBRUIJN_VERBOSE(
    printf("[deBruijn]: Calling Outgoing on index %d and value %d\n", idx__, gval__);
  )

  // get index of edge we should follow
  edge_idx = deBruijn_Find_Edge(dB__, idx__, gval__);

  // check if such edge exist
  if (edge_idx == -1) return -1;

  // return index of new node
  return deBruijn_Forward_(dB__, edge_idx);
}

int32_t deBruijn_Indegree(deBruijn_graph *dB__, int32_t idx__) {
  if (idx__ < dB__->F_[0]) return 0;
  return 1;
}

int32_t deBruijn_Incomming(deBruijn_graph *dB__, int32_t idx__, Graph_value gval__) {
  UNUSED(dB__);
  UNUSED(idx__);
  UNUSED(gval__);

  FATAL("Not Implemented");
  return 0;
}

void deBruijn_Label(deBruijn_graph *dB__, int32_t idx__, char *buffer__) {
  int8_t symbol;
  int32_t pos;

#ifdef OMIT_EXCESSIVE_DOLLAR_SIGNS_
  memset(buffer__, ' ', CONTEXT_LENGTH+1);
#else
  memset(buffer__, '$', CONTEXT_LENGTH+1);
#endif

  pos = CONTEXT_LENGTH;
  do {
    if (pos < 0)
      FATAL("deBruijn_Label context is longer than CONTEXT_LENGTH");

    symbol = GET_VALUE_FROM_IDX(idx__, dB__);

    buffer__[pos--] = GET_SYMBOL_FROM_VALUE(symbol);
    idx__ = deBruijn_Backward_(dB__, idx__);
  } while (idx__ != -1);
}

void deBruijn_Print(deBruijn_graph *dB__, bool labels__) {
  char label[CONTEXT_LENGTH + 2];
  int32_t i, j, size;
  int32_t nextPos = 0;
  int32_t next = 0;
  Graph_Line line;

  // print header for main structure
  if (labels__) {
    printf("     F  L  Label  ");
    for (i = 0; i < CONTEXT_LENGTH - 5; i++) printf(" ");
    printf("W   P\n------------------------");
    for (i = 0; i < CONTEXT_LENGTH - 5; i++) printf("-");
    printf("\n");
  } else {
    printf("     F  L  W   P\n-----------------\n");
  }

  size = Graph_Size(&(dB__->Graph_));
  for (i = 0; i < size; i++) {
    printf("%4d: ", i);

    // handle base positions for all symbols
    if (i == nextPos) {
      if (i == dB__->F_[3]) {
        printf("T  ");
      } else if (i == dB__->F_[2]) {
        printf("G  ");
      } else if (i == dB__->F_[1]) {
        printf("C  ");
      } else if (i == dB__->F_[0]) {
        printf("A  ");
      } else {
        printf("$  ");
      }

      // find next position for another symbol
      while (dB__->F_[next] == i && next <= SYMBOL_COUNT) next++;
      nextPos = (next <= SYMBOL_COUNT) ? dB__->F_[next] : -1;
    } else {
      printf("   ");
    }

    // find edge label of given edge (outgoing edge symbol)
    GLine_Get(&(dB__->Graph_), (uint32_t)i, &line);

    printf("%d  ", line.L_);

    // handle finding of all the labels
    if (labels__) {
      label[CONTEXT_LENGTH + 1] = 0;
      deBruijn_Label(dB__, i, label);

      printf("%s  ", label);
      for (j = 0; j < 5 - CONTEXT_LENGTH; j++) {
        printf(" ");
      }
    }
    printf("%c   ", GET_SYMBOL_FROM_VALUE(line.W_));
    printf("%d\n", line.P_);
  }
}

int32_t deBruijn_Get_common_suffix_len_(deBruijn_graph *dB__, int32_t idx__, int32_t limit__) {
  int32_t common, idx1, idx2;
  int32_t symbol1, symbol2;

  DEBRUIJN_VERBOSE(
    printf("[deBruijn]: Calling Get_common_suffix_len on index %d (limit: %d)\n", idx__, limit__);
  )

  // there is no common suffix for top most line
  if (!idx__) return 0;

  common = 0;

  idx1 = idx__;
  idx2 = idx__ - 1;

  // limit size of suffix for better performance
  while (common < limit__) {
    // get symbols itself
    symbol1 = GET_VALUE_FROM_IDX(idx1, dB__);
    symbol2 = GET_VALUE_FROM_IDX(idx2, dB__);

    // dollars are not context
    // (we can check only one - next condition will handle the other)
    if (symbol1 == 4) break;

    // symbols are not the same
    if (symbol1 != symbol2) break;

    // continue backwards
    idx1 = deBruijn_Backward_(dB__, idx1);
    idx2 = deBruijn_Backward_(dB__, idx2);

    common++;

    if (idx1 == -1 || idx2 == -1) break;
  }
  return common;
}

void deBruijn_update_csl(deBruijn_graph *dB__, int32_t target__) {

#if defined(INTEGER_CONTEXT_SHORTENING) \
  || defined(RAS_CONTEXT_SHORTENING)

  int32_t graph_size;

  graph_size = Graph_Size(&(dB__->Graph_));
  assert(target__ <= graph_size);

  /* there is nothing to update for the root node */
  if (target__ == 0) return;

  Graph_Set_csl(&(dB__->Graph_), target__, 
      deBruijn_Get_common_suffix_len_(dB__, target__, CONTEXT_LENGTH));

  /* target is at the bottom of the list - only one csl to update */
  if (target__ == graph_size - 1) return;

  Graph_Set_csl(&(dB__->Graph_), target__ + 1,
      deBruijn_Get_common_suffix_len_(dB__, target__ + 1, CONTEXT_LENGTH));

#elif defined(LABEL_CONTEXT_SHORTENING) \
    || defined(TREE_CONTEXT_SHORTENING)
  UNUSED(dB__);
  UNUSED(target__);

#endif
}

int32_t deBruijn_Shorten_context(deBruijn_graph *dB__, int32_t idx__, int32_t ctx_len__
#if defined(TREE_CONTEXT_SHORTENING)
                                ,Graph_value *label__, int32_t lptr__
#endif
  ) {

  DEBRUIJN_VERBOSE(
    printf("[deBruijn]: Calling Shorten_context on index %d (ctx len: %d)\n", idx__, ctx_len__);
  )

  /* if this is root node it is not possible to shorten context */
  if (idx__ < dB__->F_[0]) return -1;

  /* context of len 0 points surely to root node */
  if (ctx_len__ == 0) return dB__->F_[0] - 1;

  /* context can be surely shortened beyond this point */

#if defined(TREE_CONTEXT_SHORTENING)
  int32_t i, j, temp, current;

  /* get starting position for current context length */
  i = lptr__ - ctx_len__;
  if (i < 0) i += CONTEXT_LENGTH;

  /* find the target node */
  current = 0;
  for (j = 0; j < ctx_len__; j++, i = (i + 1 == CONTEXT_LENGTH) ? 0 : i + 1) {
    temp = deBruijn_Find_Edge(dB__, current, label__[i]);
    current = deBruijn_Forward_(dB__, temp);
  }
  return current;

#else

  while (idx__) {
    /* check for length of common suffix */
#if defined(LABEL_CONTEXT_SHORTENING)
    if (deBruijn_Get_common_suffix_len_(dB__, idx__, ctx_len__) < ctx_len__)
      return idx__;
#elif defined(INTEGER_CONTEXT_SHORTENING) \
   || defined(RAS_CONTEXT_SHORTENING)
    if (Graph_Get_csl(&(dB__->Graph_), idx__) < ctx_len__)
      return idx__;
#endif

    /* move one line higher */
    idx__--;
  }

  UNREACHABLE
  return 0;

#endif  /* endif defined(TREE_CONTEXT_SHORTENING) */
}

void deBruijn_Get_symbol_frequency(deBruijn_graph *dB__, uint32_t idx__, cfreq* freq__) {
#ifdef ENABLE_CLEVER_NODE_SPLIT
  Graph_Get_symbol_frequency(&(dB__->Graph_), idx__, freq__);
#else

  DEBRUIJN_VERBOSE(
    printf("[deBruijn]: Calling Get_symbol_frequency on index %d\n", idx__);
  )

  int32_t idx, cnt;
  Graph_value i;
  Graph_Line line;

  freq__->total_ = 0;

  memset(freq__, 0, sizeof(*freq__));
  for (cnt = 0, i = VALUE_A; i <= VALUE_T; i++) {
    idx = deBruijn_Find_Edge(dB__, idx__, i);
    if (idx == -1) continue;

    cnt++;

    GLine_Get(&(dB__->Graph_), (uint32_t)idx, &line);
    freq__->symbol_[i] = line.P_;
    freq__->total_ += line.P_;
  }

  freq__->symbol_[VALUE_ESC] = cnt;
  freq__->total_ += cnt;
#endif
}

void deBruijn_Insert_test_data(deBruijn_graph *dB__, const Graph_value *L__, const Graph_value *W__,
                               const int32_t *P__, const int32_t F__[SYMBOL_COUNT],
                               const int32_t size__) {
  int32_t i;
  Graph_Line line;

  DEBRUIJN_VERBOSE(
    printf("[deBruijn]: Inserting test data\n");
  )

  Graph_Init(&(dB__->Graph_));

  memcpy(dB__->F_, F__, sizeof(dB__->F_));

  /* insert test data */
  for (i = 0; i < size__; i++) {
    GLine_Fill(&line, L__[i], W__[i], P__[i]);
    GLine_Insert(&(dB__->Graph_), i, &line);
  }

  /* updated common suffix lengths after insertion is done */
  for (i = 1; i < size__; i += 2)
    deBruijn_update_csl(dB__, i);
  deBruijn_update_csl(dB__, size__ - 1);
}
