#include "cache.h"

#ifdef ENABLE_LOOKUP_CACHE

#ifdef ENABLE_CACHE_STATS
void cache_stats_prep() {
  cache_hits = 0;
  cache_misses = 0;
}

void cache_stats_print() {
  printf("Cache stats (%lf):\n",
    ((double)cache_hits / (cache_hits + cache_misses)));
  printf("    hit rate: %ld\n", cache_hits);
  printf("    miss rate: %ld\n", cache_misses);
}
#endif

void reset_cache() {
  cache_next = 0;
  memset(cache, -1, CACHE_SIZE * sizeof(cache_line));
}

void add_to_cache(uint32_t idx__, uint32_t inside_idx__, LeafRef leaf_ref__) {
  cache[cache_next].idx_ = idx__;
  cache[cache_next].inside_idx_ = inside_idx__;
  cache[cache_next].leaf_ref_ = leaf_ref__;

  cache_next = (cache_next + 1) % CACHE_SIZE;
}

bool lookup_cache(uint32_t* idx__, LeafRef* leaf_ref) {
  int32_t i;

  for (i = 0; i < CACHE_SIZE; i++) {
    if (cache[i].idx_ == *idx__) {
      *idx__ = cache[i].inside_idx_;
      *leaf_ref = cache[i].leaf_ref_;
#ifdef ENABLE_CACHE_STATS
        cache_hits++;
#endif
      return true;
    }
  }
#ifdef ENABLE_CACHE_STATS
  cache_misses++;
#endif
  return false;
}

#endif  // ENABLE_LOOKUP_CACHE
