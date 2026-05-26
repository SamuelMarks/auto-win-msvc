/* posix-time.c - Strict C89 Implementation */
/* clang-format off */
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#ifdef _WIN32
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0500
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#include <signal.h>

#endif
#include "posix-time.h"

#ifdef _WIN32
#if defined(_M_IX86)
#ifndef _X86_
#define _X86_
#endif
#elif defined(_M_AMD64)
#ifndef _AMD64_
#define _AMD64_
#endif
#elif defined(_M_ARM64)
#ifndef _ARM64_
#define _ARM64_
#endif
#elif defined(_M_ARM)
#ifndef _ARM_
#define _ARM_
#endif
#endif

#if defined(_MSC_VER) && _MSC_VER < 1700
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#else
#include <fileapi.h>
#include <handleapi.h>
#include <minwindef.h>
#include <sysinfoapi.h>
#include <timezoneapi.h>
/* clang-format on */
#endif

#ifndef FILE_FLAG_BACKUP_SEMANTICS
#define FILE_FLAG_BACKUP_SEMANTICS 0x02000000
#endif

#ifndef INVALID_HANDLE_VALUE
/** \brief INVALID_HANDLE_VALUE macro. */
#define INVALID_HANDLE_VALUE ((HANDLE)(size_t)-1)
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_MSC_VER) && !defined(_WIN64)
#define POSIX_TIME_STDCALL __stdcall
#else
#define POSIX_TIME_STDCALL
#endif

#if !defined(__MINGW32__) && !defined(__MINGW64__)
struct _SECURITY_ATTRIBUTES;
__declspec(dllimport) HANDLE POSIX_TIME_STDCALL
    CreateWaitableTimerA(struct _SECURITY_ATTRIBUTES *lpTimerAttributes,
                         int bManualReset, const char *lpTimerName);
typedef void(POSIX_TIME_STDCALL *PTIMERAPCROUTINE)(void *, unsigned long,
                                                   unsigned long);
__declspec(dllimport) int POSIX_TIME_STDCALL
    SetWaitableTimer(HANDLE hTimer, const LARGE_INTEGER *pDueTime, long lPeriod,
                     PTIMERAPCROUTINE pfnCompletionRoutine,
                     void *lpArgToCompletionRoutine, int fResume);
__declspec(dllimport) int POSIX_TIME_STDCALL CloseHandle(HANDLE hObject);
__declspec(dllimport) unsigned long POSIX_TIME_STDCALL
    WaitForSingleObject(HANDLE hHandle, unsigned long dwMilliseconds);
#endif

#ifdef __cplusplus
}
#endif

#if defined(_MSC_VER)
#define POSIX_TIME_EPOCH 116444736000000000i64
#define POSIX_TIME_10M 10000000i64
#define POSIX_TIME_10 10i64
#elif defined(__GNUC__)
#define POSIX_TIME_EPOCH __extension__ 116444736000000000LL
#define POSIX_TIME_10M __extension__ 10000000LL
#define POSIX_TIME_10 __extension__ 10LL
#else
#define POSIX_TIME_EPOCH 116444736000000000LL
#define POSIX_TIME_10M 10000000LL
#define POSIX_TIME_10 10LL
#endif

#ifdef _WIN32
typedef void(__stdcall *posix_GetSystemTimePreciseAsFileTime_t)(FILETIME *);
static void posix_get_system_time(FILETIME *ft) {
  static posix_GetSystemTimePreciseAsFileTime_t p = NULL;
  static int init = 0;
  if (!init) {
    HMODULE k = GetModuleHandleA("kernel32.dll");
    if (k)
      p = (posix_GetSystemTimePreciseAsFileTime_t)(size_t)GetProcAddress(
          k, "GetSystemTimePreciseAsFileTime");
    init = 1;
  }
  if (p)
    p(ft);
  else
    GetSystemTimeAsFileTime(ft);
}
#endif

/* Polyfill for gettimeofday using GetSystemTimeAsFileTime */
int gettimeofday(struct timeval *tv, struct timezone *tz) {
  if (tv) {
    FILETIME ft;
    ULARGE_INTEGER uli;
    posix_get_system_time(&ft);
    uli.LowPart = ft.dwLowDateTime;
    uli.HighPart = ft.dwHighDateTime;
    /* Convert from 100-nanosecond intervals since 1601 to microseconds since
     * 1970 */
    uli.QuadPart -= POSIX_TIME_EPOCH;
    tv->tv_sec = (long)(uli.QuadPart / POSIX_TIME_10M);
    tv->tv_usec = (long)((uli.QuadPart % POSIX_TIME_10M) / POSIX_TIME_10);
  }

  if (tz) {
    TIME_ZONE_INFORMATION tzi;
    DWORD res;
    res = GetTimeZoneInformation(&tzi);
    tz->tz_minuteswest = tzi.Bias;
    tz->tz_dsttime = (res == TIME_ZONE_ID_DAYLIGHT) ? 1 : 0;
  }

  return 0;
}

