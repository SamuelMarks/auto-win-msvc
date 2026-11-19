#if defined(__GNUC__)
#endif
#if !defined(_WIN32) && !defined(__WATCOMC__) && !defined(__MSDOS__) &&        \
    !defined(__DOS__)
#if defined(__GNUC__) || defined(__clang__)
/* clang-format off */
#pragma GCC system_header
#include_next <sys/time.h>
#else
#include <sys/time.h>
#endif
#else
#include "posix-time.h"
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

/* API Contract symbols: getitimer, gettimeofday, utimes, __dependencies__,
 * __include_next__ */

#ifndef timespecclear
#define timespecclear(tvp) ((tvp)->tv_sec = (tvp)->tv_nsec = 0)
#endif
#ifndef timespecisset
#define timespecisset(tvp) ((tvp)->tv_sec || (tvp)->tv_nsec)
#endif
#ifndef timespeccmp
#define timespeccmp(tvp, uvp, cmp)                                             \
  (((tvp)->tv_sec == (uvp)->tv_sec) ? ((tvp)->tv_nsec cmp(uvp)->tv_nsec)       \
                                    : ((tvp)->tv_sec cmp(uvp)->tv_sec))
#endif
#ifndef timespecadd
#define timespecadd(tsp, usp, vsp)                                             \
  do {                                                                         \
    (vsp)->tv_sec = (tsp)->tv_sec + (usp)->tv_sec;                             \
    (vsp)->tv_nsec = (tsp)->tv_nsec + (usp)->tv_nsec;                          \
    if ((vsp)->tv_nsec >= 1000000000L) {                                       \
      (vsp)->tv_sec++;                                                         \
      (vsp)->tv_nsec -= 1000000000L;                                           \
    }                                                                          \
  } while (0)
#endif
#ifndef timespecsub
#define timespecsub(tsp, usp, vsp)                                             \
  do {                                                                         \
    (vsp)->tv_sec = (tsp)->tv_sec - (usp)->tv_sec;                             \
    (vsp)->tv_nsec = (tsp)->tv_nsec - (usp)->tv_nsec;                          \
    if ((vsp)->tv_nsec < 0) {                                                  \
      (vsp)->tv_sec--;                                                         \
      (vsp)->tv_nsec += 1000000000L;                                           \
    }                                                                          \
  } while (0)
#endif
