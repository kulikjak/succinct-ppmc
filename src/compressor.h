#ifndef _COMPRESSOR__
#define _COMPRESSOR__

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

  arcd_enc encoder_;
} compressor;


void variable_Tracker_push(compressor* C__, int32_t *a__);

void variable_Tracker_pop(compressor* C__);

void variable_Tracker_update(compressor* C__, int32_t val__);

/*
 * Initialize compressor object.
 *
 * @param  C__  Reference to compressor object.
 */
void Compressor_Init(compressor* C__);


void Compressor_Finalize(compressor * C__);

/*
 * Free all memory associated with compressor object.
 *
 * @param  C__  Reference to compressor object.
 */
void Compressor_Free(compressor* C__);

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
int32_t Compressor_Compress_symbol(compressor *C__, int32_t idx__, Graph_value gval__);

// make hidden function visible to unit testing framework
#ifdef _UNITY

/*
 * Recursively increase P array for given index and its suffix predecessors.
 *
 * @param  C__  Reference to compressor object.
 * @param  idx__  Node index (line) in deBruijn graph.
 * @param  gval__ Additional symbol (Graph_value).
 */
void Compressor_Increase_frequency_rec_(compressor *C__, int32_t idx__, Graph_value gval__);

#endif  // _UNITY

#endif  // _COMPRESSOR__
