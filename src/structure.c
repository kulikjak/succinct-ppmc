#include "structure.h"

#ifdef RAS_CONTEXT_SHORTENING
#include "universal.h"

UWT_Struct uwt;
#endif

void Graph_Init(GraphRef Graph__) {
  Graph__->mem_ = Memory_init();
  Graph__->root_ = Memory_new_leaf(Graph__->mem_);

  LeafRef leaf_ref = MEMORY_GET_LEAF(Graph__->mem_, Graph__->root_);

  // memset each leaf variable to zero
  memset(&(leaf_ref->p_), 0, sizeof(leaf_32e));

#ifdef RAS_CONTEXT_SHORTENING
  UWT_Init(&uwt, CONTEXT_LENGTH + 1);
#endif

#ifdef DIRECT_MEMORY
  leaf_ref->is_leaf = true;
#endif

#ifdef ENABLE_LOOKUP_CACHE
  reset_cache();
#endif
}

void Graph_Free(GraphRef Graph__) {
  Memory_free(&(Graph__->mem_));

#ifdef RAS_CONTEXT_SHORTENING
  UWT_Free(&uwt);
#endif
}

void Graph_Insert_Line_(LeafRef leaf__, uint32_t pos__, GLineRef line__) {
  switch (line__->W_) {
    case VALUE_A:
      INSERT_BIT(&(leaf__->vectorWl0_), &(leaf__->rW_), pos__, 0);
      INSERT_BIT(&(leaf__->vectorWl1_), &(leaf__->rWl_), pos__, 0);
      INSERT_BIT(&(leaf__->vectorWl2_), &(leaf__->rWs_), pos__, 0);
      break;
    case VALUE_C:
      INSERT_BIT(&(leaf__->vectorWl0_), &(leaf__->rW_), pos__, 0);
      INSERT_BIT(&(leaf__->vectorWl1_), &(leaf__->rWl_), pos__, 1);
      INSERT_BIT(&(leaf__->vectorWl2_), &(leaf__->rWs_), pos__, 0);
      break;
    case VALUE_G:
      INSERT_BIT(&(leaf__->vectorWl0_), &(leaf__->rW_), pos__, 1);
      INSERT_BIT(&(leaf__->vectorWl1_), &(leaf__->rWh_), pos__, 0);
      INSERT_BIT(&(leaf__->vectorWl2_), &(leaf__->rWs_), pos__, 0);
      break;
    case VALUE_T:
      INSERT_BIT(&(leaf__->vectorWl0_), &(leaf__->rW_), pos__, 1);
      INSERT_BIT(&(leaf__->vectorWl1_), &(leaf__->rWh_), pos__, 1);
      INSERT_BIT(&(leaf__->vectorWl2_), &(leaf__->rWs_), pos__, 0);
      break;
    case VALUE_$:
      INSERT_BIT(&(leaf__->vectorWl0_), &(leaf__->rW_), pos__, 1);
      INSERT_BIT(&(leaf__->vectorWl1_), &(leaf__->rWh_), pos__, 1);
      INSERT_BIT(&(leaf__->vectorWl2_), &(leaf__->rWs_), pos__, 1);
      break;
    default:
      assert(0);
  }
  INSERT_BIT(&(leaf__->vectorL_), &(leaf__->rL_), pos__, line__->L_);

  // move all later integers to next position and insert new one
  if (pos__ < leaf__->p_) {
    memmove(&(leaf__->vectorP_[pos__ + 1]), &(leaf__->vectorP_[pos__]),
            (leaf__->p_ - pos__) * sizeof(int32_t));
#if defined(INTEGER_CONTEXT_SHORTENING)
    memmove(&(leaf__->context_[pos__ + 1]), &(leaf__->context_[pos__]),
            (leaf__->p_ - pos__) * sizeof(int32_t));
#endif

  }
  leaf__->vectorP_[pos__] = line__->P_;

  leaf__->p_++;
}

