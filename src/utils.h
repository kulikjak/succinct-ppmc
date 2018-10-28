#ifndef _SHARED_UTILS__
#define _SHARED_UTILS__

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "defines.h"

#define bool uint8_t
#define true 1
#define false 0

#define UNREACHABLE assert(0);
#define UNUSED(var) (void)(var)

static void fatal(const char* m__, const char* e__) {
  fprintf(stderr, "[Fatal]: %s%s\n", m__, e__);
  exit(EXIT_FAILURE);
}

static void verbose(const char* m__, const char* e__) {
  if (VERBOSE_) fprintf(stderr, "[Verbose]: %s%s\n", m__, e__);
}

#define FATAL(msg) (fatal(msg, ""))
#define FATAL_EXT(msg, msg2) (fatal(msg, msg2))

#define VERBOSE(func) if (VERBOSE_){ func }

#endif  // _SHARED_UTILS__
