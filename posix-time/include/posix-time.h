/* posix-time.h - Strict C89 Header */
#ifndef POSIX_TIME_H
#define POSIX_TIME_H

/* clang-format off */
#include "auto-win-msvc-error.h"

#if defined(_WIN32) || defined(__MSDOS__) || defined(__WATCOMC__)
#if defined(_WIN32)

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#endif
#else /* _WIN32 */


#endif

/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Retrieves information on posix-time module availability.
 * @param[out] out_available Pointer to integer receiving availability status
 * (1).
 * @return AUTO_WIN_MSVC_SUCCESS on success, or
 * AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT on NULL pointer.
 */
auto_win_msvc_error_t posix_time_get_info(int *out_available);

#if defined(_WIN32) || defined(__MSDOS__) || defined(__WATCOMC__)

#if defined(_WIN32)

#endif

#include <time.h>

#if defined(_MSC_VER) && _MSC_VER >= 1900
/* UCRT defines struct timespec in time.h */
#else
#ifndef _TIMESPEC_DEFINED
#define _TIMESPEC_DEFINED
/**
 * @brief Structure for representing time with nanosecond precision.
 */
struct timespec {
  time_t tv_sec; /**< Seconds. */
  long tv_nsec;  /**< Nanoseconds. */
};
#endif
#endif

#ifndef _WINSOCK2API_
#ifndef _TIMEVAL_DEFINED
#define _TIMEVAL_DEFINED
/**
 * @brief Structure for representing time with microsecond precision.
 */
struct timeval {
  long tv_sec;  /**< Seconds. */
  long tv_usec; /**< Microseconds. */
};
#endif
#endif

#ifndef _TIMEZONE_DEFINED
#define _TIMEZONE_DEFINED
/**
 * @brief Structure for representing timezone information (obsolete).
 */
struct timezone {
  int tz_minuteswest; /**< Minutes west of Greenwich. */
  int tz_dsttime;     /**< Type of DST correction. */
};
#endif

#ifndef _ITIMERVAL_DEFINED
#define _ITIMERVAL_DEFINED
/**
 * @brief Structure for configuring an interval timer.
 */
struct itimerval {
  struct timeval it_interval; /**< Timer interval. */
  struct timeval it_value;    /**< Current value. */
};
#endif

#ifndef timeradd
/**
 * @brief Adds timeval structures: result = a + b.
 */
#define timeradd(a, b, result)                                                 \
  do {                                                                         \
    (result)->tv_sec = (a)->tv_sec + (b)->tv_sec;                              \
    (result)->tv_usec = (a)->tv_usec + (b)->tv_usec;                           \
    if ((result)->tv_usec >= 1000000L) {                                       \
      ++(result)->tv_sec;                                                      \
      (result)->tv_usec -= 1000000L;                                           \
    }                                                                          \
  } while (0)
#endif

#ifndef timersub
/**
 * @brief Subtracts timeval structures: result = a - b.
 */
#define timersub(a, b, result)                                                 \
  do {                                                                         \
    (result)->tv_sec = (a)->tv_sec - (b)->tv_sec;                              \
    (result)->tv_usec = (a)->tv_usec - (b)->tv_usec;                           \
    if ((result)->tv_usec < 0L) {                                              \
      --(result)->tv_sec;                                                      \
      (result)->tv_usec += 1000000L;                                           \
    }                                                                          \
  } while (0)
#endif

#ifndef timerclear
/**
 * @brief Clears a timeval structure to zero.
 */
#define timerclear(tvp) ((tvp)->tv_sec = (tvp)->tv_usec = 0)
#endif

#ifndef timerisset
/**
 * @brief Tests whether a timeval structure is non-zero.
 */
#define timerisset(tvp) ((tvp)->tv_sec != 0 || (tvp)->tv_usec != 0)
#endif

#ifndef timercmp
/**
 * @brief Compares two timeval structures using comparison operator CMP.
 */
#define timercmp(a, b, CMP)                                                    \
  (((a)->tv_sec == (b)->tv_sec) ? ((a)->tv_usec CMP(b)->tv_usec)               \
                                : ((a)->tv_sec CMP(b)->tv_sec))
#endif

