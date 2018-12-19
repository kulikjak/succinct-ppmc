#include "utils.h"

#ifdef ENABLE_TIME_PROFILING
#include <time.h>

clock_t tprof_runtime;

void init_time_profiling() {
  tprof_runtime = clock();

  printf("Running with time profiling option enabled.\n");
  printf("Recompile without ENABLE_TIME_PROFILING to disable it.\n");
}

void finish_time_profiling() {
  tprof_runtime = clock() - tprof_runtime;

  printf("\n-------------------------------------------\n");
  printf("Total compressor runtime: %lfs\n", (double)tprof_runtime / CLOCKS_PER_SEC);
  printf("-------------------------------------------\n");
  printf("%lf\n", (double)tprof_runtime / CLOCKS_PER_SEC);
}

#endif
