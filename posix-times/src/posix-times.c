#if !defined(AUTO_WIN_MSVC_MEGA_LIBRARY) || defined(_WIN32)
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
__declspec(dllimport) int __stdcall
GetProcessTimes(void *hProcess, void *lpCreationTime, void *lpExitTime,
                void *lpKernelTime, void *lpUserTime);

#ifdef __cplusplus
}
#endif

#ifndef POSIX_TIMES_MOCK_GetProcessTimes
#define POSIX_TIMES_MOCK_GetProcessTimes GetProcessTimes
#endif

typedef struct _POSIX_TIMES_FILETIME {
  unsigned long dwLowDateTime;
  unsigned long dwHighDateTime;
} POSIX_TIMES_FILETIME;

#if defined(_MSC_VER) || defined(__WATCOMC__)
typedef unsigned __int64 posix_times_u64;
#elif defined(__GNUC__) || defined(__clang__)
__extension__ typedef unsigned long long posix_times_u64;
#else
typedef unsigned long posix_times_u64;
#endif

/**
 * @brief Retrieves process and child execution times.
 * @param[out] buf Pointer to tms structure receiving process CPU times.
 * @return Elapsed ticks (0 on Windows polyfill).
 */
clock_t posix_times(struct tms *buf) {
  POSIX_TIMES_FILETIME creation_time;
  POSIX_TIMES_FILETIME exit_time;
  POSIX_TIMES_FILETIME kernel_time;
  POSIX_TIMES_FILETIME user_time;
  if (buf != NULL) {
    if (POSIX_TIMES_MOCK_GetProcessTimes(GetCurrentProcess(), &creation_time,
                                         &exit_time, &kernel_time,
                                         &user_time)) {
      posix_times_u64 k = ((posix_times_u64)kernel_time.dwHighDateTime << 32) |
                          (posix_times_u64)kernel_time.dwLowDateTime;
      posix_times_u64 u = ((posix_times_u64)user_time.dwHighDateTime << 32) |
                          (posix_times_u64)user_time.dwLowDateTime;

      buf->tms_stime = (clock_t)(k / (posix_times_u64)10000UL);
      buf->tms_utime = (clock_t)(u / (posix_times_u64)10000UL);
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

#elif defined(__DOS__) || defined(__MSDOS__) || defined(__WATCOMC__)
/**
 * @brief Retrieves process and child execution times on DOS.
 * @param[out] buf Pointer to tms structure receiving zeroed process CPU times.
 * @return Always 0.
 */
clock_t posix_times(struct tms *buf) {
  if (buf != NULL) {
    buf->tms_utime = 0;
    buf->tms_stime = 0;
    buf->tms_cutime = 0;
    buf->tms_cstime = 0;
  }
  return (clock_t)0;
}
#endif /* defined(_MSC_VER) || defined(_WIN32) */

/**
 * @brief Retrieves information on posix-times availability.
 * @return AUTO_WIN_MSVC_SUCCESS on success, or error code on NULL pointer.
 */
auto_win_msvc_error_t posix_times_get_info(int *out_available) {
  if (out_available == NULL) {
    return AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT;
  }
  *out_available = 1;
  return AUTO_WIN_MSVC_SUCCESS;
}

typedef int make_iso_compilers_happy_tu_posix_times;

#endif
typedef int make_iso_compilers_happy_mega_batch3_23;
