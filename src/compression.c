#include "compression.h"


void variable_Tracker_push(compressor* C__, int32_t *a__) {
  assert(C__->tracker_.cnt_ + 1 < CONTEXT_LENGTH + 4);
  C__->tracker_.arr_[C__->tracker_.cnt_++] = a__;
}

void variable_Tracker_pop(compressor* C__) {
  assert(C__->tracker_.cnt_ >= 1);
  C__->tracker_.cnt_--;
}

void variable_Tracker_update(compressor* C__, int32_t val__) {
  int32_t i;

  for (i = 0; i < C__->tracker_.cnt_; i++)
    if (*(C__->tracker_.arr_[i]) >= val__) (*(C__->tracker_.arr_[i]))++;
}


void Compressor_Init(compressor* C__) {
  deBruijn_Init(&(C__->dB_));

  C__->depth_ = 0;
  C__->tracker_.cnt_ = 0;
}

void Compressor_Free(compressor* C__) {
  deBruijn_Free(&(C__->dB_));
}

void Compressor_Increase_frequency_rec_(compressor *C__, int32_t idx__, Graph_value gval__) {
  idx__ = deBruijn_Find_Edge(&(C__->dB_), idx__, gval__);

  Graph_Increase_frequency(&(C__->dB_.Graph_), idx__);

  // recursively increase frequency in higher nodes
  int32_t next = deBruijn_Shorten_context(
      &(C__->dB_), idx__, deBruijn_get_context_len_(&(C__->dB_), idx__) - 1);
  if (next == -1) return;

  Compressor_Increase_frequency_rec_(C__, next, gval__);
}

int32_t Compressor_Compress_symbol_aux(compressor *C__, int32_t idx__, Graph_value gval__, bool first);

int32_t Compressor_Compress_symbol(compressor *C__, int32_t idx__, Graph_value gval__) {
  int32_t res, backup, ctx_len;

  //printf("Xdepth %d\n", Xdepth);

  backup = idx__;

  if (C__->depth_ >= CONTEXT_LENGTH) {
    ctx_len = deBruijn_get_context_len_(&(C__->dB_), idx__);
    idx__ = deBruijn_Shorten_context(&(C__->dB_), idx__, ctx_len - 1);
    //printf("idx %d\n", idx__);
  }

  printf("pre %d\n", idx__);
  res = Compressor_Compress_symbol_aux(C__, idx__, gval__, true);

  if (C__->depth_ < CONTEXT_LENGTH) {
    C__->depth_++;
  }

  if (res == -1)
    return backup;
  return res;
}

int32_t Compressor_Compress_symbol_aux(compressor *C__, int32_t idx__, Graph_value gval__, bool first) {
  int32_t edge_pos, i, rank, temp;
  int32_t prev_node, ctx_len, x;
  Graph_Line line;

  int32_t transition;

  UNUSED(edge_pos);

  // check if transition exist from this node via given symbol
  transition = deBruijn_Find_Edge(&(C__->dB_), idx__, gval__);

  ctx_len = deBruijn_get_context_len_(&(C__->dB_), idx__);
  prev_node = deBruijn_Shorten_context(&(C__->dB_), idx__, ctx_len - 1);

  printf("%d %d\n", ctx_len, C__->depth_);

  if (transition == -1) {
    // this cannot happen because there level one is full
    // that means, that we are never outputting character itself
    assert(ctx_len != 0);

    // output escape symbol
    // FIXME if (ctx_len > Xdepth)
    if (C__->depth_ < CONTEXT_LENGTH && !first) {
      printf("escape\n");
    }
    // TODO

    // go to previous symbol
    variable_Tracker_push(C__, &idx__);
    Compressor_Compress_symbol_aux(C__, prev_node, gval__, false);
    variable_Tracker_pop(C__);
  } else {
    // we have transition in this node
    printf("output symbol\n");
    Compressor_Increase_frequency_rec_(C__, idx__, gval__);

    return deBruijn_Forward_(&(C__->dB_), idx__);
  }

  // check what symbol is in W
  GLine_Get(&(C__->dB_.Graph_), (uint32_t)idx__, &line);
  if (line.W_ == VALUE_$) {
    // current W symbol is $ - we can simply change it to new one

    // change symbol to new one and increase frequency to 1
    Graph_Change_symbol(&(C__->dB_.Graph_), idx__, gval__);
    Graph_Increase_frequency(&(C__->dB_.Graph_), idx__);

  } else {
    edge_pos = deBruijn_Find_Edge(&(C__->dB_), idx__, gval__);

    // transition already exist at this node - nothing more do
    /*if (edge_pos != -1) {
      Graph_Increase_frequency(&(dB__->Graph_), idx__);
      return deBruijn_Forward_(dB__, idx__);
    }*/

    // insert new edge into this node
    GLine_Fill(&line, VALUE_0, gval__, 1);
    GLine_Insert(&(C__->dB_.Graph_), idx__, &line);

    // update registered variables
    variable_Tracker_update(C__, idx__);

    // update F array according to added node
    for (i = 0; i < SYMBOL_COUNT; i++) {
      if (C__->dB_.F_[i] > idx__) C__->dB_.F_[i]++;
    }
  }

  // find same transition symbol above this line
  rank = Graph_Rank(&(C__->dB_.Graph_), idx__, VECTOR_W, gval__);
  // rank = WT_Rank(&(dB__->W_), idx__, symb__);

  if (!rank) {  // there is no symbol above this
    x = C__->dB_.F_[gval__];

    // update F array
    for (i = gval__ + 1; i < SYMBOL_COUNT; i++) C__->dB_.F_[i]++;

  } else {  // we found another line with this symbol
    // go forward from this node
    temp = Graph_Select(&(C__->dB_.Graph_), rank, VECTOR_W, gval__);
    x = deBruijn_Forward_(&(C__->dB_), temp - 1) + 1;

    // update F array
    for (i = 0; i < SYMBOL_COUNT; i++) {
      if (C__->dB_.F_[i] >= x) C__->dB_.F_[i]++;
    }
  }

  // insert new leaf node into the graph
  GLine_Fill(&line, VALUE_1, VALUE_$, 0);
  GLine_Insert(&(C__->dB_.Graph_), x, &line);

  // update registered variables
  variable_Tracker_update(C__, x);

  return x;
}



