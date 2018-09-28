#ifndef _COMPRESSION_MEMORY__
#define _COMPRESSION_MEMORY__

#include <limits.h>
#include <stdint.h>
#include <string.h>

#include "../shared/utils.h"

// this number must be power of two!
#define MEMORY_BLOCK_SIZE_ 4
// log2 of above defined variable
#define MEMORY_BLOCK_SIZE_LOG_ 2

// number of block pointers preallocated
#define INITIAL_BLOCK_COUNT_ 2

#define MEMORY_VERBOSE_ false

// ------------------------------------------------------------------------- //

#define MEMORY_VERBOSE(func) \
  if (MEMORY_VERBOSE_) {     \
    func                     \
  }

#define MEMORY_GET_ANY(mem, arg)                              \
  ((NodeRef)((arg & 0x1) ? (void*)MEMORY_GET_LEAF(mem, arg) \
                           : (void*)MEMORY_GET_NODE(mem, arg)))
#define MEMORY_GET_NODE(mem, arg)                                \
  (NodeRef)(&(mem->nodes_[arg >> (MEMORY_BLOCK_SIZE_LOG_ + 1)] \
                           [(arg >> 1) & (MEMORY_BLOCK_SIZE_ - 1)]))
#define MEMORY_GET_LEAF(mem, arg)                                \
  (LeafRef)(&(mem->leafs_[arg >> (MEMORY_BLOCK_SIZE_LOG_ + 1)] \
                           [(arg >> 1) & (MEMORY_BLOCK_SIZE_ - 1)]))

#define IS_LEAF(arg) (arg & 0x1)

#define mem_ptr int32_t

// big for better memory layout
#define bool32 int32_t

#pragma pack(4)
typedef struct {
  // shared counter for total number of elements
  uint32_t p_;

  // number of set bits in L vector
  uint32_t rL_;

  // number of set bits in all W vectors
  uint32_t rW_;
  uint32_t rWl_;  // for lower second level
  uint32_t rWh_;  // for higher second level
  uint32_t rWs_;  // for special last level

  // pointers (index) to child nodes/leaves
  mem_ptr left_;
  mem_ptr right_;

  // red/black node flag (this should be optimised in the future)
  bool32 rb_flag_;
} node_32e;

typedef struct {
  // shared counter for total number of elements
  uint32_t p_;

  // number of set bits in L vector
  uint32_t rL_;

  // number of set bits in all W vectors
  uint32_t rW_;
  uint32_t rWl_;  // for lower second level
  uint32_t rWh_;  // for higher second level
  uint32_t rWs_;  // for special last level

  // all graph data vectors
  uint32_t vectorL_;
  uint32_t vectorWl0_;
  uint32_t vectorWl1_;
  uint32_t vectorWl2_;
  uint32_t vectorP_[32];
} leaf_32e;

#define NodeRef node_32e*
#define LeafRef leaf_32e*

typedef struct {
  NodeRef* nodes_;
  LeafRef* leafs_;

  int32_t n_block_count_;          // number of allocated block pointers
  int32_t n_current_block_;        // index of current block
  int32_t n_current_block_index_;  // index inside current block (first free if
                                   // possible)
  int32_t n_last_index_;           // last global index (without bitshift)

  int32_t l_block_count_;          // number of allocated block pointers
  int32_t l_current_block_;        // index of current block
  int32_t l_current_block_index_;  // index inside current block (first free if
                                   // possible)
  int32_t l_last_index_;           // last global index (without bitshift)
} memory_32e;

#define MemObj memory_32e*

/*
 * Initialize and return memory object.
 *
 * @return  Reference to memory object.
 */
