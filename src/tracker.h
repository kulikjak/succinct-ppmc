#ifndef _TRACKER__
#define _TRACKER__

#include "utils.h"
#include "deBruijn.h"

struct {
int32_t cnt_;
int32_t *arr_[CONTEXT_LENGTH + 4];
} tracker_;

void Tracker_reset();

void Tracker_push(int32_t *a__);

void Tracker_pop();

void Tracker_update(int32_t idx__);

#endif  // _TRACKER__
