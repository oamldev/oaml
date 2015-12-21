#ifndef __GETTIME_H__
#define __GETTIME_H__

#include <stdint.h>

/* Returns the amount of milliseconds elapsed since the UNIX epoch. Works on both
 * windows and linux. */
uint64_t GetTimeMs64();

#endif
