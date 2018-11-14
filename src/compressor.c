#include "compressor.h"

void Process_Init(compressor* C__) {
  deBruijn_Init(&(C__->dB_));
  Tracker_reset();

  C__->depth_ = 0;
  C__->state_ = 0;

#if defined(TREE_CONTEXT_SHORTENING)
  C__->lptr_ = 0;
#endif

#if defined(ENABLE_LOOKUP_CACHE) \
  && defined(ENABLE_CACHE_STATS)
  cache_stats_prep();
#endif
}

void Process_Free(compressor * C__) {
  deBruijn_Free(&(C__->dB_));

#if defined(ENABLE_LOOKUP_CACHE) \
  && defined(ENABLE_CACHE_STATS)
    cache_stats_print();
#endif
}

void Compressor_encode_(compressor* C__, int32_t pos__, Graph_value gval__) {  
  int32_t lower, upper;
  Graph_value i;
  cfreq freq;

  deBruijn_Get_symbol_frequency(&(C__->dB_), pos__, &freq);

  if (freq.total_ == 0) {
    arithmetic_encode(0, 1, 1);
    return;
  }

  for (lower = 0, i = VALUE_A; i < gval__; i++)
    lower += freq.symbol_[i];
  upper = lower + freq.symbol_[i];

  arithmetic_encode(lower, upper, freq.total_);
}

Graph_value Decompressor_decode_(compressor* C__, int32_t idx__) {
  cfreq freq;
  Graph_value i;
  int32_t target, lower, upper;

  deBruijn_Get_symbol_frequency(&(C__->dB_), idx__, &freq);

  if (freq.total_ == 0) {
    target = arithmetic_decode_target(1);
    arithmetic_decode(0, 1, 1);
    return VALUE_ESC;
  }  
  target = arithmetic_decode_target(freq.total_);

  for (upper = 0, i = VALUE_A; i <= VALUE_ESC; i++) {
    lower = upper;
    upper += freq.symbol_[i];

    if (lower <= target && target < upper) {
      arithmetic_decode(lower, upper, freq.total_);
      return i;
    }
  }
  UNREACHABLE;
}

void Compressor_Increase_frequency_rec_(compressor *C__, int32_t idx__, Graph_value gval__, int32_t ctx_len__) {
  int32_t next;

  COMPRESSOR_VERBOSE(
    printf("[compressor] Increasing frequency at index %d (symbol %c)\n",
     idx__, GET_SYMBOL_FROM_VALUE(gval__));
  )
  idx__ = deBruijn_Find_Edge(&(C__->dB_), idx__, gval__);

  Graph_Increase_frequency(&(C__->dB_.Graph_), idx__);
#if defined(PPMD)
  Graph_Increase_frequency(&(C__->dB_.Graph_), idx__);
#endif

#if defined(TREE_CONTEXT_SHORTENING)
  next = deBruijn_Shorten_context(&(C__->dB_), idx__, ctx_len__, C__->label_, C__->lptr_);
#else
  next = deBruijn_Shorten_context(&(C__->dB_), idx__, ctx_len__);
#endif
  if (next == -1) return;

  Compressor_Increase_frequency_rec_(C__, next, gval__, ctx_len__ - 1);
}

int32_t finish_symbol_insertion_(compressor *C__, int32_t idx__, Graph_value gval__) {
  int32_t i, rank, temp, x;
  Graph_Line line;

  /* check what symbol is in W */
  GLine_Get(&(C__->dB_.Graph_), (uint32_t)idx__, &line);
  if (line.W_ == VALUE_$) {

    /* change symbol to new one and increase frequency to 1 */
    /* These is no need to upgrade the csl as we are not changing suffix */
    Graph_Change_symbol(&(C__->dB_.Graph_), idx__, gval__);
    Graph_Increase_frequency(&(C__->dB_.Graph_), idx__);

  } else {
    /* insert new edge into this node */
    /* csl must be updated but only after both nodes are inserted */
    GLine_Fill(&line, VALUE_0, gval__, 1);
    GLine_Insert(&(C__->dB_.Graph_), idx__, &line);

    /* update registered variables */
    Tracker_update(idx__);

    /* update the F array */
    for (i = 0; i < SYMBOL_COUNT; i++) {
      if (C__->dB_.F_[i] > idx__) C__->dB_.F_[i]++;
    }
  }

  /* find same transition symbol above this line */
  rank = Graph_Rank(&(C__->dB_.Graph_), idx__, VECTOR_W, gval__);

  if (!rank) {  /* there is no symbol above this */
    x = C__->dB_.F_[gval__];

    /* update the F array */
    for (i = gval__ + 1; i < SYMBOL_COUNT; i++) C__->dB_.F_[i]++;

  } else {
    /* go forward from this node */
    temp = Graph_Select(&(C__->dB_.Graph_), rank, VECTOR_W, gval__);
    x = deBruijn_Forward_(&(C__->dB_), temp - 1) + 1;

    /* update the F array */
    for (i = 0; i < SYMBOL_COUNT; i++) {
      if (C__->dB_.F_[i] >= x) C__->dB_.F_[i]++;
    }
  }

  /* insert new leaf (dollar) node into the graph */
  GLine_Fill(&line, VALUE_1, VALUE_$, 0);
  GLine_Insert(&(C__->dB_.Graph_), x, &line);

  /* update registered variables */
  Tracker_update(x);

  deBruijn_update_csl(&(C__->dB_), (x > idx__) ? idx__ : idx__ + 1);
  deBruijn_update_csl(&(C__->dB_), x);

  return x;
}

