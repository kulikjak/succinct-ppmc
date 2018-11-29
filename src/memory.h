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

#define IS_LEAF(arg) (arg & 0x1)

#define MemPtr int32_t

/* definitions for direct memory model */
#elif defined(DIRECT_MEMORY)

#define MEMORY_GET_ANY(mem, arg) ((NodeRef)(arg))
#define MEMORY_GET_NODE(mem, arg) ((NodeRef)(arg))
#define MEMORY_GET_LEAF(mem, arg) ((LeafRef)(arg))

#define IS_LEAF(arg) (arg)->is_leaf

#define MemPtr struct node_32e*

#endif

typedef struct node_32e {
  uint32_t p_;  /* shared counter for total number of elements */
  uint32_t rL_; /* number of set bits in L vector */

  /* number of set bits in all W vectors */
  uint32_t rW_[8];

#if defined(DIRECT_MEMORY)
  Bool32 is_leaf;
#endif

  MemPtr left_;  /* pointer to left child node */
  MemPtr right_; /* pointer to right child node */

  Bool32 rb_flag_; /* red/black node flag (can be optimised in the future) */
} node_32e;

typedef struct {
  uint32_t p_;  /* shared counter for total number of elements */
  uint32_t rL_; /* number of set bits in L vector */

  /* number of set bits in all W vectors */
  uint32_t rW_[8];

#if defined(DIRECT_MEMORY)
  Bool32 is_leaf;
#endif

  /* all graph data vectors */
  uint32_t vectorL_;
  uint32_t vectorW_[4];
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
