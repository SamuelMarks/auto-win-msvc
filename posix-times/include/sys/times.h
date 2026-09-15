#ifndef POSIX_SYS_TIMES_H
#define POSIX_SYS_TIMES_H

/**
 * @file sys/times.h
 * @brief POSIX <sys/times.h> compatibility definitions and functions.
 */

/* clang-format off */
#include <time.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_MSC_VER) || defined(_WIN32)

#ifndef _STRUCT_TMS_DEFINED
#define _STRUCT_TMS_DEFINED
/**
 * @struct tms
 * @brief Structure containing process execution timing information.
 */
struct tms {
  /** @brief User CPU time. */
  clock_t tms_utime;
  /** @brief System CPU time. */
  clock_t tms_stime;
  /** @brief User CPU time of terminated children. */
  clock_t tms_cutime;
  /** @brief System CPU time of terminated children. */
  clock_t tms_cstime;
};
#endif /* _STRUCT_TMS_DEFINED */

/**
 * @brief Retrieves process and child execution times.
 * @param buf Pointer to a struct tms to receive process times.
 * @return Elapsed real time in clock ticks on success, or (clock_t)-1 on error.
 */
clock_t posix_times(struct tms *buf);

#ifndef times
/** @brief Macro mapping times to posix_times. */
#define times posix_times
#endif

#endif /* defined(_MSC_VER) || defined(_WIN32) */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* POSIX_SYS_TIMES_H */
