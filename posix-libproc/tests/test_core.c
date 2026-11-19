/* clang-format off */
#include "greatest.h"
#include "posix-libproc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if !defined(_WIN32)
#include <unistd.h>
#if defined(__APPLE__) && defined(_DARWIN_C_SOURCE)
#include <sys/sysctl.h>
#endif
#endif
/* clang-format on */

extern void posix_libproc_set_alloc_hooks(void *(*malloc_fn)(size_t),
                                          void *(*calloc_fn)(size_t, size_t));
extern void posix_libproc_reset_hooks(void);

static int g_malloc_fail_at = 0;
static int g_malloc_calls = 0;
static int g_calloc_fail_at = 0;
static int g_calloc_calls = 0;

static void *mock_libproc_malloc(size_t sz) {
  g_malloc_calls++;
  if (g_malloc_fail_at > 0 && g_malloc_calls == g_malloc_fail_at) {
    return NULL;
  }
  return malloc(sz);
}

static void *mock_libproc_calloc(size_t n, size_t sz) {
  g_calloc_calls++;
  if (g_calloc_fail_at > 0 && g_calloc_calls == g_calloc_fail_at) {
    return NULL;
  }
  return calloc(n, sz);
}

#if defined(_WIN32) || defined(_WIN64)
#if defined(_WIN64)
typedef unsigned __int64 TEST_LIBPROC_UINTPTR;
#else
typedef unsigned long TEST_LIBPROC_UINTPTR;
#endif

typedef struct tagTEST_LIBPROC_PE32 {
  unsigned long dwSize;
  unsigned long cntUsage;
  unsigned long th32ProcessID;
  TEST_LIBPROC_UINTPTR th32DefaultHeapID;
  unsigned long th32ModuleID;
  unsigned long cntThreads;
  unsigned long th32ParentProcessID;
  long pcPriClassBase;
  unsigned long dwFlags;
  char szExeFile[260];
} TEST_LIBPROC_PE32;

__declspec(dllimport) unsigned long __stdcall GetCurrentProcessId(void);

extern void posix_libproc_set_win32_hooks(
    void *(*snap_fn)(unsigned long, unsigned long),
    int (*first_fn)(void *, TEST_LIBPROC_PE32 *),
    int (*next_fn)(void *, TEST_LIBPROC_PE32 *), int (*close_fn)(void *),
    unsigned long (*modname_fn)(void *, char *, unsigned long));

static void *g_mock_snap_ret = (void *)(size_t)0x55;
static int g_mock_first_ret = 1;
static int g_mock_next_idx = 0;
static unsigned long g_mock_modname_ret = 0;

static const char *const g_mock_exe_names[] = {
    "ab",
    "abcd.dll",
    "abcd.exZ",
    "abcd.eZe",
    "abcd.Zxe",
    "abcd.EXE",
    "C:?dir/app.exe",
    "C:/dir?tool.eXe",
    "only?slash.ExE",
    "only/fwd.exe",
    "very_long_executable_process_name_exceeding_thirty_two_chars.exe"};

static void *mock_snap(unsigned long flags, unsigned long pid) {
  (void)flags;
  (void)pid;
  return g_mock_snap_ret;
}

static int mock_first(void *snap, TEST_LIBPROC_PE32 *pe) {
  (void)snap;
  if (!g_mock_first_ret) {
    return 0;
  }
  memset(pe, 0, sizeof(*pe));
  pe->th32ProcessID = 0; /* System Idle Process skipped on first iteration */
#if defined(_MSC_VER)
  strcpy_s(pe->szExeFile, sizeof(pe->szExeFile), "System");
#else
  strcpy(pe->szExeFile, "System");
#endif
  return 1;
}

static int mock_next(void *snap, TEST_LIBPROC_PE32 *pe) {
  size_t total = sizeof(g_mock_exe_names) / sizeof(g_mock_exe_names[0]);
  (void)snap;
  if ((size_t)g_mock_next_idx >= total) {
    return 0;
  }
  memset(pe, 0, sizeof(*pe));
  pe->th32ProcessID = (unsigned long)(100 + g_mock_next_idx);
  pe->th32ParentProcessID = 1UL;
#if defined(_MSC_VER)
  strcpy_s(pe->szExeFile, sizeof(pe->szExeFile),
           g_mock_exe_names[g_mock_next_idx]);
#else
  strcpy(pe->szExeFile, g_mock_exe_names[g_mock_next_idx]);
#endif
  {
    char *q = pe->szExeFile;
    while (*q) {
      if (*q == '?') {
        *q = (char)92;
      }
      q++;
    }
  }
  g_mock_next_idx++;
  return 1;
}

