#ifndef _COMPRESSION_STRUCT__
#define _COMPRESSION_STRUCT__

#include "memory.h"

#include "../shared/stack.h"
#include "../shared/utils.h"

#define SYMBOL_COUNT 4

#define INSERT_BIT(vector, counter, pos, value)                          \
  {                                                                      \
    assert(pos < 32);                                                    \
    uint32_t mask = ((pos) == 0) ? 0 : (uint32_t)(~(0)) << (32 - (pos)); \
    uint32_t temp = 0;                                                   \
                                                                         \
    temp |= (*vector) & mask;                                            \
    temp |= (((*vector) & ~(mask))) >> 1;                                \
    *(vector) = temp;                                                    \
                                                                         \
    if (value) {                                                         \
      *(vector) |= 0x1 << (31 - (pos));                                  \
      (*(counter))++;                                                    \
    }                                                                    \
  }

typedef struct {
  mem_ptr root_;
  MemObj mem_;
} Graph_Struct;

typedef struct {
  bool L_;
  char W_;  
  uint32_t P_;
} Graph_Line;

#define GraphRef Graph_Struct*
#define GLineRef Graph_Line*

typedef enum {
	VECTOR_L,
	VECTOR_W
} Graph_vector;

typedef enum {
	VALUE_0 = 0,
	VALUE_1 = 1,
	VALUE_A,
	VALUE_C,
	VALUE_G,
	VALUE_T,
	VALUE_$
} Graph_value;

#include "rank.h"
#include "select.h"

int32_t get_mask_from_char_(char symb__) {
	switch (symb__) {
		case 'A':
			return 0;
		case 'C':
			return 2;
		case 'G':
			return 4;
		case 'T':
			return 6;
		case '$':
			return 7;
	}
	FATAL("Unexpected symbol");
	return 0;
}

char get_char_from_mask_(int32_t mask__) {
	switch (mask__) {
		case 0:
			return 'A';
		case 2:
			return 'C';
		case 4:
			return 'G';
		case 6:
			return 'T';
		case 7:
			return '$';
	}
	FATAL("Unexpected mask");
	return 0;	
}

/*
 * Initialize Graph_Struct object given as argument.
 *
 * @param  Graph__  Reference to Graph_Struct object.
 */
void Graph_Init(GraphRef Graph__) {
  Graph__->mem_ = Memory_init();
  Graph__->root_ = Memory_new_leaf(Graph__->mem_);

  LeafRef leaf_ref = MEMORY_GET_LEAF(Graph__->mem_, Graph__->root_);

  // memset each leaf variable to zero
  memset(&(leaf_ref->p_), 0, sizeof(leaf_32e));
}

/*
 * Free all memory associated with MS object.
 *
 * @param  Graph__  Reference to Graph_Struct object.
 */
void Graph_Free(GraphRef Graph__) {
  Graph__->root_ = -1;
  Memory_free(&(Graph__->mem_));
}


/* Simple debuging function to print whole integer vector in binary form */
void print_bitvector_(int32_t vector__) {
  int32_t i;

  for (i = 0; i < 32; i++)
    printf("%d", (vector__ >> (31 - i)) & 0x1);
  printf("\n");
}


/* Perform standard rank on more significant half of a 32 bit vector. */
#define RANK16(vector) \
  __builtin_popcount((vector) & 0xFFFF0000)


void Graph_Insert_Line_(LeafRef leaf__, uint32_t pos__, GLineRef line__) {
  switch(line__->W_) {
    case 'A':
      INSERT_BIT(&(leaf__->vectorWl0_), &(leaf__->rW_), pos__, 0);
      INSERT_BIT(&(leaf__->vectorWl1_), &(leaf__->rWl_), pos__, 0);
      INSERT_BIT(&(leaf__->vectorWl2_), &(leaf__->rWs_), pos__, 0);
      break;
    case 'C':
      INSERT_BIT(&(leaf__->vectorWl0_), &(leaf__->rW_), pos__, 0);
      INSERT_BIT(&(leaf__->vectorWl1_), &(leaf__->rWl_), pos__, 1);
      INSERT_BIT(&(leaf__->vectorWl2_), &(leaf__->rWs_), pos__, 0);
      break;
    case 'G':
      INSERT_BIT(&(leaf__->vectorWl0_), &(leaf__->rW_), pos__, 1);
      INSERT_BIT(&(leaf__->vectorWl1_), &(leaf__->rWh_), pos__, 0);
      INSERT_BIT(&(leaf__->vectorWl2_), &(leaf__->rWs_), pos__, 0);
      break;
    case 'T':
      INSERT_BIT(&(leaf__->vectorWl0_), &(leaf__->rW_), pos__, 1);
      INSERT_BIT(&(leaf__->vectorWl1_), &(leaf__->rWh_), pos__, 1);
      INSERT_BIT(&(leaf__->vectorWl2_), &(leaf__->rWs_), pos__, 0);
      break;
    case '$':
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
    memmove(&(leaf__->vectorP_[pos__ + 1]),
            &(leaf__->vectorP_[pos__]),
            (leaf__->p_ - pos__) * sizeof(int32_t));
  }
  leaf__->vectorP_[pos__] = line__->P_;

  leaf__->p_ ++;
}


