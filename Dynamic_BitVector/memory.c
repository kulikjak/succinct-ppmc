#include "memory.h"

DBVMemObj DBV_Memory_Init(void) {
  DBVMemObj mem = (DBVMemObj)malloc_(sizeof(DBVMemory_32b));

  /* initialize node and leaf blocks and first block */
  mem->nodes_ = (DBVNodeRef*)malloc_(DBV_INITIAL_BLOCK_COUNT * sizeof(DBVNodeRef));
  mem->leafs_ = (DBVLeafRef*)malloc_(DBV_INITIAL_BLOCK_COUNT * sizeof(DBVLeafRef));

  mem->nodes_[0] = (DBVNodeRef)malloc_(DBV_MEMORY_BLOCK_SIZE * sizeof(DBVNode_32b));
  mem->leafs_[0] = (DBVLeafRef)malloc_(DBV_MEMORY_BLOCK_SIZE * sizeof(DBVLeaf_32b));

  /* initialize all other counters */
  mem->n_block_count_ = DBV_INITIAL_BLOCK_COUNT;
  mem->n_current_block_ = 0;
  mem->n_current_block_index_ = 0;
  mem->n_last_index_ = -1;

  mem->l_block_count_ = DBV_INITIAL_BLOCK_COUNT;
  mem->l_current_block_ = 0;
  mem->l_current_block_index_ = 0;
  mem->l_last_index_ = -1;

  return mem;
}

void DBV_Memory_Free(DBVMemObj* mem__) {
  int32_t i;
  DBVMemObj mem = *mem__;

  for (i = 0; i <= mem->n_current_block_; i++)
    free(mem->nodes_[i]);
  free(mem->nodes_);

  for (i = 0; i <= mem->l_current_block_; i++)
    free(mem->leafs_[i]);
  free(mem->leafs_);

  free(mem);
  mem__ = NULL;
}

DBVMemPtr DBV_Memory_new_leaf(DBVMemObj mem__) {
  if (mem__->l_last_index_ >= 0x3FFFFFFF)
    FATAL("[memory]: Reached maximum possible number of leafs");

  /* check if we need new memory block */
  if (mem__->l_current_block_index_ >= DBV_MEMORY_BLOCK_SIZE) {
    DBV_MEMORY_VERBOSE(
      printf("[memory]: Creating new leaf memory block\n");
    )

    /* check if new block must be allocated */
    if (++mem__->l_current_block_ >= mem__->l_block_count_) {
      DBV_MEMORY_VERBOSE(
        printf("[memory]: Reallocation of leaf superblock pointer array\n");
      )

      /* realloc block memory */
      mem__->l_block_count_ *= 2;
      mem__->leafs_ = (DBVLeafRef*)realloc_(
          mem__->leafs_, mem__->l_block_count_ * sizeof(DBVLeafRef));
    }

    /* allocate block itself */
    mem__->l_current_block_index_ = 0;
    mem__->leafs_[mem__->l_current_block_] =
        (DBVLeafRef)malloc_(DBV_MEMORY_BLOCK_SIZE * sizeof(DBVLeaf_32b));
  }

  /* return int reference */
  mem__->l_current_block_index_++;
  return ((++mem__->l_last_index_) << 1) | 0x1;
}

DBVMemPtr DBV_Memory_new_node(DBVMemObj mem__) {
  if (mem__->n_last_index_ >= 0x3FFFFFFF)
    FATAL("[memory]: Reached maximum possible number of nodes");

  /* check if we need new memory block */
  if (mem__->n_current_block_index_ >= DBV_MEMORY_BLOCK_SIZE) {
    DBV_MEMORY_VERBOSE(
      printf("[memory]: Creating new node memory block\n");
    )

    /* check if new block must be allocated */
    if (++mem__->n_current_block_ >= mem__->n_block_count_) {
      DBV_MEMORY_VERBOSE(
        printf("[memory]: Reallocation of node superblock pointer array\n");
      )

      /* realloc block memory */
      mem__->n_block_count_ *= 2;
      mem__->nodes_ = (DBVNodeRef*)realloc_(
          mem__->nodes_, mem__->n_block_count_ * sizeof(DBVNodeRef));
    }

    /* allocate block itself */
    mem__->n_current_block_index_ = 0;
    mem__->nodes_[mem__->n_current_block_] =
        (DBVNodeRef)malloc_(DBV_MEMORY_BLOCK_SIZE * sizeof(DBVNode_32b));
  }

  /* return int reference */
  mem__->n_current_block_index_++;
  return (++mem__->n_last_index_) << 1;
}
