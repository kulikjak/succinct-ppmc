/*
First implementation of dynamic rank and select structure.

This is mostly only proof of concept. It can handle every operation, but cannot do it in O(log n) time.

Tree is not balanced and pointers and counters are much bigger then they need to O(log n).
Tree leafs also have fixed size which means, that they are wasting space.
*/

#ifndef _RANK_SELECT_1__
#define _RANK_SELECT_1__


#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

#define bool uint8_t
#define true 1
#define false 0

#define VERBOSE_ true

// ----------------------------------------------------------------- //

#define IS_LEAF(ptr) ((uintptr_t)ptr & (uintptr_t)0x1)
#define SET_LEAF_FLAG(ptr) ptr = ((RaSnode*)((uintptr_t)ptr | (uintptr_t)0x1))
#define UNSET_LEAF_FLAG(ptr) \
  ptr = ((RaSnode*)((uintptr_t)ptr & ~(uintptr_t)0x1))

#define REAL_PTR(ptr) ((RaSnode*)((uintptr_t)ptr & ~(0x1)))

#define UNUSED(var) (void)(var)


void fatal(const char* m__, const char* e__) {
  fprintf(stderr, "[Fatal]: %s%s\n", m__, e__);
  exit(EXIT_FAILURE);
}

void verbose(const char* m__, const char* e__) {
	if (VERBOSE_) fprintf(stderr, "[Verbose]: %s%s\n", m__, e__);
}

#define FATAL(msg) (fatal(msg, ""))

#define VERBOSE(func) if (VERBOSE_){ func }


#define INSERT_BIT(leaf, pos, value)                         \
  {                                                          \
    uint32_t mask = ((pos) == 0) ? 0 : ~(0) << (32 - (pos)); \
    uint32_t new = 0;                                        \
                                                             \
    new |= (leaf)->vector& mask;                             \
    new |= ((leaf)->vector & (~(mask))) >> 1;                \
    (leaf)->vector = new;                                    \
    (leaf)->p += 1;                                          \
                                                             \
    if (value) {                                             \
      (leaf)->vector |= 0x1 << (31 - (pos));                 \
      (leaf)->r += 1;                                        \
    }                                                        \
  }

#define RANK16(ret, leaf) \
  ret += ((leaf)->vector >> (31 - 0)) & 0x1; \
  ret += ((leaf)->vector >> (31 - 1)) & 0x1; \
  ret += ((leaf)->vector >> (31 - 2)) & 0x1; \
  ret += ((leaf)->vector >> (31 - 3)) & 0x1; \
  ret += ((leaf)->vector >> (31 - 4)) & 0x1; \
  ret += ((leaf)->vector >> (31 - 5)) & 0x1; \
  ret += ((leaf)->vector >> (31 - 6)) & 0x1; \
  ret += ((leaf)->vector >> (31 - 7)) & 0x1; \
  ret += ((leaf)->vector >> (31 - 8)) & 0x1; \
  ret += ((leaf)->vector >> (31 - 9)) & 0x1; \
  ret += ((leaf)->vector >> (31 - 10)) & 0x1; \
  ret += ((leaf)->vector >> (31 - 11)) & 0x1; \
  ret += ((leaf)->vector >> (31 - 12)) & 0x1; \
  ret += ((leaf)->vector >> (31 - 13)) & 0x1; \
  ret += ((leaf)->vector >> (31 - 14)) & 0x1; \
  ret += ((leaf)->vector >> (31 - 15)) & 0x1;



typedef struct RaSnode {
  uint32_t p;
  uint32_t r;
  struct RaSnode* left;
  struct RaSnode* right;
} RaSnode;

typedef struct RaSleaf {
  uint32_t p;
  uint32_t r;
  uint32_t vector;
} RaSleaf;

typedef struct RaSstruct {
  RaSnode* root;
} RaSstruct;


RaSstruct* initRaS() {
	// allocate structure itself
	RaSstruct* RaSroot_ = (RaSstruct*)malloc(sizeof(RaSstruct));

	// allocate first leaf of the tree
	RaSroot_->root = (void*)calloc(1, sizeof(RaSleaf));

	// flag newly created node as leaf
	SET_LEAF_FLAG(RaSroot_->root);

  return RaSroot_;
}

void printRaS_(RaSnode* n__) {
  uint32_t i;

  if (IS_LEAF(n__)) {
    RaSleaf* leaf_ = (RaSleaf*)REAL_PTR(n__);

    for (i = 0; i < leaf_->p; i++)
      printf("%d ", (leaf_->vector >> (31 - i)) & 0x1);

    return;
  }

  printRaS_(n__->left);
  printRaS_(n__->right);
}

void printRaS(RaSstruct* s__) {
	printRaS_(s__->root);
	printf("\n");
}

void deleteRaS_(RaSnode* n__) {
  if (!IS_LEAF(n__)) {
    deleteRaS_(n__->left);
    deleteRaS_(n__->right);
  }
  free(REAL_PTR(n__));
}

