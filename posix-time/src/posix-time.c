#if !defined(AUTO_WIN_MSVC_MEGA_LIBRARY) || defined(_WIN32) ||                 \
    defined(__DOS__) || defined(__MSDOS__) || defined(__WATCOMC__)
/**
 * @file posix-time.c
 * @brief Strict C89 implementation of POSIX sys/time.h and time.h polyfills.
 */

/* clang-format off */
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0500
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#include <signal.h>
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
#if !(defined(_MSC_VER) && _MSC_VER < 1700)
#include <fileapi.h>
#include <handleapi.h>
#include <minwindef.h>
#include <sysinfoapi.h>
#include <timezoneapi.h>
#endif
#endif
#include "posix-time.h"
/* clang-format on */

#if defined(_WIN32) || defined(__DOS__) || defined(__MSDOS__) ||               \
    defined(__WATCOMC__)
static const char *posix_parse_int(const char *s, int max_digits, int min_val,
                                   int max_val, int *out_val) {
  int val = 0;
  int count = 0;
  while (*s == ' ' || *s == (char)9) {
    s++;
  }
  while (count < max_digits && *s >= '0' && *s <= '9') {
    val = val * 10 + (*s - '0');
    s++;
    count++;
  }
  if (count == 0 || val < min_val || val > max_val) {
    return NULL;
  }
  *out_val = val;
  return s;
}

static char *posix_strptime_impl(const char *buf, const char *fmt,
                                 struct tm *tm) {
  const char *s = buf;
  const char *f = fmt;
  int val = 0;

  if (buf == NULL || fmt == NULL || tm == NULL) {
    errno = EINVAL;
    return NULL;
  }

  while (*f != '\0') {
    if (isspace((unsigned char)*f)) {
      while (isspace((unsigned char)*s)) {
        s++;
      }
      f++;
      continue;
    }
    if (*f != '%') {
      if (*s != *f) {
        return NULL;
      }
      s++;
      f++;
      continue;
    }
    f++;
    if (*f == 'E' || *f == 'O') {
      f++;
    }
    switch (*f) {
    case '%':
      if (*s != '%') {
        return NULL;
      }
      s++;
      break;
    case 'n':
    case 't':
      while (isspace((unsigned char)*s)) {
        s++;
      }
      break;
    case 'Y':
      s = posix_parse_int(s, 4, 0, 9999, &val);
      if (!s) {
        return NULL;
      }
      tm->tm_year = val - 1900;
      break;
    case 'y':
      s = posix_parse_int(s, 2, 0, 99, &val);
      if (!s) {
        return NULL;
      }
      tm->tm_year = (val < 69) ? (val + 100) : val;
      break;
    case 'm':
      s = posix_parse_int(s, 2, 1, 12, &val);
      if (!s) {
        return NULL;
      }
      tm->tm_mon = val - 1;
      break;
    case 'd':
    case 'e':
      s = posix_parse_int(s, 2, 1, 31, &val);
      if (!s) {
        return NULL;
      }
      tm->tm_mday = val;
      break;
    case 'H':
      s = posix_parse_int(s, 2, 0, 23, &val);
      if (!s) {
        return NULL;
      }
      tm->tm_hour = val;
      break;
    case 'I':
      s = posix_parse_int(s, 2, 1, 12, &val);
      if (!s) {
        return NULL;
      }
      tm->tm_hour = val % 12;
      break;
    case 'M':
      s = posix_parse_int(s, 2, 0, 59, &val);
      if (!s) {
        return NULL;
      }
      tm->tm_min = val;
      break;
    case 'S':
      s = posix_parse_int(s, 2, 0, 60, &val);
      if (!s) {
        return NULL;
      }
      tm->tm_sec = val;
      break;
    case 'j':
      s = posix_parse_int(s, 3, 1, 366, &val);
      if (!s) {
        return NULL;
      }
      tm->tm_yday = val - 1;
      break;
    case 'w':
      s = posix_parse_int(s, 1, 0, 6, &val);
      if (!s) {
        return NULL;
      }
      tm->tm_wday = val;
      break;
    case 'F':
      s = posix_strptime_impl(s, "%Y-%m-%d", tm);
      if (!s) {
        return NULL;
      }
      break;
    case 'T':
      s = posix_strptime_impl(s, "%H:%M:%S", tm);
      if (!s) {
        return NULL;
      }
      break;
    case 'R':
      s = posix_strptime_impl(s, "%H:%M", tm);
      if (!s) {
        return NULL;
      }
      break;
    case 'D':
      s = posix_strptime_impl(s, "%m/%d/%y", tm);
      if (!s) {
        return NULL;
      }
      break;
    case 'Z':
      while (isalpha((unsigned char)*s)) {
        s++;
      }
      break;
    case 'z':
      if (*s == 'Z' || *s == 'z') {
        s++;
      } else if (*s == '+' || *s == '-') {
        s++;
        s = posix_parse_int(s, 2, 0, 23, &val);
        if (!s) {
          return NULL;
        }
        if (*s == ':') {
          s++;
        }
        s = posix_parse_int(s, 2, 0, 59, &val);
        if (!s) {
          return NULL;
        }
      } else {
        return NULL;
      }
      break;
    default:
      return NULL;
    }
    f++;
  }
#if defined(__I86__)
  return (char *)(unsigned long)s;
#else
  return (char *)(size_t)s;
#endif
}
#endif

