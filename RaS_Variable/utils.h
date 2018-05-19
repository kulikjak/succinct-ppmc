#ifndef _RAS_UTILS__
#define _RAS_UTILS__

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>


#define bool uint8_t
#define true 1
#define false 0

#define VERBOSE_ false

#define UNREACHABLE assert(0);
#define UNUSED(var) (void)(var)

void fatal(const char* m__, const char* e__) {
  fprintf(stderr, "[Fatal]: %s%s\n", m__, e__);
  exit(EXIT_FAILURE);
}

void verbose(const char* m__, const char* e__) {
  if (VERBOSE_) fprintf(stderr, "[Verbose]: %s%s\n", m__, e__);
}

#define FATAL(msg) (fatal(msg, ""))
#define FATAL_EXT(msg, msg2) (fatal(msg, msg2))

#define VERBOSE(func) if (VERBOSE_){ func }



#endif  // _RAS_UTILS__
