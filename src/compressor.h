#ifndef _COMPRESSOR__
#define _COMPRESSOR__

#include "deBruijn.h"
#include "arcd/arcd.h"

#define COMPRESSOR_VERBOSE false

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


void variable_Tracker_push(compressor* C__, int32_t *a__);

void variable_Tracker_pop(compressor* C__);

void variable_Tracker_update(compressor* C__, int32_t val__);

/*
 * Initialize compressor.
 *
 * @param  C__  Reference to compressor.
 * @param  ofp__  File pointer of the output file.
 */
void Compressor_Init(compressor* C__, FILE* ofp__);

/*
 * Finalize compression.
 *
 * This function must be called after encoding all characters to output rest of
 * the buffer. It also handles header of compressed file.
 *
 * @param  C__  Reference to compressor.
 */
void Compressor_Finalize(compressor * C__);

/*
 * Free memory associated with the given compressor.
 *
 * This function doesn't close output file pointer.
 *
 * @param  C__  Reference to compressor.
 */
void Compressor_Free(compressor* C__);

/*
 * Initialize decompressor.
 *
 * @param  D__  Reference to decompressor.
 * @param  ifp__  File pointer of the input file.
 */
void Decompressor_Init(decompressor* D__, FILE* ifp__);

/*
 * Finalize decompression. (TODO)
 *
 *
 * @param  D__  Reference to decompressor.
 */
void Decompressor_Finalize(decompressor* D__);

/*
 * Free memory associated with the given decompressor.
 *
 * This function doesn't close input file pointer.
 *
 * @param  D__  Reference to decompressor.
 */
void Decompressor_Free(decompressor* D__);

/*
 * Compress symbol.
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

/*
 * Decompress symbol.
 *
 * @param  C__  Reference to compressor object.
 * @param  idx__  Node index (line) in deBruijn graph.
 * @param  gval__ [out] Additional symbol (Graph_value).
 */
int32_t Decompressor_Decompress_symbol(decompressor *D__, int32_t idx__, Graph_value* gval__);


// make hidden function visible to unit testing framework
#ifdef _UNITY

/*
 * Recursively increase P array for given index and its suffix predecessors.
 *
 * @param  C__  Reference to compressor/decompressor object.
 * @param  idx__  Node index (line) in deBruijn graph.
 * @param  gval__ Additional symbol (Graph_value).
 */
void Compressor_Increase_frequency_rec_(compressor *C__, int32_t idx__, Graph_value gval__);

#endif  // _UNITY

#endif  // _COMPRESSOR__
