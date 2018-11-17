#ifndef _DBV_MEMORY__
#define _DBV_MEMORY__

#include <limits.h>
#include <stdint.h>
#include <string.h>

#include "utils.h"

/* this number must be power of two! */
#define DBV_MEMORY_BLOCK_SIZE 1024
/* log2 of above defined variable */
#define DBV_MEMORY_BLOCK_SIZE_LOG 10

/* number of block pointers preallocated */
#define DBV_INITIAL_BLOCK_COUNT 2

#define DBV_MEMORY_VERBOSE_ false

#define DBV_MEMORY_VERBOSE(func) \
  if (DBV_MEMORY_VERBOSE_) {     \
    func                         \
  }

#define DBV_MEMORY_GET_ANY(mem, arg)                                \
  ((DBVNodeRef)((arg & 0x1) ? (void*) DBV_MEMORY_GET_LEAF(mem, arg) \
                            : (void*) DBV_MEMORY_GET_NODE(mem, arg)))
#define DBV_MEMORY_GET_NODE(mem, arg)                                \
  (DBVNodeRef)(&(mem->nodes_[arg >> (DBV_MEMORY_BLOCK_SIZE_LOG + 1)] \
                            [(arg >> 1) & (DBV_MEMORY_BLOCK_SIZE - 1)]))
#define DBV_MEMORY_GET_LEAF(mem, arg)                                \
  (DBVLeafRef)(&(mem->leafs_[arg >> (DBV_MEMORY_BLOCK_SIZE_LOG + 1)] \
                            [(arg >> 1) & (DBV_MEMORY_BLOCK_SIZE - 1)]))

#define DBV_IS_LEAF(arg) (arg & 0x1)

typedef struct DBVNode_32b {
  uint32_t p_;
  uint32_t r_;
  int32_t left_;
  int32_t right_;

  bool32 rb_flag_;
} DBVNode_32b;

typedef struct DBVLeaf_32b {
  uint32_t p_;
  uint32_t r_;
  uint32_t vector_;
} DBVLeaf_32b;

#define DBVNodeRef DBVNode_32b*
#define DBVLeafRef DBVLeaf_32b*

typedef struct {
  DBVNodeRef* nodes_;
  DBVLeafRef* leafs_;

  int32_t n_block_count_;         /* number of allocated block pointers */
  int32_t n_current_block_;       /* index of current block */
  int32_t n_current_block_index_; /* index inside current block (first free if possible) */
  int32_t n_last_index_;          /* last global index (without bitshift) */

  int32_t l_block_count_;         /* number of allocated block pointers */
  int32_t l_current_block_;       /* index of current block */
  int32_t l_current_block_index_; /* index inside current block (first free if possible) */
  int32_t l_last_index_;          /* last global index (without bitshift) */
} DBVMemory_32b;

#define DBVMemObj DBVMemory_32b*
#define DBVMemPtr int32_t

DBVMemObj DBV_Memory_Init(void);

void DBV_Memory_Free(DBVMemObj* mem__);

DBVMemPtr DBV_Memory_new_leaf(DBVMemObj mem__);

DBVMemPtr DBV_Memory_new_node(DBVMemObj mem__);

#endif
