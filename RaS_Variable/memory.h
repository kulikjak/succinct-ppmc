/*
Simple memory and stack manager for dynamic rank and select structure.

Stack is simulated as simple array of adjustable size. All operations return -1 on error.
*/

#ifndef _RAS_MEMORY__
#define _RAS_MEMORY__

#include <stdint.h>
#include <string.h>
#include "utils.h"


// used for tree recursion - maximum 256 because of 8 bit counters.
#define MAX_STACK_SIZE 64

// global stack
struct {
  int32_t stack_[MAX_STACK_SIZE];  
  int8_t current_;
} stack;


int8_t G_alloc_state;


#define STACK_GET_PARENT() \
  ((stack.current_ >= 1) ? stack.stack_[stack.current_ - 1] : -1)
#define STACK_GET_GRANDPARENT() \
  ((stack.current_ >= 2) ? stack.stack_[stack.current_ - 2] : -1)

#define STACK_PUSH(arg) {                                              \
    if (stack.current_ + 1 >= MAX_STACK_SIZE) FATAL("Stack overflow"); \
    stack.stack_[++stack.current_] = arg;                              \
  }
#define STACK_POP() ((stack.current_ == -1) ? -1 : stack.stack_[stack.current_--])
#define STACK_TOP() ((stack.current_ == -1) ? -1 : stack.stack_[stack.current_])
#define STACK_CLEAN() stack.current_ = -1;

// initialize or clean stack memory
void init_stack() {
  memset(stack.stack_, 0, sizeof(stack.stack_));
  stack.current_ = -1;
}


// declaration of all possible tree nodes

#pragma pack(1)
typedef struct Node_8b {
  uint8_t p_;
  uint8_t r_;
  int8_t left_;
  int8_t right_;
} Node_8b;

typedef struct Node_16b {
  uint16_t p_;
  uint16_t r_;
  int16_t left_;
  int16_t right_;
} Node_16b;

typedef struct Node_32b {
  uint32_t p_;
  uint32_t r_;
  int32_t left_;
  int32_t right_;
} Node_32b;

typedef struct Node_64b {
  uint64_t p_;
  uint64_t r_;
  int64_t left_;
  int64_t right_;
} Node_64b;


typedef struct Leaf_8b {
  uint8_t p_;
  uint8_t r_;
  uint8_t vector_;
} Leaf_8b;

typedef struct Leaf_16b {
  uint16_t p_;
  uint16_t r_;
  uint16_t vector_;
} Leaf_16b;

typedef struct Leaf_32b {
  uint32_t p_;
  uint32_t r_;
  uint32_t vector_;
} Leaf_32b;

typedef struct Leaf_64b {
  uint64_t p_;
  uint64_t r_;
  uint64_t vector_;
} Leaf_64b;



struct memory_8b {
  Node_8b nodes_[64];
  Leaf_8b leafs_[64];

  int8_t node_current_;
  int8_t leaf_current_;
} memory_8b;


#define MEMORY_GET_ANY(arg)                                         \
  ((Node_8b*)((arg & 0x40) ? (void*)&(memory_8b.nodes_[arg & 0xBF]) \
                           : (void*)&(memory_8b.leafs_[arg])))
#define MEMORY_GET_NODE(arg) (Node_8b*)(&(memory_8b.nodes_[arg & 0xBF]))
#define MEMORY_GET_LEAF(arg) (Leaf_8b*)(&(memory_8b.leafs_[arg]))

#define IS_LEAF(arg) (!(arg & 0x40))


// initialize memory space for 8b nodes
void init_memory() {
  G_alloc_state = 8;

  memory_8b.node_current_ = -1;
  memory_8b.leaf_current_ = -1;
}

// create new leaf (get int pointer to new leaf)
int32_t new_leaf() {
  if (memory_8b.leaf_current_ >= 63)
    FATAL("Cannot allocate new leaf - low memory");

  return ++memory_8b.leaf_current_;
}

// create new internal node (get int pointer to new node)
int32_t new_node() {
  if (memory_8b.leaf_current_ >= 63)
    FATAL("Cannot allocate new node - low memory");

  return ++memory_8b.node_current_ | 0x40;
}



#endif  // _RAS_MEMORY__
