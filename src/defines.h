#ifndef _DEFINES__
#define _DEFINES__

#include "utils.h"

/* Make whole program verbose. */
#ifndef VERBOSE_
  #define VERBOSE_ false
#endif

/* Make parts of the program verbose
 * If VERBOSE_ is set to true, these values are ignored.
 */ 
#ifndef MAIN_VERBOSE_
  #define MAIN_VERBOSE_ false
#endif
#ifndef MEMORY_VERBOSE_
  #define MEMORY_VERBOSE_ false
#endif
#ifndef STRUCTURE_VERBOSE_
  #define STRUCTURE_VERBOSE_ false
#endif
#ifndef DEBRUIJN_VERBOSE_
  #define DEBRUIJN_VERBOSE_ false
#endif
#ifndef COMPRESSOR_VERBOSE_
  #define COMPRESSOR_VERBOSE_ false
#endif

/* number of preallocated block pointers */
#ifndef INITIAL_BLOCK_COUNT_
  #define INITIAL_BLOCK_COUNT_ 2
#endif
/* Number of elements per memory block - must be power of two! */
#ifndef MEMORY_BLOCK_SIZE_
  #define MEMORY_BLOCK_SIZE_ 32
#endif
/* log2 of MEMORY_BLOCK_SIZE_ */
#ifndef MEMORY_BLOCK_SIZE_LOG_
  #define MEMORY_BLOCK_SIZE_LOG_ 5
#endif

/* Length of the PPMC context */
#ifndef CONTEXT_LENGTH
  #define CONTEXT_LENGTH 4
#endif

/*
 * Size of stack used for tree traversal. Stack must be atleast as big as
 * maximum expected tree depth. If balanding is not enabled, this number should
 * be much bigger.
 */
#ifndef MAX_STACK_SIZE
  #define MAX_STACK_SIZE 64
#endif


#ifndef REMOTE_OPTIMIZATION_CONTROL
/**************** OPTIMIZATION DEFINES **********************
 *
 * Make underlying structure tree balanced with red black tree algorithm.
 * Without this tree can (in the worst case) become a linear array with greatly
 * reduced performance */
#define ENABLE_RED_BLACK_BALANCING

/* Use cache for leaf lookup when performing rank, select and other tree
 * related operations. If cache size is 0, no cache is used. */
#define CACHE_SIZE 0
/* Calculate number of cache hits and misses */
//#define ENABLE_CACHE_STATS

/* These define the way of how context shortening is handled.
 * LABEL_CONTEXT_SHORTENING calculates context each time shortening occurs.
 * INTEGER_CONTEXT_SHORTENING saves context sizes in integer variables.
 * RAS_CONTEXT_SHORTENING uses ranks and selects to move above wavelet tree
 *   storing context lengths. It has an incredible memory overhead and is not
 *   as fast as other methods.
 *
 * Exacly one of those must be specified */
//#define LABEL_CONTEXT_SHORTENING
#define INTEGER_CONTEXT_SHORTENING
//#define RAS_CONTEXT_SHORTENING

/* These define the way how memory is allocated and managed.
 * DIRECT_MEMORY memory is directly referenced.
 * INDEXED_MEMORY memory is indexed via array indexes.
 *
 * Both models use two memory blocks for leaves and nodes.
 *
 * Direct memory access has faster access times and lesser overhead. However it
 * uses pointers which are double the size of 32bit indexes (on intel 64-bit
 * system). This means that each tree node is bigger by eight bytes.
 *
 * SIMPLE_MEMORY memory is directly allocated from system for each block.
 * Beware that this model is not deallocating memory when program finishes (as
 * it is here for testing purposes only and it's not worth to implement it)
 *
 * Exacly one of those must be specified */
//#define INDEXED_MEMORY
#define DIRECT_MEMORY
//#define SIMPLE_MEMORY

/* These define the way how frequency increases are handeled.
 * FREQ_INCREASE_NONE no frequency increase in shortened contexts.
 * FREQ_INCREASE_FIRST only first outgoing edge has its frequecny increased.
 * FREQ_INCREASE_ALL each otgoing edge has its frequency increased.
 *
 * This doesn't change the way how increase is handeled in non-shortened
 * contexts or how escape character is handeled.
 *
 * Exacly one of those must be specified */
//#define FREQ_INCREASE_NONE
//#define FREQ_INCREASE_FIRST
#define FREQ_INCREASE_ALL

/* These define the way how frequency of escape characters is calculated.
 * FREQ_COUNT_EACH escape character has frequecny equal to amount of outgoing
 *   edges.
 * FREQ_COUNT_ONCE escape character has frequecny equal to amount of distinct
 *   outgoing edges.
 *
 * Exacly one of those must be specified */