/* Interval timer definitions */
#define ITIMER_REAL                                                            \
  0 /**< Decrements in real time, and delivers SIGALRM upon expiration. */
#define ITIMER_VIRTUAL                                                         \
  1 /**< Decrements only when the process is executing, and delivers SIGVTALRM \
       upon expiration. */
#define ITIMER_PROF                                                            \
  2 /**< Decrements both when the process executes and when the system is      \
       executing on behalf of the process. */

/* Clock Types */
#ifndef CLOCK_REALTIME
#define CLOCK_REALTIME 0
#endif
#ifndef CLOCK_MONOTONIC
#define CLOCK_MONOTONIC 1
#endif
#ifndef CLOCK_PROCESS_CPUTIME_ID
#define CLOCK_PROCESS_CPUTIME_ID 2
#endif
#ifndef CLOCK_THREAD_CPUTIME_ID
#define CLOCK_THREAD_CPUTIME_ID 3
#endif

#if defined(_MSC_VER)

#endif
#define tzset _tzset     /**< Map tzset to _tzset on Windows */
#define utimbuf _utimbuf /**< Map utimbuf to _utimbuf on Windows */

/* Functions that require polyfill on Windows */

/**
 * @brief Format string for 64-bit integers, accommodating different compilers.
 */

/**
 * @brief Gets the value of an interval timer.
 * @param which The timer to get (e.g., ITIMER_REAL).
 * @param value A pointer to an itimerval structure to store the value.
 * @return 0 on success, -1 on error.
 */
int getitimer(int which, struct itimerval *value);

/**
 * @brief Gets the current time of day.
 * @param tv A pointer to a timeval structure to store the time.
 * @param tz A pointer to a timezone structure to store the timezone (obsolete,
 * usually NULL).
 * @return 0 on success, -1 on error.
 */
int gettimeofday(struct timeval *tv, struct timezone *tz);

/**
 * @brief Sets the value of an interval timer.
 * @param which The timer to set (e.g., ITIMER_REAL).
 * @param value A pointer to an itimerval structure containing the new value.
 * @param ovalue A pointer to an itimerval structure to store the old value
 * (optional).
 * @return 0 on success, -1 on error.
 */
int setitimer(int which, const struct itimerval *value,
              struct itimerval *ovalue);

/**
 * @brief Sets the access and modification times of a file with microsecond
 * precision.
 * @param filename The name of the file.
 * @param times An array of two timeval structures (access time, modification
 * time). If NULL, times are set to current time.
 * @return 0 on success, -1 on error.
 */
int utimes(const char *filename, const struct timeval times[2]);

/**
 * @brief Get the current time of the specified clock.
 * @param clk_id The clock ID (e.g., CLOCK_REALTIME, CLOCK_MONOTONIC).
 * @param tp A pointer to a timespec structure to store the time.
 * @return 0 on success, -1 on error.
 */
int clock_gettime(int clk_id, struct timespec *tp);

/**
 * @brief Set the time of the specified clock.
 * @param clk_id The clock ID (e.g., CLOCK_REALTIME).
 * @param tp A pointer to a timespec structure containing the new time.
 * @return 0 on success, -1 on error.
 */
int clock_settime(int clk_id, const struct timespec *tp);

/**
 * @brief High-resolution sleep with nanosecond precision.
 * @param req The requested time to sleep.
 * @param rem The remaining time if interrupted (optional).
 * @return 0 on success, -1 on error.
 */
int nanosleep(const struct timespec *req, struct timespec *rem);

/**
 * @brief Thread-safe version of localtime.
 * @param timep A pointer to the time_t value to convert.
 * @param result A pointer to a tm structure to store the result.
 * @return A pointer to the result, or NULL on error.
 */
struct tm *localtime_r(const time_t *timep, struct tm *result);

/**
 * @brief Parse a time string into a struct tm.
 * @param buf The string to parse.
 * @param fmt The format string.
 * @param tm The tm structure to store the parsed time.
 * @return A pointer to the first character not processed in buf, or NULL on
 * error.
 */
char *strptime(const char *buf, const char *fmt, struct tm *tm);

#else /* _WIN32 */

#endif /* _WIN32 */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* POSIX_TIME_H */
