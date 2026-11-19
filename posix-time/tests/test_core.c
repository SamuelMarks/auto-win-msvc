/* clang-format off */
#include "greatest.h"
#include <stdio.h>
#include <string.h>
#if defined(_WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
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
#else
#include <sys/time.h>
#include <time.h>
#endif
#include "posix-time.h"
/* clang-format on */

extern int dummy_posix_time(void);

#ifdef _WIN32
typedef HMODULE(WINAPI *test_time_get_mod_fn)(LPCSTR);
typedef FARPROC(WINAPI *test_time_get_proc_fn)(HMODULE, LPCSTR);
typedef void(WINAPI *test_time_get_sys_time_fn)(LPFILETIME);
typedef DWORD(WINAPI *test_time_get_tz_fn)(LPTIME_ZONE_INFORMATION);
typedef HANDLE(WINAPI *test_time_create_file_fn)(LPCSTR, DWORD, DWORD,
                                                 LPSECURITY_ATTRIBUTES, DWORD,
                                                 DWORD, HANDLE);
typedef BOOL(WINAPI *test_time_set_file_time_fn)(HANDLE, const FILETIME *,
                                                 const FILETIME *,
                                                 const FILETIME *);
typedef BOOL(WINAPI *test_time_close_handle_fn)(HANDLE);
typedef BOOL(WINAPI *test_time_create_tqt_fn)(PHANDLE, HANDLE,
                                              WAITORTIMERCALLBACK, PVOID, DWORD,
                                              DWORD, ULONG);
typedef BOOL(WINAPI *test_time_delete_tqt_fn)(HANDLE, HANDLE, HANDLE);
typedef int (*test_time_raise_fn)(int);

extern void posix_time_set_win32_hooks(
    test_time_get_mod_fn mod_fn, test_time_get_proc_fn proc_fn,
    test_time_get_sys_time_fn sys_fn, test_time_get_tz_fn tz_fn,
    test_time_create_file_fn cf_fn, test_time_set_file_time_fn sft_fn,
    test_time_close_handle_fn ch_fn, test_time_create_tqt_fn ctqt_fn,
    test_time_delete_tqt_fn dtqt_fn, test_time_raise_fn raise_fn);
extern void posix_time_reset_hooks(void);

#if !defined(_MSC_VER)
extern void posix_time_set_crt_tm_hooks(struct tm *(*lt_fn)(const time_t *),
                                        struct tm *(*gm_fn)(const time_t *));
static struct tm *mock_tm_null(const time_t *t) {
  (void)t;
  return NULL;
}
#endif

static HMODULE g_mock_mod_ret = NULL;
static FARPROC g_mock_proc_ret = NULL;
static DWORD g_mock_tz_ret = TIME_ZONE_ID_STANDARD;
static BOOL g_mock_sft_ret = 0;
static BOOL g_mock_ctqt_ret = 1;
static int g_mock_raised_sig = 0;

static HMODULE WINAPI mock_get_mod(LPCSTR name) {
  (void)name;
  return g_mock_mod_ret;
}

static FARPROC WINAPI mock_get_proc(HMODULE mod, LPCSTR name) {
  (void)mod;
  (void)name;
  return g_mock_proc_ret;
}

static void WINAPI mock_sys_time(LPFILETIME ft) {
  ft->dwLowDateTime = 0xD53E8000UL;
  ft->dwHighDateTime = 0x019DB1DEUL;
}

static DWORD WINAPI mock_get_tz(LPTIME_ZONE_INFORMATION tzi) {
  memset(tzi, 0, sizeof(*tzi));
  tzi->Bias = -60;
  return g_mock_tz_ret;
}

static HANDLE WINAPI mock_create_file_ok(LPCSTR fn, DWORD acc, DWORD share,
                                         LPSECURITY_ATTRIBUTES sa, DWORD disp,
                                         DWORD flags, HANDLE tmpl) {
  (void)fn;
  (void)acc;
  (void)share;
  (void)sa;
  (void)disp;
  (void)flags;
  (void)tmpl;
  return (HANDLE)(size_t)0x1234;
}

static BOOL WINAPI mock_set_file_time(HANDLE h, const FILETIME *c,
                                      const FILETIME *a, const FILETIME *w) {
  (void)h;
  (void)c;
  (void)a;
  (void)w;
  return g_mock_sft_ret;
}

static BOOL WINAPI mock_close_handle(HANDLE h) {
  (void)h;
  return 1;
}

