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
  #define MEMORY_BLOCK_SIZE_ 4
#endif
/* log2 of MEMORY_BLOCK_SIZE_ */
#ifndef MEMORY_BLOCK_SIZE_LOG_
  #define MEMORY_BLOCK_SIZE_LOG_ 2
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
 * LABEL_CONTEXT_SHORTENING calculates context each time shortening occurs.
 * INTEGER_CONTEXT_SHORTENING saves context sizes in integer variables.
 * TREE_CONTEXT_SHORTENING uses no additional memory by traversing the tree
 *   from the root all the way to node with shorter context. Since the context
 *   is always small and not dependent on the size of the input, this is fast.
 *
 * RAS_CONTEXT_SHORTENING was removed due to its incredible memory overhead.
 * While it would probably be pretty fast, it would go against the memory
 * efficiency targeted by this program.
 *
 * Exacly one of those must be specified */
//#define LABEL_CONTEXT_SHORTENING
//#define INTEGER_CONTEXT_SHORTENING
#define TREE_CONTEXT_SHORTENING

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
 * Exacly one of those must be specified */
//#define INDEXED_MEMORY
#define DIRECT_MEMORY

/* Use PPMD algorithm instead of PPMC. This is here mostly for compression
 * comparison with other implementations and it is not implemented most
 * efficiently. */
//#define PPMD

/* Define to hide leading $ signs in Label print.
 * This doesn't have any performance drawbacks, because label print functions
 * are never used in actual code except for debugging. */
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

#if (defined(LABEL_CONTEXT_SHORTENING) && defined(INTEGER_CONTEXT_SHORTENING)) \
 || (defined(LABEL_CONTEXT_SHORTENING) && defined(TREE_CONTEXT_SHORTENING))    \
 || (defined(INTEGER_CONTEXT_SHORTENING) && defined(TREE_CONTEXT_SHORTENING))
 #error "Exacly one context shortening algorithm must be specified."
#endif

#if (! defined(INTEGER_CONTEXT_SHORTENING)) && \
    (! defined(LABEL_CONTEXT_SHORTENING)) && \
    (! defined(TREE_CONTEXT_SHORTENING))
  #error "Exacly one context shortening algorithm must be specified."
#endif

#if (defined(DIRECT_MEMORY) && defined(INDEXED_MEMORY)) || \
    (! defined(DIRECT_MEMORY) && ! defined(INDEXED_MEMORY))
  #error "You must define exacly one memory model."
#endif

#ifdef ENABLE_LOOKUP_CACHE
  #ifndef CACHE_SIZE
    #define CACHE_SIZE 1
  #endif
#endif

#endif  // _DEFINES__
