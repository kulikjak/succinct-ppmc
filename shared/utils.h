#ifndef _SHARED_UTILS__
#define _SHARED_UTILS__

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define bool uint8_t
#define true 1
#define false 0

#define Bool32 int32_t

#define UNREACHABLE assert(0);
#define UNUSED(var) (void) (var)

static void fatal(const char* m__) {
  fprintf(stderr, "[Fatal]: %s\n", m__);
  exit(EXIT_FAILURE);
}

#define FATAL(msg) (fatal(msg))

#ifndef ENABLE_TIME_PROFILING
#define init_time_profiling() {}
#define finish_time_profiling() {}
#else
void init_time_profiling();
void finish_time_profiling();
#endif

#ifndef ENABLE_MEMORY_PROFILING
#define malloc_(a) malloc(a)
#define calloc_(a, b) calloc(a, b)
#define realloc_(a, b) realloc(a, b)
#define free_(a) free(a)

#define init_memory_profiling() {}
#define finish_memory_profiling() {}

#else
void init_memory_profiling();
void finish_memory_profiling();

void *prof_malloc(size_t size);
void *prof_calloc(size_t nmemb, size_t size);
void *prof_realloc(void *ptr, size_t size);
void prof_free(void *ptr);

#define malloc_(a) prof_malloc(a)
#define calloc_(a, b) prof_calloc(a, b)
#define realloc_(a, b) prof_realloc(a, b)
#define free_(a) prof_free(a)
#endif

#endif
