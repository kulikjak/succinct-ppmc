#include "compressor.h"

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

void compressor_debug(char* info__) {
  if (COMPRESSOR_VERBOSE)
    fprintf(stderr, "%s\n", info__);
}




/*void Compressor_getprob_aux_(arcd_char_t ch, arcd_prob *prob, void *model) {
  cfreq freq;
  Graph_value i, val = (Graph_value)ch;

  memset(prob, 0, sizeof(*prob));
  deBruijn_Get_symbol_frequency(&(((compressor*)model)->dB_), ((compressor*)model)->state_, &freq);

  //xxprintf("%d - ", ((compressor*)model)->state_);
  //xxprintf("%d %d %d %d %d %d\n", freq.symbol_[0], freq.symbol_[1], freq.symbol_[2], freq.symbol_[3], freq.symbol_[4], freq.total_);

  // in case there is no other transition, probability of escape is 100%
  if (freq.total_ == 0) {
    prob->total = prob->upper = 1;
    printf("AA %d %d %d\n", prob->lower, prob->upper, prob->total);
    return;
  }

  // handle probability if there are some transitions
  prob->total = freq.total_;
  for (i = VALUE_A; i < val; i++)
    prob->lower += freq.symbol_[i];
  prob->upper = prob->lower + freq.symbol_[i];

  printf("AA %d %d %d\n", prob->lower, prob->upper, prob->total);
}*/

void Compressor_encode(compressor* C__, int32_t pos__, Graph_value gval__) {  
  int32_t lower, upper;
  Graph_value i;
  cfreq freq;

  //Graph_value i, val = (Graph_value)ch;

  //memset(prob, 0, sizeof(*prob));
  deBruijn_Get_symbol_frequency(&(C__->dB_), pos__, &freq);

  //xxprintf("%d - ", ((compressor*)model)->state_);
  //xxprintf("%d %d %d %d %d %d\n", freq.symbol_[0], freq.symbol_[1], freq.symbol_[2], freq.symbol_[3], freq.symbol_[4], freq.total_);

  // in case there is no other transition, probability of escape is 100%
  if (freq.total_ == 0) {
    //prob->total = prob->upper = 1;
    //printf("AA %d %d %d\n", prob->lower, prob->upper, prob->total);
    arithmetic_encode(0, 1, 1);
    return;
  }

  // handle probability if there are some transitions
  for (lower = 0, i = VALUE_A; i < gval__; i++)
    lower += freq.symbol_[i];
  upper = lower + freq.symbol_[i];

  //printf("AA %d %d %d\n", prob->lower, prob->upper, prob->total);

  arithmetic_encode(lower, upper, freq.total_);
}

Graph_value Decompressor_decode(decompressor* D__, int32_t idx__) {
  cfreq freq;
  Graph_value i;
  int32_t target;
  int32_t lower, upper;

  deBruijn_Get_symbol_frequency(&(D__->dB_), idx__, &freq);

  target = arithmetic_decode_target(freq.total_);

  if (freq.total_ == 0) {
    arithmetic_decode(0, 1, 1);
    /*prob->total = prob->upper = 1;
    prob->lower = 0;

    printf("AA %d %d %d\n", prob->lower, prob->upper, prob->total);*/
    return VALUE_ESC;
  }  

  upper = 0;

  //const arcd_freq_t scaled = arcd_freq_scale(v, range, freq.total_);
  //printf("XXX: %d %d %d %d\n", v, range, freq.total_, scaled);
  for (i = VALUE_A; i <= VALUE_ESC; i++) {
    lower = upper;
    upper += freq.symbol_[i];

    if (lower <= target && target < upper) {
      //printf("AA %d %d %d\n", prob->lower, prob->upper, prob->total);
      arithmetic_decode(lower, upper, freq.total_);
      return i;
    }
  }
  UNREACHABLE;
}




