#define MYTIME_C
#include "all.h"

#include <sys/times.h>

double mytime() {
  struct tms p;
  times(&p);

/* assume 1 click tick = 0.01 second */
  return(0.01 * (double) (p.tms_utime + p.tms_stime));
}