void GLine_Insert(GraphRef Graph__, uint32_t pos__, GLineRef line__) {
  int32_t split_mask, split_offset;
  int32_t mask;
  mem_ptr current;
  uint32_t temp;
  bool parent_left, grandparent_left;

  assert(pos__ <= MEMORY_GET_ANY(Graph__->mem_, Graph__->root_)->p_);

#ifdef ENABLE_LOOKUP_CACHE
  reset_cache();
#endif

  STACK_CLEAN();

  STRUCTURE_VERBOSE(
    printf("[structure]: Inserting new line on position %u\n", pos__);
  )

#if defined(RAS_CONTEXT_SHORTENING)
    UWT_Insert(&uwt, pos__, 0);
#endif

  // traverse the tree and enter correct leaf
  mask = GET_MASK_FROM_VALUE(line__->W_);
  current = Graph__->root_;

  while (!IS_LEAF(current)) {
    STACK_PUSH(current);
    // update p and r counters as we are traversing the structure
    node_32e* node = MEMORY_GET_NODE(Graph__->mem_, current);
    node->p_ += 1;

    node->rL_ += line__->L_;
    node->rW_ += (mask & 0x4) >> 0x2;
    node->rWl_ += (mask & 0x2) && ((~mask) & 0x4);
    node->rWh_ += (mask & 0x2) && (mask & 0x4);
    node->rWs_ += (mask & 0x1) && (mask & 0x2) && (mask & 0x4);

    temp = MEMORY_GET_ANY(Graph__->mem_, node->left_)->p_;
    if (temp > pos__) {
      current = node->left_;
    } else {
      pos__ -= temp;
      current = node->right_;
    }
  }

  LeafRef current_ref = MEMORY_GET_LEAF(Graph__->mem_, current);

  if (current_ref->p_ < 32) {  // insert info leaf with space to spare
    Graph_Insert_Line_(current_ref, pos__, line__);

  } else {  // current leaf is full and we have to split it
    STRUCTURE_VERBOSE(
      printf("[structure]: Leaf is and will be split\n");
    )

    mem_ptr node = Memory_new_node(Graph__->mem_);
    NodeRef node_ref = MEMORY_GET_NODE(Graph__->mem_, node);

    node_ref->p_ = 33;
    node_ref->rL_ = current_ref->rL_ + line__->L_;
    node_ref->rW_ = current_ref->rW_ + ((mask & 0x4) >> 0x2);
    node_ref->rWl_ = current_ref->rWl_ + ((mask & 0x2) && ((~mask) & 0x4));
    node_ref->rWh_ = current_ref->rWh_ + ((mask & 0x2) && (mask & 0x4));
    node_ref->rWs_ = current_ref->rWs_ + ((mask & 0x1) && (mask & 0x2) && (mask & 0x4));

    // allocate new right leaf and reuse current as left leaf
    node_ref->right_ = Memory_new_leaf(Graph__->mem_);
    node_ref->left_ = current;

    STACK_PUSH(node);

    // find a position for clever splitting
    split_mask = 0xFFFF;
    split_offset = 0;

#ifdef ENABLE_CLEVER_NODE_SPLIT
    if ((current_ref->vectorL_ >> 16) & 0x1) {
      // do nothing
    } else if ((current_ref->vectorL_ >> 15) & 0x1) {
      split_mask = 0x7FFF;
      split_offset = 1;
    } else if ((current_ref->vectorL_ >> 17) & 0x1) {
      split_mask = 0x1FFFF;
      split_offset = -1;
    } else if ((current_ref->vectorL_ >> 14) & 0x1) {
      split_mask = 0x3FFF;
      split_offset = 2;
    } else {
      //FATAL("Node split unsuccessful, structure is corrupted.");
    }
#endif

    // initialize new right node
    LeafRef right_ref = MEMORY_GET_LEAF(Graph__->mem_, node_ref->right_);
    right_ref->vectorL_ = 0x0 | ((current_ref->vectorL_ & (split_mask)) << (16 + split_offset));

    right_ref->vectorWl0_ = 0x0 | ((current_ref->vectorWl0_ & (split_mask)) << (16 + split_offset));
    right_ref->vectorWl1_ = 0x0 | ((current_ref->vectorWl1_ & (split_mask)) << (16 + split_offset));
    right_ref->vectorWl2_ = 0x0 | ((current_ref->vectorWl2_ & (split_mask)) << (16 + split_offset));

    memcpy(right_ref->vectorP_, current_ref->vectorP_ + 16 + split_offset, (16 - split_offset) * sizeof(uint32_t));

#ifdef INTEGER_CONTEXT_SHORTENING
    memcpy(right_ref->context_, current_ref->context_ + 16 + split_offset, (16 - split_offset) * sizeof(uint32_t));
#endif

    right_ref->p_ = 16 - split_offset;

    // calculate rank on newly created leaf
    right_ref->rL_ = RANK(right_ref->vectorL_);
    right_ref->rW_ = RANK(right_ref->vectorWl0_);
    right_ref->rWl_ =
        RANK((~(right_ref->vectorWl0_)) & (right_ref->vectorWl1_));
    right_ref->rWh_ = RANK((right_ref->vectorWl0_) & (right_ref->vectorWl1_));
    right_ref->rWs_ = RANK((right_ref->vectorWl0_) & (right_ref->vectorWl1_) &
                             (right_ref->vectorWl2_));

    // initialize (reuse) old leaf as left one
    current_ref->vectorL_ = current_ref->vectorL_ & (~split_mask);

    current_ref->vectorWl0_ = current_ref->vectorWl0_ & (~split_mask);
    current_ref->vectorWl1_ = current_ref->vectorWl1_ & (~split_mask);
    current_ref->vectorWl2_ = current_ref->vectorWl2_ & (~split_mask);

    current_ref->p_ = 16 + split_offset;

    // recalculate rank on old shrinked leaf
    current_ref->rL_ = RANK(current_ref->vectorL_);
    current_ref->rW_ = RANK(current_ref->vectorWl0_);
    current_ref->rWl_ =
        RANK((~(current_ref->vectorWl0_)) & (current_ref->vectorWl1_));
    current_ref->rWh_ =
        RANK((current_ref->vectorWl0_) & (current_ref->vectorWl1_));
    current_ref->rWs_ =
        RANK((current_ref->vectorWl0_) & (current_ref->vectorWl1_) &
               (current_ref->vectorWl2_));

    // now insert bit into correct leaf
    if (pos__ < (uint32_t)(16 + split_offset)) {
      Graph_Insert_Line_(current_ref, pos__, line__);
    } else {
      Graph_Insert_Line_(right_ref, pos__ - (16 + split_offset), line__);
    }

    // finally exchange pointers to new node
    if (STACK_GET_PARENT() == STACK_ERROR) {
      Graph__->root_ = node;
    } else {
      NodeRef parent = MEMORY_GET_NODE(Graph__->mem_, STACK_GET_PARENT());
      if (parent->left_ == current)
        parent->left_ = node;
      else
        parent->right_ = node;
    }

#ifdef ENABLE_RED_BLACK_BALANCING

    STRUCTURE_VERBOSE(
      printf("[structure]: Rebalancing the tree\n");
    )

    // red black balancing
    node_ref->rb_flag_ = true;

    do {
      // current node is the root - change it to black and end
      if (STACK_GET_PARENT() == STACK_ERROR) {
        node_ref->rb_flag_ = false;
        return;
      }

      mem_ptr parent_idx = STACK_GET_PARENT();
      NodeRef parent = MEMORY_GET_NODE(Graph__->mem_, parent_idx);
      // parent node is a black node - do nothing
      if (parent->rb_flag_ == false) {
        return;
      }

      mem_ptr grandparent_idx = STACK_GET_GRANDPARENT();
      NodeRef grandparent = MEMORY_GET_NODE(Graph__->mem_, grandparent_idx);

      // get uncle
      mem_ptr uncle_idx = grandparent->left_;
      grandparent_left = false;
      if (grandparent->left_ == STACK_GET_PARENT()) {
        uncle_idx = grandparent->right_;
        grandparent_left = true;
      }

      NodeRef uncle = MEMORY_GET_ANY(Graph__->mem_, uncle_idx);

      // uncle is red - change colors and go to start
      if (!IS_LEAF(uncle_idx) && uncle->rb_flag_ == true) {
        parent->rb_flag_ = false;
        uncle->rb_flag_ = false;
        grandparent->rb_flag_ = true;

        node = grandparent_idx;
        node_ref = grandparent;
        STACK_POP();
        STACK_POP();
        continue;
      }

      // uncle is black - time for rotations
      parent_left = true;
      if (node == parent->right_) {
        parent_left = false;
      }

      mem_ptr newroot = 0;
      if (parent_left && grandparent_left) {
        grandparent->rb_flag_ = true;
        parent->rb_flag_ = false;

        grandparent->left_ = parent->right_;
        parent->right_ = grandparent_idx;

        NODE_OPERATION_2(grandparent, node_ref, -=);
        NODE_OPERATION_2(parent, uncle, +=);

        newroot = parent_idx;
      } else if (!parent_left && !grandparent_left) {
        grandparent->rb_flag_ = true;
        parent->rb_flag_ = false;

        grandparent->right_ = parent->left_;
        parent->left_ = grandparent_idx;

        NODE_OPERATION_2(grandparent, node_ref, -=);
        NODE_OPERATION_2(parent, uncle, +=);

        newroot = parent_idx;
      } else if (!parent_left && grandparent_left) {
        grandparent->rb_flag_ = true;
        node_ref->rb_flag_ = false;

        grandparent->left_ = node_ref->right_;
        parent->right_ = node_ref->left_;

        node_ref->left_ = parent_idx;
        node_ref->right_ = grandparent_idx;

        NodeRef temp = MEMORY_GET_ANY(Graph__->mem_, grandparent->left_);

        NODE_OPERATION_2(parent, temp, -=);
        NODE_OPERATION_2(grandparent, parent, -=);
        NODE_OPERATION_3(node_ref, parent, grandparent, +);

        newroot = node;
      } else if (parent_left && !grandparent_left) {
        grandparent->rb_flag_ = true;
        node_ref->rb_flag_ = false;

        grandparent->right_ = node_ref->left_;
        parent->left_ = node_ref->right_;

        node_ref->left_ = grandparent_idx;
        node_ref->right_ = parent_idx;

        NodeRef temp = MEMORY_GET_ANY(Graph__->mem_, grandparent->right_);

        NODE_OPERATION_2(parent, temp, -=);
        NODE_OPERATION_2(grandparent, parent, -=);
        NODE_OPERATION_3(node_ref, parent, grandparent, +);

        newroot = node;
      }

      // finally exchange pointers to new node
      if (STACK_GET_GRANDGRANDPARENT() == STACK_ERROR) {
        Graph__->root_ = newroot;
      } else {
        NodeRef grandgrandparent = MEMORY_GET_NODE(Graph__->mem_, STACK_GET_GRANDGRANDPARENT());
        if (grandgrandparent->left_ == grandparent_idx)
          grandgrandparent->left_ = newroot;
        else
          grandgrandparent->right_ = newroot;
      }
      break;
    } while (true);

#endif  // ENABLE_RED_BLACK_BALANCING

  }
}