static int mock_close_h(void *h) {
  (void)h;
  return 1;
}

static unsigned long mock_modname(void *hMod, char *buf, unsigned long sz) {
  (void)hMod;
  (void)buf;
  (void)sz;
  return g_mock_modname_ret;
}
#elif defined(__APPLE__) && defined(_DARWIN_C_SOURCE)
extern void posix_libproc_set_sysctl_hook(int (*sysctl_fn)(int *, unsigned int,
                                                           void *, size_t *,
                                                           void *, size_t));
static int g_sysctl_mode = 0;

static int mock_sysctl(int *name, unsigned int namelen, void *oldp,
                       size_t *oldlenp, void *newp, size_t newlen) {
  (void)name;
  (void)namelen;
  (void)newp;
  (void)newlen;
  if (g_sysctl_mode == 1) {
    return -1;
  }
  if (g_sysctl_mode == 2) {
    *oldlenp = 0;
    return 0;
  }
  if (g_sysctl_mode == 3) {
    if (oldp == NULL) {
      *oldlenp = sizeof(struct kinfo_proc);
      return 0;
    }
    return -1;
  }
  if (g_sysctl_mode == 4) {
    if (oldp == NULL) {
      *oldlenp = sizeof(struct kinfo_proc);
      return 0;
    }
    memset(oldp, 0, sizeof(struct kinfo_proc));
    ((struct kinfo_proc *)oldp)[0].kp_proc.p_pid = 0; /* <= 0 skipped */
    return 0;
  }
  return 0;
}
#endif

TEST test_posix_libproc_get_info(void) {
  int available = 0;
  ASSERT_EQ(AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT, posix_libproc_get_info(NULL));
  ASSERT_EQ(AUTO_WIN_MSVC_SUCCESS, posix_libproc_get_info(&available));
  ASSERT_EQ(1, available);
  PASS();
}

TEST test_proc_pidpath(void) {
  char buf[1024];
  int ret;

  ret = proc_pidpath(0, NULL, sizeof(buf));
  ASSERT_EQ(0, ret);

  ret = proc_pidpath(0, buf, 0);
  ASSERT_EQ(0, ret);

  ret = proc_pidpath(-1, buf, sizeof(buf));
  ASSERT_EQ(0, ret);

#if defined(_WIN32)
  {
    int cur = (int)GetCurrentProcessId();
    ret = proc_pidpath(0, buf, sizeof(buf));
    ASSERT(ret > 0);
    ASSERT(strlen(buf) > 0);

    ret = proc_pidpath(cur, buf, sizeof(buf));
    ASSERT(ret > 0);

    /* Non-matching PID */
    ret = proc_pidpath(cur + 99999, buf, sizeof(buf));
    ASSERT_EQ(0, ret);

    /* Truncated buffer (len >= buffersize) */
    ret = proc_pidpath(0, buf, 2);
    ASSERT_EQ(0, ret);

    /* Mock GetModuleFileNameA returning 0 */
    posix_libproc_set_win32_hooks(mock_snap, mock_first, mock_next,
                                  mock_close_h, mock_modname);
    g_mock_modname_ret = 0;
    ASSERT_EQ(0, proc_pidpath(0, buf, sizeof(buf)));
    posix_libproc_reset_hooks();
  }
#elif defined(__APPLE__)
  ret = proc_pidpath((int)getpid(), buf, sizeof(buf));
  ASSERT(ret > 0);
#else
  ret = proc_pidpath((int)getpid(), buf, sizeof(buf));
  ASSERT(ret >= 0);
#endif
  PASS();
}

