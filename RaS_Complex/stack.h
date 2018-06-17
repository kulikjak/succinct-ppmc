/*
 * Simple in memory simulated stack manager for dynamic rank and select structure.
 *
 * All operations return -1 on error.
 */

#ifndef _RAS_STACK__
#define _RAS_STACK__

#include "../shared/utils.h"

// used for tree traversal - maximum 256 because of 8 bit counters.
#define MAX_STACK_SIZE 64

// global stack
typedef struct stack_32b {
  int32_t stack_[MAX_STACK_SIZE];
  int32_t current_;
} stack_32b;

#define STACK_GET_PARENT() \
  ((stack.current_ >= 1) ? stack.stack_[stack.current_ - 1] : -1)
#define STACK_GET_GRANDPARENT() \
  ((stack.current_ >= 2) ? stack.stack_[stack.current_ - 2] : -1)

#define STACK_PUSH(arg)                                                \
  {                                                                    \
    if (stack.current_ + 1 >= MAX_STACK_SIZE) FATAL("Stack overflow"); \
    stack.stack_[++stack.current_] = arg;                              \
  }
#define STACK_POP() ((stack.current_ == -1) ? -1 : stack.stack_[stack.current_--])
#define STACK_TOP() ((stack.current_ == -1) ? -1 : stack.stack_[stack.current_])
#define STACK_CLEAN() stack.current_ = -1;

stack_32b stack;

inline void stack_clean() { STACK_CLEAN(); }

#endif  // _RAS_STACK_
