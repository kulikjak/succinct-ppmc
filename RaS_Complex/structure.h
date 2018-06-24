
#ifndef _RAS_STRUCTURE__
#define _RAS_STRUCTURE__

#include <stdio.h>

#include "RaS_Get.h"
#include "RaS_Insert.h"
#include "RaS_Print.h"
#include "RaS_Rank.h"
#include "RaS_Select.h"
#include "memory.h"

#include "../shared/stack.h"
#include "../shared/utils.h"


typedef struct {
  int32_t root_;
  memory_32b *mem_;
} RAS_Struct;

/*
 * Initialize RAS_Struct object given as argument.
 *
 * @param  RAS__  Reference to RAS_Struct object.
 */
void RAS_Init(RAS_Struct *RAS__) {
  RAS__->mem_ = init_memory();
  RAS__->root_ = new_leaf(RAS__->mem_);

  Leaf_32b *leaf_ref = MEMORY_GET_LEAF(RAS__->mem_, RAS__->root_);

  leaf_ref->p_ = 0;
  leaf_ref->r_ = 0;
}

/*
 * Free all memory associated with RAS object.
 *
 * @param  RAS__  Reference to RAS_Struct object.
 */
void RAS_Free(RAS_Struct *RAS__) {
  RAS__->root_ = -1;
  clean_memory(&(RAS__->mem_));
}

/*
 * Print RAS bit vector.
 *
 * @param  RAS__  RAS_Struct object.
 */
void RAS_Print(RAS_Struct RAS__) {
  RaS_Print_32_(RAS__.mem_, RAS__.root_);
  printf("\n");
}

/*
 * Insert new bit into the RaS bit vector.
 *
 * @param  RAS__  Reference to RAS_Struct object.
 * @param  pos__  Position of newly inserted bit.
 * @param  val__  Value of a new bit.
 */
void RAS_Insert(RAS_Struct *RAS__, uint32_t pos__, bool val__) {
  RaS_Insert_32_(RAS__->mem_, &(RAS__->root_), pos__, val__);
}

/*
 * Delete - this functionality is not implemented.
 *
 * @param  RAS__  Reference to RAS_Struct object.
 * @param  pos__  Position of to be deleted bit.
 */
void RAS_Delete(RAS_Struct *RAS__, uint32_t pos__) {
  UNUSED(RAS__);
  UNUSED(pos__);

  FATAL("Not Implemented");
}

/*
 * Rank RAS structure.
 *
 * @param  RAS__  RAS_Struct object.
 * @param  pos__  Rank position number.
 */
int32_t RAS_Rank(RAS_Struct RAS__, uint32_t pos__) {
  return RaS_Rank_32_(RAS__.mem_, RAS__.root_, pos__);
}

/*
 * Rank zeroes in RAS structure.
 *
 * @param  RAS__  RAS_Struct object.
 * @param  pos__  Rank position number.
 */
int32_t RAS_Rank0(RAS_Struct RAS__, uint32_t pos__) {
  return pos__ - RaS_Rank_32_(RAS__.mem_, RAS__.root_, pos__);
}

/*
 * Select RAS structure.
 *
 * @param  RAS__  RAS_Struct object.
 * @param  pos__  Select position number.
 */
int32_t RAS_Select(RAS_Struct RAS__, uint32_t pos__) {
  return RaS_Select_32_(RAS__.mem_, RAS__.root_, pos__, false);
}

/*
 * Select zeroes in RAS structure.
 *
 * @param  RAS__  RAS_Struct object.
 * @param  pos__  Select position number.
 */
int32_t RAS_Select0(RAS_Struct RAS__, uint32_t pos__) {
  return RaS_Select_32_(RAS__.mem_, RAS__.root_, pos__, true);
}

/*
 * Get bit at position in RAS structure.
 *
 * @param  RAS__  RAS_Struct object.
 * @param  pos__  Query position.
 */
int32_t RAS_Get(RAS_Struct RAS__, uint32_t pos__) {
  return RaS_Get_32_(RAS__.mem_, RAS__.root_, pos__);
}

/*
 * Get number of bits in RAS_Struct object.
 *
 * @param  RAS__  RAS_Struct object.
 */
int32_t RAS_Size(RAS_Struct RAS__) {
  Node_32b *node_ref = MEMORY_GET_ANY(RAS__.mem_, RAS__.root_);
  return node_ref->p_;
}

#endif  // _RAS_STRUCTURE__