/*arcd_char_t Decompressor_getch_aux_(const arcd_range_t v, const arcd_range_t range, arcd_prob *prob, void *model) {
  cfreq freq;
  Graph_value i;

  deBruijn_Get_symbol_frequency(&(((decompressor*)model)->dB_), ((decompressor*)model)->state_, &freq);

  //xxprintf("%d - ", ((compressor*)model)->state_);
  //xprintf("%d %d %d %d %d %d\n", freq.symbol_[0], freq.symbol_[1], freq.symbol_[2], freq.symbol_[3], freq.symbol_[4], freq.total_);

  // in case there is no other transition, probability of escape is 100%
  if (freq.total_ == 0) {
    prob->total = prob->upper = 1;
    prob->lower = 0;

    printf("AA %d %d %d\n", prob->lower, prob->upper, prob->total);
    return (arcd_char_t)VALUE_ESC;
  }

  prob->upper = 0;

  const arcd_freq_t scaled = arcd_freq_scale(v, range, freq.total_);
  //printf("XXX: %d %d %d %d\n", v, range, freq.total_, scaled);
  for (i = VALUE_A; i <= VALUE_ESC; i++) {
    prob->lower = prob->upper;
    prob->upper += freq.symbol_[i];

    if (prob->lower <= scaled && scaled < prob->upper) {
      prob->total = freq.total_;

      printf("AA %d %d %d\n", prob->lower, prob->upper, prob->total);
      return (arcd_char_t)i;
    }
  }
  UNREACHABLE;
}

unsigned Decompressor_input_aux_(arcd_buf_t* buf, void *io) {
  uint32_t read;
  FILE* ifp = (FILE*) io;

  read = fread((void*)buf, 1, 1, ifp);
  //printf("read: %d\n", read);
  return 8 * read;
}*/

void Decompressor_Init(decompressor* D__, FILE* ifp__) {
  deBruijn_Init(&(D__->dB_));

  UNUSED(ifp__);

  D__->depth_ = 0;
  D__->state_ = 0;
  D__->tracker_.cnt_ = 0;

  // initialize arythmetic decoder
  startinputtingbits();
  start_decode();
  //arcd_dec_init(&(D__->decoder_), Decompressor_getch_aux_, (void*)D__, Decompressor_input_aux_, (void*)ifp__);
}


void Decompressor_Finalize(decompressor* D__) {
  UNUSED(D__);

  finish_decode();
  doneinputtingbits();
}

void Decompressor_Free(decompressor* D__) {
  deBruijn_Free(&(D__->dB_));
}



/*void Compressor_output_aux_(arcd_buf_t buf, unsigned buf_bits, void *io) {
  //uint32_t i;
  UNUSED(buf_bits);

  FILE* ofp = (FILE*) io;

  fwrite(&buf, 1, 1, ofp);
  //for (i = 0; i < buf_bits; i++) {
  //  printf("%d ", (buf >> (7 - i)) & 0x1);
  //}
  printf("\n");*
}*/

void Compressor_Init(compressor* C__, FILE* ofp__) {
  deBruijn_Init(&(C__->dB_));

  UNUSED(ofp__);

  C__->depth_ = 0;
  C__->state_ = 0;
  C__->tracker_.cnt_ = 0;

  // initialize arythmetic encoder
  /*arcd_enc_init(&(C__->encoder_), Compressor_getprob_aux_, (void*)C__,
                Compressor_output_aux_, (void*)ofp__);*/
  startoutputtingbits();
  start_encode();
}

void Compressor_Finalize(compressor * C__) {
  UNUSED(C__);
  //arcd_enc_fin(&(C__->encoder_));

  finish_encode();
  doneoutputtingbits();
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

int32_t Compressor_Compress_symbol_aux(compressor *C__, int32_t idx__, Graph_value gval__);

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
  res = Compressor_Compress_symbol_aux(C__, idx__, gval__);

  // increase context depth if shorter than CONTEXT_LENGTH
  if (C__->depth_ < CONTEXT_LENGTH)
    C__->depth_++;

  if (res == -1)
    return backup;
  return res;
}