#ifdef _WIN32

#ifndef FILE_FLAG_BACKUP_SEMANTICS
#define FILE_FLAG_BACKUP_SEMANTICS 0x02000000
#endif

#ifndef INVALID_HANDLE_VALUE
#define INVALID_HANDLE_VALUE ((HANDLE)(size_t)-1)
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(__MINGW32__) && !defined(__MINGW64__)
struct _SECURITY_ATTRIBUTES;
__declspec(dllimport) HANDLE WINAPI
CreateWaitableTimerA(struct _SECURITY_ATTRIBUTES *lpTimerAttributes,
                     int bManualReset, const char *lpTimerName);
typedef void(WINAPI *PTIMERAPCROUTINE)(void *, unsigned long, unsigned long);
__declspec(dllimport) int WINAPI
SetWaitableTimer(HANDLE hTimer, const LARGE_INTEGER *pDueTime, long lPeriod,
                 PTIMERAPCROUTINE pfnCompletionRoutine,
                 void *lpArgToCompletionRoutine, int fResume);
__declspec(dllimport) int WINAPI CloseHandle(HANDLE hObject);
__declspec(dllimport) unsigned long WINAPI
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

typedef void(WINAPI *posix_GetSystemTimePreciseAsFileTime_t)(FILETIME *);
typedef HMODULE(WINAPI *time_get_mod_fn)(LPCSTR);
typedef FARPROC(WINAPI *time_get_proc_fn)(HMODULE, LPCSTR);
typedef void(WINAPI *time_get_sys_time_fn)(LPFILETIME);
typedef DWORD(WINAPI *time_get_tz_fn)(LPTIME_ZONE_INFORMATION);
typedef HANDLE(WINAPI *time_create_file_fn)(LPCSTR, DWORD, DWORD,
                                            LPSECURITY_ATTRIBUTES, DWORD, DWORD,
                                            HANDLE);
typedef BOOL(WINAPI *time_set_file_time_fn)(HANDLE, const FILETIME *,
                                            const FILETIME *, const FILETIME *);
typedef BOOL(WINAPI *time_close_handle_fn)(HANDLE);
typedef BOOL(WINAPI *time_create_tqt_fn)(PHANDLE, HANDLE, WAITORTIMERCALLBACK,
                                         PVOID, DWORD, DWORD, ULONG);
typedef BOOL(WINAPI *time_delete_tqt_fn)(HANDLE, HANDLE, HANDLE);
typedef int (*time_raise_fn)(int);

