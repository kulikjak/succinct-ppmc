#include "compressor.h"

void compressor_debug(char* info__) {
  if (COMPRESSOR_VERBOSE_)
    fprintf(stderr, "%s\n", info__);
}


void Compressor_Init(compressor* C__, FILE* ofp__) {
  deBruijn_Init(&(C__->dB_));
  Tracker_reset();

  C__->depth_ = 0;
  C__->state_ = 0;

  startoutputtingbits(ofp__);
  start_encode();
}
void Compressor_Finalize(compressor * C__) {
  finish_encode();
  doneoutputtingbits();

  deBruijn_Free(&(C__->dB_));
}

void Decompressor_Init(decompressor* D__, FILE* ifp__) {
  deBruijn_Init(&(D__->dB_));
  Tracker_reset();

  D__->depth_ = 0;
  D__->state_ = 0;

  // initialize arythmetic decoder
  startinputtingbits(ifp__);
  start_decode();
}
void Decompressor_Finalize(decompressor* D__) {
  finish_decode();
  doneinputtingbits();

  deBruijn_Free(&(D__->dB_));
}

void Compressor_encode_(compressor* C__, int32_t pos__, Graph_value gval__) {  
  int32_t lower, upper;
  Graph_value i;
  cfreq freq;

  deBruijn_Get_symbol_frequency(&(C__->dB_), pos__, &freq);

  // in case there is no other transition, probability of escape is 100%
  if (freq.total_ == 0) {
    arithmetic_encode(0, 1, 1);
    return;
  }

  // handle probability if there are some transitions
  for (lower = 0, i = VALUE_A; i < gval__; i++)
    lower += freq.symbol_[i];
  upper = lower + freq.symbol_[i];

  arithmetic_encode(lower, upper, freq.total_);
}

Graph_value Decompressor_decode_(decompressor* D__, int32_t idx__) {
  cfreq freq;
  Graph_value i;
  int32_t target, lower, upper;

  deBruijn_Get_symbol_frequency(&(D__->dB_), idx__, &freq);

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

int32_t Compressor_Compress_symbol_aux_(compressor *C__, int32_t idx__, Graph_value gval__, int32_t ctx_len__);
int32_t Decompressor_Decompress_symbol_aux_(decompressor *D__, int32_t idx__, Graph_value* gval__, int32_t ctx_len__);

void Compressor_Increase_frequency_rec_(compressor *C__, int32_t idx__, Graph_value gval__, int32_t ctx_len__) {
  idx__ = deBruijn_Find_Edge(&(C__->dB_), idx__, gval__);

  Graph_Increase_frequency(&(C__->dB_.Graph_), idx__);

  int32_t next = deBruijn_Shorten_context(&(C__->dB_), idx__, ctx_len__);
  if (next == -1) return;

  Compressor_Increase_frequency_rec_(C__, next, gval__, ctx_len__ - 1);
}

void Compressor_Compress_symbol(compressor *C__, Graph_value gval__) {
  int32_t res, shorter;

  if (C__->depth_ >= CONTEXT_LENGTH) {
    shorter = deBruijn_Shorten_context(&(C__->dB_), C__->state_, C__->depth_ - 1);
    res = Compressor_Compress_symbol_aux_(C__, shorter, gval__, C__->depth_ - 1);
  } else {
    res = Compressor_Compress_symbol_aux_(C__, C__->state_, gval__, C__->depth_);
  }

  C__->state_ = (res == -1) ? C__->state_ : res;

  // increase context depth if shorter than CONTEXT_LENGTH
  if (C__->depth_ < CONTEXT_LENGTH)
    C__->depth_++;
}

void Decompressor_Decompress_symbol(decompressor *D__, Graph_value* gval__) { //FIXME
  int32_t res, shorter;

  if (D__->depth_ >= CONTEXT_LENGTH) {
    shorter = deBruijn_Shorten_context(&(D__->dB_), D__->state_, D__->depth_ - 1);
    res = Decompressor_Decompress_symbol_aux_(D__, shorter, gval__, D__->depth_ - 1);
  } else {
    res = Decompressor_Decompress_symbol_aux_(D__, D__->state_, gval__, D__->depth_);
  }
  
  D__->state_ = (res == -1) ? D__->state_ : res;

  // increase context depth if shorter than CONTEXT_LENGTH
  if (D__->depth_ < CONTEXT_LENGTH)
    D__->depth_++;
}


int32_t Compressor_Compress_symbol_aux_(compressor *C__, int32_t idx__, Graph_value gval__, int32_t ctx_len__) {
  int32_t i, rank, temp;
  int32_t prev_node, x;
  int32_t transition;
  Graph_Line line;

  // check if transition exist from this node via given symbol
  transition = deBruijn_Find_Edge(&(C__->dB_), idx__, gval__);

  // transition does not exist
  if (transition == -1) {
    prev_node = deBruijn_Shorten_context(&(C__->dB_), idx__, ctx_len__ - 1);

    /* this cannot happen because there level one is full that means, that we
     * are never outputting character itself */
    assert(ctx_len__ != 0);

    compressor_debug("Escape character output.");
    Compressor_encode_(C__, idx__, VALUE_ESC);

    /* save current value of the index and go to the context shorter node */
    Tracker_push(&idx__);
    Compressor_Compress_symbol_aux_(C__, prev_node, gval__, ctx_len__ - 1);
    Tracker_pop();
  } else {
    // we have transition in this node
    compressor_debug("Symbol output.");

    // TODO use state for operations and not idx__
    Compressor_encode_(C__, idx__, gval__);
    Compressor_Increase_frequency_rec_(C__, idx__, gval__, ctx_len__ - 1);

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
    // insert new edge into this node
    GLine_Fill(&line, VALUE_0, gval__, 1);
    GLine_Insert(&(C__->dB_.Graph_), idx__, &line);

    // update registered variables
    Tracker_update(idx__);

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
  Tracker_update(x);

  return x;
}

int32_t Decompressor_Decompress_symbol_aux_(decompressor *D__, int32_t idx__, Graph_value* gval__, int32_t ctx_len__) {
  int32_t i, rank, temp;
  int32_t prev_node, x;
  int32_t transition;
  Graph_Line line;

  // get decompressed symbol
  Graph_value symbol = Decompressor_decode_(D__, idx__);

  if (symbol == VALUE_ESC) {
    compressor_debug("Escape character output.");
    prev_node = deBruijn_Shorten_context(&(D__->dB_), idx__, ctx_len__ - 1);

    /* this cannot happen because there level one is full that means, that we
     * are never outputting character itself */
    assert(ctx_len__ != 0);

    Tracker_push(&idx__);
    Decompressor_Decompress_symbol_aux_(D__, prev_node, &symbol, ctx_len__ - 1);
    Tracker_pop();
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
    Compressor_Increase_frequency_rec_((compressor*)D__, idx__, symbol, ctx_len__ - 1);
    return deBruijn_Forward_(&(D__->dB_), idx__);
  }

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
    Tracker_update(idx__);

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
  Tracker_update(x);

  return x;
}
