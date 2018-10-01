#include "deBruijn.h"

void deBruijn_Tracker_push(deBruijn_graph *dB__, int32_t *a__) {
  assert(dB__->tracker_.cnt_ + 1 < CONTEXT_LENGTH + 4);
  dB__->tracker_.arr_[dB__->tracker_.cnt_++] = a__;
}

void deBruijn_Tracker_pop(deBruijn_graph *dB__) {
  assert(dB__->tracker_.cnt_ >= 1);
  dB__->tracker_.cnt_--;
}

void deBruijn_Tracker_update(deBruijn_graph *dB__, int32_t val__) {
  int32_t i;

  for (i = 0; i < dB__->tracker_.cnt_; i++)
    if (*(dB__->tracker_.arr_[i]) >= val__) (*(dB__->tracker_.arr_[i]))++;
}

void deBruijn_Init(deBruijn_graph *dB__) {
  int32_t i;
  Graph_Line line;

  // initialize all structures
  Graph_Init(&(dB__->Graph_));

  // initialize variable tracker
  dB__->tracker_.cnt_ = 0;

  // insert root (null) node
  GLine_Fill(&line, 1, '$', 0);
  GLine_Insert(&(dB__->Graph_), 0, &line);

  // initialize F array
  for (i = 0; i < SYMBOL_COUNT; i++) dB__->F_[i] = 1;
}

void deBruijn_Free(deBruijn_graph *dB__) { Graph_Free(&(dB__->Graph_)); }

