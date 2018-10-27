#include "decompressor.h"




/* Decoder callback. Will be called once per arcd_dec_get() call. Decoder uses
 * it to get symbol with cumulative probability interval that contains value
 * (v / range). Callback must use arcd_freq_scale() to scale v based on range
 * and current frequency total value. Callback also returns corresponding
 * cumulative probability interval in prob.
 */
//typedef arcd_char_t (*arcd_getch_t)(arcd_range_t v, arcd_range_t range, arcd_prob *prob, void *model);
/* Decoder callback. Decoder uses it when it needs more bits to decode next
 * symbol. Callback returns number of valid most significant bits in buffer.
 * Once callback returns 0 it will not be called again. In that case decoder
 * will use 0's to continue the input stream.
 */
//typedef unsigned (*arcd_input_t)(arcd_buf_t *buf, void *io);



arcd_char_t adaptive_model_getch(const arcd_range_t v, const arcd_range_t range,
                 arcd_prob *const prob, void *const model)
{
  adaptive_model *const m = (adaptive_model *)model;
  const unsigned last = m->size - 1;
  const arcd_freq_t freq = arcd_freq_scale(v, range, m->freq[last]);
  for (unsigned i = 0; last > i; ++i)
  {
    if (m->freq[i] <= freq && freq < m->freq[i + 1])
    {
      prob->lower = m->freq[i];
      prob->upper = m->freq[i + 1];
      prob->total = m->freq[last];
      update(m, i);
      return i;
    }
  }
  assert(!"Bad range");
  return -1;
}



arcd_char_t Decompressor_getch_aux_(const arcd_range_t v, const arcd_range_t range, arcd_prob *prob, void *model) {
  cfreq freq;
  Graph_value i;

  deBruijn_Get_symbol_frequency(&(((decompressor*)model)->dB_), ((decompressor*)model)->state_, &freq);

  prob->higher = 0;

  const arcd_freq_t scaled = arcd_freq_scale(v, range, freq->total_);
  for (i = VALUE_A; i <= VALUE_ESC; i++) {
    prob->lower = prob->higher;
    prob->higher += freq->symbol_[i];

    if (prob->lower <= scaled && scaled < prob->higher) {
      prob->total_ = freq->total_;
      return (arcd_char_t)i;
    }
  }
  UNREACHABLE;
}

unsigned Decompressor_input_aux_(arcd_buf_t* buf, void *io) {
  uint32_t i, read;
  FILE* ifp = (FILE*) io;

  read = fread((void*)buf, 1, 1, ifp);
  printf("read: %d\n", read);
  return 8 * read;
}

void Decompressor_Init(decompressor* D__, FILE* ifp__) {
  deBruijn_Init(&(D__->dB_));

  D__->depth_ = 0;
  D__->state_ = 0;
  D__->tracker_.cnt_ = 0;

  // initialize arythmetic decoder
  arcd_dec_init(&(D__->decoder_), Decompressor_getch_aux_, (void*)D__, Decompressor_input_aux_, (void*)ifp__);
}


void Compressor_Free(compressor* D__) {
  deBruijn_Free(&(D__->dB_));
}

void Compressor_Increase_frequency_rec_(compressor *D__, int32_t idx__, Graph_value gval__) {
  idx__ = deBruijn_Find_Edge(&(D__->dB_), idx__, gval__);

  Graph_Increase_frequency(&(D__->dB_.Graph_), idx__);

  // recursively increase frequency in higher nodes
  int32_t next = deBruijn_Shorten_context(
      &(D__->dB_), idx__, deBruijn_get_context_len_(&(D__->dB_), idx__) - 1);
  if (next == -1) return;

  Compressor_Increase_frequency_rec_(D__, next, gval__);
}

int32_t Compressor_Compress_symbol_aux(compressor *D__, int32_t idx__, Graph_value gval__, bool first);

int32_t Compressor_Compress_symbol(compressor *D__, int32_t idx__, Graph_value gval__) {
  int32_t res, backup, ctx_len;

  //printf("Xdepth %d\n", Xdepth);

  printf("aaaaaa\n");
  backup = idx__;

  if (D__->depth_ >= CONTEXT_LENGTH) {
    ctx_len = deBruijn_get_context_len_(&(D__->dB_), idx__);
    idx__ = deBruijn_Shorten_context(&(D__->dB_), idx__, ctx_len - 1);
    //printf("idx %d\n", idx__);
  }

  printf("pre %d\n", idx__);
  res = Compressor_Compress_symbol_aux(D__, idx__, gval__, true);

  if (D__->depth_ < CONTEXT_LENGTH) {
    D__->depth_++;
  }

  if (res == -1)
    return backup;
  return res;
}

