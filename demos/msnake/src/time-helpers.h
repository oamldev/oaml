#ifndef TIME_HELPERS_H
#define TIME_HELPERS_H

#include <time.h>
#include <sys/time.h>
#include <stdio.h>

// mac ?
#ifdef __MACH__
#include <mach/clock.h>
#include <mach/mach.h>
#endif

// windows?
#ifdef _WIN32
// we need windows.h to implement clock_gettime on windows
#include <windows.h>

// this may not be defined on windows
#ifndef CLOCK_REALTIME
#define CLOCK_REALTIME 15 /* most random value evarr! */
#endif

// the timespec structure is not available on windows
struct timespec {
	time_t tv_sec;
	unsigned long tv_nsec;
};

#endif /* _WIN32 */

long long timeval_diff(struct timespec* tv1, struct timespec* tv2);
void current_utc_time(struct timespec *ts);
#endif /* TIME_HELPERS_H */
