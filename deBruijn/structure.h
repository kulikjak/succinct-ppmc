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

#define GET_VALUE_FROM_SYMBOL(symb__) \
      (((symb__) == 'A') ? 0          \
    : ((symb__) == 'C') ? 1           \
    : ((symb__) == 'G') ? 2           \
    : ((symb__) == 'T') ? 3 : 4)

#define GET_VALUE_FROM_IDX(idx__, dB__) \
      ((idx__ < dB__->F_[0]) ? 4        \
    : (idx__ < dB__->F_[1]) ? 0         \
    : (idx__ < dB__->F_[2]) ? 1         \
    : (idx__ < dB__->F_[3]) ? 2 : 3)



typedef struct {
  int32_t F_[SYMBOL_COUNT];
  RAS_Struct L_;
  WT_Struct W_;

  struct {
    int32_t cnt_;
    int32_t* arr_[CONTEXT_LENGTH + 4];
  } tracker_;
} deBruijn_graph;


void deBruijn_Tracker_push(deBruijn_graph *dB__, int32_t* a__) {
  assert(dB__->tracker_.cnt_ + 1 < CONTEXT_LENGTH + 4);
  dB__->tracker_.arr_[dB__->tracker_.cnt_++] = a__;
}

void deBruijn_Tracker_pop(deBruijn_graph *dB__) {
  assert(dB__->tracker_.cnt_ >= 1);
  dB__->tracker_.cnt_ --;
}

void deBruijn_Tracker_update(deBruijn_graph *dB__, int32_t val__) {
  int32_t i;

  for (i = 0; i < dB__->tracker_.cnt_; i++)
    if (*(dB__->tracker_.arr_[i]) >= val__)
      (*(dB__->tracker_.arr_[i]))++;
}

/*
 * Initialize deBruijn_graph object.
 *
 * @param  dB__  Reference to deBruijn_graph object.
 */
void deBruijn_Init(deBruijn_graph *dB__) {
  memset(dB__->F_, 0, sizeof(dB__->F_));

  RAS_Init(&(dB__->L_));
  WT_Init(&(dB__->W_));

  dB__->tracker_.cnt_ = 0;
}

