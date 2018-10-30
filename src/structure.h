#ifndef _COMPRESSION_STRUCT__
#define _COMPRESSION_STRUCT__

#include "memory.h"
#include "stack.h"
#include "cache.h"
#include "utils.h"
#include "defines.h"

#define STRUCTURE_VERBOSE(func) \
  if (STRUCTURE_VERBOSE_) {     \
    func                        \
  }

#define SYMBOL_COUNT 4

#define GET_MASK_FROM_VALUE(symb__)  \
      (((symb__) == VALUE_A) ? 0     \
    : ((symb__) == VALUE_C) ? 2      \
    : ((symb__) == VALUE_G) ? 4      \
    : ((symb__) == VALUE_T) ? 6 : 7)

#define GET_VALUE_FROM_MASK(mask__)        \
      (((mask__) == 0) ? VALUE_A           \
    : ((mask__) == 2) ? VALUE_C            \
    : ((mask__) == 4) ? VALUE_G            \
    : ((mask__) == 6) ? VALUE_T : VALUE_$)

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

#define RANK(vector) __builtin_popcount((vector))

#define NODE_OPERATION_2(r1, r2, op) \
  {                                  \
    r1->p_ op r2->p_;                \
    r1->rL_ op r2->rL_;              \
    r1->rW_ op r2->rW_;              \
    r1->rWl_ op r2->rWl_;            \
    r1->rWh_ op r2->rWh_;            \
    r1->rWs_ op r2->rWs_;            \
  }

#define NODE_OPERATION_3(r1, r2, r3, op) \
  {                                      \
    r1->p_ = r2->p_ op r3->p_;           \
    r1->rL_ = r2->rL_ op r3->rL_;        \
    r1->rW_ = r2->rW_ op r3->rW_;        \
    r1->rWl_ = r2->rWl_ op r3->rWl_;     \
    r1->rWh_ = r2->rWh_ op r3->rWh_;     \
    r1->rWs_ = r2->rWs_ op r3->rWs_;     \
  }

typedef enum { VECTOR_L, VECTOR_W } Graph_vector;

typedef enum {
  VALUE_0 = 0,
  VALUE_1 = 1,

  VALUE_A = 0,
  VALUE_C = 1,
  VALUE_G = 2,
  VALUE_T = 3,
  VALUE_$ = 4,
  VALUE_ESC = 4
} Graph_value;

typedef struct {
  mem_ptr root_;
  MemObj mem_;
} Graph_Struct;

typedef struct {
  Graph_value L_;
  Graph_value W_;
  uint32_t P_;
} Graph_Line;

typedef struct {
  uint32_t symbol_[SYMBOL_COUNT+1];
  uint32_t total_;
} cfreq;

#define GraphRef Graph_Struct*
#define GLineRef Graph_Line*

/* Defines for rank and select macro expansion */
#define EXPAND_L 0
#define EXPAND_WTOP 1
#define EXPAND_WLEFT 2
#define EXPAND_WRIGHT 3
#define EXPAND_WBOTTOM 4

/*
 * Macro that contains whole tree search loop.
 *
 * @param  Graph__  [In] Reference to graph structure.
 * @param  pos__  [In/Out] Query position -> position within the leaf.
 * @param  current  [Out] mem_ptr set to correct leaf.
 * @param  leaf_ref  [Out] Actual LeafRef set to correct leaf.
 * @param  with_stack  If stack should be used (filled) during the query
 */
