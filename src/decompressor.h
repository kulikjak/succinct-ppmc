#ifndef _DECOMPRESSOR__
#define _DECOMPRESSOR__

#include "deBruijn.h"
#include "arcd/arcd.h"

typedef struct {
  deBruijn_graph dB_;
  int32_t depth_;
  int32_t state_;

  struct {
    int32_t cnt_;
    int32_t *arr_[CONTEXT_LENGTH + 4];
  } tracker_;

  arcd_dec decoder_;
} decompressor;




void Decompressor_Init(decompressor* D__);

void Decompressor_Finalize(decompressor * D__);

void Decompressor_Free(decompressor* D__);

/*
 * Append new symbol to given context.
 *
 * Symbol automatically appended to all shorter contexts. Those which have this
 * symbol have their frequency increased. This function does basically
 * everything needed for PPM compression algorithm.
 *
 * @param  C__  Reference to compressor object.
 * @param  idx__  Node index (line) in deBruijn graph.
 * @param  gval__ Additional symbol (Graph_value).
 */
int32_t Decompressor_Compress_symbol(compressor *C__, int32_t idx__, Graph_value gval__);



