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

  // insert root (null) node
  /*int32_t i;
  GLine_Fill(&line, VALUE_1, VALUE_$, 0);
  GLine_Insert(&(dB__->Graph_), 0, &line);

  // initialize F array
  for (i = 0; i < SYMBOL_COUNT; i++) dB__->F_[i] = 1;*/
}

void deBruijn_Free(deBruijn_graph *dB__) {
  Graph_Free(&(dB__->Graph_));
}

int32_t deBruijn_Forward_(deBruijn_graph *dB__, int32_t idx__) {
  int32_t rank, spos, temp;
  Graph_Line line;

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

  assert(idx__ < Graph_Size(&(dB__->Graph_)) && idx__ >= 0);

  // get node index
  node_id = Graph_Rank(&(dB__->Graph_), idx__, VECTOR_L, VALUE_1);

  // calculate outdegree itself
  return Graph_Select(&(dB__->Graph_), node_id + 1, VECTOR_L, VALUE_1) -
         Graph_Select(&(dB__->Graph_), node_id, VECTOR_L, VALUE_1);
}

int32_t deBruijn_Find_Edge(deBruijn_graph *dB__, int32_t idx__, Graph_value gval__) {
  int32_t node_id, last_pos, range, select, temp;

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
}

int32_t deBruijn_Outgoing(deBruijn_graph *dB__, int32_t idx__, Graph_value gval__) {
  int32_t edge_idx;

  // get index of edge we should follow
  edge_idx = deBruijn_Find_Edge(dB__, idx__, gval__);

  // check if such edge exist
  if (edge_idx == -1) return -1;

  // return index of new node
  return deBruijn_Forward_(dB__, edge_idx);
}

void deBruijn_Label(deBruijn_graph *dB__, int32_t idx__, char *buffer__) {
  int8_t symbol;
  int32_t pos;

#ifdef OMIT_EXCESSIVE_DOLLAR_SIGNS_
  memset(buffer__, ' ', CONTEXT_LENGTH);
#else
  memset(buffer__, '$', CONTEXT_LENGTH);
#endif

  pos = CONTEXT_LENGTH - 1;
  do {
    symbol = GET_VALUE_FROM_IDX(idx__, dB__);

    buffer__[pos--] = GET_SYMBOL_FROM_VALUE(symbol);
    idx__ = deBruijn_Backward_(dB__, idx__);
  } while (idx__ != -1);
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

void deBruijn_Print(deBruijn_graph *dB__, bool labels__) {
  char label[CONTEXT_LENGTH + 10];
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
    printf("%3d: ", i);

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
      label[CONTEXT_LENGTH] = 0;
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

void deBruijn_Insert_test_data(deBruijn_graph *dB__, const Graph_value *L__, const Graph_value *W__,
                               const int32_t *P__, const int32_t F__[SYMBOL_COUNT],
                               const int32_t size__) {
  int32_t i;
  Graph_Line line;

  Graph_Init(&(dB__->Graph_));

  // initialize variable tracker
  dB__->tracker_.cnt_ = 0;

  // insert test data
  for (i = 0; i < size__; i++) {
    GLine_Fill(&line, L__[i], W__[i], P__[i]);
    GLine_Insert(&(dB__->Graph_), i, &line);
  }

  memcpy(dB__->F_, F__, sizeof(dB__->F_));
}

int32_t deBruijn_Get_common_suffix_len_(deBruijn_graph *dB__, int32_t idx__,
                                        int32_t limit__) {
  int32_t common, idx1, idx2;
  int32_t symbol1, symbol2;

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

int32_t deBruijn_Shorten_context(deBruijn_graph *dB__, int32_t idx__, int32_t ctx_len__) {
  // if this is root node it is not possible to shorten context
  if (idx__ < dB__->F_[0]) return -1;

  // we cannot shorten context at all
  if (deBruijn_Get_common_suffix_len_(dB__, idx__--, ctx_len__) < ctx_len__)
    return -1;

  // context can be surely shortened beyond this point

  // context of len 0 points surely to root node
  if (ctx_len__ == 0) return dB__->F_[0] - 1;

  while (idx__) {
    // check for length of common suffix
    if (deBruijn_Get_common_suffix_len_(dB__, idx__, ctx_len__) < ctx_len__)
      return idx__;

    // move one line higher
    idx__--;
  }

  UNREACHABLE
  return 0;
}

int32_t deBruijn_get_context_len_(deBruijn_graph *dB__, int32_t idx__) {
  int8_t symbol;
  int32_t count = 0;

  do {
    symbol = GET_VALUE_FROM_IDX(idx__, dB__);
    if (symbol == 4) break;

    count++;
    idx__ = deBruijn_Backward_(dB__, idx__);
  } while (idx__ != -1);

  return count;
}

void deBruijn_Get_cumulative_frequency(deBruijn_graph *dB__, uint32_t idx__, Graph_value gval__, cfreq* freq__) {
  int32_t idx;
  Graph_value i;
  Graph_Line line;

  memset(freq__, 0, sizeof(*freq__));

  for (i = VALUE_A; i <= VALUE_T; i++) {
    idx = deBruijn_Find_Edge(dB__, idx__, i);
    if (idx == -1) continue;

    GLine_Get(&(dB__->Graph_), (uint32_t)idx, &line);
    if (line.W_ < gval__) {
      freq__->lower_ += line.P_;
      freq__->upper_ += line.P_;
    } else if (line.W_ == gval__) {
      freq__->upper_ += line.P_;
    }
    freq__->total_ += line.P_;
  }
}