void GLine_Fill(GLineRef line__, Graph_value L__, Graph_value W__, uint32_t P__) {
  line__->L_ = L__;
  line__->W_ = W__;
  line__->P_ = P__;
}

void GLine_Explode(GLineRef line__, Graph_value* L__, Graph_value* W__, uint32_t* P__) {
  assert(*L__ == 0 || *L__ == 1);

  *L__ = line__->L_;
  *W__ = line__->W_;
  *P__ = line__->P_;
}

void Graph_Print(GraphRef Graph__) {
  int32_t i, size;

  Graph_Line line;

  // print header for main structure
  printf("  L  W   P\n-----------------\n");

  size = Graph_Size(Graph__);
  for (i = 0; i < size; i++) {
    printf("%3d: ", i);
    GLine_Get(Graph__, i, &line);
    printf("%d  %c   %d\n", line.L_, line.W_, line.P_);
  }
}

void GLine_Get(GraphRef Graph__, uint32_t pos__, GLineRef line__) {
  int32_t wavelet_mask;
  mem_ptr current;
  LeafRef leaf_ref;

  STRUCTURE_VERBOSE(
    printf("[structure]: Getting line at position %u\n", pos__);
  )

  GET_TARGET_LEAF(Graph__, pos__, current, leaf_ref, WITHOUT_STACK);

  wavelet_mask = (leaf_ref->vectorWl2_ >> (31 - pos__) & 0x1) |
                 (leaf_ref->vectorWl1_ >> (31 - pos__) & 0x1) << 0x1 |
                 (leaf_ref->vectorWl0_ >> (31 - pos__) & 0x1) << 0x2;

  line__->L_ = leaf_ref->vectorL_ >> (31 - pos__) & 0x1;
  line__->W_ = GET_VALUE_FROM_MASK(wavelet_mask);
  line__->P_ = leaf_ref->vectorP_[pos__];
}

