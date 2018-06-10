
#ifndef _RAS_MEMORY__
#define _RAS_MEMORY__

#include <limits.h>
#include <stdint.h>
#include <string.h>
#include "utils.h"

// this number must be power of two!
#define MEMORY_BLOCK_SIZE 1024
// log2 of above defined variable
#define MEMORY_BLOCK_SIZE_LOG 10

// number of block pointers preallocated
#define INITIAL_BLOCK_COUNT 2

#define RAS_MEMORY_VEBOSE_ false

// ------------------------------------------------------------------------- //

#define RAS_MEMORY_VEBOSE(func) \
  if (RAS_MEMORY_VEBOSE_) {     \
    func                        \
  }

#define MEMORY_GET_ANY(mem, arg)                              \
  ((Node_32b*)((arg & 0x1) ? (void*)MEMORY_GET_LEAF(mem, arg) \
                           : (void*)MEMORY_GET_NODE(mem, arg)))
#define MEMORY_GET_NODE(mem, arg)                               \
  (Node_32b*)(&(mem->nodes_[arg >> (MEMORY_BLOCK_SIZE_LOG + 1)] \
                           [(arg >> 1) & (MEMORY_BLOCK_SIZE - 1)]))
#define MEMORY_GET_LEAF(mem, arg)                               \
  (Leaf_32b*)(&(mem->leafs_[arg >> (MEMORY_BLOCK_SIZE_LOG + 1)] \
                           [(arg >> 1) & (MEMORY_BLOCK_SIZE - 1)]))

#define IS_LEAF(arg) (arg & 0x1)


#define IS_RED(arg) (!IS_LEAF(arg) & (arg->left_ & 0x80000000))
#define IS_BLACK(arg) (!(IS_RED(arg)))
#define UNCOLOR_PTR(arg) (arg & 0x7FFFFFFF)

#define MAKE_RED(arg) ((arg) | 0x80000000)
#define MAKE_BLACK(arg) UNCOLOR_PTR(arg)


#pragma pack(1)
typedef struct Node_32b {
  uint32_t p_;
  uint32_t r_;
  int32_t left_;
  int32_t right_;
} Node_32b;

typedef struct Leaf_32b {
  uint32_t p_;
  uint32_t r_;
  uint32_t vector_;
} Leaf_32b;

typedef struct memory_32b {
  Node_32b** nodes_;
  Leaf_32b** leafs_;

  int32_t n_block_count;          // number of allocated block pointers
  int32_t n_current_block;        // index of current block
  int32_t n_current_block_index;  // index inside current block (first free if
                                  // possible)
  int32_t n_last_index;           // last global index (without bitshift)

  int32_t l_block_count;          // number of allocated block pointers
  int32_t l_current_block;        // index of current block
  int32_t l_current_block_index;  // index inside current block (first free if
                                  // possible)
  int32_t l_last_index;           // last global index (without bitshift)
} memory_32b;

/*
 * Initialize and return memory object.
 */
memory_32b* init_memory(void) {
  memory_32b* mem = (memory_32b*)malloc(sizeof(memory_32b));

  // initialize node and leaf blocks and first block
  mem->nodes_ = (Node_32b**)malloc(INITIAL_BLOCK_COUNT * sizeof(Node_32b*));
  mem->leafs_ = (Leaf_32b**)malloc(INITIAL_BLOCK_COUNT * sizeof(Leaf_32b*));

  mem->nodes_[0] = (Node_32b*)malloc(MEMORY_BLOCK_SIZE * sizeof(Node_32b));
  mem->leafs_[0] = (Leaf_32b*)malloc(MEMORY_BLOCK_SIZE * sizeof(Leaf_32b));

  // initialize all other counters
  mem->n_block_count = INITIAL_BLOCK_COUNT;
  mem->n_current_block = 0;
  mem->n_current_block_index = 0;
  mem->n_last_index = -1;

  mem->l_block_count = INITIAL_BLOCK_COUNT;
  mem->l_current_block = 0;
  mem->l_current_block_index = 0;
  mem->l_last_index = -1;

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
void clean_memory(memory_32b** mem__) {
  int32_t i;
  memory_32b* mem = *mem__;

  for (i = 0; i <= mem->n_current_block; i++) free(mem->nodes_[i]);
  free(mem->nodes_);

  for (i = 0; i <= mem->l_current_block; i++) free(mem->leafs_[i]);
  free(mem->leafs_);

  free(mem);
  mem__ = NULL;
}

/*
 * Create new leaf and return its number reference to memory.
 *
 * @param  mem__  Reference to memory object.
 */
int32_t new_leaf(memory_32b* mem__) {
  if (mem__->l_last_index >= 0x3FFFFFFF)
    FATAL("Reached maximum possible number of leafs");

  // check if we need new memory block
  if (mem__->l_current_block_index >= MEMORY_BLOCK_SIZE) {
    RAS_MEMORY_VEBOSE(printf("New block (leafs)\n");)

    mem__->l_current_block++;
    mem__->l_current_block_index = 0;

    // check if new block must be allocated
    if (mem__->l_current_block >= mem__->l_block_count) {
      RAS_MEMORY_VEBOSE(
          printf("Reallocation of block memory pointer array (leafs)\n");)

      // realloc block memory
      mem__->l_block_count *= 2;
      mem__->leafs_ = (Leaf_32b**)realloc(
          mem__->leafs_, mem__->l_block_count * sizeof(Leaf_32b*));
    }

    // allocate block itself
    mem__->leafs_[mem__->l_current_block] =
        (Leaf_32b*)malloc(MEMORY_BLOCK_SIZE * sizeof(Leaf_32b));
  }

  // return int reference
  mem__->l_current_block_index++;
  return ((++mem__->l_last_index) << 1) | 0x1;
}

/*
 * Create new node and return its number reference to memory.
 *
 * @param  mem__  Reference to memory object.
 */
int32_t new_node(memory_32b* mem__) {
  if (mem__->n_last_index >= 0x3FFFFFFF)
    FATAL("Reached maximum possible number of nodes");

  // check if we need new memory block
  if (mem__->n_current_block_index >= MEMORY_BLOCK_SIZE) {
    RAS_MEMORY_VEBOSE(printf("New block (nodes)\n");)

    mem__->n_current_block++;
    mem__->n_current_block_index = 0;

    // check if new block must be allocated
    if (mem__->n_current_block >= mem__->n_block_count) {
      RAS_MEMORY_VEBOSE(
          printf("Reallocation of block memory pointer array (nodes)\n");)

      // realloc block memory
      mem__->n_block_count *= 2;
      mem__->nodes_ = (Node_32b**)realloc(
          mem__->nodes_, mem__->n_block_count * sizeof(Node_32b*));
    }

    // allocate block itself
    mem__->nodes_[mem__->n_current_block] =
        (Node_32b*)malloc(MEMORY_BLOCK_SIZE * sizeof(Node_32b));
  }

  // return int reference
  mem__->n_current_block_index++;
  return (++mem__->n_last_index) << 1;
}


#endif  // _RAS_MEMORY__