int32_t deBruijn_Forward_(deBruijn_graph *dB__, int32_t idx__) {
  int32_t rank, spos, temp;
  Graph_value symbol;
  Graph_Line line;

  // find edge label of given edge (outgoing edge symbol)
  GLine_Get(&(dB__->Graph_), idx__, &line);
  symbol = GET_VALUE_FROM_SYMBOL(line.W_);

  // if edge label is dollar, there is nowhere to go
  if (symbol == VALUE_$) return -1;

  // calculate rank of edge label in the W array
  rank = Graph_Rank(&(dB__->Graph_), idx__ + 1, VECTOR_W, symbol);

  // get starting position of edge label TODO CAN BE OPTIMIZED (cannot be $)
  spos = (symbol != VALUE_$) ? dB__->F_[symbol] : 0;

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

int32_t deBruijn_Edge_Check(deBruijn_graph *dB__, int32_t idx__, char symb__) {
  int32_t node_id, last_pos, range, select, temp;
  Graph_value symbol;

  symbol = GET_VALUE_FROM_SYMBOL(symb__);

  // get last edge index for this node
  node_id = Graph_Rank(&(dB__->Graph_), idx__, VECTOR_L, VALUE_1);
  last_pos = Graph_Select(&(dB__->Graph_), node_id + 1, VECTOR_L, VALUE_1);

  // find range (number of edges leaving current node)
  range = last_pos - Graph_Select(&(dB__->Graph_), node_id, VECTOR_L, VALUE_1);

  // get position of last 'symbol' up to this node
  temp = Graph_Rank(&(dB__->Graph_), last_pos, VECTOR_W, symbol);
  select = Graph_Select(&(dB__->Graph_), temp, VECTOR_W, symbol) - 1;

  // check if this position is in the range (transition exist)
  if (last_pos - range <= select) return select;
  return -1;
}

int32_t deBruijn_Outgoing(deBruijn_graph *dB__, int32_t idx__, char symb__) {
  int32_t edge_idx;

  // get index of edge we should follow
  edge_idx = deBruijn_Edge_Check(dB__, idx__, symb__);

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

int32_t deBruijn_Incomming(deBruijn_graph *dB__, int32_t idx__, char symb__) {
  UNUSED(dB__);
  UNUSED(idx__);
  UNUSED(symb__);

  FATAL("Not Implemented");
  return 0;
}

void deBruijn_Print(deBruijn_graph *dB__, bool labels__) {
  char label[CONTEXT_LENGTH + 1];
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
    printf("%c   ", line.W_);
    printf("%d\n", line.P_);
  }
}

void deBruijn_Insert_test_data(deBruijn_graph *dB__, const int8_t *L__, const char *W__,
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

int32_t deBruijn_Shorten_context(deBruijn_graph *dB__, int32_t idx__,
                                 int32_t ctx_len__) {
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

void deBruijn_Increase_frequency_rec_(deBruijn_graph *dB__, int32_t idx__,
                                      char symb__) {
  Graph_Line line;

  GLine_Get(&(dB__->Graph_), (uint32_t)idx__, &line);

  if (line.W_ != symb__) idx__ = deBruijn_Edge_Check(dB__, idx__, symb__);

  Graph_Increase_frequency(&(dB__->Graph_), idx__);

  // recursively increase frequency in higher nodes
  int32_t next = deBruijn_Shorten_context(
      dB__, idx__, deBruijn_get_context_len_(dB__, idx__) - 1);
  if (next == -1) return;

  deBruijn_Increase_frequency_rec_(dB__, next, symb__);
}

void deBruijn_Add_context_symbol(deBruijn_graph *dB__, int32_t idx__,
                                 char symb__) {
  int32_t edge_pos, i, rank, temp;
  int32_t prev_node, ctx_len;
  int32_t x, sv;
  Graph_Line line;

  // check if shorter context already added this symbol
  // if not, symbol will be recursively added from top to this node
  ctx_len = deBruijn_get_context_len_(dB__, idx__);
  if (ctx_len) {
    prev_node = deBruijn_Shorten_context(dB__, idx__, ctx_len - 1);

    // check if prev node has what it needs
    if (deBruijn_Edge_Check(dB__, prev_node, symb__) == -1) {
      deBruijn_Tracker_push(dB__, &idx__);
      deBruijn_Add_context_symbol(dB__, prev_node, symb__);
      deBruijn_Tracker_pop(dB__);

    } else {
      // increase frequency of these lines
      deBruijn_Increase_frequency_rec_(dB__, prev_node, symb__);
    }
  }

  // check what symbol is in W
  GLine_Get(&(dB__->Graph_), (uint32_t)idx__, &line);
  if (line.W_ == '$') {
    // current W symbol is $ - we can simply change it to new one

    // change symbol to new one and increase frequency to 1
    Graph_Change_symbol(&(dB__->Graph_), idx__, GET_VALUE_FROM_SYMBOL(symb__));
    Graph_Increase_frequency(&(dB__->Graph_), idx__);

  } else {
    edge_pos = deBruijn_Edge_Check(dB__, idx__, symb__);

    // transition already exist at this node - nothing more do
    if (edge_pos != -1) {
      return;
    }

    // insert new edge into this node
    GLine_Fill(&line, 0, symb__, 1);
    GLine_Insert(&(dB__->Graph_), idx__, &line);

    // update registered variables
    deBruijn_Tracker_update(dB__, idx__);

    // update F array according to added node
    for (i = 0; i < SYMBOL_COUNT; i++) {
      if (dB__->F_[i] > idx__) dB__->F_[i]++;
    }
  }

  // find same transition symbol above this line
  rank = Graph_Rank(&(dB__->Graph_), idx__, VECTOR_W,
                    GET_VALUE_FROM_SYMBOL(symb__));
  // rank = WT_Rank(&(dB__->W_), idx__, symb__);

  if (!rank) {  // there is no symbol above this
    sv = GET_VALUE_FROM_SYMBOL(symb__);
    x = dB__->F_[sv];

    // update F array
    for (i = sv + 1; i < SYMBOL_COUNT; i++) dB__->F_[i]++;

  } else {  // we found another line with this symbol
    // go forward from this node
    temp = Graph_Select(&(dB__->Graph_), rank, VECTOR_W,
                        GET_VALUE_FROM_SYMBOL(symb__));
    x = deBruijn_Forward_(dB__, temp - 1) + 1;

    // update F array
    for (i = 0; i < SYMBOL_COUNT; i++) {
      if (dB__->F_[i] >= x) dB__->F_[i]++;
    }
  }

  // insert new leaf node into the graph
  GLine_Fill(&line, 1, '$', 0);
  GLine_Insert(&(dB__->Graph_), x, &line);

  // update registered variables
  deBruijn_Tracker_update(dB__, x);
}
