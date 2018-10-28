#include "tracker.h"

void Tracker_reset() {
  tracker_.cnt_ = 0;
}

void Tracker_push(int32_t *a__) {
  assert(tracker_.cnt_ + 1 < CONTEXT_LENGTH + 4);
  tracker_.arr_[tracker_.cnt_++] = a__;
}

void Tracker_pop() {
  assert(tracker_.cnt_ >= 1);
  tracker_.cnt_--;
}

void Tracker_update(int32_t idx__) {
  int32_t i;

  for (i = 0; i < tracker_.cnt_; i++)
    if (*(tracker_.arr_[i]) >= idx__) (*(tracker_.arr_[i]))++;
}