static BOOL WINAPI mock_create_tqt(PHANDLE phNewTimer, HANDLE TimerQueue,
                                   WAITORTIMERCALLBACK Callback,
                                   PVOID Parameter, DWORD DueTime, DWORD Period,
                                   ULONG Flags) {
  (void)TimerQueue;
  (void)DueTime;
  (void)Period;
  (void)Flags;
  if (!g_mock_ctqt_ret) {
    return 0;
  }
  *phNewTimer = (HANDLE)(size_t)0x7777;
  Callback(Parameter, 1);
  return 1;
}

static BOOL WINAPI mock_delete_tqt(HANDLE q, HANDLE t, HANDLE ev) {
  (void)q;
  (void)t;
  (void)ev;
  return 1;
}

static int mock_raise(int sig) {
  g_mock_raised_sig = sig;
  return 0;
}
#endif

TEST test_posix_time_get_info(void) {
  auto_win_msvc_error_t rc;
  int info = 0;

  rc = posix_time_get_info(NULL);
  ASSERT_EQ(AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT, rc);

  rc = posix_time_get_info(&info);
  ASSERT_EQ(AUTO_WIN_MSVC_SUCCESS, rc);
  ASSERT_EQ(1, info);

  ASSERT_EQ(0, dummy_posix_time());
  PASS();
}

TEST test_clock_gettime_and_settime(void) {
  struct timespec ts;
  int res;

  memset(&ts, 0, sizeof(ts));
  ASSERT_EQ(0, clock_gettime(CLOCK_REALTIME, &ts));
  ASSERT(ts.tv_sec > 0);

  memset(&ts, 0, sizeof(ts));
  ASSERT_EQ(0, clock_gettime(CLOCK_MONOTONIC, &ts));
  ASSERT_EQ(-1, clock_gettime(9999, &ts));
#if defined(_MSC_VER)
  ASSERT_EQ(-1, clock_gettime(CLOCK_REALTIME, NULL));
  ASSERT_EQ(-1, clock_settime(CLOCK_REALTIME, NULL));
#endif

  ts.tv_nsec = -1;
  ASSERT_EQ(-1, clock_settime(CLOCK_REALTIME, &ts));

  ts.tv_nsec = 2000000000L;
  ASSERT_EQ(-1, clock_settime(CLOCK_REALTIME, &ts));

  clock_gettime(CLOCK_REALTIME, &ts);
  res = clock_settime(9999, &ts);
#if defined(_WIN32)
  ASSERT_EQ(-1, res);
#else
  ASSERT(res == -1 || res == 0);
#endif

  clock_gettime(CLOCK_REALTIME, &ts);
  res = clock_settime(CLOCK_REALTIME, &ts);
  ASSERT(res == -1 || res == 0);

  PASS();
}

TEST test_gettimeofday_nanosleep_tm(void) {
  struct timeval tv;
  struct timezone tz;
  struct timespec req;
  struct timespec rem;
  time_t now;
  struct tm res_tm;

  memset(&tv, 0, sizeof(tv));
  memset(&tz, 0, sizeof(tz));
  ASSERT_EQ(0, gettimeofday(&tv, &tz));
  ASSERT(tv.tv_sec > 0);
  ASSERT_EQ(0, gettimeofday(&tv, NULL));

#if defined(_WIN32) || defined(_MSC_VER)
  ASSERT_EQ(0, gettimeofday(NULL, &tz));
  ASSERT_EQ(0, gettimeofday(NULL, NULL));
#endif

  req.tv_sec = 0;
  req.tv_nsec = 1000000;
  memset(&rem, 0, sizeof(rem));
  ASSERT_EQ(0, nanosleep(&req, &rem));
  ASSERT_EQ(0, nanosleep(&req, NULL));
#if defined(_MSC_VER)
  ASSERT_EQ(-1, nanosleep(NULL, &rem));
#endif

  now = time(NULL);
  memset(&res_tm, 0, sizeof(res_tm));
  ASSERT_EQ(&res_tm, localtime_r(&now, &res_tm));
  ASSERT(res_tm.tm_year > 100);
  ASSERT_EQ(&res_tm, gmtime_r(&now, &res_tm));

#if defined(_WIN32) || defined(_MSC_VER)
  ASSERT_EQ(NULL, localtime_r(NULL, &res_tm));
  ASSERT_EQ(NULL, localtime_r(&now, NULL));
  ASSERT_EQ(NULL, gmtime_r(NULL, &res_tm));
  ASSERT_EQ(NULL, gmtime_r(&now, NULL));
#if defined(_MSC_VER)
  {
    time_t bad_time = (time_t)-172800;
    ASSERT_EQ(NULL, localtime_r(&bad_time, &res_tm));
    ASSERT_EQ(NULL, gmtime_r(&bad_time, &res_tm));
  }
#else
  posix_time_set_crt_tm_hooks(NULL, NULL);
  ASSERT_EQ(&res_tm, localtime_r(&now, &res_tm));
  ASSERT_EQ(&res_tm, gmtime_r(&now, &res_tm));
  posix_time_set_crt_tm_hooks(mock_tm_null, mock_tm_null);
  ASSERT_EQ(NULL, localtime_r(&now, &res_tm));
  ASSERT_EQ(NULL, gmtime_r(&now, &res_tm));
  posix_time_reset_hooks();
#endif
#endif

  PASS();
}

