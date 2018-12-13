#ifndef _COMPRESSION_MEMORY__
#define _COMPRESSION_MEMORY__

#include <limits.h>
#include <stdint.h>
#include <string.h>

#include "defines.h"
#include "utils.h"

#define MEMORY_VERBOSE(func) \
  if (MEMORY_VERBOSE_) {     \
    func                     \
  }

/* definitions for indexed memory model */
#if defined(INDEXED_MEMORY)

#define MEMORY_GET_ANY(mem, arg)                            \
  ((NodeRef)((arg & 0x1) ? (void*)MEMORY_GET_LEAF(mem, arg) \
                         : (void*)MEMORY_GET_NODE(mem, arg)))
#define MEMORY_GET_NODE(mem, arg)                              \
  (NodeRef)(&(mem->nodes_[arg >> (MEMORY_BLOCK_SIZE_LOG_ + 1)] \
                         [(arg >> 1) & (MEMORY_BLOCK_SIZE_ - 1)]))
#define MEMORY_GET_LEAF(mem, arg)                              \
  (LeafRef)(&(mem->leafs_[arg >> (MEMORY_BLOCK_SIZE_LOG_ + 1)] \
                         [(arg >> 1) & (MEMORY_BLOCK_SIZE_ - 1)]))

#define MAKE_NODE(arg) {}
#define MAKE_LEAF(arg) {}
#define IS_LEAF(arg) (arg & 0x1)

#define MemPtr int32_t

/* definitions for direct memory model */
#elif defined(DIRECT_MEMORY)  /* defined(INDEXED_MEMORY) */

#define MEMORY_GET_ANY(mem, arg) ((NodeRef)(arg))
#define MEMORY_GET_NODE(mem, arg) ((NodeRef)(arg))
#define MEMORY_GET_LEAF(mem, arg) ((LeafRef)(arg))

#if defined(EMBEDED_FLAGS)

#define MAKE_NODE(arg) (arg)->rW_[5] &= 0x7FFFFFFF
#define MAKE_LEAF(arg) (arg)->rW_[5] |= 0x80000000
#define IS_LEAF(arg) (((arg)->rW_[5]) & 0x80000000)

#else  /* defined(EMBEDED_FLAGS) */

#define MAKE_NODE(arg) (arg)->is_leaf = false
#define MAKE_LEAF(arg) (arg)->is_leaf = true
#define IS_LEAF(arg) ((arg)->is_leaf)

#endif  /* defined(EMBEDED_FLAGS) */

#define MemPtr struct node_32e*

#endif  /* defined(INDEXED_MEMORY) */


#if defined(EMBEDED_FLAGS)

#define IS_RED(arg) (((arg)->rW_[6]) & 0x80000000)
#define MAKE_RED(arg) (arg)->rW_[6] |= 0x80000000
#define MAKE_BLACK(arg) (arg)->rW_[6] &= 0x7FFFFFFF

#define GET_RVECTOR(arg, idx) \
  ((idx == 5 || idx == 6) ? ((arg->rW_[idx]) & 0x7FFFFFFF) : (arg->rW_[idx]))

#else  /* defined(EMBEDED_FLAGS) */

#define IS_RED(arg) ((arg)->rb_flag_)
#define MAKE_RED(arg) (arg)->rb_flag_ = true
#define MAKE_BLACK(arg) (arg)->rb_flag_ = false

#define GET_RVECTOR(arg, idx) (arg->rW_[idx])

#endif  /* defined(EMBEDED_FLAGS) */


typedef struct node_32e {
  uint32_t p_;  /* shared counter for total number of elements */
  uint32_t rL_; /* number of set bits in L vector */

  /* number of set bits in all W vectors */
  uint32_t rW_[7];

  /*
   * Highest bits of r5 and r6 are used as a flags for red black tree balancing algorithm and the
   * is_leaf macro. While this might not be the perfect place to put them, it is pretty good
   * considering that this prototype application uses many defines to change behavior and these
   * two counters are always there without any substantial change.
   *
   * r5 -> leaf indicator
   * r6 -> node color for rb balancing
   */

#if defined(DIRECT_MEMORY) && (!defined(EMBEDED_FLAGS))
  Bool32 is_leaf;
#endif

  MemPtr left_;  /* pointer to left child node */
  MemPtr right_; /* pointer to right child node */

#if !defined(EMBEDED_FLAGS)
  Bool32 rb_flag_; /* red/black node flag */
#endif
} node_32e;

typedef struct {
  uint32_t p_;  /* shared counter for total number of elements */
  uint32_t rL_; /* number of set bits in L vector */

  /* number of set bits in all W vectors */
  uint32_t rW_[7];

#if defined(DIRECT_MEMORY) && (!defined(EMBEDED_FLAGS))
  Bool32 is_leaf;
#endif

  /* all graph data vectors */
  uint32_t vectorL_;
  uint32_t vectorW_[3];
  uint32_t vectorP_[32];

#if defined(INTEGER_CONTEXT_SHORTENING)
  uint8_t context_[32];
#endif

} leaf_32e;

#define NodeRef node_32e*
#define LeafRef leaf_32e*

typedef struct {
  NodeRef* nodes_;
  LeafRef* leafs_;

  int32_t n_block_count_;         /* number of allocated block pointers */
  int32_t n_current_block_;       /* index of current block */
  int32_t n_current_block_index_; /* index inside current block (first free if possible) */
  int32_t n_last_index_;          /* last global index (without bitshift) */

  int32_t l_block_count_;         /* number of allocated block pointers */
  int32_t l_current_block_;       /* index of current block */
  int32_t l_current_block_index_; /* index inside current block (first free if possible) */
  int32_t l_last_index_;          /* last global index (without bitshift) */
} memory_32e;

#define MemObj memory_32e*

/*
 * Initialize and return memory object.
 *
 * @return  Reference to memory object.
 */
MemObj Memory_init(void);

/*
 * Free all memory associated with given memory object.
 *
 * After this function call all allocated nodes will be freed
 * and given pointer will be NULL.
 *
 * @param  mem__  Reference to pointer to memory object.
 */
void Memory_free(MemObj* mem__);

/*
 * Create new memory leaf.
 *
 * @param  mem__  Reference to memory object.
 */
MemPtr Memory_new_leaf(MemObj mem__);

/*
 * Create new memory internal node.
 *
 * @param  mem__  Reference to memory object.
 */
MemPtr Memory_new_node(MemObj mem__);

#endif
