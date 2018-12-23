#include "compressor.h"

void Process_Init(CompressorRef C__) {
  deBruijn_Init(&(C__->dB_));
  C__->state_ = 4;

#if defined(ENABLE_CACHE_STATS)
  cache_stats_prep();
#endif
}

void Process_Free(CompressorRef C__) {
  deBruijn_Free(&(C__->dB_));

#if defined(ENABLE_CACHE_STATS)
  cache_stats_print();
#endif
}

void Compressor_encode_(cfreq* freq__, Graph_value gval__) {
  int32_t lower, upper;
  Graph_value i;

  if (freq__->total_ == 0) {
    arithmetic_encode(0, 1, 1);
    return;
  }

  for (lower = 0, i = VALUE_A; i < gval__ >> 0x1; i ++)
    lower += freq__->symbol_[i];
  upper = lower + freq__->symbol_[i];

  arithmetic_encode(lower, upper, freq__->total_);
}

Graph_value Decompressor_decode_(cfreq* freq__) {
  int32_t target, lower, upper;
  Graph_value i;

  if (freq__->total_ == 0) {
    target = arithmetic_decode_target(1);
    arithmetic_decode(0, 1, 1);
    return VALUE_ESC;
  }
  target = arithmetic_decode_target(freq__->total_);

  for (upper = 0, i = VALUE_A; i <= (VALUE_ESC >> 0x1); i++) {
    lower = upper;
    upper += freq__->symbol_[i];

    if (lower <= target && target < upper) {
      arithmetic_decode(lower, upper, freq__->total_);
      return (i << 0x1);
    }
  }
  UNREACHABLE;
}

int32_t finish_symbol_insertion_(CompressorRef C__, int32_t idx__, Graph_value gval__) {
  int32_t i, rank, temp, len, x;
  Graph_Line line;

  bool exists_above = false;
  bool exists_bellow = false;
  Graph_value gval = gval__;

  /* check if target node already exists above this line */
  rank = Graph_Rank(&(C__->dB_.Graph_), idx__, VECTOR_W, gval__);
  if (rank) {
    temp = Graph_Select(&(C__->dB_.Graph_), rank, VECTOR_W, gval__) - 1;
    len = deBruijn_Get_common_suffix_len_(&(C__->dB_), idx__, temp);
    if (len >= CONTEXT_LENGTH - 1) {
      exists_above = true;
      gval += 1;
    }
  }

  if (!exists_above) {
    /* check if target node already exists below this line */
    rank = Graph_Rank(&(C__->dB_.Graph_), idx__ + 1, VECTOR_W, gval__);
    temp = Graph_Select(&(C__->dB_.Graph_), rank + 1, VECTOR_W, gval__) - 1;

    if (temp > 0) {
      len = deBruijn_Get_common_suffix_len_(&(C__->dB_), idx__, temp);
      if (len >= CONTEXT_LENGTH - 1) {
        exists_bellow = true;
      }
    }
  }

  /* check what symbol is in W */
  GLine_Get(&(C__->dB_.Graph_), (uint32_t) idx__, &line);
  if (line.W_ == VALUE_$) {
    /* change symbol to new one and increase frequency to 1 */
    /* These is no need to upgrade the csl as we are not changing suffix */

    Graph_Change_symbol(&(C__->dB_.Graph_), idx__, gval);
    Graph_Increase_frequency(&(C__->dB_.Graph_), idx__, 1);

  } else {
    /* insert new edge into this node */
    /* csl must be updated but only after both nodes are inserted */
    GLine_Fill(&line, VALUE_0, gval, 1);
    GLine_Insert(&(C__->dB_.Graph_), idx__, &line);

    /* update temp variable if newly inserted node moved it */
    temp += (temp >= idx__) ? 1 : 0;

    /* update the F array */
    for (i = 0; i < SYMBOL_COUNT; i++) {
      if (C__->dB_.F_[i] > idx__)
        C__->dB_.F_[i]++;
    }
  }

  if (exists_above) {
    return deBruijn_Forward_(&(C__->dB_), idx__);
  }

  if (exists_bellow) {
    Graph_Change_symbol(&(C__->dB_.Graph_), temp, gval + 1);
    return deBruijn_Forward_(&(C__->dB_), idx__);
  }

  /* find same transition symbol above this line */
  rank = Graph_Rank(&(C__->dB_.Graph_), idx__, VECTOR_W, gval__);

  if (!rank) { /* there is no symbol above this */
    x = C__->dB_.F_[gval__ >> 0x1];

    /* update the F array */
    for (i = (gval__ >> 0x1) + 1; i < SYMBOL_COUNT; i++)
      C__->dB_.F_[i]++;

  } else {
    /* go forward from this node */
    temp = Graph_Select(&(C__->dB_.Graph_), rank, VECTOR_W, gval__) - 1;
    x = deBruijn_Forward_(&(C__->dB_), temp) + 1;

    /* update the F array */
    for (i = 0; i < SYMBOL_COUNT; i++) {
      if (C__->dB_.F_[i] >= x)
        C__->dB_.F_[i]++;
    }
  }

  /* insert new leaf (dollar) node into the graph */
  GLine_Fill(&line, VALUE_1, VALUE_$, 0);
  GLine_Insert(&(C__->dB_.Graph_), x, &line);

  deBruijn_update_csl(&(C__->dB_), (x > idx__) ? idx__ : idx__ + 1);
  deBruijn_update_csl(&(C__->dB_), x);

  return x;
}