static HMODULE WINAPI default_time_get_mod(LPCSTR name) {
  return GetModuleHandleA(name);
}
static FARPROC WINAPI default_time_get_proc(HMODULE mod, LPCSTR name) {
  return GetProcAddress(mod, name);
}
static void WINAPI default_time_get_sys_time(LPFILETIME ft) {
  GetSystemTimeAsFileTime(ft);
}
static DWORD WINAPI default_time_get_tz(LPTIME_ZONE_INFORMATION tzi) {
  return GetTimeZoneInformation(tzi);
}
static HANDLE WINAPI default_time_create_file(LPCSTR fn, DWORD acc, DWORD sh,
                                              LPSECURITY_ATTRIBUTES sa,
                                              DWORD disp, DWORD fl, HANDLE tm) {
  return CreateFileA(fn, acc, sh, sa, disp, fl, tm);
}
static BOOL WINAPI default_time_set_file_time(HANDLE h, const FILETIME *c,
                                              const FILETIME *a,
                                              const FILETIME *w) {
  return SetFileTime(h, c, a, w);
}
static BOOL WINAPI default_time_close_handle(HANDLE h) {
  return CloseHandle(h);
}
static BOOL WINAPI default_time_create_tqt(PHANDLE ph, HANDLE q,
                                           WAITORTIMERCALLBACK cb, PVOID param,
                                           DWORD due, DWORD per, ULONG fl) {
  return CreateTimerQueueTimer(ph, q, cb, param, due, per, fl);
}
static BOOL WINAPI default_time_delete_tqt(HANDLE q, HANDLE t, HANDLE ev) {
  return DeleteTimerQueueTimer(q, t, ev);
}
static int default_time_raise(int sig) {
  (void)sig;
  return 0;
}
#if !defined(_MSC_VER)
static struct tm *default_time_localtime(const time_t *t) {
  return localtime(t);
}
static struct tm *default_time_gmtime(const time_t *t) { return gmtime(t); }
#endif

static posix_GetSystemTimePreciseAsFileTime_t g_time_precise_fn = NULL;
static int g_time_precise_init = 0;
static time_get_mod_fn g_time_get_mod = default_time_get_mod;
static time_get_proc_fn g_time_get_proc = default_time_get_proc;
static time_get_sys_time_fn g_time_get_sys_time = default_time_get_sys_time;
static time_get_tz_fn g_time_get_tz = default_time_get_tz;
static time_create_file_fn g_time_create_file = default_time_create_file;
static time_set_file_time_fn g_time_set_file_time = default_time_set_file_time;
static time_close_handle_fn g_time_close_handle = default_time_close_handle;
static time_create_tqt_fn g_time_create_tqt = default_time_create_tqt;
static time_delete_tqt_fn g_time_delete_tqt = default_time_delete_tqt;
static time_raise_fn g_time_raise = default_time_raise;
#if !defined(_MSC_VER)
static struct tm *(*g_time_localtime)(const time_t *) = default_time_localtime;
static struct tm *(*g_time_gmtime)(const time_t *) = default_time_gmtime;
#endif

static struct itimerval g_timers[3] = {
    {{0, 0}, {0, 0}}, {{0, 0}, {0, 0}}, {{0, 0}, {0, 0}}};
static HANDLE g_hTimers[3] = {NULL, NULL, NULL};

/**
 * @brief Overrides Win32 kernel/CRT hooks for deterministic posix-time testing.
 */
void posix_time_set_win32_hooks(
    time_get_mod_fn mod_fn, time_get_proc_fn proc_fn,
    time_get_sys_time_fn sys_fn, time_get_tz_fn tz_fn,
    time_create_file_fn cf_fn, time_set_file_time_fn sft_fn,
    time_close_handle_fn ch_fn, time_create_tqt_fn ctqt_fn,
    time_delete_tqt_fn dtqt_fn, time_raise_fn raise_fn);
void posix_time_set_win32_hooks(
    time_get_mod_fn mod_fn, time_get_proc_fn proc_fn,
    time_get_sys_time_fn sys_fn, time_get_tz_fn tz_fn,
    time_create_file_fn cf_fn, time_set_file_time_fn sft_fn,
    time_close_handle_fn ch_fn, time_create_tqt_fn ctqt_fn,
    time_delete_tqt_fn dtqt_fn, time_raise_fn raise_fn) {
  g_time_precise_fn = NULL;
  g_time_precise_init = 0;
  g_time_get_mod = (mod_fn != NULL) ? mod_fn : default_time_get_mod;
  g_time_get_proc = (proc_fn != NULL) ? proc_fn : default_time_get_proc;
  g_time_get_sys_time = (sys_fn != NULL) ? sys_fn : default_time_get_sys_time;
  g_time_get_tz = (tz_fn != NULL) ? tz_fn : default_time_get_tz;
  g_time_create_file = (cf_fn != NULL) ? cf_fn : default_time_create_file;
  g_time_set_file_time = (sft_fn != NULL) ? sft_fn : default_time_set_file_time;
  g_time_close_handle = (ch_fn != NULL) ? ch_fn : default_time_close_handle;
  g_time_create_tqt = (ctqt_fn != NULL) ? ctqt_fn : default_time_create_tqt;
  g_time_delete_tqt = (dtqt_fn != NULL) ? dtqt_fn : default_time_delete_tqt;
  g_time_raise = (raise_fn != NULL) ? raise_fn : default_time_raise;
}