#define FREQ_COUNT_EACH
//#define FREQ_COUNT_ONCE

/* Embed flags for red black tree and the leaf indicator directly into the
 * other variables to save space occupied by them. */
#define EMBEDED_FLAGS

/* Following defines enable single traversal ranks and selects. There is no
 * downside to those and thus they should be enabled for optimal performance.
 */
#define FAST_RANK
#define FAST_SELECT

#endif

/**************** ARITHMETIC CODING DEFINES **********************
 *
 * from bitio.h:
 * Normally count input and output bytes so program can report stats.
 * With FAST_BITIO set, no counting is maintained, which means file sizes
 * reported with the '-v' option will be meaningless, but should improve
 * speed slightly.
 *
 * This is not useful for anything in this algorithm and therefore FAST_BITIO
 * can be enabled without any functionality loss.
 */
#define FAST_BITIO

/*
 * from arith.c:
 * The following arithmetic coding routines contain two different
 * methods for doing the required arithmetic. The first method uses
 * multiplication and division, the second simulates it with shifts and adds.
 *
 * Shifting method has much bigger output files (bigger than input files) and
 * should not be used -> this define should be enabled.
 */
#define MULT_DIV

/******************* END OF ADJUSTABLE DEFINES *******************/

#ifndef VERBOSE_
  #define VERBOSE_ false
#endif

#if VERBOSE_
  #undef MAIN_VERBOSE_
  #define MAIN_VERBOSE_ true
  #undef MEMORY_VERBOSE_
  #define MEMORY_VERBOSE_ true
  #undef STRUCTURE_VERBOSE_
  #define STRUCTURE_VERBOSE_ true
  #undef DEBRUIJN_VERBOSE_
  #define DEBRUIJN_VERBOSE_ true
  #undef COMPRESSOR_VERBOSE_
  #define COMPRESSOR_VERBOSE_ true
#else
  #ifndef MAIN_VERBOSE_
    #define MAIN_VERBOSE_ false
  #endif
  #ifndef MEMORY_VERBOSE_
    #define MEMORY_VERBOSE_ false
  #endif
  #ifndef STRUCTURE_VERBOSE_
    #define STRUCTURE_VERBOSE_ false
  #endif
  #ifndef DEBRUIJN_VERBOSE_
    #define DEBRUIJN_VERBOSE_ false
  #endif
  #ifndef COMPRESSOR_VERBOSE_
    #define COMPRESSOR_VERBOSE_ false
  #endif
#endif

#if (defined(LABEL_CONTEXT_SHORTENING) && defined(INTEGER_CONTEXT_SHORTENING)) \
 || (defined(LABEL_CONTEXT_SHORTENING) && defined(RAS_CONTEXT_SHORTENING))     \
 || (defined(INTEGER_CONTEXT_SHORTENING) && defined(RAS_CONTEXT_SHORTENING))
 #error "Exacly one context shortening algorithm must be specified."
#endif

#if (!defined(INTEGER_CONTEXT_SHORTENING)) && \
    (!defined(LABEL_CONTEXT_SHORTENING)) && \
    (!defined(RAS_CONTEXT_SHORTENING))
  #error "Exacly one context shortening algorithm must be specified."
#endif

#if (defined(DIRECT_MEMORY) && defined(INDEXED_MEMORY)) \
 || (defined(DIRECT_MEMORY) && defined(SIMPLE_MEMORY))  \
 || (defined(INDEXED_MEMORY) && defined(SIMPLE_MEMORY))
 #error "You must define exacly one memory model."
#endif

#if (!defined(INDEXED_MEMORY)) && (!defined(DIRECT_MEMORY)) && (!defined(SIMPLE_MEMORY))
  #error "You must define exacly one memory model."
#endif

#if (defined(FREQ_INCREASE_NONE) && defined(FREQ_INCREASE_FIRST)) \
 || (defined(FREQ_INCREASE_NONE) && defined(FREQ_INCREASE_ALL))  \
 || (defined(FREQ_INCREASE_FIRST) && defined(FREQ_INCREASE_ALL))
 #error "You must define exacly one frequency increase model."
#endif

#if (!defined(FREQ_INCREASE_FIRST)) && (!defined(FREQ_INCREASE_NONE)) && (!defined(FREQ_INCREASE_ALL))
  #error "You must define exacly one frequency increase model."
#endif

#if CACHE_SIZE
#define ENABLE_LOOKUP_CACHE
#endif

#if (!defined(ENABLE_LOOKUP_CACHE) && defined(ENABLE_CACHE_STATS))
#error "Cache stats cannot be enabled when cache itself is disabled"
#endif

#endif