void Compressor_Compress_symbol_aux_(CompressorRef C__, Graph_value gval__, int32_t lo__, int32_t up__, int32_t ctx_len__) {
  int32_t rank1, rank2, i, temp, count;
  cfreq freq;

  /* check if given transition exists in this range */
  rank1 = Graph_Rank(&(C__->dB_.Graph_), lo__, VECTOR_W, ((gval__ >> 0x1) | 0x10));
  rank2 = Graph_Rank(&(C__->dB_.Graph_), up__ + 1, VECTOR_W, ((gval__ >> 0x1) | 0x10));
  count = (rank2 - rank1);
  if (count) {

    deBruijn_Get_symbol_frequency_range(&(C__->dB_), lo__, up__, &freq);
    Compressor_encode_(&freq, gval__);

    for (i = rank1 + 1; i <= rank2; i++) {
      temp = Graph_Select(&(C__->dB_.Graph_), i, VECTOR_W, ((gval__ >> 0x1) | 0x10)) - 1;
#if defined(FREQ_INCREASE_ALL)
      Graph_Increase_frequency(&(C__->dB_.Graph_), temp, 1);
#elif defined(FREQ_INCREASE_FIRST)
      Graph_Increase_frequency(&(C__->dB_.Graph_), temp, 1);
      break;
#elif defined(FREQ_INCREASE_NONE)
      UNUSED(temp);
      break;
#endif
    }

  } else {

    deBruijn_Get_symbol_frequency_range(&(C__->dB_), lo__, up__, &freq);
    Compressor_encode_(&freq, VALUE_ESC);

    /* find range of shorter context */
    int lo = deBruijn_shorten_lower(&(C__->dB_), C__->state_, ctx_len__ - 1);
    int up = deBruijn_shorten_upper(&(C__->dB_), C__->state_, ctx_len__ - 1);

    Compressor_Compress_symbol_aux_(C__, gval__, lo, up, ctx_len__ - 1);
  }
}

