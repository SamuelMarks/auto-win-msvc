/* posix-times.c - Strict C89 Implementation */
#include "sys/times.h"
#include <stddef.h>

#if defined(POSIX_TIMES_MSVC)

/* Forward declare required Windows structures and functions */
__declspec(dllimport) void* __stdcall GetCurrentProcess(void);
__declspec(dllimport) int __stdcall GetProcessTimes(void* hProcess, void* lpCreationTime, void* lpExitTime, void* lpKernelTime, void* lpUserTime);

typedef struct _POSIX_TIMES_FILETIME {
    unsigned long dwLowDateTime;
    unsigned long dwHighDateTime;
} POSIX_TIMES_FILETIME;

/** \brief posix_times function. */
clock_t posix_times(struct tms *buf) {
    POSIX_TIMES_FILETIME creation_time, exit_time, kernel_time, user_time;
    if (buf != NULL) {
        if (GetProcessTimes(GetCurrentProcess(), &creation_time, &exit_time, &kernel_time, &user_time)) {
            /* Convert 100-nanosecond intervals to clock ticks.
               Assuming CLOCKS_PER_SEC is 1000 on MSVC. */
            unsigned __int64 k = ((unsigned __int64)kernel_time.dwHighDateTime << 32) | kernel_time.dwLowDateTime;
            unsigned __int64 u = ((unsigned __int64)user_time.dwHighDateTime << 32) | user_time.dwLowDateTime;
            
            buf->tms_stime = (clock_t)(k / 10000ULL);
            buf->tms_utime = (clock_t)(u / 10000ULL);
            buf->tms_cstime = 0;
            buf->tms_cutime = 0;
        } else {
            buf->tms_stime = 0;
            buf->tms_utime = 0;
            buf->tms_cstime = 0;
            buf->tms_cutime = 0;
        }
    }
    return (clock_t)0;
}

#endif /* POSIX_TIMES_MSVC */
