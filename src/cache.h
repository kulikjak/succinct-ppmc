#ifndef _LEAF_LOOKUP_CACHE__
#define _LEAF_LOOKUP_CACHE__

#include "memory.h"
#include "defines.h"
#include "structure.h"

#ifdef ENABLE_LOOKUP_CACHE

typedef struct {
  uint32_t idx_;
  uint32_t inside_idx_;
  LeafRef leaf_ref_;
} cache_line;

#ifdef ENABLE_CACHE_STATS
int64_t cache_hits;
int64_t cache_misses;

void cache_stats_prep();
void cache_stats_print();
#endif

cache_line cache[CACHE_SIZE];
int32_t cache_next;

void reset_cache();

void add_to_cache(uint32_t idx__, uint32_t inside_idx__, LeafRef leaf_ref__);

bool lookup_cache(uint32_t* idx__, LeafRef* leaf_ref);

#else

#define add_to_cache(idx__, inside_idx__, leaf_ref__) (UNUSED(idx__))
#define lookup_cache(idx__, leaf_ref) (false)

#endif  /* ENABLE_LOOKUP_CACHE */

#endif
