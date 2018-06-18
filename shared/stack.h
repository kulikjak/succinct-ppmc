/*
 * Simple in memory simulated stack manager.
 * All operations return -1 on error.
 */

#ifndef _SHARED_STACK__
#define _SHARED_STACK__

#include "utils.h"

// used for tree traversal
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
#define STACK_POP() \
  ((stack.current_ == -1) ? -1 : stack.stack_[stack.current_--])
#define STACK_TOP() ((stack.current_ == -1) ? -1 : stack.stack_[stack.current_])
#define STACK_CLEAN() stack.current_ = -1;

stack_32b stack;

inline void stack_clean() { STACK_CLEAN(); }

#endif  // _SHARED_STACK__