int32_t Graph_Size(GraphRef Graph__) {
  NodeRef node_ref = MEMORY_GET_ANY(Graph__->mem_, Graph__->root_);
  return node_ref->p_;
}

void Graph_Change_symbol(GraphRef Graph__, uint32_t pos__, Graph_value val__) {
  int32_t nchar_mask, ochar_mask;
  mem_ptr current;

  NodeRef node_ref;
  LeafRef leaf_ref;

  STRUCTURE_VERBOSE(
    printf("[structure]: Changing symbol at position %u to %d\n", pos__, val__);
  )

  GET_TARGET_LEAF(Graph__, pos__, current, leaf_ref, WITH_STACK);

  // get masks for both characters
  nchar_mask = GET_MASK_FROM_VALUE(val__);
  ochar_mask = (leaf_ref->vectorWl2_ >> (31 - pos__) & 0x1) |
               (leaf_ref->vectorWl1_ >> (31 - pos__) & 0x1) << 0x1 |
               (leaf_ref->vectorWl0_ >> (31 - pos__) & 0x1) << 0x2;

  // change old character to the new one
  leaf_ref->vectorWl0_ ^= (-(unsigned)(!!(nchar_mask & 0x4)) ^ leaf_ref->vectorWl0_) & (0x1 << (31 - pos__));
  leaf_ref->vectorWl1_ ^= (-(unsigned)(!!(nchar_mask & 0x2)) ^ leaf_ref->vectorWl1_) & (0x1 << (31 - pos__));
  leaf_ref->vectorWl2_ ^= (-(unsigned)(!!(nchar_mask & 0x1)) ^ leaf_ref->vectorWl2_) & (0x1 << (31 - pos__));

  do {
    node_ref = MEMORY_GET_ANY(Graph__->mem_, current);

    // decrease counters
    if (ochar_mask & 0x1) node_ref->rWs_ -= 1;
    if ((ochar_mask & 0x2) && (!(ochar_mask & 0x4))) node_ref->rWl_ -= 1;
    if ((ochar_mask & 0x2) && (ochar_mask & 0x4)) node_ref->rWh_ -= 1;
    if (ochar_mask & 0x4) node_ref->rW_ -= 1;

    // increase counter
    if (nchar_mask & 0x1) node_ref->rWs_ += 1;
    if ((nchar_mask & 0x2) && (!(nchar_mask & 0x4))) node_ref->rWl_ += 1;
    if ((nchar_mask & 0x2) && (nchar_mask & 0x4)) node_ref->rWh_ += 1;
    if (nchar_mask & 0x4) node_ref->rW_ += 1;

    current = STACK_POP();
  } while (current != STACK_ERROR);
}

