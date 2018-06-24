#ifndef _DEBRUIJN_GRAPH__
#define _DEBRUIJN_GRAPH__

#include <stdlib.h>
#include <stdio.h>

#include "../shared/utils.h"

#include "../RaS_Complex/structure.h"
#include "../Wavelet_tree/structure_ext.h"

// Length of the PPMC context (not used yet)
#define CONTEXT_LENGTH 4

// Define to hide leading $ signs in Label print and function
#define OMIT_EXCESSIVE_DOLLAR_SIGNS_

// Cann't be simply changed to work with more symbols - it works as a constant
#define SYMBOL_COUNT 4

#define GET_SYMBOL_FROM_VALUE(symb__) \
      (((symb__) == 0) ? 'A'          \
    : ((symb__) == 1) ? 'C'           \
    : ((symb__) == 2) ? 'G'           \
    : ((symb__) == 3) ? 'T' : '$')

typedef struct {
  int32_t F_[SYMBOL_COUNT];
  RAS_Struct L_;
  WT_Struct W_;
} deBruijn_graph;

/*
 * Initialize deBruijn_graph object.
 *
 * @param  dB__  Reference to deBruijn_graph object.
 */
void deBruijn_Init(deBruijn_graph *dB__) {
  memset(dB__->F_, 0, sizeof(dB__->F_));

  RAS_Init(&(dB__->L_));
  WT_Init(&(dB__->W_));
}

/*
 * Free all memory associated with deBruijn graph object.
 *
 * @param  dB__  Reference to deBruijn_graph object.
 */
void deBruijn_Free(deBruijn_graph *dB__) {
  RAS_Free(&(dB__->L_));
  WT_Free(&(dB__->W_));
}

/*
 * Move to next node pointed to by given edge (line) index.
 *
 * @param  dB__  Reference to deBruijn_graph object.
 * @param  idx__  Edge index (line) in deBruijn graph.
 *
 * @return  Index of last edge of the next node pointed to by given edge or -1
 * if there is no next node.
 */
int32_t deBruijn_Forward_(deBruijn_graph *dB__, int32_t idx__) {
  int8_t symbol;
  int32_t rank, spos;

  // find edge label of given edge (outgoing edge symbol)
  symbol = WT_VGet(&(dB__->W_), idx__);
  assert(symbol != -1);

  // if edge label is dollar, there is nowhere to go
  if (symbol == 4) return -1;

  // calculate rank of edge label in the W array
  rank = WT_VRank(&(dB__->W_), idx__ + 1, symbol);

  // get starting position of edge label
  spos = (symbol != 4) ? dB__->F_[symbol] : 0;

  // get index of the last edge of the node pointed to by given edge
  return RAS_Select(dB__->L_, RAS_Rank(dB__->L_, spos) + rank) - 1;
}

/*
 * Move to parent node of given one.
 *
 * @param  dB__  Reference to deBruijn_graph object.
 * @param  idx__  Edge index (line) in deBruijn graph.
 *
 * @return  Index of parent node (its edge pointing to given one) or -1 if there
 * is no parent node.
 */
int32_t deBruijn_Backward_(deBruijn_graph *dB__, int32_t idx__) {
  int8_t symbol;
  int32_t base, temp;

  assert(idx__ < RAS_Size(dB__->L_) && idx__ >= 0);

  // find last symbol of this node
  symbol = (idx__ < dB__->F_[0]) ? 4
         : (idx__ < dB__->F_[1]) ? 0
         : (idx__ < dB__->F_[2]) ? 1
         : (idx__ < dB__->F_[3]) ? 2 : 3;

  // if last symbol is dollar, there is nowhere to go
  if (symbol == 4) return -1;

  // rank to current base
  base = RAS_Rank(dB__->L_, dB__->F_[symbol]);

  // rank to given line (including it)
  temp = RAS_Rank(dB__->L_, idx__ + 1);

  // if given line is not last edge of the node, add that node
  temp += RAS_Get(dB__->L_, idx__) ? 0 : 1;

  // get index of the edge leading to given node
  return WT_VSelect(&(dB__->W_), temp - base, symbol) - 1;
}

/*
 * Get number of outgoing edges from given node.
 *
 * @param  dB__  Reference to deBruijn_graph object.
 * @param  idx__  Edge index (line) in deBruijn graph.
 *
 * @return  A number of outgoing edges.
 */
int32_t deBruijn_Outdegree(deBruijn_graph *dB__, int32_t idx__) {
  int32_t node_id;

  assert(idx__ < RAS_Size(dB__->L_) && idx__ >= 0);

  // get node index
  node_id = RAS_Rank(dB__->L_, idx__);

  // calculate outdegree itself
  return RAS_Select(dB__->L_, node_id + 1) - RAS_Select(dB__->L_, node_id);
}

/*
 * From given node follow edge labeled by given symbol.
 *
 * @param  dB__  Reference to deBruijn_graph object.
 * @param  idx__  Edge index (line) in deBruijn graph.
 * @param  symb__  Symbol to follow.
 *
 * @return  Index of new node.
 */
int32_t deBruijn_Outgoing(deBruijn_graph *dB__, int32_t idx__, char symb__) {
  int32_t node_id, last_pos, range, select;

  // get last edge index for this node
  node_id = RAS_Rank(dB__->L_, idx__);
  last_pos = RAS_Select(dB__->L_, node_id + 1);

  // find range (number of edges leaving current node)
  range = last_pos - RAS_Select(dB__->L_, node_id);

  // get position of last 'symbol' up to this node
  select = WT_Select(&(dB__->W_), WT_Rank(&(dB__->W_), last_pos, symb__), symb__) - 1;

  // check if this position is in the range (transition exist)
  if (last_pos - range <= select)
    return deBruijn_Forward_(dB__, select);
  return -1;
}