TEST test_getitimer_setitimer_utimes_macros(void) {
  struct itimerval itv;
  struct itimerval old_itv;
  struct timeval times[2];
  struct timeval t1;
  struct timeval t2;
  struct timeval tres;
  FILE *f;
  const char *test_path = "test_posix_time_tmp.txt";

  ASSERT_EQ(-1, getitimer(-1, &itv));
  ASSERT_EQ(-1, getitimer(99, &itv));
  ASSERT_EQ(-1, getitimer(ITIMER_REAL, NULL));
  ASSERT_EQ(0, getitimer(ITIMER_REAL, &itv));

  ASSERT_EQ(-1, setitimer(-1, &itv, NULL));
#if defined(_WIN32) || defined(_MSC_VER)
  ASSERT_EQ(-1, setitimer(99, &itv, NULL));
  ASSERT_EQ(-1, setitimer(ITIMER_REAL, NULL, NULL));
#endif

  memset(&itv, 0, sizeof(itv));
  itv.it_value.tv_sec = 60;
  ASSERT_EQ(0, setitimer(ITIMER_REAL, &itv, &old_itv));
  memset(&itv, 0, sizeof(itv));
  ASSERT_EQ(0, setitimer(ITIMER_REAL, &itv, &old_itv));
  ASSERT_EQ(0, setitimer(ITIMER_REAL, &itv, NULL));

#if defined(_WIN32) || defined(_MSC_VER)
  ASSERT_EQ(-1, utimes(NULL, NULL));
#endif
  ASSERT_EQ(-1, utimes("non_existent_file_12345.xyz", NULL));

#if defined(_MSC_VER)
  if (fopen_s(&f, test_path, "w") != 0) {
    f = NULL;
  }
#else
  f = fopen(test_path, "w");
#endif
  ASSERT(f != NULL);
  fputs("hello", f);
  fclose(f);

  ASSERT_EQ(0, utimes(test_path, NULL));
  times[0].tv_sec = 1000000;
  times[0].tv_usec = 0;
  times[1].tv_sec = 1000000;
  times[1].tv_usec = 0;
  ASSERT_EQ(0, utimes(test_path, times));
  remove(test_path);

  t1.tv_sec = 1;
  t1.tv_usec = 600000;
  t2.tv_sec = 2;
  t2.tv_usec = 700000;
  timeradd(&t1, &t2, &tres);
  ASSERT_EQ(4, (long)tres.tv_sec);
  ASSERT_EQ(300000L, (long)tres.tv_usec);

  timersub(&tres, &t1, &tres);
  ASSERT_EQ(2, (long)tres.tv_sec);
  ASSERT_EQ(700000L, (long)tres.tv_usec);
  ASSERT(timerisset(&tres));
  ASSERT(timercmp(&tres, &t1, >));
  ASSERT(!timercmp(&tres, &t1, <));
  timerclear(&tres);
  ASSERT(!timerisset(&tres));

  PASS();
}