TEST test_posix_proc_enum(void) {
  void *cursor;
  struct posix_proc_entry pe;
  int count = 0;

  ASSERT_EQ(0, posix_proc_next(NULL, &pe));
  posix_proc_close(NULL);

  cursor = posix_proc_open();
  ASSERT(cursor != NULL);
  ASSERT_EQ(0, posix_proc_next(cursor, NULL));
  while (posix_proc_next(cursor, &pe)) {
    ASSERT(pe.pid > 0);
    count++;
    if (count > 5000) {
      break;
    }
  }
  ASSERT(count > 0);
  ASSERT_EQ(0, posix_proc_next(cursor, &pe));
  posix_proc_close(cursor);

  /* OOM on initial cursor malloc */
  posix_libproc_set_alloc_hooks(mock_libproc_malloc, mock_libproc_calloc);
  g_malloc_calls = 0;
  g_malloc_fail_at = 1;
  g_calloc_fail_at = 0;
  ASSERT_EQ(NULL, posix_proc_open());
  posix_libproc_reset_hooks();

#if defined(_WIN32) || defined(_WIN64)
  /* Snapshot failure */
  posix_libproc_set_win32_hooks(mock_snap, mock_first, mock_next, mock_close_h,
                                mock_modname);
  g_mock_snap_ret = (void *)(ptrdiff_t)(-1);
  ASSERT_EQ(NULL, posix_proc_open());

  /* Process32First failure */
  g_mock_snap_ret = (void *)(size_t)0x55;
  g_mock_first_ret = 0;
  ASSERT_EQ(NULL, posix_proc_open());

  /* Full mocked enumeration covering pid==0 skip, path slashes, and suffixes */
  g_mock_first_ret = 1;
  g_mock_next_idx = 0;
  cursor = posix_proc_open();
  ASSERT(cursor != NULL);
  count = 0;
  while (posix_proc_next(cursor, &pe)) {
    count++;
  }
  ASSERT_EQ(11, count);
  posix_proc_close(cursor);
  posix_libproc_reset_hooks();
#elif defined(__APPLE__) && defined(_DARWIN_C_SOURCE)
  /* OOM on kp malloc (2nd malloc call) -> falls back to busybox entry */
  posix_libproc_set_alloc_hooks(mock_libproc_malloc, mock_libproc_calloc);
  g_malloc_calls = 0;
  g_malloc_fail_at = 2;
  g_calloc_calls = 0;
  g_calloc_fail_at = 0;
  cursor = posix_proc_open();
  ASSERT(cursor != NULL);
  ASSERT_EQ(1, posix_proc_next(cursor, &pe));
  ASSERT_STR_EQ("busybox", pe.comm);
  posix_proc_close(cursor);

  /* OOM on kp malloc AND fallback calloc */
  g_malloc_calls = 0;
  g_malloc_fail_at = 2;
  g_calloc_calls = 0;
  g_calloc_fail_at = 1;
  ASSERT_EQ(NULL, posix_proc_open());

  /* OOM on entries calloc (1st calloc), succeed on fallback calloc (2nd calloc)
   */
  g_malloc_calls = 0;
  g_malloc_fail_at = 0;
  g_calloc_calls = 0;
  g_calloc_fail_at = 1;
  cursor = posix_proc_open();
  ASSERT(cursor != NULL);
  posix_proc_close(cursor);
  posix_libproc_reset_hooks();

  /* sysctl failure modes and pid <= 0 skipping */
  posix_libproc_set_sysctl_hook(mock_sysctl);
  g_sysctl_mode = 1;
  cursor = posix_proc_open();
  ASSERT(cursor != NULL);
  posix_proc_close(cursor);

  g_sysctl_mode = 2;
  cursor = posix_proc_open();
  ASSERT(cursor != NULL);
  posix_proc_close(cursor);

  g_sysctl_mode = 3;
  cursor = posix_proc_open();
  ASSERT(cursor != NULL);
  posix_proc_close(cursor);

  g_sysctl_mode = 4;
  cursor = posix_proc_open();
  ASSERT(cursor != NULL);
  posix_proc_close(cursor);

  posix_libproc_reset_hooks();
#else
  /* Non-Apple POSIX fallback OOM on calloc */
  posix_libproc_set_alloc_hooks(mock_libproc_malloc, mock_libproc_calloc);
  g_malloc_calls = 0;
  g_malloc_fail_at = 0;
  g_calloc_calls = 0;
  g_calloc_fail_at = 1;
  ASSERT_EQ(NULL, posix_proc_open());
  posix_libproc_reset_hooks();
#endif

  PASS();
}

SUITE(suite_posix_libproc_core) {
  RUN_TEST(test_posix_libproc_get_info);
  RUN_TEST(test_proc_pidpath);
  RUN_TEST(test_posix_proc_enum);
}
