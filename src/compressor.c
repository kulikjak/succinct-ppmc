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


void Compressor_getprob_aux_(arcd_char_t ch, arcd_prob *prob, void *model) {

  deBruijn_Get_cumulative_frequency(&(((compressor*)model)->dB_), ((compressor*)model)->state_,(Graph_value) ch, (cfreq*) prob);

  printf("%d %d %d\n", prob->lower, prob->upper, prob->total);
  if (prob->total == 0 && prob->upper == 0 && prob->lower == 0) {
    prob->total = 1;
    prob->upper = 1;
  }
  printf("%d %d %d\n", prob->lower, prob->upper, prob->total);
}

void Compressor_output_aux_(arcd_buf_t buf, unsigned buf_bits, void *io) {
  uint32_t i;

  for (i = 0; i < buf_bits; i++) {
    printf("%d\n", (buf >> (7 - i)) & 0x1);
  }
  UNUSED(io);
}

void Compressor_Init(compressor* C__) {
  deBruijn_Init(&(C__->dB_));

  C__->depth_ = 0;
  C__->state_ = 0;
  C__->tracker_.cnt_ = 0;

  // initialize arythmetic encoder
  arcd_enc_init(&(C__->encoder_), Compressor_getprob_aux_, (void*) C__, Compressor_output_aux_, NULL);
}

void Compressor_Finalize(compressor * C__) {
  arcd_enc_fin(&(C__->encoder_));  
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

  backup = idx__;

  if (C__->depth_ >= CONTEXT_LENGTH) {
    // optimize this!
    ctx_len = deBruijn_get_context_len_(&(C__->dB_), idx__);
    idx__ = deBruijn_Shorten_context(&(C__->dB_), idx__, ctx_len - 1);
    //printf("idx %d\n", idx__);
  }

  // 
  res = Compressor_Compress_symbol_aux(C__, idx__, gval__, true);

  // increase context depth if shorter than CONTEXT_LENGTH
  if (C__->depth_ < CONTEXT_LENGTH)
    C__->depth_++;

  if (res == -1)
    return backup;
  return res;
}

int32_t Compressor_Compress_symbol_aux(compressor *C__, int32_t idx__, Graph_value gval__) {
  int32_t edge_pos, i, rank, temp;
  int32_t prev_node, ctx_len, x;
  int32_t transition;
  Graph_Line line;

  // check if transition exist from this node via given symbol
  transition = deBruijn_Find_Edge(&(C__->dB_), idx__, gval__);

  //printf("%d %d\n", ctx_len, C__->depth_);

  // transition does not exist
  if (transition == -1) {

    ctx_len = deBruijn_get_context_len_(&(C__->dB_), idx__);
    prev_node = deBruijn_Shorten_context(&(C__->dB_), idx__, ctx_len - 1);

    /* this cannot happen because there level one is full
     that means, that we are never outputting character itself */
    assert(ctx_len != 0);

    //printf("here %d\n", idx__);
    // output escape symbol
    // FIXME if (ctx_len > Xdepth)

    /*
     * Output escape every time we are shortening the context except when at CONTEXT_LENGTH limit. This is not the most efficient solution since there are cases when escape is the only thing that can be done (when in node with no transition) however it will stil fast, correct and can be optimized later.
     */
    if (C__->depth_ < CONTEXT_LENGT) {
      C__->state_ = idx__;
      compressor_debug("Escape character output.");
      //deBruijn_Print(&(C__->dB_), true);
      arcd_enc_put(&(C__->encoder_), VALUE_ESC);
    }

    /* save current value of the index and go to the context shorter node */
    //printf("%d\n", C__->tracker_.cnt_);
    variable_Tracker_push(C__, &idx__);
    Compressor_Compress_symbol_aux(C__, prev_node, gval__, false);
    variable_Tracker_pop(C__);
  } else {
    // we have transition in this node
    printf("output symbol\n");
    C__->state_ = idx__;
    arcd_enc_put(&(C__->encoder_), gval__);
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



int32_t Compressor_Decompress_symbol_aux(compressor *C__, int32_t idx__, Graph_value gval__, bool first);

int32_t Compressor_Decompress_symbol(compressor *C__, int32_t idx__, Graph_value gval__) {
  int32_t res, backup, ctx_len;

  //printf("Xdepth %d\n", Xdepth);

  printf("aaaaaa\n");
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

int32_t Compressor_Decompress_symbol_aux(compressor *C__, int32_t idx__, Graph_value gval__, bool first) {
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

    printf("here %d\n", idx__);
    // output escape symbol
    // FIXME if (ctx_len > Xdepth)
    if (C__->depth_ < CONTEXT_LENGTH && !first) {
      C__->state_ = idx__;
      deBruijn_Print(&(C__->dB_), true);
      printf("escape %d\n", idx__);
      arcd_enc_put(&(C__->encoder_), VALUE_ESC);
    }
    // TODO


    // go to previous symbol
    printf("%d\n", C__->tracker_.cnt_);
    variable_Tracker_push(C__, &idx__);
    Compressor_Compress_symbol_aux(C__, prev_node, gval__, false);
    variable_Tracker_pop(C__);
  } else {
    // we have transition in this node
    printf("output symbol\n");
    C__->state_ = idx__;
    arcd_enc_put(&(C__->encoder_), gval__);
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





