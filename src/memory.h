#ifndef _COMPRESSION_MEMORY__
#define _COMPRESSION_MEMORY__

#include <limits.h>
#include <stdint.h>
#include <string.h>

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

//#pragma pack(4)
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
mem_ptr Memory_new_leaf(MemObj mem__);

/*
 * Create new memory internal node.
 *
 * @param  mem__  Reference to memory object.
 */
mem_ptr Memory_new_node(MemObj mem__);


#endif  // _COMPRESSION_MEMORY__