/*
 * Get label of a node corresponding to given line.
 *
 * Output buffer MUST have a size of at least CONTEXT_LENGTH otherwise buffer
 * overflow can occur. Output buffer doesn't have terminating null byte after
 * the last character and therefore it cannot be automatically printed out as a
 * string.
 *
 * @param  dB__  Reference to deBruijn_graph object.
 * @param  idx__  Edge index (line) in deBruijn graph.
 * @param  buffer__  [out] Output buffer with label in symbols.
 */
void deBruijn_Label(deBruijn_graph *dB__, int32_t idx__, char* buffer__) {
  int8_t symbol;
  int32_t pos;

#ifdef OMIT_EXCESSIVE_DOLLAR_SIGNS_
  memset(buffer__, ' ', CONTEXT_LENGTH);
#else
  memset(buffer__, '$', CONTEXT_LENGTH);
#endif

  pos = CONTEXT_LENGTH - 1;
  do {
    symbol = (idx__ < dB__->F_[0]) ? 4
           : (idx__ < dB__->F_[1]) ? 0
           : (idx__ < dB__->F_[2]) ? 1
           : (idx__ < dB__->F_[3]) ? 2 : 3;

    buffer__[pos--] = GET_SYMBOL_FROM_VALUE(symbol);
    idx__ = deBruijn_Backward_(dB__, idx__);
  } while (idx__ != -1);
}

/*
 * Get number of edges that point to current node.
 *
 * This function is not universal and works only for trees. Therefore it returns
 * 0 for root and 1 for every other node (each node except root in a tree has
 * always one predecessor).
 *
 * @param  dB__  Reference to deBruijn_graph object.
 * @param  idx__  Edge index (line) in deBruijn graph.
 *
 * @return  A number of predecessor nodes.
 */
int32_t deBruijn_Indegree(deBruijn_graph *dB__, int32_t idx__) {
  if (idx__ < dB__->F_[0]) return 0;
  return 1;
}

/*
 * Get node starting with given symbol that has an edge to given node.
 *
 * As this is not important for compression and it's not super straightforward,
 * it is not implemented.
 */
int32_t deBruijn_Incomming(deBruijn_graph *dB__, int32_t idx__, char symb__) {
  UNUSED(dB__);
  UNUSED(idx__);
  UNUSED(symb__);

  FATAL("Not Implemented");
  return 0;
}

/*
 * Print whole deBruijn graph struct.
 *
 * Printing graph with labels is much slower because they are not explicitly
 * stored and for each edge, they must be calculated by going back all the way
 * to the root.
 *
 * @param  dB__  Reference to deBruijn_graph object.
 * @param  labels__  Node labels should be printed as well.
 */
void deBruijn_Print(deBruijn_graph *dB__, bool labels__) {
  char label[CONTEXT_LENGTH + 1];
  int32_t i, j, size;
  int32_t nextPos = 0;
  int32_t next = 0;

  // print header for main structure
  if (labels__) {
    printf("     F  L  Label  ");
    for (i = 0; i < CONTEXT_LENGTH - 5; i++)
      printf(" ");
    printf("W\n--------------------");
    for (i = 0; i < CONTEXT_LENGTH - 5; i++)
      printf("-");
    printf("\n");
  } else {
    printf("     F  L  W\n-------------\n");
  }

  size = RAS_Size(dB__->L_);
  for (i = 0; i < size; i++) {
    printf("%3d: ", i);

    // handle base positions for all symbols
    if (i == nextPos) {
      if (i == dB__->F_[3]) {
        printf("T  ");
      } else if (i == dB__->F_[2]) {
        printf("G  ");
      } else if (i == dB__->F_[1]) {
        printf("C  ");
      } else if (i == dB__->F_[0]) {
        printf("A  ");
      } else {
        printf("$  ");
      }

      // find next position for another symbol
      while (dB__->F_[next] == i && next <= SYMBOL_COUNT) next++;
      nextPos = (next <= SYMBOL_COUNT) ? dB__->F_[next] : -1;
    } else {
      printf("   ");
    }

    printf("%d  ", RAS_Get(dB__->L_, (uint32_t)i));

    // handle finding of all the labels
    if (labels__) {
      label[CONTEXT_LENGTH] = 0;
      deBruijn_Label(dB__, i, label);

      printf("%s  ", label);
      for (j = 0; j < 5 - CONTEXT_LENGTH; j++) {
        printf(" ");
      }
    }
    printf("%c\n", WT_Get(&(dB__->W_), (uint32_t)i));
  }
}

/*
 * Insert test data into the deBruijn graph.
 *
 * This function is for testing purposes only. It simply fills all the
 * structures with given data. There is no error checking (except for checks of
 * underlying structures) and if used incorrectly, all other functions can have
 * wrong results. Also it does not delete any previously inserted values.
 *
 * @param  dB__  Reference to deBruijn_graph object.
 * @param  L__  L array (last edge of corresponding node).
 * @param  W__  W array (label of given outgoing edge).
 * @param  F__  F array (base positions of symbols)
 * @param  size__  Size of arrays L and W.
 */
void deBruijn_Insert_test_data_(deBruijn_graph *dB__, int8_t *L__, char *W__, int32_t F__[SYMBOL_COUNT], int32_t size__) {
  int32_t i;

  for (i = 0; i < size__; i++) {
    RAS_Insert(&(dB__->L_), i, L__[i]);
    WT_Insert(&(dB__->W_), i, W__[i]);
  }

  memcpy(dB__->F_, F__, sizeof(dB__->F_));
}


#endif  // _DEBRUIJN_GRAPH__
