#ifndef _OPTIMIZED_WAVELET_EXT_TREE__
#define _OPTIMIZED_WAVELET_EXT_TREE__

#include <stdio.h>

#include "dbv.h"
#include "memory.h"
#include "utils.h"

typedef struct {
  DBV_Struct DBV_[8];
} OWTE_Struct;

#define OWTEStructRef OWTE_Struct*

/*
 * Initialize OWTE_Struct object.
 *
 * Since this is for DNA, this structure is for five symbols only, and mapping
 * is done automatically (the last one is a special $ symbol used in de Bruijn
 * graphs).
 *
 * @param  OWTE__  OWTE structure reference.
 */
void OWTE_Init(OWTEStructRef OWTE__);

/*
 * Free all memory associated with the object.
 *
 * @param  OWTE__  OWTE structure reference.
 */
void OWTE_Free(OWTEStructRef OWTE__);

/*
 * Delete symbol from given position.
 *
 * @param  OWTE__  OWTE structure reference.
 * @param  pos__  Delete position index.
 */
void OWTE_Delete(OWTEStructRef OWTE__, uint32_t pos__);

/*
 * Insert new symbol into the given position.
 *
 * @param  OWTE__  OWTE structure reference.
 * @param  pos__  Insert position.
 * @param  symb__  To be inserted symbol.
 */
void OWTE_Insert(OWTEStructRef OWTE__, uint32_t pos__, int8_t symb__);

/*
 * Get symbol from given position.
 *
 * @param  OWTE__  OWTE structure reference.
 * @param  pos__  Query position.
 */
int8_t OWTE_Get(OWTEStructRef OWTE__, uint32_t pos__);

/*
 * Print underlying bit vectors.
 *
 * @param  OWTE__  OWTE structure reference.
 */
void OWTE_Print_BitVectors(OWTEStructRef OWTE__);

/*
 * Print wavelet tree.
 *
 * This function is not very effecient and
 * should be only used for testing purposes.
 *
 * @param  OWTE__  OWTE structure reference.
 */
void OWTE_Print_Symbols(OWTEStructRef OWTE__);

/*
 * Rank operations for given position.
 *
 * @param  OWTE__  OWTE structure reference.
 * @param  pos__  Query position.
 * @param  symb__  Query symbol.
 */
int32_t OWTE_Rank(OWTEStructRef OWTE__, uint32_t pos__, int8_t symb__);

/*
 * Select operations for given position.
 *
 * @param  OWTE__  OWTE structure reference.
 * @param  num__  Query position.
 * @param  symb__  Query symbol.
 */
int32_t OWTE_Select(OWTEStructRef OWTE__, uint32_t num__, int8_t symb__);

#endif
