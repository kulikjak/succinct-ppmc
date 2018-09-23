#ifndef _INDEL_SIMPLE_STRUCTURE__
#define _INDEL_SIMPLE_STRUCTURE__

/*
 * Very simple version of Integer array with insertions and deletions.
 *
 * BEWARE: This version of integer Indel structure is the worst possible (well,
 * except for stupid ones) and should not be used for anything else than rapid
 * prototyping.
 */

#include <stdio.h>
#include <string.h>

#include "../shared/utils.h"

#define INDEL_INITIAL_SIZE 1024

typedef struct {
  int32_t size_;
  int32_t last_;
  int32_t *mem_;
} Indel_Struct;

/*
 * Initialize Indel_Struct object given as argument.
 *
 * @param  IA__  Reference to Indel_Struct object.
 */
void Indel_Init(Indel_Struct *IA__) {
  IA__->size_ = INDEL_INITIAL_SIZE;
  IA__->last_ = 0;
  IA__->mem_ = (int32_t *)malloc(IA__->size_ * sizeof(int32_t));
}

/*
 * Free all memory associated with IA object.
 *
 * @param  IA__  Reference to Indel_Struct object.
 */
void Indel_Free(Indel_Struct *IA__) {
  free(IA__->mem_);

  IA__->size_ = 0;
  IA__->last_ = 0;
  IA__->mem_ = NULL;
}

/*
 * Doubles size of memory array of given IA object.
 *
 * @param  IA__  Reference to Indel_Struct object.
 */
void Indel_Resize_(Indel_Struct *IA__) {
  assert(IA__->mem_ != NULL);
  assert(IA__->size_ != 0);

  IA__->size_ *= 2;
  IA__->mem_ = (int32_t *)realloc(IA__->mem_, IA__->size_ * sizeof(int32_t));
}

/*
 * Insert new element into the IA object.
 *
 * @param  IA__  Reference to Indel_Struct object.
 * @param  pos__  Position of newly inserted element.
 * @param  numb__  Value of the new integer.
 */
void Indel_Insert(Indel_Struct *IA__, int32_t pos__, int32_t numb__) {
  assert(pos__ >= 0 && pos__ <= IA__->last_);

  // resize if there is no more space
  if (IA__->size_ <= IA__->last_) Indel_Resize_(IA__);

  // move all later integers to next position and insert new one
  if (pos__ < IA__->last_) {
    memmove(&(IA__->mem_[pos__ + 1]), &(IA__->mem_[pos__]),
            (IA__->last_ - pos__) * sizeof(int32_t));
  }

  IA__->mem_[pos__] = numb__;
  IA__->last_++;
}

/*
 * Delete element from the IA object.
 *
 * This is not implemented as it's not important for our usage.
 *
 * @param  IA__  Reference to Indel_Struct object.
 * @param  pos__  Position of to be deleted element.
 */
void Indel_Delete(Indel_Struct *IA__, int32_t pos__) {
  UNUSED(IA__);
  UNUSED(pos__);

  FATAL("Not implemented");
}

/*
 * Print IA integer vector.
 *
 * @param  IA__  Indel_Struct object.
 */
void Indel_Print(Indel_Struct *IA__) {
  int32_t i;

  printf("Indel array:\n");
  for (i = 0; i < IA__->last_; i++) printf("%d\t%d\n", i, IA__->mem_[i]);
}

/*
 * Get number of elements in Indel_Struct object.
 *
 * @param  IA__  Indel_Struct object.
 */
int32_t Indel_Size(Indel_Struct *IA__) { return IA__->last_; }

/*
 * Set value of an element in IA object.
 *
 * @param  IA__  Indel_Struct object.
 * @param  pos__  Position of element.
 */
int32_t Indel_Get(Indel_Struct *IA__, int32_t pos__) {
  assert(pos__ >= 0 && pos__ < IA__->last_);
  return IA__->mem_[pos__];
}

/*
 * Increase value of an element in IA object by one.
 *
 * @param  IA__  Indel_Struct object.
 * @param  pos__  Position of element.
 */
void Indel_Inc(Indel_Struct *IA__, int32_t pos__) {
  IA__->mem_[pos__] += 1;
}

/*
 * Set value of an element in IA object.
 *
 * @param  IA__  Indel_Struct object.
 * @param  pos__  Position of element.
 * @param  val__  New value of given element.
 */
void Indel_Set(Indel_Struct *IA__, int32_t pos__, int32_t val__) {
  assert(pos__ >= 0 && pos__ < IA__->last_);
  IA__->mem_[pos__] = val__;
}

#endif  // _INDEL_SIMPLE_STRUCTURE__