MemObj Memory_init(void) {
  MemObj mem = (memory_32e*)malloc(sizeof(memory_32e));

  // initialize node and leaf blocks and first block
  mem->nodes_ = (NodeRef*)malloc(INITIAL_BLOCK_COUNT_ * sizeof(NodeRef));
  mem->leafs_ = (LeafRef*)malloc(INITIAL_BLOCK_COUNT_ * sizeof(LeafRef));

  mem->nodes_[0] = (NodeRef)malloc(MEMORY_BLOCK_SIZE_ * sizeof(node_32e));
  mem->leafs_[0] = (LeafRef)malloc(MEMORY_BLOCK_SIZE_ * sizeof(leaf_32e));

  // initialize all counters
  mem->n_block_count_ = INITIAL_BLOCK_COUNT_;
  mem->n_current_block_ = 0;
  mem->n_current_block_index_ = 0;
  mem->n_last_index_ = -1;

  mem->l_block_count_ = INITIAL_BLOCK_COUNT_;
  mem->l_current_block_ = 0;
  mem->l_current_block_index_ = 0;
  mem->l_last_index_ = -1;

  return mem;
}

/*
 * Free all memory associated with given memory object.
 *
 * After this function call all allocated nodes will be freed
 * and given pointer will be NULL.
 *
 * @param  mem__  Reference to pointer to memory object.
 */
void Memory_free(MemObj* mem__) {
  int32_t i;
  MemObj mem = *mem__;

  for (i = 0; i <= mem->n_current_block_; i++)
    free(mem->nodes_[i]);
  free(mem->nodes_);

  for (i = 0; i <= mem->l_current_block_; i++)
    free(mem->leafs_[i]);
  free(mem->leafs_);

  free(mem);
  mem__ = NULL;
}

/*
 * Create new memory leaf.
 *
 * @param  mem__  Reference to memory object.
 */
mem_ptr Memory_new_leaf(MemObj mem__) {
  // Check if all memory was depleated, this is an omezeni TODO in a design and program cannot recover at this point.
  if (mem__->l_last_index_ >= 0x3FFFFFFF)
    FATAL("Reached maximum possible number of leafs");

  // check if we need new memory block
  if (mem__->l_current_block_index_ >= MEMORY_BLOCK_SIZE_) {
    MEMORY_VERBOSE(
      printf("Creating new leaf memory block\n");
    )

    // check if new superblock must be allocated
    if (++mem__->l_current_block_ >= mem__->l_block_count_) {
      MEMORY_VERBOSE(
        printf("Reallocation of leaf superblock pointer array\n");
      )

      // realloc block memory
      mem__->l_block_count_ *= 2;
      mem__->leafs_ = (LeafRef*)realloc(
          mem__->leafs_, mem__->l_block_count_ * sizeof(LeafRef));
    }

    // allocate new memory block
    mem__->l_current_block_index_ = 0;
    mem__->leafs_[mem__->l_current_block_] =
        (LeafRef)malloc(MEMORY_BLOCK_SIZE_ * sizeof(leaf_32e));
  }

  // return mem_ptr reference
  mem__->l_current_block_index_++;
  return ((++mem__->l_last_index_) << 1) | 0x1;
}

/*
 * Create new memory internal node.
 *
 * @param  mem__  Reference to memory object.
 */
mem_ptr Memory_new_node(MemObj mem__) {
  // Check if all memory was depleated, this is an omezeni TODO in a design and program cannot recover at this point.
  if (mem__->n_last_index_ >= 0x3FFFFFFF)
    FATAL("Reached maximum possible number of nodes");

  // check if we need new memory block
  if (mem__->n_current_block_index_ >= MEMORY_BLOCK_SIZE_) {
    MEMORY_VERBOSE(
      printf("Creating new node memory block\n");
    )

    // check if new superblock must be allocated
    if (++mem__->n_current_block_ >= mem__->n_block_count_) {
      MEMORY_VERBOSE(
        printf("Reallocation of node superblock pointer array\n");
      )

      // realloc block memory
      mem__->n_block_count_ *= 2;
      mem__->nodes_ = (NodeRef*)realloc(
          mem__->nodes_, mem__->n_block_count_ * sizeof(NodeRef));
    }

    // allocate new memory block
    mem__->n_current_block_index_ = 0;
    mem__->nodes_[mem__->n_current_block_] =
        (NodeRef)malloc(MEMORY_BLOCK_SIZE_ * sizeof(node_32e));
  }

  // return mem_ptr reference
  mem__->n_current_block_index_++;
  return (++mem__->n_last_index_) << 1;
}

#endif  // _COMPRESSION_MEMORY__