int32_t Compressor_Compress_symbol_aux(compressor *C__, int32_t idx__, Graph_value gval__) {
  int32_t i, rank, temp;
  int32_t prev_node, ctx_len, x;
  int32_t transition;
  Graph_Line line;

  // check if transition exist from this node via given symbol
  transition = deBruijn_Find_Edge(&(C__->dB_), idx__, gval__);

  //printf("%d %d\n", ctx_len, C__->depth_);

  // transition does not exist
  if (transition == -1) {

    // TODO get this information from the depth C__ variable
    ctx_len = deBruijn_get_context_len_(&(C__->dB_), idx__);
    prev_node = deBruijn_Shorten_context(&(C__->dB_), idx__, ctx_len - 1);

    /* this cannot happen because there level one is full that means, that we
     * are never outputting character itself */
    assert(ctx_len != 0);

    /* Output escape every time we are shortening the context except when at
     * CONTEXT_LENGTH limit. This is not the most efficient solution since there
     * are cases when escape is the only thing that can be done (when in node
     * with no transition) however it will stil fast, correct and can be
     * optimized later. */
    //if (C__->depth_ < CONTEXT_LENGTH) {
      C__->state_ = idx__;
      compressor_debug("Escape character output.");
      // deBruijn_Print(&(C__->dB_), true);
      //arcd_enc_put(&(C__->encoder_), VALUE_ESC);
      Compressor_encode(C__, idx__, VALUE_ESC);

      //printf("%d ", VALUE_ESC);
    //}

    /* save current value of the index and go to the context shorter node */
    variable_Tracker_push(C__, &idx__);
    Compressor_Compress_symbol_aux(C__, prev_node, gval__);
    variable_Tracker_pop(C__);
  } else {
    // we have transition in this node
    compressor_debug("Symbol output.");

    // TODO use state for operations and not idx__
    C__->state_ = idx__;
    //arcd_enc_put(&(C__->encoder_), gval__);
    Compressor_encode(C__, idx__, gval__);
    //printf("%d ", gval__);
    Compressor_Increase_frequency_rec_(C__, idx__, gval__);

    //printf("xxxxx %d\n", idx__);
    //deBruijn_Print(&(C__->dB_), true);
    return deBruijn_Forward_(&(C__->dB_), idx__);
  }

  //xxprintf("a %d %d\n", idx__, gval__);
  // check what symbol is in W
  GLine_Get(&(C__->dB_.Graph_), (uint32_t)idx__, &line);
  if (line.W_ == VALUE_$) {
    // current W symbol is $ - we can simply change it to new one

    // change symbol to new one and increase frequency to 1
    Graph_Change_symbol(&(C__->dB_.Graph_), idx__, gval__);
    Graph_Increase_frequency(&(C__->dB_.Graph_), idx__);

  } else {
    //edge_pos = deBruijn_Find_Edge(&(C__->dB_), idx__, gval__);

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


int32_t Decompressor_Decompress_symbol_aux(decompressor *D__, int32_t idx__, Graph_value* gval__);

int32_t Decompressor_Decompress_symbol(decompressor *D__, int32_t idx__, Graph_value* gval__) { //FIXME
  int32_t res, backup, ctx_len;

  backup = idx__;

  if (D__->depth_ >= CONTEXT_LENGTH) {
    // optimize this!
    ctx_len = deBruijn_get_context_len_(&(D__->dB_), idx__);
    idx__ = deBruijn_Shorten_context(&(D__->dB_), idx__, ctx_len - 1);
    //printf("idx %d\n", idx__);
  }

  res = Decompressor_Decompress_symbol_aux(D__, idx__, gval__);

  // increase context depth if shorter than CONTEXT_LENGTH
  if (D__->depth_ < CONTEXT_LENGTH)
    D__->depth_++;

  if (res == -1)
    return backup;
  return res;
}

int32_t Decompressor_Decompress_symbol_aux(decompressor *D__, int32_t idx__, Graph_value* gval__) {
  int32_t i, rank, temp;
  int32_t prev_node, ctx_len, x;
  int32_t transition;
  Graph_Line line;

  // get decompressed symbol
  D__->state_ = idx__;
  Graph_value symbol = Decompressor_decode(D__, idx__); //(Graph_value)arcd_dec_get(&(D__->decoder_));
  //printf("%d ", symbol);

  if (symbol == VALUE_ESC) {
    compressor_debug("Escape character output.");
    // TODO get this information from the depth D__ variable
    ctx_len = deBruijn_get_context_len_(&(D__->dB_), idx__);
    prev_node = deBruijn_Shorten_context(&(D__->dB_), idx__, ctx_len - 1);

    /* this cannot happen because there level one is full that means, that we
     * are never outputting character itself */
    assert(ctx_len != 0);

    variable_Tracker_push((compressor*)D__, &idx__);
    Decompressor_Decompress_symbol_aux(D__, prev_node, &symbol);
    variable_Tracker_pop((compressor*)D__);
    *gval__ = symbol;
  } else {

    compressor_debug("Symbol character output.");
    //printf("%d\n", symbol);

    transition = deBruijn_Find_Edge(&(D__->dB_), idx__, symbol);
    if (transition == -1) {
      printf("Fatal - there is no transition\n");
      exit(1);
    }

    *gval__ = symbol;
    Compressor_Increase_frequency_rec_((compressor*)D__, idx__, symbol);
    //printf("xxxxx %d\n", idx__);
    //deBruijn_Print(&(D__->dB_), true);
    return deBruijn_Forward_(&(D__->dB_), idx__);
  }


  // check if transition exist from this node via given symbol
  //transition = deBruijn_Find_Edge(&(D__->dB_), idx__, symbol);

  //printf("%d %d\n", ctx_len, C__->depth_);

  // transition does not exist
  /* if (transition == -1) {

    // TODO get this information from the depth C__ variable
    ctx_len = deBruijn_get_context_len_(&(C__->dB_), idx__);
    prev_node = deBruijn_Shorten_context(&(C__->dB_), idx__, ctx_len - 1);

    assert(ctx_len != 0);

    if (C__->depth_ < CONTEXT_LENGTH) {
      C__->state_ = idx__;
      compressor_debug("Escape character output.");
      // deBruijn_Print(&(C__->dB_), true);
      arcd_enc_put(&(C__->encoder_), VALUE_ESC);
    }

    variable_Tracker_push((compressor*)D__, &idx__);
    Decompressor_Decompress_symbol_aux(D__, prev_node, gval__);
    variable_Tracker_pop((compressor*)D__);
  } else {
    // we have transition in this node
    compressor_debug("Symbol output.");

    // TODO use state for operations and not idx__
    C__->state_ = idx__;
    arcd_enc_put(&(C__->encoder_), gval__);
    Compressor_Increase_frequency_rec_(C__, idx__, gval__);

    return deBruijn_Forward_(&(C__->dB_), idx__);
  } */

  // SAME BELLOW

  //xxprintf("a %d %d\n", idx__, symbol);
  // check what symbol is in W
  GLine_Get(&(D__->dB_.Graph_), (uint32_t)idx__, &line);
  if (line.W_ == VALUE_$) {
    // current W symbol is $ - we can simply change it to new one

    // change symbol to new one and increase frequency to 1
    Graph_Change_symbol(&(D__->dB_.Graph_), idx__, symbol);
    Graph_Increase_frequency(&(D__->dB_.Graph_), idx__);

  } else {
    // insert new edge into this node
    GLine_Fill(&line, VALUE_0, symbol, 1);
    GLine_Insert(&(D__->dB_.Graph_), idx__, &line);

    // update registered variables
    variable_Tracker_update((compressor*)D__, idx__);

    // update F array according to added node
    for (i = 0; i < SYMBOL_COUNT; i++) {
      if (D__->dB_.F_[i] > idx__) D__->dB_.F_[i]++;
    }
  }

  // find same transition symbol above this line
  rank = Graph_Rank(&(D__->dB_.Graph_), idx__, VECTOR_W, symbol);

  if (!rank) {  // there is no symbol above this
    x = D__->dB_.F_[symbol];

    // update F array
    for (i = symbol + 1; i < SYMBOL_COUNT; i++) D__->dB_.F_[i]++;

  } else {  // we found another line with this symbol
    // go forward from this node
    temp = Graph_Select(&(D__->dB_.Graph_), rank, VECTOR_W, symbol);
    x = deBruijn_Forward_(&(D__->dB_), temp - 1) + 1;

    // update F array
    for (i = 0; i < SYMBOL_COUNT; i++) {
      if (D__->dB_.F_[i] >= x) D__->dB_.F_[i]++;
    }
  }

  // insert new leaf node into the graph
  GLine_Fill(&line, VALUE_1, VALUE_$, 0);
  GLine_Insert(&(D__->dB_.Graph_), x, &line);

  // update registered variables
  variable_Tracker_update((compressor*)D__, x);

  return x;
}



/*
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





*/