void deleteRaS(RaSstruct** s__) {
  // delete whole tree
  deleteRaS_(REAL_PTR((*s__)->root));

  // free structure and set pointer to NULL
  free(*s__);
  *s__ = NULL;
}


void insert(RaSstruct* s__, uint32_t pos__, bool val__) {
  if (pos__ > REAL_PTR(s__->root)->p)
  	FATAL("Index out of range");

  VERBOSE( fprintf(stderr, "Inserting value %d on position %u\n", val__, pos__); )

  // set current to structture root
  RaSnode* current = (RaSnode*)s__->root;
  RaSnode* parent = NULL;

  // traverse the tree and enter correct leaf
  while (!IS_LEAF(current)) {
    parent = REAL_PTR(current);

    // update p and r counters as we are traversing the structure
    REAL_PTR(current)->p += 1;
    REAL_PTR(current)->r += (val__) ? 1 : 0;

    int32_t temp = REAL_PTR(REAL_PTR(current)->left)->p;
    if (temp >= (int32_t)pos__) {
      current = REAL_PTR(current)->left;
    } else {
      pos__ -= temp;
      current = REAL_PTR(current)->right;
    }
  }

  RaSleaf* leaf = (RaSleaf*)REAL_PTR(current);

	// check if we can add more bits into this leaf
  if (leaf->p < 32) {
    INSERT_BIT(leaf, pos__, val__);

  } else {
    VERBOSE( fprintf(stderr, "Splitting full node\n"); )

    // allocate new internal node and set counters
    RaSnode* newNode = (void*)malloc(sizeof(RaSnode));
    newNode->p = 33;
    newNode->r = leaf->r + ((val__) ? 1 : 0);

    // allocate two new leafs
    newNode->left = (RaSnode*)malloc(sizeof(RaSleaf));
    newNode->right = (RaSnode*)malloc(sizeof(RaSleaf));

    // initialize new left node
    ((RaSleaf*)(newNode->left))->vector = leaf->vector & (0xFFFF0000);
    ((RaSleaf*)(newNode->left))->p = 16;
    ((RaSleaf*)(newNode->left))->r = 0;

    // calculate rank on newly created node
    RANK16(((RaSleaf*)(newNode->left))->r, (RaSleaf*)newNode->left)

    // initialize new right node
    ((RaSleaf*)(newNode->right))->vector = (leaf->vector & (0x0000FFFF)) << 16;
    ((RaSleaf*)(newNode->right))->p = 16;
    ((RaSleaf*)(newNode->right))->r = leaf->r - ((RaSleaf*)(newNode->left))->r;

    // now insert bit into correct leaf
    if (pos__ <= 16) {
      INSERT_BIT((RaSleaf*)newNode->left, pos__, val__);
    } else {
      INSERT_BIT((RaSleaf*)newNode->right, pos__ - 16, val__);
    }

    SET_LEAF_FLAG(newNode->left);
    SET_LEAF_FLAG(newNode->right);

    if (parent == NULL) {
    	s__->root = newNode;
    } else {
	    if (REAL_PTR(parent->left) == (RaSnode*)leaf)
	      parent->left = newNode;
	    else
	      parent->right = newNode;
	  }

    free(leaf);
  }
}

int32_t rankRaS(RaSstruct* s__, int32_t pos__) {
  int32_t i, rank = 0;

  // set current to structture root
  RaSnode* current = (RaSnode*)s__->root;

  // traverse the tree and enter correct leaf
  while (!IS_LEAF(current)) {
    int32_t temp = REAL_PTR(REAL_PTR(current)->left)->p;
    if (temp >= pos__) {
      current = REAL_PTR(current)->left;
    } else {
      pos__ -= temp;
      rank += REAL_PTR(REAL_PTR(current)->left)->r;
      current = REAL_PTR(current)->right;
    }
  }

  RaSleaf* leaf = (RaSleaf*)REAL_PTR(current);

  // traverse current leaf up to position pos__ and sum rank
  int32_t limit = (pos__ <= 32) ? pos__ : 32;
  for (i = 0; i < limit; i++)
  	rank += (leaf->vector >> (31 - i)) & 0x1;

  return rank;
}

int32_t selectRaS(RaSstruct* s__, int32_t num__) {
  int32_t i, select = 0;

  // set current to structture root
  RaSnode* current = (RaSnode*)s__->root;

  // traverse the tree and enter correct leaf
  while (!IS_LEAF(current)) {
    int32_t temp = REAL_PTR(REAL_PTR(current)->left)->r;
    if (temp >= num__) {
      current = REAL_PTR(current)->left;
    } else {
      num__ -= temp;
      select += REAL_PTR(REAL_PTR(current)->left)->p;
      current = REAL_PTR(current)->right;
    }
  }

  RaSleaf* leaf = (RaSleaf*)REAL_PTR(current);

  if (num__ > (int32_t)leaf->r)
  	return -1;

  // traverse current leaf up to position pos__ and get select
  for (i = 0; num__; i++) {
  	num__ -= (leaf->vector >> (31 - i)) & 0x1;
  }

  return select + i;
}

#endif  // _RANK_SELECT_1__
