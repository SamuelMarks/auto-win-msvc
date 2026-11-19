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

#if defined(_MSC_VER) && _MSC_VER < 1600
/** @brief 32-bit signed integer type for time conversion. */
typedef __int32 bsd_tc_int32_t;
/** @brief 64-bit signed integer type for time conversion. */
typedef __int64 bsd_tc_int64_t;
#else
/** @brief 32-bit signed integer type for time conversion. */
typedef int32_t bsd_tc_int32_t;
/** @brief 64-bit signed integer type for time conversion. */
typedef int64_t bsd_tc_int64_t;
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Converts time_t to 32-bit signed integer. */
static __inline bsd_tc_int32_t _time_to_time32(time_t t) {
  return (bsd_tc_int32_t)t;
}
/** @brief Converts 32-bit signed integer to time_t. */
static __inline time_t _time32_to_time(bsd_tc_int32_t t32) {
  return (time_t)t32;
}
/** @brief Converts time_t to 64-bit signed integer. */
static __inline bsd_tc_int64_t _time_to_time64(time_t t) {
  return (bsd_tc_int64_t)t;
}
/** @brief Converts 64-bit signed integer to time_t. */
static __inline time_t _time64_to_time(bsd_tc_int64_t t64) {
  return (time_t)t64;
}
/** @brief Converts time_t to long. */
static __inline long _time_to_long(time_t t) { return (long)t; }
/** @brief Converts long to time_t. */
static __inline time_t _long_to_time(long tl) { return (time_t)tl; }
/** @brief Converts time_t to int. */
static __inline int _time_to_int(time_t t) { return (int)t; }
/** @brief Converts int to time_t. */
static __inline time_t _int_to_time(int ti) { return (time_t)ti; }

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* BSD_TIMECONV_H */