/*void deBruijn_Increase_frequency_rec_(deBruijn_graph *dB__, int32_t idx__, Graph_value gval__) {
  Graph_Line line;

  GLine_Get(&(dB__->Graph_), (uint32_t)idx__, &line);

  if (line.W_ != gval__) idx__ = deBruijn_Find_Edge(dB__, idx__, gval__);

  Graph_Increase_frequency(&(dB__->Graph_), idx__);

  // recursively increase frequency in higher nodes
  int32_t next = deBruijn_Shorten_context(
      dB__, idx__, deBruijn_get_context_len_(dB__, idx__) - 1);
  if (next == -1) return;

  deBruijn_Increase_frequency_rec_(dB__, next, gval__);
}*/

/*int32_t current_line;

void Compression_Init();

void deBruijn_Compress(deBruijn_graph *dB__, int32_t idx__, Graph_value gval__) {
  Graph_Line line;

  GLine_Get(&(dB__->Graph_), (uint32_t)idx__, &line);

  // first insertion into the graph is little bit different
  if (line.W_ == VALUE_$) {

  }


  // symbol on current idx is a $
  edge_pos = deBruijn_Find_Edge(dB__, idx__, gval__);

  // we don't have to add nothing more
  if (edge_pos != 1) {

  } else {
    // we have to add a new symbol

    // first let's output escape symbol

  }
}*/

/*int32_t deBruijn_Add_context_symbol_aux(deBruijn_graph *dB__, int32_t idx__, Graph_value gval__, bool first);

int32_t deBruijn_Add_context_symbol(deBruijn_graph *dB__, int32_t idx__, Graph_value gval__) {
  UNUSED(dB__);
  UNUSED(idx__);
  UNUSED(gval__);
}*/


/*

int32_t deBruijn_Add_context_symbol_auxx(deBruijn_graph *dB__, int32_t idx__, Graph_value gval__, bool first) {
  int32_t edge_pos, i, rank, temp;
  int32_t prev_node, ctx_len, x;
  Graph_Line line;

  // check if shorter context already added this symbol
  // if not, symbol will be recursively added from top to this node
  ctx_len = deBruijn_get_context_len_(dB__, idx__);
  printf("comparison %d %d\n", ctx_len, Xdepth);
  if (ctx_len) {
    prev_node = deBruijn_Shorten_context(dB__, idx__, ctx_len - 1);

    // check if prev node has what it needs
    if (deBruijn_Find_Edge(dB__, prev_node, gval__) == -1) {
      deBruijn_Tracker_push(dB__, &idx__);
      if (!first || Xdepth >= CONTEXT_LENGTH) {
        printf("escape\n");
      }
      deBruijn_Add_context_symbol_aux(dB__, prev_node, gval__, false);
      deBruijn_Tracker_pop(dB__);

    } else {
      // increase frequency of these lines
      deBruijn_Increase_frequency_rec_(dB__, prev_node, gval__);
    }
  }

  // check what symbol is in W
  GLine_Get(&(dB__->Graph_), (uint32_t)idx__, &line);
  if (line.W_ == VALUE_$) {
    // current W symbol is $ - we can simply change it to new one

    // change symbol to new one and increase frequency to 1
    Graph_Change_symbol(&(dB__->Graph_), idx__, gval__);
    Graph_Increase_frequency(&(dB__->Graph_), idx__);

  } else {
    edge_pos = deBruijn_Find_Edge(dB__, idx__, gval__);

    // transition already exist at this node - nothing more do
    if (edge_pos != -1) {
      Graph_Increase_frequency(&(dB__->Graph_), idx__);
      return deBruijn_Forward_(dB__, idx__);
    }

    // insert new edge into this node
    GLine_Fill(&line, VALUE_0, gval__, 1);
    GLine_Insert(&(dB__->Graph_), idx__, &line);

    // update registered variables
    deBruijn_Tracker_update(dB__, idx__);

    // update F array according to added node
    for (i = 0; i < SYMBOL_COUNT; i++) {
      if (dB__->F_[i] > idx__) dB__->F_[i]++;
    }
  }

  // find same transition symbol above this line
  rank = Graph_Rank(&(dB__->Graph_), idx__, VECTOR_W, gval__);
  // rank = WT_Rank(&(dB__->W_), idx__, symb__);

  if (!rank) {  // there is no symbol above this
    x = dB__->F_[gval__];

    // update F array
    for (i = gval__ + 1; i < SYMBOL_COUNT; i++) dB__->F_[i]++;

  } else {  // we found another line with this symbol
    // go forward from this node
    temp = Graph_Select(&(dB__->Graph_), rank, VECTOR_W, gval__);
    x = deBruijn_Forward_(dB__, temp - 1) + 1;

    // update F array
    for (i = 0; i < SYMBOL_COUNT; i++) {
      if (dB__->F_[i] >= x) dB__->F_[i]++;
    }
  }

  // insert new leaf node into the graph
  GLine_Fill(&line, VALUE_1, VALUE_$, 0);
  GLine_Insert(&(dB__->Graph_), x, &line);

  // update registered variables
  deBruijn_Tracker_update(dB__, x);

  return x;
}
*/