void GLine_Insert(GraphRef Graph__, uint32_t pos__, GLineRef line__) {
  assert(pos__ <= MEMORY_GET_ANY(Graph__->mem_, Graph__->root_)->p_);

  STACK_CLEAN();

  VERBOSE( fprintf(stderr, "Inserting new line on position %u\n", pos__); )

  // traverse the tree and enter correct leaf
  int32_t mask = get_mask_from_char_(line__->W_);
  int32_t current = Graph__->root_;
  STACK_PUSH(current);

  while (!IS_LEAF(current)) {
    // update p and r counters as we are traversing the structure
    node_32e* node = MEMORY_GET_NODE(Graph__->mem_, current);
    node->p_ += 1;

    node->rL_ += line__->L_;
		node->rW_ += (mask & 0x4) >> 0x2;
		node->rWl_ += (mask & 0x2) && ((~mask) & 0x4);
		node->rWh_ += (mask & 0x2) && (mask & 0x4);
		node->rWs_ += (mask & 0x1) && (mask & 0x2) && (mask & 0x4);

    uint32_t temp = MEMORY_GET_ANY(Graph__->mem_, node->left_)->p_;
    if (temp >= pos__) {
      current = node->left_;
    } else {
      pos__ -= temp;
      current = node->right_;
    }

    STACK_PUSH(current);
  }

  LeafRef current_ref = MEMORY_GET_LEAF(Graph__->mem_, current);

  if (current_ref->p_ < 32) {  // insert info leaf with space to spare
  	Graph_Insert_Line_(current_ref, pos__, line__);

  } else {  // current leaf is full and we have to split it
    VERBOSE( fprintf(stderr, "Splitting full node\n"); )

    mem_ptr node = Memory_new_node(Graph__->mem_);
    NodeRef node_ref = MEMORY_GET_NODE(Graph__->mem_, node);

    node_ref->p_ = 33;
    node_ref->rL_ = current_ref->rL_ + line__->L_;
		node_ref->rW_ = current_ref->rW_ + ((mask & 0x4) >> 0x2);
		node_ref->rWl_ = current_ref->rWl_ + ((mask & 0x2) && ((~mask) & 0x4));
		node_ref->rWh_ = current_ref->rWh_ + ((mask & 0x2) && (mask & 0x4));
		node_ref->rWs_ = current_ref->rWs_ + ((mask & 0x1) && (mask & 0x2) && (mask & 0x4));

    // allocate new right leaf and reuse current as left leaf
    node_ref->right_ = Memory_new_leaf(Graph__->mem_); // FIXME
    node_ref->left_ = current;

    // initialize new right node
    LeafRef right_ref = MEMORY_GET_LEAF(Graph__->mem_, node_ref->right_);
    right_ref->vectorL_ = (current_ref->vectorL_ & (0xFFFF)) << 16;

    right_ref->vectorWl0_ = (current_ref->vectorWl0_ & (0xFFFF)) << 16;
    right_ref->vectorWl1_ = (current_ref->vectorWl1_ & (0xFFFF)) << 16;
    right_ref->vectorWl2_ = (current_ref->vectorWl2_ & (0xFFFF)) << 16;

    memcpy(right_ref->vectorP_, current_ref->vectorP_ + 16, 16 * sizeof(uint32_t));

    right_ref->p_ = 16;

    // calculate rank on newly created leaf
    right_ref->rL_ = RANK16(right_ref->vectorL_);
    right_ref->rW_ = RANK16(right_ref->vectorWl0_);
    right_ref->rWl_ = RANK16((~(right_ref->vectorWl0_)) & (right_ref->vectorWl1_));
    right_ref->rWh_ = RANK16((right_ref->vectorWl0_) & (right_ref->vectorWl1_));
    right_ref->rWs_ = RANK16((right_ref->vectorWl0_) & (right_ref->vectorWl1_) & (right_ref->vectorWl2_));

    // initialize (reuse) old leaf as left one
    current_ref->vectorL_ = current_ref->vectorL_ & (0xFFFF0000);

    current_ref->vectorWl0_ = current_ref->vectorWl0_ & (0xFFFF0000);
    current_ref->vectorWl1_ = current_ref->vectorWl1_ & (0xFFFF0000);
    current_ref->vectorWl2_ = current_ref->vectorWl2_ & (0xFFFF0000);

    current_ref->p_ = 16;

    // recalculate rank on old shrinked leaf
    current_ref->rL_ = RANK16(current_ref->vectorL_);
    current_ref->rW_ = RANK16(current_ref->vectorWl0_);
    current_ref->rWl_ = RANK16((~(current_ref->vectorWl0_)) & (current_ref->vectorWl1_));
    current_ref->rWh_ = RANK16((current_ref->vectorWl0_) & (current_ref->vectorWl1_));
    current_ref->rWs_ = RANK16((current_ref->vectorWl0_) & (current_ref->vectorWl1_) & (current_ref->vectorWl2_));

    // now insert bit into correct leaf
    if (pos__ <= 16) {
    	Graph_Insert_Line_(current_ref, pos__, line__);
    } else {
    	Graph_Insert_Line_(right_ref, pos__ - 16, line__);
    }

    // finally exchange pointers to new node
    if (STACK_GET_PARENT() == -1) {
      Graph__->root_ = node;
    } else {
      NodeRef parent = MEMORY_GET_NODE(Graph__->mem_, STACK_GET_PARENT());
      if (parent->left_ == current)
        parent->left_ = node;
      else
        parent->right_ = node;
    }
  }
}

