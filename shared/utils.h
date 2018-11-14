#ifndef _SHARED_UTILS__
#define _SHARED_UTILS__

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define bool uint8_t
#define true 1
#define false 0

#define bool32 int32_t

#define UNREACHABLE assert(0);
#define UNUSED(var) (void)(var)

static void fatal(const char* m__, const char* e__) {
  fprintf(stderr, "[Fatal]: %s%s\n", m__, e__);
  exit(EXIT_FAILURE);
}

#define FATAL(msg) (fatal(msg, ""))

#define malloc_(a) malloc(a)
#define calloc_(a, b) calloc(a, b)
#define realloc_(a, b) realloc(a, b)
#define free_(a) free(a)

#endif
