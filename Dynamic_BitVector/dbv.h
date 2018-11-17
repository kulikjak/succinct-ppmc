#ifndef _DBV_MAIN__
#define _DBV_MAIN__

#include "utils.h"

#include "dbv_memory.h"

#ifdef __cplusplus
#define static_ static
#else
#define static_
#endif

typedef struct {
  DBVMemPtr root_;
  DBVMemObj mem_;
} DBV_Struct;

#define DBVStructRef DBV_Struct*

/* used for tree traversal */
#define DBV_MAX_STACK_SIZE 64

#define DBV_ENABLE_RED_BLACK_BALANCING

/* global dbv_stack */
typedef struct dbv_stack_32b {
  int32_t stack_[DBV_MAX_STACK_SIZE];
  int32_t current_;
} dbv_stack_32b;

#define DBV_STACK_GET_PARENT() \
  ((dbv_stack.current_ >= 1) ? dbv_stack.stack_[dbv_stack.current_ - 1] : -1)
#define DBV_STACK_GET_GRANDPARENT() \
  ((dbv_stack.current_ >= 2) ? dbv_stack.stack_[dbv_stack.current_ - 2] : -1)
#define DBV_STACK_GET_GRANDGRANDPARENT() \
  ((dbv_stack.current_ >= 3) ? dbv_stack.stack_[dbv_stack.current_ - 3] : -1)
#define DBV_STACK_PUSH(arg)  {                      \
  if (dbv_stack.current_ + 1 >= DBV_MAX_STACK_SIZE) \
    FATAL("Stack overflow");                        \
  dbv_stack.stack_[++dbv_stack.current_] = arg;     \
}
#define DBV_STACK_POP() ((dbv_stack.current_ == -1) ? -1 : dbv_stack.stack_[dbv_stack.current_--])
#define DBV_STACK_TOP() ((dbv_stack.current_ == -1) ? -1 : dbv_stack.stack_[dbv_stack.current_])
#define DBV_STACK_CLEAN() dbv_stack.current_ = -1;

static_ dbv_stack_32b dbv_stack;

/*
 * Initialize DBV_Struct object.
 *
 * @param  DBV__  DBV structure reference.
 */
void DBV_Init(DBVStructRef DBV__);

/*
 * Free all memory associated with the object.
 *
 * @param  DBV__  DBV structure reference.
 */
void DBV_Free(DBVStructRef DBV__);

/*
 * Get number of bits in the vector.
 *
 * @param  DBV__  DBV structure reference.
 */
int32_t DBV_Size(DBVStructRef DBV__);

/*
 * Get bit from given position.
 *
 * @param  DBV__  DBV structure reference.
 * @param  pos__  Position number.
 */
int32_t DBV_Get(DBVStructRef DBV__, uint32_t pos__);

/*
 * Print bit vector.
 *
 * @param  DBV__  DBV structure reference.
 */
void DBV_Print(DBVStructRef DBV__);

/*
 * Rank operations for given position.
 *
 * @param  DBV__  DBV structure reference.
 * @param  pos__  Select position number.
 */
int32_t DBV_Rank(DBVStructRef DBV__, uint32_t pos__);
int32_t DBV_Rank0(DBVStructRef DBV__, uint32_t pos__);

/*
 * Select operations for given position.
 *
 * @param  DBV__  DBV structure reference.
 * @param  pos__  Select position number.
 */
int32_t DBV_Select(DBVStructRef DBV__, uint32_t pos__);
int32_t DBV_Select0(DBVStructRef DBV__, uint32_t pos__);

/*
 * Delete bit from given location.
 *
 * Function never merges leaves and so performance of the structure can be
 * worse after several uses.
 *
 * @param  DBV__  DBV structure reference.
 * @param  pos__  Delete position index.
 */
void DBV_Delete(DBVStructRef DBV__, uint32_t pos__);

/*
 * Insert new bit into the given position.
 *
 * @param  DBV__  DBV structure reference.
 * @param  pos__  Insert position.
 * @param  val__  Value of inserted bit.
 */
void DBV_Insert(DBVStructRef DBV__, uint32_t pos__, bool val__);

#endif