/*
 * Fill Graph_Line struct with given information.
 *
 * @param  line__  [Out] Reference to Graph_Line object.
 * @param  L__  Corresponding line variable.
 * @param  W__  Corresponding line variable.
 * @param  P__  Corresponding line variable.
 */
void GLine_Fill(GLineRef line__, bool L__, char W__, uint32_t P__) {
  line__->L_ = L__;
  line__->W_ = W__;
  line__->P_ = P__;
}

/*
 * Explode Graph_Line struct variables into separate variables.
 *
 * @param  line__  Reference to Graph_Line object.
 * @param  L__  [Out] Reference to corresponding line variable.
 * @param  W__  [Out] Reference to corresponding line variable.
 * @param  P__  [Out] Reference to corresponding line variable.
 */
void GLine_Explode(GLineRef line__, bool* L__, char* W__, uint32_t* P__) {
	assert(*L__ == 0 || *L__ == 1);

  *L__ = line__->L_;
  *W__ = line__->W_;
  *P__ = line__->P_;
}

/*
 * Get one line in given Graph_Struct object.
 *
 * Corresponding line is returned in the form of Graph_Line object.
 *
 * @param  Graph__  Reference to Graph_Struct object.
 * @param  pos__  Index of requested line.
 * @param  line__  [Out] Reference to Graph_Line object.
 */
void GLine_Get(GraphRef Graph__, uint32_t pos__, GLineRef line__) {
  int32_t wavelet_mask;
  mem_ptr temp;
  mem_ptr current = Graph__->root_;

  NodeRef node_ref;
  LeafRef leaf_ref;

  // check correct boundaries
  node_ref = MEMORY_GET_ANY(Graph__->mem_, current);

  assert(pos__ < node_ref->p_);

  // traverse the tree and enter correct leaf
  while (!IS_LEAF(current)) {
    node_ref = MEMORY_GET_NODE(Graph__->mem_, current);

    // get p_ counter of left child and act accordingly
    temp = MEMORY_GET_ANY(Graph__->mem_, node_ref->left_)->p_;
    if ((uint32_t)temp > pos__) {
      current = node_ref->left_;
    } else {
      pos__ -= temp;
      current = node_ref->right_;
    }
  }

  assert(line__ != NULL);

  // fill in the line struct
  leaf_ref = MEMORY_GET_LEAF(Graph__->mem_, current);
  wavelet_mask = (leaf_ref->vectorWl2_ >> (31 - pos__) & 0x1) |
                 (leaf_ref->vectorWl1_ >> (31 - pos__) & 0x1) << 0x1 |
                 (leaf_ref->vectorWl0_ >> (31 - pos__) & 0x1) << 0x2;

  line__->L_ = leaf_ref->vectorL_ >> (31 - pos__) & 0x1;
  line__->W_ = get_char_from_mask_(wavelet_mask);
  line__->P_ = leaf_ref->vectorP_[pos__];
}

