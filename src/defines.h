#ifndef _DEFINES__
#define _DEFINES__

#include "utils.h"

/* Make whole program verbose. */
#define VERBOSE_ false

/* Make parts of the program verbose
 * If VERBOSE_ is set to true, these values are ignored.
 */ 
#define MAIN_VERBOSE_ false
#define MEMORY_VERBOSE_ false
#define STRUCTURE_VERBOSE_ false
#define DEBRUIJN_VERBOSE_ false
#define COMPRESSOR_VERBOSE_ false

/* number of preallocated block pointers */
#define INITIAL_BLOCK_COUNT_ 2
/* Number of elements per memory block - must be power of two! */
#define MEMORY_BLOCK_SIZE_ 4
/* log2 of MEMORY_BLOCK_SIZE_ */
#define MEMORY_BLOCK_SIZE_LOG_ 2

/* Length of the PPMC context */
#define CONTEXT_LENGTH 4

/*
 * Size of stack used for tree traversal. Stack must be atleast as big as
 * maximum expected tree depth. If balanding is not enabled, this number should
 * be much bigger.
 */
#define MAX_STACK_SIZE 64

/**************** OPTIMIZATION DEFINES **********************
 *
 * Make underlying structure tree balanced with red black tree algorithm.
 * Without this tree can (in the worst case) become a linear array with greatly
 * reduced performance */
#define ENABLE_RED_BLACK_BALANCING

/* When splitting nodes, always keep transitions form one node in the same leaf.
 * This can increase used memory by a little bit, but can improve performance.
 * Operations line finding transition in given node or getting frequency of
 * transitions in given node can find everything in one leaf and doesn't have
 * to search for each transition separately. */
#define ENABLE_CLEVER_NODE_SPLIT

/* Use cache for leaf lookup when performing rank, select and other tree
 * related operations */
//#define ENABLE_LOOKUP_CACHE
/* Size of the lookup cache */
#define CACHE_SIZE 1
/* Calculate number of cache hits and misses */
#define ENABLE_CACHE_STATS

/* These define the way of how context shortening is handled.
 * EXPLICIT_CONTEXT_SHORTENING calculates context each time shortening occurs.
 * INTEGER_CONTEXT_SHORTENING saves context sizes in integer variables.
 * RAS_CONTEXT_SHORTENING builds RaS structure above integer sizes.
 *
 * Exacly one of those must be specified */
//#define EXPLICIT_CONTEXT_SHORTENING
//#define INTEGER_CONTEXT_SHORTENING
#define RAS_CONTEXT_SHORTENING

// Define to hide leading $ signs in Label print and function TODO
#define OMIT_EXCESSIVE_DOLLAR_SIGNS_

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

#ifdef EXPLICIT_CONTEXT_SHORTENING
  #ifdef INTEGER_CONTEXT_SHORTENING
    #error "Exacly one context shortening algorithm must be specified."
  #endif
  #ifdef RAS_CONTEXT_SHORTENING
    #error "Exacly one context shortening algorithm must be specified."
  #endif
#endif

#ifdef INTEGER_CONTEXT_SHORTENING
  #ifdef RAS_CONTEXT_SHORTENING
    #error "Exacly one context shortening algorithm must be specified."
  #endif
#endif

#ifndef EXPLICIT_CONTEXT_SHORTENING
  #ifndef INTEGER_CONTEXT_SHORTENING
    #ifndef RAS_CONTEXT_SHORTENING
      #error "Exacly one context shortening algorithm must be specified."
    #endif
  #endif
#endif

#ifdef ENABLE_LOOKUP_CACHE
  #ifndef CACHE_SIZE
    #define CACHE_SIZE 1
  #endif
#endif

#endif  // _DEFINES__