/* Polyfill for utimes using SetFileTime */
int utimes(const char *filename, const struct timeval times[2]) {
  HANDLE hFile;
  FILETIME ftAccess, ftWrite;
  FILETIME *pAccess = NULL, *pWrite = NULL;
  ULARGE_INTEGER uli;

  if (!filename) {
    errno = EINVAL;
    return -1;
  }

  hFile = CreateFileA(filename, FILE_WRITE_ATTRIBUTES,
                      FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                      NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
  if (hFile == INVALID_HANDLE_VALUE) {
    errno = ENOENT;
    return -1;
  }

  if (times) {
    uli.QuadPart = (ULONGLONG)times[0].tv_sec * POSIX_TIME_10M +
                   (ULONGLONG)times[0].tv_usec * POSIX_TIME_10 +
                   POSIX_TIME_EPOCH;
    ftAccess.dwLowDateTime = uli.LowPart;
    ftAccess.dwHighDateTime = uli.HighPart;
    pAccess = &ftAccess;

    uli.QuadPart = (ULONGLONG)times[1].tv_sec * POSIX_TIME_10M +
                   (ULONGLONG)times[1].tv_usec * POSIX_TIME_10 +
                   POSIX_TIME_EPOCH;
    ftWrite.dwLowDateTime = uli.LowPart;
    ftWrite.dwHighDateTime = uli.HighPart;
    pWrite = &ftWrite;
  } else {
    GetSystemTimeAsFileTime(&ftAccess);
    ftWrite = ftAccess;
    pAccess = &ftAccess;
    pWrite = &ftWrite;
  }

  if (!SetFileTime(hFile, NULL, pAccess, pWrite)) {
    CloseHandle(hFile);
    errno = EACCES;
    return -1;
  }

  CloseHandle(hFile);
  return 0;
}

#ifdef _WIN32
static struct itimerval g_timers[3] = {
    {{0, 0}, {0, 0}}, {{0, 0}, {0, 0}}, {{0, 0}, {0, 0}}};
static HANDLE g_hTimers[3] = {NULL, NULL, NULL};

static void WINAPI posix_timer_callback(PVOID lpParameter,
                                        BOOLEAN TimerOrWaitFired) {
  int which = (int)(size_t)lpParameter;
  (void)TimerOrWaitFired;
  (void)which;
  /* Ideally we would raise(SIGALRM) here, but MSVC's raise() aborts on
   * unsupported signals. We can attempt to call raise(14) (SIGALRM) if
   * the environment intercepts it, but we'll use a generic approach:
   */
  raise(14); /* SIGALRM = 14 */
}

int getitimer(int which, struct itimerval *value) {
  if (which < ITIMER_REAL || which > ITIMER_PROF || !value) {
    errno = EINVAL;
    return -1;
  }
  *value = g_timers[which];
  return 0;
}

/** \brief setitimer function. */
int setitimer(int which, const struct itimerval *value,
              struct itimerval *ovalue) {
  DWORD dueTime = 0;
  DWORD period = 0;

  if (which < ITIMER_REAL || which > ITIMER_PROF || !value) {
    errno = EINVAL;
    return -1;
  }

  if (ovalue) {
    *ovalue = g_timers[which];
  }
  g_timers[which] = *value;

  if (g_hTimers[which]) {
    DeleteTimerQueueTimer(NULL, g_hTimers[which], INVALID_HANDLE_VALUE);
    g_hTimers[which] = NULL;
  }

  if (value->it_value.tv_sec != 0 || value->it_value.tv_usec != 0) {
    dueTime =
        (DWORD)(value->it_value.tv_sec * 1000 + value->it_value.tv_usec / 1000);
    period = (DWORD)(value->it_interval.tv_sec * 1000 +
                     value->it_interval.tv_usec / 1000);
    if (!CreateTimerQueueTimer(&g_hTimers[which], NULL, posix_timer_callback,
                               (PVOID)(size_t)which, dueTime, period,
                               0x00000000 /* WT_EXECUTEDEFAULT */)) {
      errno = EINVAL;
      return -1;
    }
  }

  return 0;
}
#else
/* Minimal stubs for non-Windows (Cygwin/Linux native POSIX functions are used)
 */
static struct itimerval g_timers[3] = {
    {{0, 0}, {0, 0}}, {{0, 0}, {0, 0}}, {{0, 0}, {0, 0}}};
int getitimer(int which, struct itimerval *value) {
  (void)which;
  (void)value;
  return 0;
}
int setitimer(int which, const struct itimerval *value,
              struct itimerval *ovalue) {
  (void)which;
  (void)value;
  (void)ovalue;
  return 0;
}
#endif

#if !defined(__MINGW32__) && !defined(__MINGW64__)
/** \brief clock_gettime function. */
int clock_gettime(int clk_id, struct timespec *tp) {
  if (!tp) {
    errno = EINVAL;
    return -1;
  }
  if (clk_id == CLOCK_REALTIME) {
    FILETIME ft;
    ULARGE_INTEGER uli;
    posix_get_system_time(&ft);
    uli.LowPart = ft.dwLowDateTime;
    uli.HighPart = ft.dwHighDateTime;
    uli.QuadPart -= POSIX_TIME_EPOCH;
    tp->tv_sec = (time_t)(uli.QuadPart / POSIX_TIME_10M);
    tp->tv_nsec = (long)((uli.QuadPart % POSIX_TIME_10M) * 100);
    return 0;
  } else if (clk_id == CLOCK_MONOTONIC) {
    LARGE_INTEGER count, freq;
    if (QueryPerformanceFrequency(&freq) && QueryPerformanceCounter(&count)) {
      tp->tv_sec = (time_t)(count.QuadPart / freq.QuadPart);
      tp->tv_nsec = (long)(((count.QuadPart % freq.QuadPart) * 1000000000) /
                           freq.QuadPart);
      return 0;
    }
  }
  /* Fallback or unsupported clock ID */
  errno = EINVAL;
  return -1;
}

int nanosleep(const struct timespec *req, struct timespec *rem) {
  HANDLE timer;
  LARGE_INTEGER li;

  if (!req) {
    errno = EFAULT;
    return -1;
  }

  timer = CreateWaitableTimerA(NULL, TRUE, NULL);
  if (!timer) {
    errno = EINVAL;
    return -1;
  }

  /* Negative value for relative time in 100-nanosecond intervals */
  li.QuadPart = -((LONGLONG)req->tv_sec * POSIX_TIME_10M + req->tv_nsec / 100);
  if (!SetWaitableTimer(timer, &li, 0, NULL, NULL, 0)) {
    CloseHandle(timer);
    errno = EINVAL;
    return -1;
  }

  WaitForSingleObject(timer, INFINITE);
  CloseHandle(timer);

  if (rem) {
    rem->tv_sec = 0;
    rem->tv_nsec = 0;
  }

  return 0;
}
#endif /* !__MINGW32__ */

struct tm *localtime_r(const time_t *timep, struct tm *result) {
  if (!timep || !result)
    return NULL;
#if defined(_MSC_VER)
  if (localtime_s(result, timep) == 0)
    return result;
  return NULL;
#else
  {
    struct tm *res = localtime(timep);
    if (!res)
      return NULL;
    *result = *res;
    return result;
  }
#endif
}

#elif defined(__MSDOS__) || defined(__WATCOMC__)

#ifndef ENOSYS
#define ENOSYS 38
#endif

int getitimer(int which, struct itimerval *value) {
  (void)which;
  (void)value;
  errno = ENOSYS;
  return -1;
}

int gettimeofday(struct timeval *tv, struct timezone *tz) {
  (void)tv;
  (void)tz;
  errno = ENOSYS;
  return -1;
}

int setitimer(int which, const struct itimerval *value,
              struct itimerval *ovalue) {
  (void)which;
  (void)value;
  (void)ovalue;
  errno = ENOSYS;
  return -1;
}

int utimes(const char *filename, const struct timeval times[2]) {
  (void)filename;
  (void)times;
  errno = ENOSYS;
  return -1;
}

int clock_gettime(int clk_id, struct timespec *tp) {
  (void)clk_id;
  (void)tp;
  errno = ENOSYS;
  return -1;
}

int nanosleep(const struct timespec *req, struct timespec *rem) {
  (void)req;
  (void)rem;
  errno = ENOSYS;
  return -1;
}

struct tm *localtime_r(const time_t *timep, struct tm *result) {
  (void)timep;
  (void)result;
  errno = ENOSYS;
  return NULL;
}

#endif /* _WIN32 */
/* Prevent empty translation unit */
typedef int make_iso_compilers_happy_tu;

/* Dummy function to prevent empty translation unit */
int dummy_posix_time(void) { return 0; }

typedef int make_iso_compilers_happy_tu_posix_time;