/*
 * Get number of elements saved in the Graph_Struct.
 *
 * @param  Graph__  Reference to Graph_Struct object.
 */
int32_t Graph_Size(GraphRef Graph__) {
  NodeRef node_ref = MEMORY_GET_ANY(Graph__->mem_, Graph__->root_);
  return node_ref->p_;
}

/*
 * Print whole graph structure.
 *
 * @param  Graph__  Reference to Graph_Struct object.
 */
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

/*
 * Rank Graph_struct.
 *
 * @param  Graph__  Reference to graph structure object.
 * @param  pos__  Query position.
 * @param  type__  Sub structure that should be queried [enum: [Graph_vector]].
 * @param  val__  Query value [enum: Graph_value].
 */
int32_t Graph_Rank(GraphRef Graph__, uint32_t pos__, Graph_vector type__, Graph_value val__) {
	int32_t temp;

	if (type__ == VECTOR_L) {
		if (val__ == VALUE_0)
			return pos__ - graph_Lrank_(*Graph__, pos__);
		else if (val__ == VALUE_1)
			return graph_Lrank_(*Graph__, pos__);
		else
			FATAL("VECTOR_L does not support this query value.");

	} else if (type__ == VECTOR_W) {
	  switch (val__) {
	    case VALUE_A:
	    	temp = pos__ - graph_Wrank_top_(*Graph__, pos__);
	    	return temp - graph_Wrank_left_(*Graph__, temp);
	    case VALUE_C:
	    	temp = pos__ - graph_Wrank_top_(*Graph__, pos__);
	    	return graph_Wrank_left_(*Graph__, temp);
	    case VALUE_G:
	    	temp = graph_Wrank_top_(*Graph__, pos__);
	    	return temp - graph_Wrank_right_(*Graph__, temp);
	    case VALUE_T:
	    	temp = graph_Wrank_top_(*Graph__, pos__);
	    	temp = graph_Wrank_right_(*Graph__, temp);
	    	return temp - graph_Wrank_bottom_(*Graph__, temp);
	    case VALUE_$:
	    	temp = graph_Wrank_top_(*Graph__, pos__);
	    	temp = graph_Wrank_right_(*Graph__, temp);
	    	return graph_Wrank_bottom_(*Graph__, temp);
			default:
				FATAL("VECTOR_W does not support this query value.");
		}
	}
	FATAL("Unknown vector type");
	return 0;
}

/*
 * Select Graph_struct.
 *
 * @param  Graph__  Reference to WT_Struct object.
 * @param  pos__  Query position.
 * @param  type__  Sub structure that should be queried [enum: [Graph_vector]].
 * @param  val__  Query value [enum: Graph_value].
 */
int32_t Graph_Select(GraphRef Graph__, uint32_t pos__, Graph_vector type__, Graph_value val__) {
	int32_t temp;

	if (type__ == VECTOR_L) {
		if (val__ == VALUE_0)
			return graph_Lselect_(*Graph__, pos__, true);
		else if (val__ == VALUE_1)
			return graph_Lselect_(*Graph__, pos__, false);
		else
			FATAL("VECTOR_L does not support this query value.");

	} else if (type__ == VECTOR_W) {
	  switch (val__) {
	    case VALUE_A:
	    	temp = graph_Wselect_left_(*Graph__, pos__, true);
	    	return graph_Wselect_top_(*Graph__, temp, true);
	    case VALUE_C:
        temp = graph_Wselect_left_(*Graph__, pos__, false);
        return graph_Wselect_top_(*Graph__, temp, true);
	    case VALUE_G:
        temp = graph_Wselect_right_(*Graph__, pos__, true);
        return graph_Wselect_top_(*Graph__, temp, false);
	    case VALUE_T:
        temp = graph_Wselect_bottom_(*Graph__, pos__, true);
        temp = graph_Wselect_right_(*Graph__, temp, false);
        return graph_Wselect_top_(*Graph__, temp, false);
	    case VALUE_$:
        temp = graph_Wselect_bottom_(*Graph__, pos__, false);
        temp = graph_Wselect_right_(*Graph__, temp, false);
        return graph_Wselect_top_(*Graph__, temp, false);
			default:
				FATAL("VECTOR_W does not support this query value.");
		}
	}
	FATAL("Unknown vector type");
	return 0;
}

#endif  // _COMPRESSION_STRUCT__