#define GET_TARGET_LEAF(Graph__, pos__, current, leaf_ref, with_stack) { \
  if (with_stack) STACK_CLEAN();                                         \
  mem_ptr Xtemp;                                                         \
  current = Graph__->root_;                                              \
  uint32_t backup = pos__;                                               \
  if (with_stack || !lookup_cache(&pos__, &leaf_ref)) {                  \
    NodeRef Xnode_ref = MEMORY_GET_ANY(Graph__->mem_, current);          \
    assert(pos__ < Xnode_ref->p_);                                       \
                                                                         \
    /* traverse the tree and enter correct leaf */                       \
    while (!IS_LEAF(current)) {                                          \
      if (with_stack) STACK_PUSH(current);                               \
      Xnode_ref = MEMORY_GET_NODE(Graph__->mem_, current);               \
                                                                         \
      /* get p_ counter of left child and act accordingly */             \
      Xtemp = MEMORY_GET_ANY(Graph__->mem_, Xnode_ref->left_)->p_;       \
      if ((uint32_t)Xtemp > pos__) {                                     \
        current = Xnode_ref->left_;                                      \
      } else {                                                           \
        pos__ -= Xtemp;                                                  \
        current = Xnode_ref->right_;                                     \
      }                                                                  \
    }                                                                    \
    leaf_ref = MEMORY_GET_LEAF(Graph__->mem_, current);                  \
    add_to_cache(backup, pos__, leaf_ref);                               \
  }                                                                      \
}

#define WITH_STACK true
#define WITHOUT_STACK false

/*
 * Initialize Graph_Struct object given as argument.
 *
 * @param  Graph__  Reference to Graph_Struct object.
 */
void Graph_Init(GraphRef Graph__);

/*
 * Free all memory associated with MS object.
 *
 * @param  Graph__  Reference to Graph_Struct object.
 */
void Graph_Free(GraphRef Graph__);

/*
 * Insert whole one line into the given Graph_Struct object.
 *
 * @param  Graph__  Reference to Graph_Struct object.
 * @param  pos__  Index of newly inserted line.
 * @param  line__  Reference to Graph_Line object.
 */
void GLine_Insert(GraphRef Graph__, uint32_t pos__, GLineRef line__);

/*
 * Fill Graph_Line struct with given information.
 *
 * @param  line__  [Out] Reference to Graph_Line object.
 * @param  L__  Corresponding line variable.
 * @param  W__  Corresponding line variable.
 * @param  P__  Corresponding line variable.
 */
void GLine_Fill(GLineRef line__, Graph_value L__, Graph_value W__, uint32_t P__);

/*
 * Explode Graph_Line struct variables into separate variables.
 *
 * @param  line__  Reference to Graph_Line object.
 * @param  L__  [Out] Reference to corresponding line variable.
 * @param  W__  [Out] Reference to corresponding line variable.
 * @param  P__  [Out] Reference to corresponding line variable.
 */
void GLine_Explode(GLineRef line__, Graph_value* L__, Graph_value* W__, uint32_t* P__);

/*
 * Get one line in given Graph_Struct object.
 *
 * Corresponding line is returned in the form of Graph_Line object.
 *
 * @param  Graph__  Reference to Graph_Struct object.
 * @param  pos__  Index of requested line.
 * @param  line__  [Out] Reference to Graph_Line object.
 */
void GLine_Get(GraphRef Graph__, uint32_t pos__, GLineRef line__);

/*
 * Get number of elements saved in the Graph_Struct.
 *
 * @param  Graph__  Reference to Graph_Struct object.
 */
int32_t Graph_Size(GraphRef Graph__);

/*
 * Print whole graph structure.
 *
 * @param  Graph__  Reference to Graph_Struct object.
 */
void Graph_Print(GraphRef Graph__);

/*
 * Rank Graph_struct.
 *
 * VECTOR_L operations are optimized for values known during the compilation
 * (eliminating unnecessary function call) because code only uses those.
 *
 * @param  Graph__  Reference to Graph_Struct object.
 * @param  pos__  Query position.
 * @param  type__  Sub structure that should be queried [enum: Graph_vector].
 * @param  val__  Query value [enum: Graph_value].
 */
#define Graph_Rank(Graph__, pos__, type__, val__) \
  ((type__ == VECTOR_L)                           \
    ? (val__ == VALUE_0)                          \
      ? (pos__ - graph_Lrank_(*Graph__, pos__))   \
      : (graph_Lrank_(*Graph__, pos__))           \
    : Graph_Rank_W(Graph__, pos__, val__))

int32_t graph_Lrank_(Graph_Struct Graph__, uint32_t pos__);

