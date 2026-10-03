/**
 * @file posix-times.c
 * @brief Implementation of posix-times polyfills.
 */

/* clang-format off */
#include "posix-times.h"
#include <stddef.h>
/* clang-format on */

#if defined(_MSC_VER) || defined(_WIN32)

#ifdef __cplusplus
extern "C" {
#endif

/* Forward declare required Windows structures and functions */
__declspec(dllimport) void *__stdcall GetCurrentProcess(void);
__declspec(dllimport) int __stdcall GetProcessTimes(void *hProcess,
                                                    void *lpCreationTime,
                                                    void *lpExitTime,
                                                    void *lpKernelTime,
                                                    void *lpUserTime);

#ifdef __cplusplus
}
#endif

typedef struct _POSIX_TIMES_FILETIME {
  unsigned long dwLowDateTime;
  unsigned long dwHighDateTime;
} POSIX_TIMES_FILETIME;

/**
 * @brief Retrieves process and child execution times.
 */
clock_t posix_times(struct tms *buf) {
  POSIX_TIMES_FILETIME creation_time, exit_time, kernel_time, user_time;
  if (buf != NULL) {
    if (GetProcessTimes(GetCurrentProcess(), &creation_time, &exit_time,
                        &kernel_time, &user_time)) {
      /* Convert 100-nanosecond intervals to clock ticks.
         Assuming CLOCKS_PER_SEC is 1000 on MSVC. */
      unsigned __int64 k =
          ((unsigned __int64)kernel_time.dwHighDateTime << 32) |
          kernel_time.dwLowDateTime;
      unsigned __int64 u = ((unsigned __int64)user_time.dwHighDateTime << 32) |
                           user_time.dwLowDateTime;

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

#endif /* defined(_MSC_VER) || defined(_WIN32) */

/**
 * @brief Retrieves information on posix-times availability.
 */
auto_win_msvc_error_t posix_times_get_info(int *out_available) {
  if (out_available == NULL) {
    return AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT;
  }
  *out_available = 1;
  return AUTO_WIN_MSVC_SUCCESS;
}

typedef int make_iso_compilers_happy_tu_posix_times;