#if !defined(_MSC_VER)
/**
 * @brief Overrides CRT localtime/gmtime hooks on non-MSVC Windows toolchains.
 */
void posix_time_set_crt_tm_hooks(struct tm *(*lt_fn)(const time_t *),
                                 struct tm *(*gm_fn)(const time_t *));
void posix_time_set_crt_tm_hooks(struct tm *(*lt_fn)(const time_t *),
                                 struct tm *(*gm_fn)(const time_t *)) {
  g_time_localtime = (lt_fn != NULL) ? lt_fn : default_time_localtime;
  g_time_gmtime = (gm_fn != NULL) ? gm_fn : default_time_gmtime;
}
#endif

/**
 * @brief Resets all posix-time hooks and cached precise-clock pointers.
 */
void posix_time_reset_hooks(void);
void posix_time_reset_hooks(void) {
  g_time_precise_fn = NULL;
  g_time_precise_init = 0;
  g_time_get_mod = default_time_get_mod;
  g_time_get_proc = default_time_get_proc;
  g_time_get_sys_time = default_time_get_sys_time;
  g_time_get_tz = default_time_get_tz;
  g_time_create_file = default_time_create_file;
  g_time_set_file_time = default_time_set_file_time;
  g_time_close_handle = default_time_close_handle;
  g_time_create_tqt = default_time_create_tqt;
  g_time_delete_tqt = default_time_delete_tqt;
  g_time_raise = default_time_raise;
#if !defined(_MSC_VER)
  g_time_localtime = default_time_localtime;
  g_time_gmtime = default_time_gmtime;
#endif
}

static void posix_get_system_time(FILETIME *ft) {
  if (!g_time_precise_init) {
    HMODULE k = g_time_get_mod("kernel32.dll");
    if (k) {
      g_time_precise_fn =
          (posix_GetSystemTimePreciseAsFileTime_t)(size_t)g_time_get_proc(
              k, "GetSystemTimePreciseAsFileTime");
    }
    g_time_precise_init = 1;
  }
  if (g_time_precise_fn) {
    g_time_precise_fn(ft);
  } else {
    g_time_get_sys_time(ft);
  }
}

int gettimeofday(struct timeval *tv, struct timezone *tz) {
  if (tv) {
    FILETIME ft;
    ULARGE_INTEGER uli;
    posix_get_system_time(&ft);
    uli.LowPart = ft.dwLowDateTime;
    uli.HighPart = ft.dwHighDateTime;
    uli.QuadPart -= POSIX_TIME_EPOCH;
    tv->tv_sec = (long)(uli.QuadPart / POSIX_TIME_10M);
    tv->tv_usec = (long)((uli.QuadPart % POSIX_TIME_10M) / POSIX_TIME_10);
  }

  if (tz) {
    TIME_ZONE_INFORMATION tzi;
    DWORD res;
    res = g_time_get_tz(&tzi);
    tz->tz_minuteswest = tzi.Bias;
    tz->tz_dsttime = (res == TIME_ZONE_ID_DAYLIGHT) ? 1 : 0;
  }

  return 0;
}

int utimes(const char *filename, const struct timeval times[2]) {
  HANDLE hFile;
  FILETIME ftAccess, ftWrite;
  FILETIME *pAccess = NULL, *pWrite = NULL;
  ULARGE_INTEGER uli;

  if (!filename) {
    errno = EINVAL;
    return -1;
  }

  hFile =
      g_time_create_file(filename, FILE_WRITE_ATTRIBUTES,
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
    g_time_get_sys_time(&ftAccess);
    ftWrite = ftAccess;
    pAccess = &ftAccess;
    pWrite = &ftWrite;
  }

  if (!g_time_set_file_time(hFile, NULL, pAccess, pWrite)) {
    g_time_close_handle(hFile);
    errno = EACCES;
    return -1;
  }

  g_time_close_handle(hFile);
  return 0;
}

