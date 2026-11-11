#ifndef BSD_TIMECONV_H
#define BSD_TIMECONV_H

/**
 * @file timeconv.h
 * @brief FreeBSD <timeconv.h> time_t conversion helpers.
 */

/* clang-format off */
#include <sys/types.h>
#include <time.h>
#if !defined(_MSC_VER) || _MSC_VER >= 1600
#include <stdint.h>
#endif
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

static __inline int32_t _time_to_time32(time_t t) { return (int32_t)t; }
static __inline time_t _time32_to_time(int32_t t32) { return (time_t)t32; }
static __inline int64_t _time_to_time64(time_t t) { return (int64_t)t; }
static __inline time_t _time64_to_time(int64_t t64) { return (time_t)t64; }
static __inline long _time_to_long(time_t t) { return (long)t; }
static __inline time_t _long_to_time(long tl) { return (time_t)tl; }
static __inline int _time_to_int(time_t t) { return (int)t; }
static __inline time_t _int_to_time(int ti) { return (time_t)ti; }

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* BSD_TIMECONV_H */
