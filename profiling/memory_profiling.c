#include "utils.h"

#ifdef ENABLE_MEMORY_PROFILING
#include <string.h>

struct {
  uint32_t malloc_count;
  uint32_t calloc_count;
  uint32_t realloc_count;
  uint32_t free_count;

  uint64_t malloc_size;
  uint64_t calloc_size;
  uint64_t realloc_size;
  uint64_t realloc_size_in;
} mprof_res;

struct {
  uint32_t size;
  uint32_t last;
  struct mptrs {
    void *ptr;
    size_t size;
  } * mptrs;
} mprof_ptrs;

void prof_enlarge_() {
  if (mprof_ptrs.last == mprof_ptrs.size) {
    mprof_ptrs.size *= 2;
    mprof_ptrs.mptrs =
        (struct mptrs *) realloc(mprof_ptrs.mptrs, mprof_ptrs.size * sizeof(struct mptrs));
  }
}
void *prof_malloc(size_t size) {
  prof_enlarge_();
  mprof_ptrs.mptrs[mprof_ptrs.last].ptr = malloc(size);
  mprof_ptrs.mptrs[mprof_ptrs.last++].size = size;

  mprof_res.malloc_count += 1;
  mprof_res.malloc_size += (uint64_t) size;
  return mprof_ptrs.mptrs[mprof_ptrs.last - 1].ptr;
}
void *prof_calloc(size_t nmemb, size_t size) {
  prof_enlarge_();
  mprof_ptrs.mptrs[mprof_ptrs.last].ptr = calloc(nmemb, size);
  mprof_ptrs.mptrs[mprof_ptrs.last++].size = size;

  mprof_res.calloc_count += 1;
  mprof_res.calloc_size += (uint64_t)(nmemb * size);
  return mprof_ptrs.mptrs[mprof_ptrs.last - 1].ptr;
}
void *prof_realloc(void *ptr, size_t size) {
  uint32_t i;
  for (i = 0; i < mprof_ptrs.last; i++) {
    if (mprof_ptrs.mptrs[i].ptr == ptr) {
      mprof_res.realloc_count += 1;
      mprof_res.realloc_size += size;
      mprof_res.realloc_size_in += mprof_ptrs.mptrs[i].size;

      mprof_ptrs.mptrs[i].ptr = realloc(ptr, size);
      mprof_ptrs.mptrs[i].size = size;

      return mprof_ptrs.mptrs[i].ptr;
    }
  }
  FATAL("Realloc is called on non existent memory pointer");
  return NULL;
}
void prof_free(void *ptr) {
  mprof_res.free_count += 1;
  free(ptr);
}

void init_memory_profiling(void) {
  memset(&mprof_res, 0, sizeof(mprof_res));

  mprof_ptrs.size = 100;
  mprof_ptrs.last = 0;
  mprof_ptrs.mptrs = (struct mptrs *) malloc(mprof_ptrs.size * sizeof(struct mptrs));

  printf("Running with memory profiling option enabled.\n");
  printf("Recompile without ENABLE_MEMORY_PROFILING to disable it.\n");
}

void finish_memory_profiling(void) {
  uint32_t i;
  size_t total = 0;

  printf("\n-------------------------------------------\n");
  printf("Total number of malloc() calls: %u\n", mprof_res.malloc_count);
  printf("Total number of calloc() calls: %u\n", mprof_res.calloc_count);
  printf("Total number of realloc() calls: %u\n", mprof_res.realloc_count);
  printf("Total number of free() calls: %u\n", mprof_res.free_count);

  printf("-------------------------------------------\n");
  printf("Memory allocated by malloc(): %lu B\n", mprof_res.malloc_size);
  printf("Memory allocated by calloc(): %lu B\n", mprof_res.calloc_size);
  printf("Memory allocated by realloc(): %lu B\n", mprof_res.realloc_size);
  printf("Memory freed by realloc(): %lu B\n", mprof_res.realloc_size_in);

  for (i = 0; i < mprof_ptrs.last; i++)
    total += mprof_ptrs.mptrs[i].size;

  printf("-------------------------------------------\n");
  printf("Highest memory usage in any given moment: %lu B\n", total);
  printf("Highest number of allocated blocks %u\n", mprof_ptrs.last);

  printf("-------------------------------------------\n");
  printf("%u:%u:%u:%u:%lu:%lu:%lu:%lu:%lu:%u\n", mprof_res.malloc_count,
          mprof_res.calloc_count, mprof_res.realloc_count, mprof_res.free_count,
          mprof_res.malloc_size, mprof_res.calloc_size, mprof_res.realloc_size,
          mprof_res.realloc_size_in, total, mprof_ptrs.last);

  free(mprof_ptrs.mptrs);
}

#endif