static void WINAPI posix_timer_callback(PVOID lpParameter,
                                        BOOLEAN TimerOrWaitFired) {
  (void)lpParameter;
  (void)TimerOrWaitFired;
  g_time_raise(14);
}

int getitimer(int which, struct itimerval *value) {
  if (which < ITIMER_REAL || which > ITIMER_PROF || !value) {
    errno = EINVAL;
    return -1;
  }
  *value = g_timers[which];
  return 0;
}

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
    g_time_delete_tqt(NULL, g_hTimers[which], INVALID_HANDLE_VALUE);
    g_hTimers[which] = NULL;
  }

  if (value->it_value.tv_sec != 0 || value->it_value.tv_usec != 0) {
    dueTime =
        (DWORD)(value->it_value.tv_sec * 1000 + value->it_value.tv_usec / 1000);
    period = (DWORD)(value->it_interval.tv_sec * 1000 +
                     value->it_interval.tv_usec / 1000);
    if (!g_time_create_tqt(&g_hTimers[which], NULL, posix_timer_callback,
                           (PVOID)(size_t)which, dueTime, period, 0x00000000)) {
      errno = EINVAL;
      return -1;
    }
  }

  return 0;
}

#if !defined(__MINGW32__) && !defined(__MINGW64__)
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
  errno = EINVAL;
  return -1;
}

int clock_settime(int clk_id, const struct timespec *tp) {
  if (tp == NULL || tp->tv_nsec < 0 || tp->tv_nsec >= 1000000000L) {
    errno = EINVAL;
    return -1;
  }
  if (clk_id != CLOCK_REALTIME) {
    errno = EINVAL;
    return -1;
  }
  errno = EPERM;
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

  li.QuadPart = -((LONGLONG)req->tv_sec * POSIX_TIME_10M + req->tv_nsec / 100);
  if (!SetWaitableTimer(timer, &li, 0, NULL, NULL, 0)) {
    g_time_close_handle(timer);
    errno = EINVAL;
    return -1;
  }

  WaitForSingleObject(timer, INFINITE);
  g_time_close_handle(timer);

  if (rem) {
    rem->tv_sec = 0;
    rem->tv_nsec = 0;
  }

  return 0;
}
#endif /* !__MINGW32__ */

struct tm *localtime_r(const time_t *timep, struct tm *result) {
  if (!timep || !result) {
    return NULL;
  }
#if defined(_MSC_VER)
  if (!localtime_s(result, timep)) {
    return result;
  }
  return NULL;
#else
  {
    struct tm *res = g_time_localtime(timep);
    if (!res) {
      return NULL;
    }
    *result = *res;
    return result;
  }
#endif
}

struct tm *gmtime_r(const time_t *timep, struct tm *result) {
  if (!timep || !result) {
    return NULL;
  }
#if defined(_MSC_VER)
  if (!gmtime_s(result, timep)) {
    return result;
  }
  return NULL;
#else
  {
    struct tm *res = g_time_gmtime(timep);
    if (!res) {
      return NULL;
    }
    *result = *res;
    return result;
  }
#endif
}

char *strptime(const char *buf, const char *fmt, struct tm *tm) {
  return posix_strptime_impl(buf, fmt, tm);
}

#elif defined(__DOS__) || defined(__MSDOS__) || defined(__WATCOMC__)

#ifndef ENOSYS
#define ENOSYS 38
#endif

static struct itimerval g_dos_timers[3] = {
    {{0, 0}, {0, 0}}, {{0, 0}, {0, 0}}, {{0, 0}, {0, 0}}};

int getitimer(int which, struct itimerval *value) {
  if (which < ITIMER_REAL || which > ITIMER_PROF || !value) {
    errno = EINVAL;
    return -1;
  }
  *value = g_dos_timers[which];
  return 0;
}

int gettimeofday(struct timeval *tv, struct timezone *tz) {
  if (tv) {
    clock_t c = clock();
    tv->tv_sec = (long)time(NULL);
    tv->tv_usec = (long)((c % CLOCKS_PER_SEC) * (1000000L / CLOCKS_PER_SEC));
  }
  if (tz) {
    tz->tz_minuteswest = 0;
    tz->tz_dsttime = 0;
  }
  return 0;
}

