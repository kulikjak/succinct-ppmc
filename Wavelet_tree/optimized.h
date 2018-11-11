#ifndef _WT_EXT_STRUCTURE__
#define _WT_EXT_STRUCTURE__

#include <stdio.h>

#include "dbv.h"
#include "memory.h"
#include "utils.h"

typedef struct {
  DBV_Struct DBV_[4];
} OWT_Struct;

#define OWTStructRef OWT_Struct*

/*
 * Initialize OWT_Struct object.
 *
 * Since this is for DNA, this structure is for five symbols only, and mapping
 * is done automatically (the last one is a special $ symbol used in de Bruijn
 * graphs).
 *
 * @param  OWT__  OWT structure reference.
 */
void OWT_Init(OWTStructRef OWT__);

/*
 * Free all memory associated with the object.
 *
 * @param  OWT__  OWT structure reference.
 */
void OWT_Free(OWTStructRef OWT__);

/*
 * Delete symbol from given position.
 *
 * @param  OWT__  OWT structure reference.
 * @param  pos__  Delete position index.
 */
void OWT_Delete(OWTStructRef OWT__, uint32_t pos__);

/*
 * Insert new symbol into the given position.
 *
 * @param  OWT__  OWT structure reference.
 * @param  pos__  Insert position.
 * @param  symb__  To be inserted symbol.
 */
void OWT_Insert(OWTStructRef OWT__, uint32_t pos__, int8_t symb__);

/*
 * Get symbol from given position.
 *
 * @param  OWT__  OWT structure reference.
 * @param  pos__  Query position.
 */
int8_t OWT_Get(OWTStructRef OWT__, uint32_t pos__);

/*
 * Print underlying bit vectors.
 *
 * @param  OWT__  OWT structure reference.
 */
void OWT_Print_BitVectors(OWTStructRef OWT__);

/*
 * Print wavelet tree.
 *
 * This function is not very effecient and 
 * should be only used for testing purposes.
 *
 * @param  OWT__  OWT structure reference.
 */
void OWT_Print_Symbols(OWTStructRef OWT__);

/*
 * Rank operations for given position.
 *
 * @param  OWT__  OWT structure reference.
 * @param  pos__  Query position.
 * @param  symb__  Query symbol.
 */
int32_t OWT_Rank(OWTStructRef OWT__, uint32_t pos__, int8_t symb__);

/*
 * Select operations for given position.
 *
 * @param  OWT__  OWT structure reference.
 * @param  num__  Query position.
 * @param  symb__  Query symbol.
 */
int32_t OWT_Select(OWTStructRef OWT__, uint32_t num__, int8_t symb__);

#endif