void deBruijn_Insert_root_node(deBruijn_graph *dB__) {
  // insert first node (root)
  RAS_Insert(&(dB__->L_), 0, 1);
  WT_Insert(&(dB__->W_), 0, '$');

  deBruijn_Tracker_update(dB__, 0);

  dB__->F_[0] = 1;
  dB__->F_[1] = 1;
  dB__->F_[2] = 1;
  dB__->F_[3] = 1;
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
  symbol = GET_VALUE_FROM_IDX(idx__, dB__);

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
 * Get position of given edge symbol in given node.
 *
 * @param  dB__  Reference to deBruijn_graph object.
 * @param  idx__  Edge index (line) in deBruijn graph.
 * @param  symb__  Symbol to find.
 *
 * @return  Index of edge in given node.
 */
int32_t deBruijn_Edge_Check(deBruijn_graph *dB__, int32_t idx__, char symb__) {
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
    return select;
  return -1;
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
  int32_t edge_idx;

  // get index of edge we should follow
  edge_idx = deBruijn_Edge_Check(dB__, idx__, symb__);

  // check if such edge exist
  if (edge_idx == -1)
    return -1;

  // return index of new node
  return deBruijn_Forward_(dB__, edge_idx);
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
    symbol = GET_VALUE_FROM_IDX(idx__, dB__);

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

  for (i = 0; i < 4; i++) {
    printf("%d ", dB__->F_[i]);
  }
  printf("\n");

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

/*
 * Get length of common suffix of given line and line above.
 *
 * This is the simpliest (and slowest) way of determining length of common
 * suffix. It is however the most memory efficient one as it does not store
 * any aditional information.
 *
 * @param  dB__  Reference to deBruijn_graph object.
 * @param  idx__  Edge index (line) in deBruijn graph.
 * @param  limit__ Limit of checking.
 *
 * @return  Length of longest common suffix
 */
int32_t deBruijn_Get_common_suffix_len_(deBruijn_graph *dB__, int32_t idx__, int32_t limit__) {
  int32_t common, idx1, idx2;
  int32_t symbol1, symbol2;

  // there is no common suffix for top most line
  if (!idx__) return 0;

  common = 0;

  idx1 = idx__;
  idx2 = idx__ - 1;

  // limit size of suffix for better performance
  while (common < limit__) {

    // get symbols itself
    symbol1 = GET_VALUE_FROM_IDX(idx1, dB__);
    symbol2 = GET_VALUE_FROM_IDX(idx2, dB__);

    // dollars are not context
    // (we can check only one - next condition will handle the other)
    if (symbol1 == 4)
      break;

    // symbols are not the same
    if (symbol1 != symbol2)
      break;

    // continue backwards
    idx1 = deBruijn_Backward_(dB__, idx1);
    idx2 = deBruijn_Backward_(dB__, idx2);

    common++;

    if (idx1 == -1 || idx2 == -1)
      break;
  }
  return common;
}

/*
 * Shorten current context.
 *
 * This function effectively follows suffix links of PPM tree.
 *
 * @param  dB__  Reference to deBruijn_graph object.
 * @param  idx__  Edge index (line) in deBruijn graph.
 * @param  ctx_len__ Desired new context length.
 *
 * @return  Node with shorter context or -1 if one doesn't exist
 */
int32_t deBruijn_Shorten_context(deBruijn_graph *dB__, int32_t idx__, int32_t ctx_len__) {
  // if this is root node it is not possible to shorten context
  if (idx__ < dB__->F_[0])
    return -1;

  // we cannot shorten context at all
  if (deBruijn_Get_common_suffix_len_(dB__, idx__--, ctx_len__) < ctx_len__)
    return -1;

  // context can be surely shortened beyond this point

  // context of len 0 points surely to root node
  if (ctx_len__ == 0)
    return dB__->F_[0] - 1;

  while (idx__) { 
    // check for length of common suffix
    if (deBruijn_Get_common_suffix_len_(dB__, idx__, ctx_len__) < ctx_len__)
      return idx__;

    // move one line higher
    idx__ --;
  }

  UNREACHABLE
  return 0;
}


int32_t deBruijn_get_context_len(deBruijn_graph *dB__, int32_t idx__) {
  int8_t symbol;
  int32_t count = 0;

  do {
    symbol = GET_VALUE_FROM_IDX(idx__, dB__);
    if (symbol == 4)
      break;

    count ++;
    idx__ = deBruijn_Backward_(dB__, idx__);
  } while (idx__ != -1);

  return count;
}


/*
 * Add symbol to given context. 
 *
 * TODO desc
 * TODO function
 *
 * @param  dB__  Reference to deBruijn_graph object.
 * @param  idx__  Node index (line) in deBruijn graph.
 * @param  symb__ Additional symbol.
 */
void deBruijn_Add_context_symbol(deBruijn_graph *dB__, int32_t idx__, char symb__) {

  int32_t edge_pos, i;

  // check if shorter context already added this symbol
  // if not, symbol will be recursively added from top to this node

  int32_t ctx_len = deBruijn_get_context_len(dB__, idx__);
  if (ctx_len) {
    int32_t prev_node = deBruijn_Shorten_context(dB__, idx__, ctx_len - 1);

    // check if prev node has what it needs
    if (deBruijn_Edge_Check(dB__, prev_node, symb__) == -1) {
      deBruijn_Tracker_push(dB__, &idx__);
      deBruijn_Add_context_symbol(dB__, prev_node, symb__);
      deBruijn_Tracker_pop(dB__);

      deBruijn_Print(dB__, true);

    }
  }

  // check what symbol is in W
  if (WT_Get(&(dB__->W_), idx__) == '$') {
    // current W symbol is $ - we can simply change it to new one


    // change symbol to new one
    WT_Delete(&(dB__->W_), idx__);
    WT_Insert(&(dB__->W_), idx__, symb__);

  } else {
    edge_pos = deBruijn_Edge_Check(dB__, idx__, symb__);

    // transition already exist at this node - nothing more do
    if (edge_pos != -1) {
      return;
    }

    // insert new edge into this node
    RAS_Insert(&(dB__->L_), idx__, 0);
    WT_Insert(&(dB__->W_), idx__, symb__);

    deBruijn_Tracker_update(dB__, idx__);

    // update F array according to added node
    for (i = 0; i < SYMBOL_COUNT; i++) {
      if (dB__->F_[i] > idx__)
        dB__->F_[i]++;
    }
  }

  // find same transition symbol above this line
  int32_t rank = WT_Rank(&(dB__->W_), idx__, symb__);

  int32_t x, y;

  if (!rank) {
    // there is no symbol above this
    int32_t sn = GET_VALUE_FROM_SYMBOL(symb__);
    x = dB__->F_[sn];

    WT_Insert(&(dB__->W_), x, '$');
    RAS_Insert(&(dB__->L_), x, 1);

    deBruijn_Tracker_update(dB__, x);

    for (i = sn + 1; i < SYMBOL_COUNT; i++) {
      dB__->F_[i]++;
    }

    return;

  } else {
    y = WT_Select(&(dB__->W_), rank, symb__);
    x = deBruijn_Forward_(dB__, y-1) + 1;
  }

  WT_Insert(&(dB__->W_), x, '$');
  RAS_Insert(&(dB__->L_), x, 1);

  deBruijn_Tracker_update(dB__, x);

  for (i = 0; i < SYMBOL_COUNT; i++) {
    if (dB__->F_[i] >= x)
      dB__->F_[i]++;
  }
}

// pridavani symbolu:

// jsem v uzlu, za ktery chci retezit
// check jestli kratsi context ma tento symbol pridany - pokud ne, jdu nejprve do nej

// v momentalnim miste zjistim, jake je W
// pokud je $, mohu ho proste zmenit na novy symbol

// pokud neni $, musim najit v tom samem uzlu, jestli neni uz odchozi chrana na symbol
// pokud je, neni co pridavat
// pokud neni, musim najit kam to vlozit ??
// vlozim do range na misto lexikograficky ?? a upravim F a L!

// pokud mam v danem uzlu spravnou hranu, hledam nad ni nejblizsi stejny symbol jako vkladam
// pokud existuje, pokracuji z teto hrany vpred (x)

// pokud neexistuje, x = F[symbol] (existuje vzdy pokud nejsem uplne nahore mimo kontext)

// na dane misto vlozim novy symbol $ a L nastavime na 1 (urcite je posledni)
// upravime F!



#endif  // _DEBRUIJN_GRAPH__