int32_t Compressor_Compress_symbol_aux(compressor *D__, int32_t idx__, Graph_value gval__, bool first) {
  int32_t edge_pos, i, rank, temp;
  int32_t prev_node, ctx_len, x;
  Graph_Line line;

  int32_t transition;

  UNUSED(edge_pos);

  // check if transition exist from this node via given symbol
  transition = deBruijn_Find_Edge(&(D__->dB_), idx__, gval__);

  ctx_len = deBruijn_get_context_len_(&(D__->dB_), idx__);
  prev_node = deBruijn_Shorten_context(&(D__->dB_), idx__, ctx_len - 1);

  printf("%d %d\n", ctx_len, D__->depth_);

  if (transition == -1) {
    // this cannot happen because there level one is full
    // that means, that we are never outputting character itself
    assert(ctx_len != 0);

    printf("here %d\n", idx__);
    // output escape symbol
    // FIXME if (ctx_len > Xdepth)
    if (D__->depth_ < CONTEXT_LENGTH && !first) {
      D__->state_ = idx__;
      deBruijn_Print(&(D__->dB_), true);
      printf("escape %d\n", idx__);
      arcd_enc_put(&(D__->encoder_), VALUE_ESC);
    }
    // TODO


    // go to previous symbol
    printf("%d\n", D__->tracker_.cnt_);
    variable_Tracker_push(D__, &idx__);
    Compressor_Compress_symbol_aux(D__, prev_node, gval__, false);
    variable_Tracker_pop(D__);
  } else {
    // we have transition in this node
    printf("output symbol\n");
    D__->state_ = idx__;
    arcd_enc_put(&(D__->encoder_), gval__);
    Compressor_Increase_frequency_rec_(D__, idx__, gval__);

    return deBruijn_Forward_(&(D__->dB_), idx__);
  }

  // check what symbol is in W
  GLine_Get(&(D__->dB_.Graph_), (uint32_t)idx__, &line);
  if (line.W_ == VALUE_$) {
    // current W symbol is $ - we can simply change it to new one

    // change symbol to new one and increase frequency to 1
    Graph_Change_symbol(&(D__->dB_.Graph_), idx__, gval__);
    Graph_Increase_frequency(&(D__->dB_.Graph_), idx__);

  } else {
    edge_pos = deBruijn_Find_Edge(&(D__->dB_), idx__, gval__);

    // transition already exist at this node - nothing more do
    /*if (edge_pos != -1) {
      Graph_Increase_frequency(&(dB__->Graph_), idx__);
      return deBruijn_Forward_(dB__, idx__);
    }*/

    // insert new edge into this node
    GLine_Fill(&line, VALUE_0, gval__, 1);
    GLine_Insert(&(D__->dB_.Graph_), idx__, &line);

    // update registered variables
    variable_Tracker_update(D__, idx__);

    // update F array according to added node
    for (i = 0; i < SYMBOL_COUNT; i++) {
      if (D__->dB_.F_[i] > idx__) D__->dB_.F_[i]++;
    }
  }

  // find same transition symbol above this line
  rank = Graph_Rank(&(D__->dB_.Graph_), idx__, VECTOR_W, gval__);
  // rank = WT_Rank(&(dB__->W_), idx__, symb__);

  if (!rank) {  // there is no symbol above this
    x = D__->dB_.F_[gval__];

    // update F array
    for (i = gval__ + 1; i < SYMBOL_COUNT; i++) D__->dB_.F_[i]++;

  } else {  // we found another line with this symbol
    // go forward from this node
    temp = Graph_Select(&(D__->dB_.Graph_), rank, VECTOR_W, gval__);
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
  variable_Tracker_update(D__, x);

  return x;
}



int32_t Compressor_Decompress_symbol_aux(compressor *D__, int32_t idx__, Graph_value gval__, bool first);

int32_t Compressor_Decompress_symbol(compressor *D__, int32_t idx__, Graph_value gval__) {
  int32_t res, backup, ctx_len;

  //printf("Xdepth %d\n", Xdepth);

  printf("aaaaaa\n");
  backup = idx__;

  if (D__->depth_ >= CONTEXT_LENGTH) {
    ctx_len = deBruijn_get_context_len_(&(D__->dB_), idx__);
    idx__ = deBruijn_Shorten_context(&(D__->dB_), idx__, ctx_len - 1);
    //printf("idx %d\n", idx__);
  }

  printf("pre %d\n", idx__);
  res = Compressor_Compress_symbol_aux(D__, idx__, gval__, true);

  if (D__->depth_ < CONTEXT_LENGTH) {
    D__->depth_++;
  }

  if (res == -1)
    return backup;
  return res;
}

int32_t Compressor_Decompress_symbol_aux(compressor *D__, int32_t idx__, Graph_value gval__, bool first) {
  int32_t edge_pos, i, rank, temp;
  int32_t prev_node, ctx_len, x;
  Graph_Line line;

  int32_t transition;

  UNUSED(edge_pos);

  // check if transition exist from this node via given symbol
  transition = deBruijn_Find_Edge(&(D__->dB_), idx__, gval__);

  ctx_len = deBruijn_get_context_len_(&(D__->dB_), idx__);
  prev_node = deBruijn_Shorten_context(&(D__->dB_), idx__, ctx_len - 1);

  printf("%d %d\n", ctx_len, D__->depth_);

  if (transition == -1) {
    // this cannot happen because there level one is full
    // that means, that we are never outputting character itself
    assert(ctx_len != 0);

    printf("here %d\n", idx__);
    // output escape symbol
    // FIXME if (ctx_len > Xdepth)
    if (D__->depth_ < CONTEXT_LENGTH && !first) {
      D__->state_ = idx__;
      deBruijn_Print(&(D__->dB_), true);
      printf("escape %d\n", idx__);
      arcd_enc_put(&(D__->encoder_), VALUE_ESC);
    }
    // TODO


    // go to previous symbol
    printf("%d\n", D__->tracker_.cnt_);
    variable_Tracker_push(D__, &idx__);
    Compressor_Compress_symbol_aux(D__, prev_node, gval__, false);
    variable_Tracker_pop(D__);
  } else {
    // we have transition in this node
    printf("output symbol\n");
    D__->state_ = idx__;
    arcd_enc_put(&(D__->encoder_), gval__);
    Compressor_Increase_frequency_rec_(D__, idx__, gval__);

    return deBruijn_Forward_(&(D__->dB_), idx__);
  }

  // check what symbol is in W
  GLine_Get(&(D__->dB_.Graph_), (uint32_t)idx__, &line);
  if (line.W_ == VALUE_$) {
    // current W symbol is $ - we can simply change it to new one

    // change symbol to new one and increase frequency to 1
    Graph_Change_symbol(&(D__->dB_.Graph_), idx__, gval__);
    Graph_Increase_frequency(&(D__->dB_.Graph_), idx__);

  } else {
    edge_pos = deBruijn_Find_Edge(&(D__->dB_), idx__, gval__);

    // transition already exist at this node - nothing more do
    /*if (edge_pos != -1) {
      Graph_Increase_frequency(&(dB__->Graph_), idx__);
      return deBruijn_Forward_(dB__, idx__);
    }*/

    // insert new edge into this node
    GLine_Fill(&line, VALUE_0, gval__, 1);
    GLine_Insert(&(D__->dB_.Graph_), idx__, &line);

    // update registered variables
    variable_Tracker_update(D__, idx__);

    // update F array according to added node
    for (i = 0; i < SYMBOL_COUNT; i++) {
      if (D__->dB_.F_[i] > idx__) D__->dB_.F_[i]++;
    }
  }

  // find same transition symbol above this line
  rank = Graph_Rank(&(D__->dB_.Graph_), idx__, VECTOR_W, gval__);
  // rank = WT_Rank(&(dB__->W_), idx__, symb__);

  if (!rank) {  // there is no symbol above this
    x = D__->dB_.F_[gval__];

    // update F array
    for (i = gval__ + 1; i < SYMBOL_COUNT; i++) D__->dB_.F_[i]++;

  } else {  // we found another line with this symbol
    // go forward from this node
    temp = Graph_Select(&(D__->dB_.Graph_), rank, VECTOR_W, gval__);
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
  variable_Tracker_update(D__, x);

  return x;
}





