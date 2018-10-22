#ifndef _DEBRUIJN_GRAPH__
#define _DEBRUIJN_GRAPH__

#include <stdlib.h>
#include <stdio.h>

#include "structure.h"
#include "utils.h"

// Length of the PPMC context (not used yet)
#define CONTEXT_LENGTH 4

// Define to hide leading $ signs in Label print and function
#define OMIT_EXCESSIVE_DOLLAR_SIGNS_

#define GET_SYMBOL_FROM_VALUE(symb__) \
      (((symb__) == VALUE_A) ? 'A'          \
    : ((symb__) == VALUE_C) ? 'C'           \
    : ((symb__) == VALUE_G) ? 'G'           \
    : ((symb__) == VALUE_T) ? 'T' : '$')

#define GET_VALUE_FROM_SYMBOL(symb__)        \
      (((symb__) == 'A') ? VALUE_A           \
    : ((symb__) == 'C') ? VALUE_C            \
    : ((symb__) == 'G') ? VALUE_G            \
    : ((symb__) == 'T') ? VALUE_T : VALUE_$)

#define GET_VALUE_FROM_IDX(idx__, dB__)          \
      ((idx__ < dB__->F_[0]) ? VALUE_$           \
    : (idx__ < dB__->F_[1]) ? VALUE_A            \
    : (idx__ < dB__->F_[2]) ? VALUE_C            \
    : (idx__ < dB__->F_[3]) ? VALUE_G : VALUE_T)

typedef struct {
  int32_t F_[SYMBOL_COUNT];
  Graph_Struct Graph_;

  int32_t depth;

  struct {
    int32_t cnt_;
    int32_t *arr_[CONTEXT_LENGTH + 4];
  } tracker_;
} deBruijn_graph;

/*
 * Initialize deBruijn_graph object.
 *
 * @param  dB__  Reference to deBruijn_graph object.
 */
void deBruijn_Init(deBruijn_graph *dB__);

/*
 * Free all memory associated with deBruijn graph object.
 *
 * @param  dB__  Reference to deBruijn_graph object.
 */
void deBruijn_Free(deBruijn_graph *dB__);

/*
 * Get number of outgoing edges from given node.
 *
 * @param  dB__  Reference to deBruijn_graph object.
 * @param  idx__  Edge index (line) in deBruijn graph.
 *
 * @return  A number of outgoing edges.
 */
int32_t deBruijn_Outdegree(deBruijn_graph *dB__, int32_t idx__);

/*
 * Get position of given edge symbol in given node.
 *
 * @param  dB__  Reference to deBruijn_graph object.
 * @param  idx__  Edge index (line) in deBruijn graph.
 * @param  gval__  Symbol (Graph_value) to find.
 *
 * @return  Index of edge in given node.
 */
int32_t deBruijn_Find_Edge(deBruijn_graph *dB__, int32_t idx__, Graph_value gval__);

/*
 * From given node follow edge labeled by given symbol.
 *
 * @param  dB__  Reference to deBruijn_graph object.
 * @param  idx__  Edge index (line) in deBruijn graph.
 * @param  gval__  Symbol (Graph_value) to follow.
 *
 * @return  Index of new node.
 */
int32_t deBruijn_Outgoing(deBruijn_graph *dB__, int32_t idx__, Graph_value gval__);

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
void deBruijn_Label(deBruijn_graph *dB__, int32_t idx__, char *buffer__);

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
int32_t deBruijn_Indegree(deBruijn_graph *dB__, int32_t idx__);

/*
 * Get node starting with given symbol that has an edge to given node.
 *
 * As this is not important for compression and it's not super straightforward,
 * it is not implemented.
 */
int32_t deBruijn_Incomming(deBruijn_graph *dB__, int32_t idx__, Graph_value gval__);

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
void deBruijn_Print(deBruijn_graph *dB__, bool labels__);

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
int32_t deBruijn_Shorten_context(deBruijn_graph *dB__, int32_t idx__,
                                 int32_t ctx_len__);

/*
 * Get cumulative frequency from node pointed to by given index.
 *
 * @param  dB__  Reference to deBruijn_graph object.
 * @param  idx__  Edge index (line) in deBruijn graph.
 * @param  gval__  Symbol (Graph_value) to follow.
 * @param  freq__  [Out] Frequency count structure.
 */
void deBruijn_Get_cumulative_frequency(deBruijn_graph *dB__, uint32_t idx__,
                                       Graph_value gval__, cfreq *freq__);

/*
 * Move to next node pointed to by given edge (line) index.
 *
 * @param  dB__  Reference to deBruijn_graph object.
 * @param  idx__  Edge index (line) in deBruijn graph.
 *
 * @return  Index of last edge of the next node pointed to by given edge or -1
 * if there is no next node.
 */
int32_t deBruijn_Forward_(deBruijn_graph *dB__, int32_t idx__);

/*
 * Move to parent node of given one.
 *
 * @param  dB__  Reference to deBruijn_graph object.
 * @param  idx__  Edge index (line) in deBruijn graph.
 *
 * @return  Index of parent node (its edge pointing to given one) or -1 if there
 * is no parent node.
 */
int32_t deBruijn_Backward_(deBruijn_graph *dB__, int32_t idx__);

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
int32_t deBruijn_Get_common_suffix_len_(deBruijn_graph *dB__, int32_t idx__, int32_t limit__);

/*
 * Initialize structure with given test data.
 *
 * This function is for testing purposes only. It simply fills all the
 * structures with given data. There is no error checking (except for checks of
 * underlying structures) and if used incorrectly, all other functions can have
 * wrong results.
 *
 * Structure should not be initialized whne this is called!
 *
 * @param  dB__  Reference to deBruijn_graph object.
 * @param  L__  L array (last edge of corresponding node).
 * @param  W__  W array (label of given outgoing edge).
 * @param  P__  P array (frequencies of each symbol in context).
 * @param  F__  F array (base positions of symbols)
 * @param  size__  Size of arrays L and W.
 */
void deBruijn_Insert_test_data(deBruijn_graph *dB__, const Graph_value *L__, const Graph_value *W__,
                               const int32_t *P__, const int32_t F__[SYMBOL_COUNT],
                               const int32_t size__);



int32_t deBruijn_get_context_len_(deBruijn_graph *dB__, int32_t idx__);


// make hidden function visible to unit testing framework
#ifdef _UNITY

#endif  // _UNITY

#endif  // _DEBRUIJN_GRAPH__
