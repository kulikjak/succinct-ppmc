#include "memory.h"

#if defined(DIRECT_MEMORY) || defined(INDEXED_MEMORY)

MemObj Memory_init(void) {
  MemObj mem = (memory_32e*) malloc_(sizeof(memory_32e));

  /* initialize node and leaf blocks and first block */
  mem->nodes_ = (NodeRef*) malloc_(INITIAL_BLOCK_COUNT_ * sizeof(NodeRef));
  mem->leafs_ = (LeafRef*) malloc_(INITIAL_BLOCK_COUNT_ * sizeof(LeafRef));

  mem->nodes_[0] = (NodeRef) malloc_(MEMORY_BLOCK_SIZE_ * sizeof(node_32e));
  mem->leafs_[0] = (LeafRef) malloc_(MEMORY_BLOCK_SIZE_ * sizeof(leaf_32e));

  /* initialize all counters */
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

void Memory_free(MemObj* mem__) {
  int32_t i;
  MemObj mem = *mem__;

  for (i = 0; i <= mem->n_current_block_; i++)
    free_(mem->nodes_[i]);
  free_(mem->nodes_);

  for (i = 0; i <= mem->l_current_block_; i++)
    free_(mem->leafs_[i]);
  free_(mem->leafs_);

  free_(mem);
  mem__ = NULL;
}

MemPtr Memory_new_leaf(MemObj mem__) {
#if defined(INDEXED_MEMORY)
  /* Check if all memory was depleted. This is a limitation in the program design, and it is not
   * possible to continue if this happens. */
  if (mem__->l_last_index_ >= 0x3FFFFFFF)
    FATAL("[memory]: Reached maximum possible number of leafs");
#endif

  /* check if we need new memory block */
  if (mem__->l_current_block_index_ >= MEMORY_BLOCK_SIZE_) {
    MEMORY_VERBOSE(
      printf("[memory]: Creating new leaf memory block\n");
    )

    /* check if new superblock must be allocated */
    if (++mem__->l_current_block_ >= mem__->l_block_count_) {
      MEMORY_VERBOSE(
        printf("[memory]: Reallocation of leaf superblock pointer array\n");
      )

      /* realloc block memory */
      mem__->l_block_count_ *= 2;
      mem__->leafs_ = (LeafRef*) realloc_(mem__->leafs_, mem__->l_block_count_ * sizeof(LeafRef));
    }

    /* allocate new memory block */
    mem__->l_current_block_index_ = 0;
    mem__->leafs_[mem__->l_current_block_] =
        (LeafRef) malloc_(MEMORY_BLOCK_SIZE_ * sizeof(leaf_32e));
  }

  /* return MemPtr reference */
#if defined(INDEXED_MEMORY)
  mem__->l_current_block_index_++;
  return ((++mem__->l_last_index_) << 1) | 0x1;
#elif defined(DIRECT_MEMORY)
  MAKE_LEAF(&(mem__->leafs_[mem__->l_current_block_][mem__->l_current_block_index_++]));
  return (MemPtr)(&(mem__->leafs_[mem__->l_current_block_][mem__->l_current_block_index_ - 1]));
#endif
}

MemPtr Memory_new_node(MemObj mem__) {
#if defined(INDEXED_MEMORY)
  /* Check if all memory was depleted. This is a limitation in the program design, and it is not
   * possible to continue if this happens. */
  if (mem__->n_last_index_ >= 0x3FFFFFFF)
    FATAL("[memory]: Reached maximum possible number of nodes");
#endif

  /* check if we need new memory block */
  if (mem__->n_current_block_index_ >= MEMORY_BLOCK_SIZE_) {
    MEMORY_VERBOSE(
      printf("[memory]: Creating new node memory block\n");
    )

    /* check if new superblock must be allocated */
    if (++mem__->n_current_block_ >= mem__->n_block_count_) {
      MEMORY_VERBOSE(
        printf("[memory]: Reallocation of node superblock pointer array\n");
      )

      /* realloc block memory */
      mem__->n_block_count_ *= 2;
      mem__->nodes_ = (NodeRef*) realloc_(mem__->nodes_, mem__->n_block_count_ * sizeof(NodeRef));
    }

    /* allocate new memory block */
    mem__->n_current_block_index_ = 0;
    mem__->nodes_[mem__->n_current_block_] =
        (NodeRef) malloc_(MEMORY_BLOCK_SIZE_ * sizeof(node_32e));
  }

  /* return MemPtr reference */
#if defined(INDEXED_MEMORY)
  mem__->n_current_block_index_++;
  return (++mem__->n_last_index_) << 1;
#elif defined(DIRECT_MEMORY)
  MAKE_NODE(&(mem__->nodes_[mem__->n_current_block_][mem__->n_current_block_index_++]));
  return (MemPtr)(&(mem__->nodes_[mem__->n_current_block_][mem__->n_current_block_index_ - 1]));
#endif
}

#else  /* defined(DIRECT_MEMORY) || defined(INDEXED_MEMORY) */

MemObj Memory_init(void) {
  return NULL;
}

void Memory_free(MemObj* mem__) {
  UNUSED(mem__);
  /* This model is not deallocating anything at this time */
}

MemPtr Memory_new_leaf(MemObj mem__) {
  UNUSED(mem__);

  MemPtr leaf = (MemPtr) malloc_(sizeof(leaf_32e));
  MAKE_LEAF(leaf);
  return leaf;
}

MemPtr Memory_new_node(MemObj mem__) {
  UNUSED(mem__);

  MemPtr node = (MemPtr) malloc_(sizeof(node_32e));
  MAKE_NODE(node);
  return node;
}

#endif  /* defined(DIRECT_MEMORY) || defined(INDEXED_MEMORY) */