/*
 * Rank L vector of given Graph_struct.
 *
 * Consider using Graph_Rank macro instead. Macro completely eliminates
 * this function call if val__ is known during the compilation.
 *
 * @param  Graph__  Reference to Graph_Struct object.
 * @param  pos__  Query position.
 * @param  val__  Query value [enum: Graph_value].
 */
int32_t Graph_Rank_L(GraphRef Graph__, uint32_t pos__, Graph_value val__);

/*
 * Rank W vector of given Graph_struct.
 *
 * Consider using Graph_Rank macro instead.
 *
 * @param  Graph__  Reference to Graph_Struct object.
 * @param  pos__  Query position.
 * @param  val__  Query value [enum: Graph_value].
 */
int32_t Graph_Rank_W(GraphRef Graph__, uint32_t pos__, Graph_value val__);

/*
 * Select Graph_struct.
 *
 * VECTOR_L operations are optimized for values known during the compilation
 * (eliminating unnecessary function call) because code only uses those.
 *
 * @param  Graph__  Reference to Graph_Struct object.
 * @param  pos__  Query position.
 * @param  type__  Sub structure that should be queried [enum: Graph_vector].
 * @param  val__  Query value [enum: Graph_value].
 */
#define Graph_Select(Graph__, pos__, type__, val__) \
  ((type__ == VECTOR_L)                             \
    ? (val__ == VALUE_0)                            \
      ? (graph_Lselect_(*Graph__, pos__, true))     \
      : (graph_Lselect_(*Graph__, pos__, false))    \
    : Graph_Select_W(Graph__, pos__, val__))

int32_t graph_Lselect_(Graph_Struct Graph__, uint32_t num__, bool zero__);

/*
 * Select L vector of given Graph_struct.
 *
 * Consider using Graph_Select macro instead. Macro completely eliminates
 * this function call if val__ is known during the compilation.
 *
 * @param  Graph__  Reference to Graph_Struct object.
 * @param  pos__  Query position.
 * @param  val__  Query value [enum: Graph_value].
 */
int32_t Graph_Select_L(GraphRef Graph__, uint32_t pos__, Graph_value val__);

/*
 * Select W vector of given Graph_struct.
 *
 * Consider using Graph_Select macro instead.
 *
 * @param  Graph__  Reference to Graph_Struct object.
 * @param  pos__  Query position.
 * @param  val__  Query value [enum: Graph_value].
 */
int32_t Graph_Select_W(GraphRef Graph__, uint32_t pos__, Graph_value val__);

/*
 * Update value in W vector of Graph_struct.
 *
 * @param  Graph__  Reference to Graph_Struct object.
 * @param  pos__  Update position.
 * @param  val__  New symbol [enum: Graph_value].
 */
void Graph_Change_symbol(GraphRef Graph__, uint32_t pos__, Graph_value val__);

/*
 * Increase frequency in P vector of Graph_struct.
 *
 * @param  Graph__  Reference to Graph_Struct object.
 * @param  pos__  Update position.
 */
void Graph_Increase_frequency(GraphRef Graph__, uint32_t pos__);

#ifdef ENABLE_CLEVER_NODE_SPLIT

/*
 * Get symbol frequencies from node pointed to by given index.
 *
 * @param  Graph__  Reference to Graph_Struct object.
 * @param  pos__  Edge index (line) in deBruijn graph.
 * @param  freq__  [Out] Frequency count structure.
 */
void Graph_Get_symbol_frequency(GraphRef Graph__, uint32_t pos__, cfreq* freq__);

/*
 * Get position of given edge symbol in given node.
 *
 * @param  Graph__  Reference to Graph_Struct object.
 * @param  pos__  Edge index (line) in deBruijn graph.
 * @param  gval__  Symbol (Graph_value) to find.
 *
 * @return  Index of edge in given node.
 */
int32_t Graph_Find_Edge(GraphRef Graph__, uint32_t pos__, Graph_value val__);

#endif

#endif  // _COMPRESSION_STRUCT__
