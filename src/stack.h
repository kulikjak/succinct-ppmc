/*
 * Simple in memory simulated stack manager.
 * All operations return -1 on error.
 */

#ifndef _SHARED_STACK__
#define _SHARED_STACK__

#include "utils.h"

typedef struct stack_32b {
  MemPtr stack_[MAX_STACK_SIZE];
  int32_t current_;
} stack_32b;

#if defined(INDEXED_MEMORY)
  #define STACK_ERROR -1
#elif defined(DIRECT_MEMORY) || defined(SIMPLE_MEMORY)
  #define STACK_ERROR NULL
#endif

#define STACK_GET_PARENT() \
  ((stack.current_ >= 1) ? stack.stack_[stack.current_ - 1] : STACK_ERROR)
#define STACK_GET_GRANDPARENT() \
  ((stack.current_ >= 2) ? stack.stack_[stack.current_ - 2] : STACK_ERROR)
#define STACK_GET_GRANDGRANDPARENT() \
  ((stack.current_ >= 3) ? stack.stack_[stack.current_ - 3] : STACK_ERROR)
#define STACK_PUSH(arg) {                     \
    if (stack.current_ + 1 >= MAX_STACK_SIZE) \
      FATAL("Stack overflow");                \
    stack.stack_[++stack.current_] = arg;     \
  }
#define STACK_POP() ((stack.current_ == -1) ? STACK_ERROR : stack.stack_[stack.current_--])
#define STACK_TOP() ((stack.current_ == -1) ? STACK_ERROR : stack.stack_[stack.current_])
#define STACK_CLEAN() stack.current_ = -1;

stack_32b stack;

#endif