int setitimer(int which, const struct itimerval *value,
              struct itimerval *ovalue) {
  if (which < ITIMER_REAL || which > ITIMER_PROF || !value) {
    errno = EINVAL;
    return -1;
  }
  if (ovalue) {
    *ovalue = g_dos_timers[which];
  }
  g_dos_timers[which] = *value;
  return 0;
}

int utimes(const char *filename, const struct timeval times[2]) {
  FILE *f;
  if (filename == NULL) {
    errno = EINVAL;
    return -1;
  }
  (void)times;
  f = fopen(filename, "rb");
  if (!f) {
    errno = ENOENT;
    return -1;
  }
  fclose(f);
  return 0;
}

int clock_gettime(int clk_id, struct timespec *tp) {
  if (tp == NULL) {
    errno = EINVAL;
    return -1;
  }
  if (clk_id == CLOCK_REALTIME) {
    clock_t c = clock();
    tp->tv_sec = time(NULL);
    tp->tv_nsec = (long)((c % CLOCKS_PER_SEC) * (1000000000L / CLOCKS_PER_SEC));
    return 0;
  } else if (clk_id == CLOCK_MONOTONIC) {
    clock_t c = clock();
    tp->tv_sec = (time_t)(c / CLOCKS_PER_SEC);
    tp->tv_nsec = (long)((c % CLOCKS_PER_SEC) * (1000000000L / CLOCKS_PER_SEC));
    return 0;
  }
  errno = EINVAL;
  return -1;
}

int clock_settime(int clk_id, const struct timespec *tp) {
  if (tp == NULL || tp->tv_nsec < 0 || tp->tv_nsec >= 1000000000L) {
    errno = EINVAL;
    return -1;
  }
  if (clk_id != CLOCK_REALTIME) {
    errno = EINVAL;
    return -1;
  }
  errno = EPERM;
  return -1;
}

int nanosleep(const struct timespec *req, struct timespec *rem) {
  if (req == NULL || (long)req->tv_sec < 0 || req->tv_nsec < 0 ||
      req->tv_nsec >= 1000000000L) {
    errno = EINVAL;
    return -1;
  }
  if (rem != NULL) {
    rem->tv_sec = 0;
    rem->tv_nsec = 0;
  }
  return 0;
}

struct tm *localtime_r(const time_t *timep, struct tm *result) {
  struct tm *res;
  if (!timep || !result) {
    return NULL;
  }
  res = localtime(timep);
  if (!res) {
    return NULL;
  }
  *result = *res;
  return result;
}

struct tm *gmtime_r(const time_t *timep, struct tm *result) {
  struct tm *res;
  if (!timep || !result) {
    return NULL;
  }
  res = gmtime(timep);
  if (!res) {
    return NULL;
  }
  *result = *res;
  return result;
}

char *strptime(const char *buf, const char *fmt, struct tm *tm) {
  return posix_strptime_impl(buf, fmt, tm);
}

#elif defined(__CYGWIN__)
#undef clock_settime
int posix_cygwin_clock_settime(clockid_t clk_id, const struct timespec *tp) {
  if (tp == NULL || tp->tv_nsec < 0 || tp->tv_nsec >= 1000000000L) {
    errno = EINVAL;
    return -1;
  }
  return clock_settime(clk_id, tp);
}

#endif /* _WIN32 */

/**
 * @brief Retrieves information on posix-time module availability.
 * @return AUTO_WIN_MSVC_SUCCESS on success, or
 * AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT on NULL pointer.
 */
auto_win_msvc_error_t posix_time_get_info(int *out_available) {
  if (out_available == NULL) {
    return AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT;
  }
  *out_available = 1;
  return AUTO_WIN_MSVC_SUCCESS;
}

/**
 * @brief Dummy symbol ensuring non-empty translation unit across linkers.
 * @return Always returns 0.
 */
int dummy_posix_time(void);
int dummy_posix_time(void) { return 0; }

typedef int make_iso_compilers_happy_tu_posix_time;

#endif
typedef int make_iso_compilers_happy_mega_batch3_22;