void Graph_Increase_frequency(GraphRef Graph__, uint32_t pos__) {
  mem_ptr current;
  LeafRef leaf_ref;

  STRUCTURE_VERBOSE(
    printf("[structure]: Increasing frequency of transition at position %u\n", pos__);
  )

  GET_TARGET_LEAF(Graph__, pos__, current, leaf_ref, WITHOUT_STACK)

  leaf_ref->vectorP_[pos__] ++;
}


#ifdef ENABLE_CLEVER_NODE_SPLIT

void Graph_Get_symbol_frequency(GraphRef Graph__, uint32_t pos__, cfreq* freq__) {
  mem_ptr current;
  Graph_value value;
  LeafRef leaf_ref;
  int32_t l_bit, ochar_mask, cnt;

  STRUCTURE_VERBOSE(
    printf("[structure]: Getting symbol frequency at position %u\n", pos__);
  )

  GET_TARGET_LEAF(Graph__, pos__, current, leaf_ref, WITHOUT_STACK)

  // get to the beginning of this node
  int32_t pos = (int32_t)pos__;
  pos --;
  while (pos >= 0) {
    l_bit = leaf_ref->vectorL_ >> (31 - pos) & 0x1;
    if (l_bit) break;
    pos --;
  }
  pos ++;

  // clear freq structure
  cnt = 0;
  memset(freq__, 0, sizeof(*freq__));

  // go through all transitions in this node
  do {
    ochar_mask = (leaf_ref->vectorWl2_ >> (31 - pos) & 0x1) |
          (leaf_ref->vectorWl1_ >> (31 - pos) & 0x1) << 0x1 |
          (leaf_ref->vectorWl0_ >> (31 - pos) & 0x1) << 0x2;

    value = GET_VALUE_FROM_MASK(ochar_mask);
    if (value == VALUE_$) {
      return;
    }
    cnt++;

    freq__->symbol_[value] = leaf_ref->vectorP_[pos];
    freq__->total_ += leaf_ref->vectorP_[pos];

    l_bit = leaf_ref->vectorL_ >> (31 - pos++) & 0x1;
    if (l_bit) break;
  } while (1);

  freq__->symbol_[VALUE_ESC] = cnt;
  freq__->total_ += cnt;
}

