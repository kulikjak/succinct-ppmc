#ifndef _COMPRESSOR__
#define _COMPRESSOR__

#include "tracker.h"
#include "deBruijn.h"

#include "arith/bitio.h"
#include "arith/arith.h"

#define COMPRESSOR_VERBOSE false

typedef struct {
  deBruijn_graph dB_;
  int32_t depth_;
  int32_t state_;
} compressor;

typedef struct {
  deBruijn_graph dB_;
  int32_t depth_;
  int32_t state_;
} decompressor;

/*
 * Initialize compressor.
 *
 * @param  C__  Reference to compressor.
 * @param  ofp__  File pointer of the output file.
 */
void Compressor_Init(compressor* C__, FILE* ofp__);

/*
 * Finalize compression and free memory associated with the compressor.
 *
 * This function must be called after encoding all characters to output rest of
 * the buffer. It also handles header of compressed file.
 *
 * This function doesn't close output file pointer.
 *
 * @param  C__  Reference to compressor.
 */
void Compressor_Finalize(compressor * C__);


/*
 * Initialize decompressor.
 *
 * @param  D__  Reference to decompressor.
 * @param  ifp__  File pointer of the input file.
 */
void Decompressor_Init(decompressor* D__, FILE* ifp__);

/*
 * Finalize decompression and free memory associated with the decompressor.
 *
 * This function must be called after decoding all characters to output rest of
 * the buffer.
 *
 * This function doesn't close input file pointer.
 *
 * @param  D__  Reference to decompressor.
 */
void Decompressor_Finalize(decompressor* D__);

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
void Compressor_Compress_symbol(compressor *C__, Graph_value gval__);

/*
 * Decompress symbol.
 *
 * @param  C__  Reference to compressor object.
 * @param  gval__ [out] Additional symbol (Graph_value).
 */
void Decompressor_Decompress_symbol(decompressor *D__, Graph_value* gval__);


// make hidden function visible to unit testing framework
#ifdef _UNITY

/*
 * Recursively increase P array for given index and its suffix predecessors.
 *
 * @param  C__  Reference to compressor/decompressor object.
 * @param  idx__  Node index (line) in deBruijn graph.
 * @param  gval__ Additional symbol (Graph_value).
 */
void Compressor_Increase_frequency_rec_(compressor *C__, int32_t idx__, Graph_value gval__, int32_t ctx_len__);

#endif  // _UNITY

#endif  // _COMPRESSOR__