TEST test_strptime_comprehensive(void) {
  struct tm tm_val;
  char *rest;

  memset(&tm_val, 0, sizeof(tm_val));
  rest = strptime("2026-11-05T14:30:45Z", "%Y-%m-%dT%H:%M:%SZ", &tm_val);
  ASSERT(rest != NULL);
  ASSERT_EQ('\0', *rest);
  ASSERT_EQ(2026 - 1900, tm_val.tm_year);
  ASSERT_EQ(10, tm_val.tm_mon);
  ASSERT_EQ(5, tm_val.tm_mday);

#if defined(_WIN32) || defined(_MSC_VER)
  /* NULL input validation */
  ASSERT_EQ(NULL, strptime(NULL, "%Y", &tm_val));
  ASSERT_EQ(NULL, strptime("2026", NULL, &tm_val));
  ASSERT_EQ(NULL, strptime("2026", "%Y", NULL));

  /* Whitespace, literal, %%, %n, %t, %E/%O modifiers, %y < 69 and >= 69 */
  {
    char tab_buf[32];
    memcpy(tab_buf, "X26 %X 85 11", 13);
    tab_buf[0] = (char)9;
    tab_buf[5] = (char)9;
    rest = strptime(tab_buf, "%Ey %%%n%t%Oy %Om", &tm_val);
  }
  ASSERT(rest != NULL);
  ASSERT_EQ(85, tm_val.tm_year);
  rest = strptime(" 26", "%y", &tm_val);
  ASSERT(rest != NULL);
  ASSERT_EQ(126, tm_val.tm_year);

  /* %e, %I (12 -> 0 and 05 -> 5), %j, %w, %F, %T, %R, %D, %Z, %z variants */
  rest = strptime(" 5 12 05 365 4", "%e %I %I %j %w", &tm_val);
  ASSERT(rest != NULL);
  ASSERT_EQ(5, tm_val.tm_hour);
  ASSERT_EQ(364, tm_val.tm_yday);
  ASSERT_EQ(4, tm_val.tm_wday);

  rest = strptime("12", "%I", &tm_val);
  ASSERT(rest != NULL);
  ASSERT_EQ(0, tm_val.tm_hour);

  rest = strptime("2026-11-17 14:30:60 09:15 11/17/26", "%F %T %R %D", &tm_val);
  ASSERT(rest != NULL);

  rest = strptime("UTC1", "%Z", &tm_val);
  ASSERT(rest != NULL);
  ASSERT_STR_EQ("1", rest);

  ASSERT(strptime("Z", "%z", &tm_val) != NULL);
  ASSERT(strptime("z", "%z", &tm_val) != NULL);
  ASSERT(strptime("+02:30", "%z", &tm_val) != NULL);
  ASSERT(strptime("-0500", "%z", &tm_val) != NULL);

  /* Stop at max_digits, *s < '0', *s > '9', and all failure branches */
  rest = strptime("20269", "%Y", &tm_val);
  ASSERT(rest != NULL);
  ASSERT_STR_EQ("9", rest);
  ASSERT_EQ(NULL, strptime("2026/01", "%Y-%m", &tm_val));
  ASSERT_EQ(NULL, strptime("x", "%%", &tm_val));
  ASSERT_EQ(NULL, strptime("abcd", "%Y", &tm_val));
  ASSERT_EQ(NULL, strptime("xx", "%y", &tm_val));
  ASSERT_EQ(NULL, strptime("00", "%m", &tm_val));
  ASSERT_EQ(NULL, strptime("13", "%m", &tm_val));
  ASSERT_EQ(NULL, strptime("32", "%d", &tm_val));
  ASSERT_EQ(NULL, strptime("24", "%H", &tm_val));
  ASSERT_EQ(NULL, strptime("00", "%I", &tm_val));
  ASSERT_EQ(NULL, strptime("60", "%M", &tm_val));
  ASSERT_EQ(NULL, strptime("61", "%S", &tm_val));
  ASSERT_EQ(NULL, strptime("000", "%j", &tm_val));
  ASSERT_EQ(NULL, strptime("7", "%w", &tm_val));
  ASSERT_EQ(NULL, strptime("2026-99-01", "%F", &tm_val));
  ASSERT_EQ(NULL, strptime("25:00:00", "%T", &tm_val));
  ASSERT_EQ(NULL, strptime("12:99", "%R", &tm_val));
  ASSERT_EQ(NULL, strptime("13/01/26", "%D", &tm_val));
  ASSERT_EQ(NULL, strptime("+25:00", "%z", &tm_val));
  ASSERT_EQ(NULL, strptime("+02:99", "%z", &tm_val));
  ASSERT_EQ(NULL, strptime("X0200", "%z", &tm_val));
  ASSERT_EQ(NULL, strptime("2026", "%Q", &tm_val));
#endif

  PASS();
}

