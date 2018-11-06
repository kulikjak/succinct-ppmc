#ifndef _COMPRESSOR__
#define _COMPRESSOR__

#include "arith/bitio.h"
#include "arith/arith.h"

#include "defines.h"
#include "deBruijn.h"
#include "tracker.h"
#include "defines.h"

#define COMPRESSOR_VERBOSE(func) \
  if (COMPRESSOR_VERBOSE_) {     \
    func                     \
  }

typedef struct {
  deBruijn_graph dB_;
  int32_t depth_;

  int32_t state_;
#ifdef CLEVER_CONTEXT_SHORTENING
  char label_[CONTEXT_LENGTH]; /* label of the current state_ */
#endif
} compressor;

#define Compression_Start(ofp__) { \
  startoutputtingbits(ofp__);      \
  start_encode();                  \
}
#define Compression_Finalize() { \
  finish_encode();               \
  doneoutputtingbits();          \
}

#define Decompression_Start(ifp__) { \
  startinputtingbits(ifp__);         \
  start_decode();                    \
}
#define Decompression_Finalize() { \
  finish_decode();                 \
  doneinputtingbits();             \
}

/*
 * Initialize compressor object for eighter compression or decompression.
 *
 * @param  C__  Reference to compressor object.
 */
void Process_Init(compressor* C__);

/*
 * Free memory associated with the compressor.
 *
 * @param  C__  Reference to compressor object.
 */
void Process_Free(compressor * C__);


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
 * @param  D__  Reference to compressor object.
 * @param  gval__ [out] Additional symbol (Graph_value).
 */
void Decompressor_Decompress_symbol(compressor *C__, Graph_value* gval__);


// make hidden function visible to unit testing framework
#ifdef _UNITY

/*
 * Recursively increase P array for given index and its suffix predecessors.
 *
 * @param  C__  Reference to compressor/decompressor object.
 * @param  idx__  Node index (line) in deBruijn graph.
 * @param  gval__ Additional symbol (Graph_value).
 * @param  ctx_len__ Length of context in given node.
 */
void Compressor_Increase_frequency_rec_(compressor *C__, int32_t idx__, Graph_value gval__, int32_t ctx_len__);

#endif  // _UNITY

#endif  // _COMPRESSOR__