int32_t Compressor_Compress_symbol_aux_(compressor *C__, int32_t idx__, Graph_value gval__, int32_t ctx_len__) {
  int32_t prev_node, transition;

  /* check if transition exist from this node via given symbol */
  transition = deBruijn_Find_Edge(&(C__->dB_), idx__, gval__);

  if (transition == -1) {
    COMPRESSOR_VERBOSE(
      printf("[compressor] Escape character output\n");
    )

#if defined(TREE_CONTEXT_SHORTENING)
    prev_node = deBruijn_Shorten_context(&(C__->dB_), idx__, ctx_len__ - 1, C__->label_, C__->lptr_);
#else
    prev_node = deBruijn_Shorten_context(&(C__->dB_), idx__, ctx_len__ - 1);
#endif

    /* this cannot happen because there level one is full that means, that we
     * are never outputting character itself */
    assert(ctx_len__ != 0);
    Compressor_encode_(C__, idx__, VALUE_ESC);

    /* save current value of the index and go to the context shorter node */
    Tracker_push(&idx__);
    Compressor_Compress_symbol_aux_(C__, prev_node, gval__, ctx_len__ - 1);
    Tracker_pop();
  } else {
    /* we have a transition in this node */
    COMPRESSOR_VERBOSE(
      printf("[compressor] Output symbol %c\n", GET_SYMBOL_FROM_VALUE(gval__));
    )

    Compressor_encode_(C__, idx__, gval__);
    Compressor_Increase_frequency_rec_(C__, idx__, gval__, ctx_len__ - 1);

    return deBruijn_Forward_(&(C__->dB_), idx__);
  }

  return finish_symbol_insertion_(C__, idx__, gval__);
}

int32_t Decompressor_Decompress_symbol_aux_(compressor *C__, int32_t idx__, Graph_value* gval__, int32_t ctx_len__) {
  int32_t prev_node, transition;

  /* get decompressed symbol */
  Graph_value symbol = Decompressor_decode_(C__, idx__);

  if (symbol == VALUE_ESC) {
    COMPRESSOR_VERBOSE(
      printf("[compressor] Escape character output\n");
    )

#if defined(TREE_CONTEXT_SHORTENING)
    prev_node = deBruijn_Shorten_context(&(C__->dB_), idx__, ctx_len__ - 1, C__->label_, C__->lptr_);
#else
    prev_node = deBruijn_Shorten_context(&(C__->dB_), idx__, ctx_len__ - 1);
#endif

    /* this cannot happen because there level one is full that means, that we
     * are never outputting character itself */
    assert(ctx_len__ != 0);

    Tracker_push(&idx__);
    Decompressor_Decompress_symbol_aux_(C__, prev_node, &symbol, ctx_len__ - 1);
    Tracker_pop();
    *gval__ = symbol;
  } else {
    COMPRESSOR_VERBOSE(
      printf("[compressor] Output symbol %c\n", GET_SYMBOL_FROM_VALUE(symbol));
    )

    transition = deBruijn_Find_Edge(&(C__->dB_), idx__, symbol);
    if (transition == -1) {
      printf("Fatal - there is no transition\n");
      exit(1);
    }

    *gval__ = symbol;
    Compressor_Increase_frequency_rec_((compressor*)C__, idx__, symbol, ctx_len__ - 1);
    return deBruijn_Forward_(&(C__->dB_), idx__);
  }

  return finish_symbol_insertion_(C__, idx__, symbol);
}

void Compressor_Compress_symbol(compressor *C__, Graph_value gval__) {
  int32_t res, shorter;

  if (C__->depth_ >= CONTEXT_LENGTH) {
#if defined(TREE_CONTEXT_SHORTENING)
    shorter = deBruijn_Shorten_context(&(C__->dB_), C__->state_, C__->depth_ - 1, C__->label_, C__->lptr_);
#else
    shorter = deBruijn_Shorten_context(&(C__->dB_), C__->state_, C__->depth_ - 1);
#endif
    res = Compressor_Compress_symbol_aux_(C__, shorter, gval__, C__->depth_ - 1);
  } else {
    res = Compressor_Compress_symbol_aux_(C__, C__->state_, gval__, C__->depth_);
  }
  C__->state_ = (res == -1) ? C__->state_ : res;

#if defined(TREE_CONTEXT_SHORTENING)
  C__->label_[C__->lptr_++] = gval__;
  if (C__->lptr_ >= CONTEXT_LENGTH) C__->lptr_ -= CONTEXT_LENGTH;
#endif
  /* increase context depth if shorter than CONTEXT_LENGTH */
  if (C__->depth_ < CONTEXT_LENGTH) C__->depth_++;
}

void Decompressor_Decompress_symbol(compressor *C__, Graph_value *gval__) {
  int32_t res, shorter;

  if (C__->depth_ >= CONTEXT_LENGTH) {
#if defined(TREE_CONTEXT_SHORTENING)
    shorter = deBruijn_Shorten_context(&(C__->dB_), C__->state_, C__->depth_ - 1, C__->label_, C__->lptr_);
#else
    shorter = deBruijn_Shorten_context(&(C__->dB_), C__->state_, C__->depth_ - 1);
#endif
    res = Decompressor_Decompress_symbol_aux_(C__, shorter, gval__, C__->depth_ - 1);
  } else {
    res = Decompressor_Decompress_symbol_aux_(C__, C__->state_, gval__, C__->depth_);
  }
  C__->state_ = (res == -1) ? C__->state_ : res;

#if defined(TREE_CONTEXT_SHORTENING)
  C__->label_[C__->lptr_++] = *gval__;
  if (C__->lptr_ >= CONTEXT_LENGTH) C__->lptr_ -= CONTEXT_LENGTH;
#endif
  /* increase context depth if shorter than CONTEXT_LENGTH */
  if (C__->depth_ < CONTEXT_LENGTH) C__->depth_++;
}