#ifdef _WIN32
TEST test_posix_time_win32_hooks_branches(void) {
  struct timeval tv;
  struct timezone tz;
  struct itimerval itv;

  /* Default NULL fallbacks + real timer arm/disarm for default_* coverage */
  posix_time_set_win32_hooks(NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
                             NULL, NULL);
  memset(&itv, 0, sizeof(itv));
  itv.it_value.tv_sec = 60;
  ASSERT_EQ(0, setitimer(ITIMER_REAL, &itv, NULL));
  memset(&itv, 0, sizeof(itv));
  ASSERT_EQ(0, setitimer(ITIMER_REAL, &itv, NULL));
#if !defined(_MSC_VER)
  posix_time_set_win32_hooks(NULL, NULL, NULL, NULL, NULL, NULL, NULL,
                             mock_create_tqt, mock_delete_tqt, NULL);
  g_mock_ctqt_ret = 1;
  itv.it_value.tv_sec = 1;
  ASSERT_EQ(0, setitimer(ITIMER_REAL, &itv, NULL));
  memset(&itv, 0, sizeof(itv));
  ASSERT_EQ(0, setitimer(ITIMER_REAL, &itv, NULL));
  posix_time_reset_hooks();
#endif

  /* 1. GetModuleHandleA == NULL -> GetSystemTimeAsFileTime + STANDARD tz */
  g_mock_mod_ret = NULL;
  g_mock_proc_ret = NULL;
  g_mock_tz_ret = TIME_ZONE_ID_STANDARD;
  posix_time_set_win32_hooks(mock_get_mod, mock_get_proc, mock_sys_time,
                             mock_get_tz, mock_create_file_ok,
                             mock_set_file_time, mock_close_handle,
                             mock_create_tqt, mock_delete_tqt, mock_raise);
  ASSERT_EQ(0, gettimeofday(&tv, &tz));
  ASSERT_EQ(0, tz.tz_dsttime);
  /* Second call hits g_time_precise_init == 1 */
  g_mock_tz_ret = TIME_ZONE_ID_DAYLIGHT;
  ASSERT_EQ(0, gettimeofday(&tv, &tz));
  ASSERT_EQ(1, tz.tz_dsttime);

  /* 2. GetModuleHandleA != NULL, GetProcAddress == NULL and != NULL */
  g_mock_mod_ret = (HMODULE)(size_t)0x1000;
  g_mock_proc_ret = NULL;
  posix_time_set_win32_hooks(mock_get_mod, mock_get_proc, mock_sys_time,
                             mock_get_tz, mock_create_file_ok,
                             mock_set_file_time, mock_close_handle,
                             mock_create_tqt, mock_delete_tqt, mock_raise);
  ASSERT_EQ(0, gettimeofday(&tv, NULL));

  g_mock_proc_ret = (FARPROC)(size_t)mock_sys_time;
  posix_time_set_win32_hooks(mock_get_mod, mock_get_proc, mock_sys_time,
                             mock_get_tz, mock_create_file_ok,
                             mock_set_file_time, mock_close_handle,
                             mock_create_tqt, mock_delete_tqt, mock_raise);
  ASSERT_EQ(0, gettimeofday(&tv, NULL));

  /* 3. SetFileTime returning FALSE */
  g_mock_sft_ret = 0;
  ASSERT_EQ(-1, utimes("dummy.txt", NULL));

  /* 4. setitimer arming (tv_sec > 0), re-arming (tv_usec > 0, deletes old),
   * callback invocation, and CreateTimerQueueTimer failure */
  memset(&itv, 0, sizeof(itv));
  itv.it_value.tv_sec = 1;
  itv.it_value.tv_usec = 0;
  g_mock_ctqt_ret = 1;
  g_mock_raised_sig = 0;
  ASSERT_EQ(0, setitimer(ITIMER_REAL, &itv, NULL));
  ASSERT_EQ(14, g_mock_raised_sig);

  itv.it_value.tv_sec = 0;
  itv.it_value.tv_usec = 5000;
  posix_time_set_win32_hooks(mock_get_mod, mock_get_proc, mock_sys_time,
                             mock_get_tz, mock_create_file_ok,
                             mock_set_file_time, mock_close_handle,
                             mock_create_tqt, mock_delete_tqt, NULL);
  ASSERT_EQ(0, setitimer(ITIMER_REAL, &itv, NULL));

  /* Disarm deletes existing timer */
  memset(&itv, 0, sizeof(itv));
  ASSERT_EQ(0, setitimer(ITIMER_REAL, &itv, NULL));

  /* Failure in CreateTimerQueueTimer */
  itv.it_value.tv_sec = 1;
  g_mock_ctqt_ret = 0;
  ASSERT_EQ(-1, setitimer(ITIMER_REAL, &itv, NULL));

  posix_time_reset_hooks();
  PASS();
}
#endif

SUITE(suite_posix_time_core) {
  RUN_TEST(test_posix_time_get_info);
  RUN_TEST(test_clock_gettime_and_settime);
  RUN_TEST(test_gettimeofday_nanosleep_tm);
  RUN_TEST(test_getitimer_setitimer_utimes_macros);
  RUN_TEST(test_strptime_comprehensive);
#ifdef _WIN32
  RUN_TEST(test_posix_time_win32_hooks_branches);
#endif
}
