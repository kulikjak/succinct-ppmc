#ifndef _STRUCTURE_COMMON__
#define _STRUCTURE_COMMON__

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifndef RANDOM_SEED
#define RANDOM_SEED 0
#endif

#define VAR_IGNORE 1

struct {
  double seq_insertion;
  double rand_insertion;
  double seq_rank;
  double seq_select;
  double seq_access;
  double rand_rank;
  double rand_select;
  double rand_access;
} res;

clock_t start_time;

int32_t sample_size;
int32_t secondary_size;

#define START_BENCHMARK { \
  srand(RANDOM_SEED);     \
  start_time = clock();   \
}

#define END_BENCHMARK(var__) \
  { res.var__ = ((double)(clock() - start_time)) / CLOCKS_PER_SEC; }

void init_results() {
  memset(&res, 0, sizeof(res));
}
void show_results() {
  printf("Sequential insertion:\t%lf\n", res.seq_insertion);
  printf("Random insertion:\t%lf\n", res.rand_insertion);
  printf("Sequential rank:\t%lf\n", res.seq_rank);
  printf("Sequential select:\t%lf\n", res.seq_select);
  printf("Sequential access:\t%lf\n", res.seq_access);
  printf("Random rank:\t\t%lf\n", res.rand_rank);
  printf("Random select:\t\t%lf\n", res.rand_select);
  printf("Random access:\t\t%lf\n", res.rand_access);
  printf("---------------------------------------\n");
  printf("%lf:%lf:%lf:%lf:%lf:%lf:%lf:%lf\n", res.seq_insertion, res.rand_insertion, res.seq_rank,
         res.seq_select, res.seq_access, res.rand_rank, res.rand_select, res.rand_access);
}

#endif
