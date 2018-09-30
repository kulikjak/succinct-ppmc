#ifndef _COMPRESSION_STRUCT__
#define _COMPRESSION_STRUCT__

#include "memory.h"
#include "stack.h"
#include "utils.h"

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

/* Perform standard rank on more significant half of a 32 bit vector. */
#define RANK16(vector) __builtin_popcount((vector)&0xFFFF0000)

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

typedef enum { VECTOR_L, VECTOR_W } Graph_vector;

typedef enum {
  VALUE_0 = 0,
  VALUE_1 = 1,
  VALUE_A,
  VALUE_C,
  VALUE_G,
  VALUE_T,
  VALUE_$
} Graph_value;

int32_t get_mask_from_char_(char symb__);
char get_char_from_mask_(int32_t mask__);

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
void GLine_Fill(GLineRef line__, bool L__, char W__, uint32_t P__);

/*
 * Explode Graph_Line struct variables into separate variables.
 *
 * @param  line__  Reference to Graph_Line object.
 * @param  L__  [Out] Reference to corresponding line variable.
 * @param  W__  [Out] Reference to corresponding line variable.
 * @param  P__  [Out] Reference to corresponding line variable.
 */
void GLine_Explode(GLineRef line__, bool* L__, char* W__, uint32_t* P__);

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
 * @param  Graph__  Reference to graph structure object.
 * @param  pos__  Query position.
 * @param  type__  Sub structure that should be queried [enum: [Graph_vector]].
 * @param  val__  Query value [enum: Graph_value].
 */
int32_t Graph_Rank(GraphRef Graph__, uint32_t pos__, Graph_vector type__,
                   Graph_value val__);

/*
 * Select Graph_struct.
 *
 * @param  Graph__  Reference to WT_Struct object.
 * @param  pos__  Query position.
 * @param  type__  Sub structure that should be queried [enum: [Graph_vector]].
 * @param  val__  Query value [enum: Graph_value].
 */
int32_t Graph_Select(GraphRef Graph__, uint32_t pos__, Graph_vector type__,
                     Graph_value val__);

#endif  // _COMPRESSION_STRUCT__