int32_t Graph_Find_Edge(GraphRef Graph__, uint32_t pos__, Graph_value val__) {
  mem_ptr current;
  Graph_value value;
  LeafRef leaf_ref;
  int32_t l_bit, ochar_mask, backup = (int32_t)pos__;

  STRUCTURE_VERBOSE(
    printf("[structure]: Looking for transition for symbol %d at position %u\n", val__, pos__);
  )

  GET_TARGET_LEAF(Graph__, pos__, current, leaf_ref, WITHOUT_STACK)

  // get to the beginning of this node
  int32_t pos = (int32_t)pos__;
  pos --;
  while (pos >= 0) {
    l_bit = leaf_ref->vectorL_ >> (31 - pos) & 0x1;
    if (l_bit) break;
    pos --;
  }
  pos ++;

  // go through all transitions in this node
  do {
    ochar_mask = (leaf_ref->vectorWl2_ >> (31 - pos) & 0x1) |
          (leaf_ref->vectorWl1_ >> (31 - pos) & 0x1) << 0x1 |
          (leaf_ref->vectorWl0_ >> (31 - pos) & 0x1) << 0x2;

    value = GET_VALUE_FROM_MASK(ochar_mask);
    if (value == val__) {
      return backup + (pos - (int32_t)pos__);
    }

    l_bit = leaf_ref->vectorL_ >> (31 - pos++) & 0x1;
    if (l_bit) break;
  } while (1);

  return -1;
}

#if defined(INTEGER_CONTEXT_SHORTENING) \
 || defined(RAS_CONTEXT_SHORTENING)

void Graph_Set_csl(GraphRef Graph__, uint32_t pos__, int32_t csl__) {
  STRUCTURE_VERBOSE(
    printf("[structure]: Setting common suffix len at position %u\n", pos__);
  )

#if defined(INTEGER_CONTEXT_SHORTENING)
  mem_ptr current;
  LeafRef leaf_ref;

  GET_TARGET_LEAF(Graph__, pos__, current, leaf_ref, WITHOUT_STACK)
  leaf_ref->context_[pos__] = csl__;
#elif defined(RAS_CONTEXT_SHORTENING)
  UNUSED(Graph__);

  UWT_Delete(&uwt, pos__);
  UWT_Insert(&uwt, pos__, csl__);
#endif
}

int32_t Graph_Get_csl(GraphRef Graph__, uint32_t pos__) {
  STRUCTURE_VERBOSE(
    printf("[structure]: Getting common suffix len at position %u\n", pos__);
  )

#if defined(INTEGER_CONTEXT_SHORTENING)
  mem_ptr current;
  LeafRef leaf_ref;

  GET_TARGET_LEAF(Graph__, pos__, current, leaf_ref, WITHOUT_STACK)
  return leaf_ref->context_[pos__];
#elif defined(RAS_CONTEXT_SHORTENING)
  UNUSED(Graph__);

  return UWT_Get(&uwt, pos__);
#endif
}

#endif  // INTEGER_CONTEXT_SHORTENING

#endif  // ENABLE_CLEVER_NODE_SPLIT
