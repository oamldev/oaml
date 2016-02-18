#include "time-helpers.h"

// calculate the difference between two timespec structs
long long timeval_diff(struct timespec* tv1, struct timespec* tv2) {
  long long diff = 0;
  // calculate the diffrence of the nanoseconds
  long nano = (tv2->tv_nsec - tv1->tv_nsec);
  int add = 0;

  // some calculations if the nanoseconds of struct 1 were bigger
  if(nano < 0) {
    nano = 1000000000 + nano;
    add = 1;
  }
  diff = nano;
  // add the seconds
  diff += (tv2->tv_sec - tv1->tv_sec - add) * 1000000000;
  return diff;
}

#ifdef _WIN32 
// windows implementation of clock_gettime: Carl Staelin (http://stackoverflow.com/questions/5404277/porting-clock-gettime-to-windows)
// a little helper function for the windows implementation
LARGE_INTEGER getFILETIMEoffset() {
    SYSTEMTIME s;
    FILETIME f;
    LARGE_INTEGER t;

    s.wYear = 1970;
    s.wMonth = 1;
    s.wDay = 1;
    s.wHour = 0;
    s.wMinute = 0;
    s.wSecond = 0;
    s.wMilliseconds = 0;
    SystemTimeToFileTime(&s, &f);
    t.QuadPart = f.dwHighDateTime;
    t.QuadPart <<= 32;
    t.QuadPart |= f.dwLowDateTime;
    return (t);
}

int clock_gettime(int X, struct timeval *tv);

int clock_gettime(int X, struct timeval *tv) {
    LARGE_INTEGER           t;
    FILETIME                f;
    double                  microseconds;
    static LARGE_INTEGER    offset;
    static double           frequencyToMicroseconds;
    static int              initialized = 0;
    static BOOL             usePerformanceCounter = 0;

    if (!initialized) {
        LARGE_INTEGER performanceFrequency;
        initialized = 1;
        usePerformanceCounter = QueryPerformanceFrequency(&performanceFrequency);
        if (usePerformanceCounter) {
            QueryPerformanceCounter(&offset);
            frequencyToMicroseconds = (double)performanceFrequency.QuadPart / 1000000.;
        } else {
            offset = getFILETIMEoffset();
            frequencyToMicroseconds = 10.;
        }
    }
    if (usePerformanceCounter) QueryPerformanceCounter(&t);
    else {
        GetSystemTimeAsFileTime(&f);
        t.QuadPart = f.dwHighDateTime;
        t.QuadPart <<= 32;
        t.QuadPart |= f.dwLowDateTime;
    }

    t.QuadPart -= offset.QuadPart;
    microseconds = (double)t.QuadPart / frequencyToMicroseconds;
    t.QuadPart = microseconds;
    tv->tv_sec = t.QuadPart / 1000000;
    tv->tv_usec = t.QuadPart % 1000000;
    return (0);
}
#endif /* _WIN32 */


// the portable function for clock_gettime ;-)
void current_utc_time(struct timespec *ts) {
// mac implementation of clock_get_time by: jbenet (https://gist.github.com/1087739)
#ifdef __MACH__ // OS X does not have clock_gettime, use clock_get_time
  clock_serv_t cclock;
  mach_timespec_t mts;
  host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
  clock_get_time(cclock, &mts);
  mach_port_deallocate(mach_task_self(), cclock);
  ts->tv_sec = mts.tv_sec;
  ts->tv_nsec = mts.tv_nsec;
#elif _WIN32
  struct timeval tv;
  clock_gettime(CLOCK_REALTIME, &tv);
  ts->tv_sec = tv.tv_sec;
  ts->tv_nsec = tv.tv_usec * 1000;
#else /* LINUX */
  clock_gettime(CLOCK_REALTIME, ts);
#endif
}