void Decompressor_Decompress_symbol_aux_(CompressorRef C__, Graph_value* gval__, int32_t lo__, int32_t up__, int32_t ctx_len__) {
  int32_t rank1, rank2, i, temp, count;
  cfreq freq;

  /* get decompressed symbol */
  deBruijn_Get_symbol_frequency_range(&(C__->dB_), lo__, up__, &freq);
  Graph_value symbol = Decompressor_decode_(&freq);

  if (symbol == VALUE_ESC) {
    COMPRESSOR_VERBOSE(
      printf("[compressor] Escape character output\n");
    )

    /* find range of shorter context */
    int lo = deBruijn_shorten_lower(&(C__->dB_), C__->state_, ctx_len__ - 1);
    int up = deBruijn_shorten_upper(&(C__->dB_), C__->state_, ctx_len__ - 1);

    Decompressor_Decompress_symbol_aux_(C__, gval__, lo, up, ctx_len__ - 1);

  } else {
    COMPRESSOR_VERBOSE(
      printf("[compressor] Output symbol %c\n", GET_SYMBOL_FROM_VALUE(symbol));
    )

    /* check if given transition exists in this range */
    rank1 = Graph_Rank(&(C__->dB_.Graph_), lo__, VECTOR_W, ((symbol >> 0x1) | 0x10));
    rank2 = Graph_Rank(&(C__->dB_.Graph_), up__ + 1, VECTOR_W, ((symbol >> 0x1) | 0x10));

    count = (rank2 - rank1);
    if (count) {

      for (i = rank1 + 1; i <= rank2; i++) {
        temp = Graph_Select(&(C__->dB_.Graph_), i, VECTOR_W, ((symbol >> 0x1) | 0x10)) - 1;
#if defined(FREQ_INCREASE_ALL)
        Graph_Increase_frequency(&(C__->dB_.Graph_), temp, 1);
#elif defined(FREQ_INCREASE_FIRST)
        Graph_Increase_frequency(&(C__->dB_.Graph_), temp, 1);
        break;
#elif defined(FREQ_INCREASE_NONE)
        UNUSED(temp);
        break;
#endif
      }
    } else {
      FATAL("There is no transition");
    }

    *gval__ = symbol;
  }
}

void Compressor_Compress_symbol(CompressorRef C__, Graph_value gval__) {
  int32_t transition;
  cfreq freq;

  transition = deBruijn_Find_Edge(&(C__->dB_), C__->state_, gval__ & 0xE);

  if (transition == -1) {
    COMPRESSOR_VERBOSE(
      printf("[compressor] Escape character output\n");
    )
    /* output escape character */
    deBruijn_Get_symbol_frequency(&(C__->dB_), C__->state_, &freq);
    Compressor_encode_(&freq, VALUE_ESC);

    /* find range of shorter context */
    int lo = deBruijn_shorten_lower(&(C__->dB_), C__->state_, CONTEXT_LENGTH - 1);
    int up = deBruijn_shorten_upper(&(C__->dB_), C__->state_, CONTEXT_LENGTH - 1);

    Compressor_Compress_symbol_aux_(C__, gval__, lo, up, CONTEXT_LENGTH - 1);

    /* insert new node into the graph */
    C__->state_ = finish_symbol_insertion_(C__, C__->state_, gval__);

  } else {
    /* we have a transition in this node */
    COMPRESSOR_VERBOSE(
      printf("[compressor] Output symbol %c\n", GET_SYMBOL_FROM_VALUE(gval__));
    )

    /* output given character */
    deBruijn_Get_symbol_frequency(&(C__->dB_), C__->state_, &freq);
    Compressor_encode_(&freq, gval__);

    Graph_Increase_frequency(&(C__->dB_.Graph_), transition, 1);
    C__->state_ = deBruijn_Forward_(&(C__->dB_), transition);
  }
}

void Decompressor_Decompress_symbol(CompressorRef C__, Graph_value* gval__) {
  int32_t transition;
  cfreq freq;

  /* get decompressed symbol */
  deBruijn_Get_symbol_frequency(&(C__->dB_), C__->state_, &freq);
  Graph_value symbol = Decompressor_decode_(&freq);

  if (symbol == VALUE_ESC) {
    COMPRESSOR_VERBOSE(
      printf("[compressor] Escape character output\n");
    )

    /* find range of shorter context */
    int lo = deBruijn_shorten_lower(&(C__->dB_), C__->state_, CONTEXT_LENGTH - 1);
    int up = deBruijn_shorten_upper(&(C__->dB_), C__->state_, CONTEXT_LENGTH - 1);

    Decompressor_Decompress_symbol_aux_(C__, gval__, lo, up, CONTEXT_LENGTH - 1);

    /* insert new node into the graph */
    C__->state_ = finish_symbol_insertion_(C__, C__->state_, *gval__);

  } else {
    COMPRESSOR_VERBOSE(
      printf("[compressor] Output symbol %c\n", GET_SYMBOL_FROM_VALUE(symbol));
    )

    transition = deBruijn_Find_Edge(&(C__->dB_), C__->state_, symbol);
    if (transition == -1) {
      FATAL("There is no transition");
    }

    *gval__ = symbol;
    Graph_Increase_frequency(&(C__->dB_.Graph_), transition, 1);
    C__->state_ = deBruijn_Forward_(&(C__->dB_), transition);
  }
}
