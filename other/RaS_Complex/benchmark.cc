#include <chrono>
#include <cstdlib>
#include <iostream>

#include "memory.h"
#include "structure.h"

void benchmark(uint64_t size, double p = 0.5) {
  using std::chrono::high_resolution_clock;
  using std::chrono::duration_cast;
  using std::chrono::duration;

  RAS_Struct RaS;

  // initialize memory structure
  RAS_Init(&RaS);

  srand(time(NULL));

  // measure insertion duration
  auto t1 = high_resolution_clock::now();

  std::cout << "insert ... " << std::flush;
  for (uint64_t i = 0; i < size; i++) {
    int8_t bit = double(rand()) / RAND_MAX < p ? 1 : 0;
    RAS_Insert(&RaS, rand() % (i + 1), bit);
  }
  std::cout << "done." << std::endl;

  // measure access duration
  auto t2 = high_resolution_clock::now();

  std::cout << "access ... " << std::flush;
  for (uint64_t i = 0; i < size; ++i) {
    RAS_Get(RaS, rand() % size);
  }
  std::cout << "done." << std::endl;

  // measure rank 1 duration
  auto t3 = high_resolution_clock::now();

  std::cout << "rank 0 ... " << std::flush;
  for (uint64_t i = 0; i < size; ++i) {
    RAS_Rank0(RaS, rand() % (size + 1));
  }
  std::cout << "done." << std::endl;

  auto t4 = high_resolution_clock::now();

  std::cout << "rank 1 ... " << std::flush;
  for (uint64_t i = 0; i < size; ++i) {
    RAS_Rank(RaS, rand() % (size + 1));
  }
  std::cout << "done." << std::endl;

  auto t5 = high_resolution_clock::now();

  uint64_t nr_1 = RAS_Rank(RaS, size + 1) + 1;

  // measure select 1 duration
  auto t6 = high_resolution_clock::now();

  std::cout << "select 0 ... " << std::flush;
  for (uint64_t i = 0; i < size; ++i) {
    RAS_Select0(RaS, rand() % nr_1);
  }
  std::cout << "done." << std::endl;

  auto t7 = high_resolution_clock::now();

  std::cout << "select 1 ... " << std::flush;
  for (uint64_t i = 0; i < size; ++i) {
    RAS_Select(RaS, rand() % nr_1);
  }
  std::cout << "done." << std::endl;

  auto t8 = high_resolution_clock::now();

  uint64_t sec_insert =
      std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
  uint64_t sec_access =
      std::chrono::duration_cast<std::chrono::microseconds>(t3 - t2).count();
  uint64_t sec_rank0 =
      std::chrono::duration_cast<std::chrono::microseconds>(t4 - t3).count();
  uint64_t sec_rank1 =
      std::chrono::duration_cast<std::chrono::microseconds>(t5 - t4).count();
  uint64_t sec_sel0 =
      std::chrono::duration_cast<std::chrono::microseconds>(t7 - t6).count();
  uint64_t sec_sel1 =
      std::chrono::duration_cast<std::chrono::microseconds>(t8 - t7).count();

  std::cout << (double)sec_insert / size << " microseconds/insert" << std::endl;
  std::cout << (double)sec_access / size << " microseconds/access" << std::endl;
  std::cout << (double)sec_rank0 / size << " microseconds/rank0" << std::endl;
  std::cout << (double)sec_rank1 / size << " microseconds/rank1" << std::endl;
  std::cout << (double)sec_sel0 / size << " microseconds/select0" << std::endl;
  std::cout << (double)sec_sel1 / size << " microseconds/select1" << std::endl;

  RAS_Free(&RaS);
}

int main(int argc, char* argv[]) {
  if (argc != 3) {
    std::cout << "USAGE: " << argv[0] << " <size> <P>" << std::endl;
    return EXIT_FAILURE;
  }

  uint64_t n = atoi(argv[1]);
  double P = atof(argv[2]);

  std::cout << "size = " << n << ". P = " << P << std::endl;

  benchmark(n, P);

  return EXIT_SUCCESS;
}
