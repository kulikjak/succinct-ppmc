#ifndef _TRACKER__
#define _TRACKER__

#include "deBruijn.h"
#include "utils.h"

struct {
  int32_t cnt_;
  int32_t *arr_[CONTEXT_LENGTH + 4];
} tracker_;

#define Tracker_reset() \
  tracker_.cnt_ = 0;

#define Tracker_push(a__)                         \
  assert(tracker_.cnt_ + 1 < CONTEXT_LENGTH + 4); \
  tracker_.arr_[tracker_.cnt_++] = a__;

#define Tracker_pop()         \
  assert(tracker_.cnt_ >= 1); \
  tracker_.cnt_--;

#define Tracker_update(idx__) {                  \
  for (int32_t ti = 0; ti < tracker_.cnt_; ti++) \
    if (*(tracker_.arr_[ti]) >= idx__)           \
      (*(tracker_.arr_[ti]))++;                  \
}

#endif
