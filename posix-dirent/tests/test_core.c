/* clang-format off */
#include "greatest.h"
#include "posix-dirent.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
#include <direct.h>
#if defined(_MSC_VER) && _MSC_VER >= 1900
#include <../ucrt/io.h>
#else
#include <io.h>
#endif
#if defined(__MINGW32__) || defined(__MINGW64__)
#include <stdint.h>
#endif
#elif defined(__APPLE__) && defined(_DARWIN_C_SOURCE)
#include <sys/sysctl.h>
#endif
/* clang-format on */

extern void posix_dirent_set_alloc_hooks(void *(*malloc_fn)(size_t),
                                         void *(*calloc_fn)(size_t, size_t),
                                         void *(*realloc_fn)(void *, size_t));
extern void posix_dirent_reset_hooks(void);

#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32) ||                 \
    !defined(__linux__)
static int g_malloc_calls = 0;
static int g_malloc_fail_at = 0;
static int g_malloc_fail_from = 0;
static int g_calloc_calls = 0;
static int g_calloc_fail_at = 0;
static int g_realloc_calls = 0;
static int g_realloc_fail_at = 0;

static void *mock_dir_malloc(size_t sz) {
  g_malloc_calls++;
  if (g_malloc_fail_at > 0 && g_malloc_calls == g_malloc_fail_at) {
    return NULL;
  }
  if (g_malloc_fail_from > 0 && g_malloc_calls >= g_malloc_fail_from) {
    return NULL;
  }
  return malloc(sz);
}

static void *mock_dir_calloc(size_t n, size_t sz) {
  g_calloc_calls++;
  if (g_calloc_fail_at > 0 && g_calloc_calls == g_calloc_fail_at) {
    return NULL;
  }
  return calloc(n, sz);
}

static void *mock_dir_realloc(void *p, size_t sz) {
  g_realloc_calls++;
  if (g_realloc_fail_at > 0 && g_realloc_calls == g_realloc_fail_at) {
    return NULL;
  }
  return realloc(p, sz);
}
#endif

#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
static int filter_non_dot(const struct dirent *entry) {
  return entry->d_name[0] != '.';
}

typedef void *TEST_DIR_HANDLE;
typedef unsigned long TEST_DIR_DWORD;
typedef int TEST_DIR_BOOL;

typedef struct tagTEST_DIR_PE32 {
  TEST_DIR_DWORD dwSize;
  TEST_DIR_DWORD cntUsage;
  TEST_DIR_DWORD th32ProcessID;
  uintptr_t th32DefaultHeapID;
  TEST_DIR_DWORD th32ModuleID;
  TEST_DIR_DWORD cntThreads;
  TEST_DIR_DWORD th32ParentProcessID;
  long pcPriClassBase;
  TEST_DIR_DWORD dwFlags;
  char szExeFile[260];
} TEST_DIR_PE32;

typedef TEST_DIR_HANDLE(__stdcall *test_snap_fn)(TEST_DIR_DWORD,
                                                 TEST_DIR_DWORD);
typedef TEST_DIR_BOOL(__stdcall *test_p32_fn)(TEST_DIR_HANDLE, TEST_DIR_PE32 *);
typedef TEST_DIR_BOOL(__stdcall *test_close_h_fn)(TEST_DIR_HANDLE);
typedef void *(__stdcall *test_get_mod_fn)(const char *);
typedef void *(__stdcall *test_get_proc_fn)(void *, const char *);
typedef void *(__stdcall *test_create_file_fn)(const char *, unsigned long,
                                               unsigned long, void *,
                                               unsigned long, unsigned long,
                                               void *);
typedef intptr_t (*test_get_osfh_fn)(int);
typedef int (*test_open_osfh_fn)(intptr_t, int);
typedef int (*test_crt_close_fn)(int);

extern void posix_dirent_set_win32_hooks(
    test_snap_fn snap_fn, test_p32_fn first_fn, test_p32_fn next_fn,
    test_close_h_fn close_h_fn, test_get_mod_fn mod_fn,
    test_get_proc_fn proc_fn, test_create_file_fn cf_fn,
    test_get_osfh_fn get_osfh_fn, test_open_osfh_fn open_osfh_fn,
    test_crt_close_fn crt_close_fn);

static TEST_DIR_HANDLE g_mock_snap = (TEST_DIR_HANDLE)(intptr_t)0x55;
static int g_mock_p32first_ret = 1;
static int g_mock_p32_mode = 0;
static int g_mock_p32_step = 0;
static intptr_t g_mock_osfh = (intptr_t)0x99;
static void *g_mock_mod = (void *)(intptr_t)0x1111;
static void *g_mock_proc = NULL;
static unsigned long g_mock_final_len = 0;
static const char *g_mock_final_path = ".";
static void *g_mock_cf_ret = (void *)(intptr_t)0x2222;

static TEST_DIR_HANDLE __stdcall mock_snap(TEST_DIR_DWORD f,
                                           TEST_DIR_DWORD pid) {
  (void)f;
  (void)pid;
  return g_mock_snap;
}

static TEST_DIR_BOOL __stdcall mock_p32first(TEST_DIR_HANDLE h,
                                             TEST_DIR_PE32 *pe) {
  (void)h;
  if (!g_mock_p32first_ret) {
    return 0;
  }
  memset(pe, 0, sizeof(*pe));
  pe->th32ProcessID = 0; /* PID 0 skipped in readdir while loop */
  return 1;
}

static TEST_DIR_BOOL __stdcall mock_p32next(TEST_DIR_HANDLE h,
                                            TEST_DIR_PE32 *pe) {
  (void)h;
  g_mock_p32_step++;
  if (g_mock_p32_mode == 1) {
    return 0; /* Fails immediately while skipping PID 0 */
  }
  if (g_mock_p32_step == 1) {
    pe->th32ProcessID = 42;
    return 1;
  }
  if (g_mock_p32_step == 2) {
    pe->th32ProcessID = 43;
    return 1;
  }
  return 0;
}

static TEST_DIR_BOOL __stdcall mock_close_h(TEST_DIR_HANDLE h) {
  (void)h;
  return 1;
}

static void *__stdcall mock_get_mod(const char *name) {
  (void)name;
  return g_mock_mod;
}

static unsigned long __stdcall
mock_final_path_fn(void *h, char *buf, unsigned long sz, unsigned long flags) {
  (void)h;
  (void)sz;
  (void)flags;
  if (g_mock_final_len > 0 && g_mock_final_len < sz && g_mock_final_path) {
    memcpy(buf, g_mock_final_path, strlen(g_mock_final_path) + 1);
  }
  return g_mock_final_len;
}

static void *__stdcall mock_get_proc(void *m, const char *name) {
  (void)m;
  (void)name;
  return g_mock_proc;
}

static void *__stdcall mock_create_file(const char *fn, unsigned long a,
                                        unsigned long s, void *sa,
                                        unsigned long d, unsigned long fl,
                                        void *t) {
  (void)fn;
  (void)a;
  (void)s;
  (void)sa;
  (void)d;
  (void)fl;
  (void)t;
  return g_mock_cf_ret;
}

static intptr_t mock_get_osfh(int fd) {
  (void)fd;
  return g_mock_osfh;
}

static int mock_open_osfh(intptr_t h, int fl) {
  (void)h;
  (void)fl;
  return 77;
}

static int mock_crt_close(int fd) {
  (void)fd;
  return 0;
}
#elif defined(__APPLE__) && defined(_DARWIN_C_SOURCE)
extern void posix_dirent_set_sysctl_hook(int (*sysctl_fn)(int *, u_int, void *,
                                                          size_t *, void *,
                                                          size_t));
static int g_sysctl_mode = 0;

static int mock_dirent_sysctl(int *name, u_int namelen, void *oldp,
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
      *oldlenp = 2 * sizeof(struct kinfo_proc);
      return 0;
    }
    memset(oldp, 0, 2 * sizeof(struct kinfo_proc));
    ((struct kinfo_proc *)oldp)[0].kp_proc.p_pid = 0;
    ((struct kinfo_proc *)oldp)[1].kp_proc.p_pid = 123;
    return 0;
  }
  return 0;
}
#endif

TEST test_posix_dirent_get_info(void) {
  int info = 0;
  ASSERT_EQ(AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT, posix_dirent_get_info(NULL));
  ASSERT_EQ(AUTO_WIN_MSVC_SUCCESS, posix_dirent_get_info(&info));
  ASSERT_EQ(1, info);
  PASS();
}

TEST test_posix_dirent_comprehensive(void) {
#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
  DIR *d;
  struct dirent *entry;
  struct dirent **namelist;
  int n;
  int i;
  int fd;
  char fname[64];
  const char *tmp_dir = "test_dirent_sub_tmp";

  /* Default hook NULL resets */
  posix_dirent_set_alloc_hooks(NULL, NULL, NULL);
  posix_dirent_set_win32_hooks(NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
                               NULL, NULL);

  /* Create temp subdir with 5 regular files so DT_DIR, DT_REG, and realloc are
   * all exercised deterministically */
  (void)_mkdir(tmp_dir);
  for (i = 0; i < 5; i++) {
    FILE *fp;
#if defined(_MSC_VER)
    sprintf_s(fname, sizeof(fname), "%s/file_%d.txt", tmp_dir, i);
    if (fopen_s(&fp, fname, "w") != 0) {
      fp = NULL;
    }
#else
    sprintf(fname, "%s/file_%d.txt", tmp_dir, i);
    fp = fopen(fname, "w");
#endif
    ASSERT(fp != NULL);
    fputs("x", fp);
    fclose(fp);
  }

  /* Null / empty / non-existent / slash variants */
  ASSERT_EQ(NULL, opendir(NULL));
  ASSERT_EQ(NULL, opendir(""));
  ASSERT_EQ(NULL, opendir("non_existent_directory_xyz_12345"));
  ASSERT_EQ(-1, closedir(NULL));
  ASSERT_EQ(NULL, readdir(NULL));
  ASSERT_EQ(-1, telldir(NULL));
  rewinddir(NULL);
  seekdir(NULL, 0);

  d = opendir("./");
  ASSERT(d != NULL);
  rewinddir(d);
  ASSERT_EQ(0, closedir(d));

  {
    static const char dot_bs[3] = {'.', 92, 0};
    d = opendir(dot_bs);
    ASSERT(d != NULL);
    rewinddir(d);
    ASSERT_EQ(0, closedir(d));
  }

  d = opendir(tmp_dir);
  ASSERT(d != NULL);
  n = 0;
  while ((entry = readdir(d)) != NULL) {
    ASSERT(entry->d_type == DT_DIR || entry->d_type == DT_REG);
    n++;
  }
  ASSERT(n >= 7);
  rewinddir(d);
  ASSERT_EQ(0L, telldir(d));
  seekdir(d, 2);
  ASSERT_EQ(2L, telldir(d));
  seekdir(d, 999999L); /* Hits readdir == NULL break */

  /* dirfd and fdopendir real path */
  ASSERT_EQ(-1, dirfd(NULL));
  fd = dirfd(d);
  ASSERT(fd >= 0);
  ASSERT_EQ(0, closedir(d));
  d = fdopendir(fd);
  ASSERT(d != NULL);
  ASSERT_EQ(0, closedir(d));

  /* Real /proc variants: /proc, /proc/, \proc, \proc\ */
  d = opendir("/proc");
  ASSERT(d != NULL);
  ASSERT(readdir(d) != NULL);
  (void)readdir(d);
  rewinddir(d);
  ASSERT_EQ(0, closedir(d));

  d = opendir("/proc/");
  ASSERT(d != NULL);
  ASSERT_EQ(0, closedir(d));

  {
    static const char bs_proc[6] = {92, 'p', 'r', 'o', 'c', 0};
    static const char bs_procs[7] = {92, 'p', 'r', 'o', 'c', 92, 0};
    d = opendir(bs_proc);
    ASSERT(d != NULL);
    ASSERT_EQ(0, closedir(d));

    d = opendir(bs_procs);
    ASSERT(d != NULL);
    ASSERT_EQ(0, closedir(d));
  }

  /* scandir & fdscandir normal + filter + NULL compar */
  ASSERT_EQ(-1, scandir(NULL, &namelist, NULL, alphasort));
  ASSERT_EQ(-1, scandir(tmp_dir, NULL, NULL, alphasort));
  ASSERT_EQ(-1, scandir("non_existent_998877", &namelist, NULL, alphasort));

  namelist = NULL;
  n = scandir(tmp_dir, &namelist, filter_non_dot, alphasort);
  ASSERT_EQ(5, n);
  for (i = 0; i < n; i++) {
    free(namelist[i]);
  }
  free(namelist);

  namelist = NULL;
  n = scandir(tmp_dir, &namelist, NULL, NULL);
  ASSERT(n >= 7);
  for (i = 0; i < n; i++) {
    free(namelist[i]);
  }
  free(namelist);

  ASSERT_EQ(-1, fdscandir(0, NULL, NULL, NULL));
  ASSERT_EQ(-1, fdscandir(-1, &namelist, NULL, NULL));
  d = opendir(tmp_dir);
  ASSERT(d != NULL);
  fd = dirfd(d);
  ASSERT_EQ(0, closedir(d));
  namelist = NULL;
  n = fdscandir(fd, &namelist, filter_non_dot, alphasort);
  ASSERT_EQ(5, n);
  for (i = 0; i < n; i++) {
    free(namelist[i]);
  }
  free(namelist);

  /* OOM tests across opendir, rewinddir, and scandir */
  posix_dirent_set_alloc_hooks(mock_dir_malloc, mock_dir_calloc,
                               mock_dir_realloc);

  /* opendir OOM at call 1 (dirp), call 2 (search_path), call 3 (dirp->name) */
  for (i = 1; i <= 3; i++) {
    g_malloc_calls = 0;
    g_malloc_fail_at = i;
    ASSERT_EQ(NULL, opendir(tmp_dir));
  }
  /* /proc opendir OOM on dirp->name (call 2) */
  g_malloc_calls = 0;
  g_malloc_fail_at = 2;
  ASSERT_EQ(NULL, opendir("/proc"));

  /* rewinddir OOM leaves handle == -1, exercising handle == -1 in second
   * rewinddir and closedir */
  g_malloc_fail_at = 0;
  d = opendir(tmp_dir);
  ASSERT(d != NULL);
  g_malloc_calls = 0;
  g_malloc_fail_from = 1;
  rewinddir(d);
  rewinddir(d);
  ASSERT_EQ(0, closedir(d));
  g_malloc_fail_from = 0;

  /* scandir OOM on names array (call 4), 1st entry copy (call 5), 2nd entry
   * copy (call 6 -> frees 1st), and realloc failure */
  for (i = 4; i <= 6; i++) {
    g_malloc_calls = 0;
    g_malloc_fail_at = i;
    g_realloc_fail_at = 0;
    ASSERT_EQ(-1, scandir(tmp_dir, &namelist, NULL, alphasort));
  }
  g_malloc_fail_at = 0;
  g_realloc_calls = 0;
  g_realloc_fail_at = 1;
  ASSERT_EQ(-1, scandir(tmp_dir, &namelist, NULL, alphasort));
  posix_dirent_reset_hooks();

  /* Mocked Win32 snapshot & fdopendir / dirfd edge cases */
  posix_dirent_set_win32_hooks(mock_snap, mock_p32first, mock_p32next,
                               mock_close_h, mock_get_mod, mock_get_proc,
                               mock_create_file, mock_get_osfh, mock_open_osfh,
                               mock_crt_close);

  /* Snapshot == INVALID_HANDLE_VALUE */
  g_mock_snap = (TEST_DIR_HANDLE)(intptr_t)(-1);
  ASSERT_EQ(NULL, opendir("/proc"));

  /* Process32First == 0 */
  g_mock_snap = (TEST_DIR_HANDLE)(intptr_t)0x55;
  g_mock_p32first_ret = 0;
  ASSERT_EQ(NULL, opendir("/proc"));

  /* /proc readdir: PID 0 skip -> Process32Next fails immediately */
  g_mock_p32first_ret = 1;
  g_mock_p32_mode = 1;
  g_mock_p32_step = 0;
  d = opendir("/proc");
  ASSERT(d != NULL);
  ASSERT_EQ(NULL, readdir(d));

  /* /proc readdir: PID 0 skip -> PID 42 -> PID 43 -> EOF; then failing snap */
  g_mock_p32_mode = 0;
  g_mock_p32_step = 0;
  rewinddir(d);
  entry = readdir(d);
  ASSERT(entry != NULL);
  ASSERT_STR_EQ("42", entry->d_name);
  entry = readdir(d);
  ASSERT(entry != NULL);
  ASSERT_STR_EQ("43", entry->d_name);
  ASSERT_EQ(NULL, readdir(d));
  g_mock_snap = (TEST_DIR_HANDLE)(intptr_t)(-1);
  rewinddir(d); /* snap_handle becomes INVALID_HANDLE_VALUE */
  rewinddir(d); /* exercises snap_handle == INVALID_HANDLE_VALUE */
  ASSERT_EQ(0, closedir(d));

  /* dirfd CreateFileA failure vs success */
  d = opendir(tmp_dir);
  ASSERT(d != NULL);
  g_mock_cf_ret = (void *)(intptr_t)(-1);
  ASSERT_EQ(-1, dirfd(d));
  g_mock_cf_ret = (void *)(intptr_t)0x2222;
  ASSERT_EQ(77, dirfd(d));
  ASSERT_EQ(0, closedir(d));

  /* fdopendir error & path prefix branches */
  g_mock_osfh = (intptr_t)(-1);
  ASSERT_EQ(NULL, fdopendir(3));
  g_mock_osfh = (intptr_t)(-2);
  ASSERT_EQ(NULL, fdopendir(3));

  g_mock_osfh = (intptr_t)0x99;
  g_mock_mod = NULL;
  ASSERT_EQ(NULL, fdopendir(3));

  g_mock_mod = (void *)(intptr_t)0x1111;
  g_mock_proc = NULL;
  ASSERT_EQ(NULL, fdopendir(3));

  *(test_get_proc_fn *)(&g_mock_proc) = NULL;
  g_mock_proc = (void *)(size_t)mock_final_path_fn;
  g_mock_final_len = 0;
  ASSERT_EQ(NULL, fdopendir(3));

  g_mock_final_len = 5000;
  ASSERT_EQ(NULL, fdopendir(3));

  /* Short valid path (< 4 chars, no extended prefix) */
  g_mock_final_path = ".";
  g_mock_final_len = 1;
  d = fdopendir(3);
  ASSERT(d != NULL);
  ASSERT_EQ(0, closedir(d));

  /* >= 4 chars prefix mismatch branches and non-existent target */
  {
    static const char p_drive[] = {'C', ':', 92,  'n', 'o',
                                   'n', 'e', '_', 'x', 0};
    static const char p_mm1[] = {92, 92, '?', 'X', 'n', 'o', 'n', 'e', 0};
    static const char p_mm2[] = {92, 92, 'X', 92, 'n', 'o', 'n', 'e', 0};
    static const char p_mm3[] = {92, 'X', '?', 92, 'n', 'o', 'n', 'e', 0};

    g_mock_final_path = p_drive;
    g_mock_final_len = (unsigned long)strlen(g_mock_final_path);
    ASSERT_EQ(NULL, fdopendir(3));

    g_mock_final_path = p_mm1;
    g_mock_final_len = (unsigned long)strlen(g_mock_final_path);
    ASSERT_EQ(NULL, fdopendir(3));

    g_mock_final_path = p_mm2;
    g_mock_final_len = (unsigned long)strlen(g_mock_final_path);
    ASSERT_EQ(NULL, fdopendir(3));

    g_mock_final_path = p_mm3;
    g_mock_final_len = (unsigned long)strlen(g_mock_final_path);
    ASSERT_EQ(NULL, fdopendir(3));
  }

  posix_dirent_reset_hooks();

  for (i = 0; i < 5; i++) {
#if defined(_MSC_VER)
    sprintf_s(fname, sizeof(fname), "%s/file_%d.txt", tmp_dir, i);
#else
    sprintf(fname, "%s/file_%d.txt", tmp_dir, i);
#endif
    remove(fname);
  }
  (void)_rmdir(tmp_dir);
#else
  DIR *d;
  struct dirent *entry;
  int count = 0;

  posix_dirent_set_alloc_hooks(NULL, NULL, NULL);
  posix_dirent_reset_hooks();

  ASSERT_EQ(NULL, posix_opendir(NULL));
  ASSERT_EQ(NULL, posix_readdir(NULL));
  ASSERT_EQ(-1, posix_closedir(NULL));
  posix_rewinddir(NULL);

  d = posix_opendir(".");
  ASSERT(d != NULL);
  entry = posix_readdir(d);
  ASSERT(entry != NULL);
  posix_rewinddir(d);
  ASSERT_EQ(0, posix_closedir(d));

  d = posix_opendir("/proc");
  ASSERT(d != NULL);
  while ((entry = posix_readdir(d)) != NULL) {
    ASSERT(entry->d_name[0] != '\0');
    count++;
  }
  ASSERT(count > 0);
  posix_rewinddir(d);
  ASSERT(posix_readdir(d) != NULL);
  ASSERT_EQ(0, posix_closedir(d));

#if !defined(__linux__)
  {
    DIR *handles[16];
    int i;
    /* "/proc/" trailing slash + exhaust 16 active proc slots */
    for (i = 0; i < 16; i++) {
      handles[i] = posix_opendir((i == 0) ? "/proc/" : "/proc");
      ASSERT(handles[i] != NULL);
    }
    /* 17th open finds slot == -1 -> falls back to opendir("/proc") == NULL */
    ASSERT_EQ(NULL, posix_opendir("/proc"));
    for (i = 0; i < 16; i++) {
      ASSERT_EQ(0, posix_closedir(handles[i]));
    }

    /* OOM on pd calloc */
    posix_dirent_set_alloc_hooks(mock_dir_malloc, mock_dir_calloc,
                                 mock_dir_realloc);
    g_calloc_calls = 0;
    g_calloc_fail_at = 1;
    ASSERT_EQ(NULL, posix_opendir("/proc"));
    g_calloc_fail_at = 0;

    /* OOM on ALL mallocs -> pd->pids stays NULL -> closedir handles NULL pids
     */
    g_malloc_calls = 0;
    g_malloc_fail_from = 1;
    d = posix_opendir("/proc");
    ASSERT(d != NULL);
    ASSERT(posix_readdir(d) != NULL); /* "." */
    ASSERT(posix_readdir(d) != NULL); /* ".." */
    ASSERT_EQ(NULL, posix_readdir(d));
    ASSERT_EQ(0, posix_closedir(d));
    g_malloc_fail_from = 0;

#if defined(__APPLE__) && defined(_DARWIN_C_SOURCE)
    /* OOM on 2nd malloc (pd->pids after sysctl), succeed on fallback malloc */
    g_malloc_calls = 0;
    g_malloc_fail_at = 2;
    d = posix_opendir("/proc");
    ASSERT(d != NULL);
    ASSERT_EQ(0, posix_closedir(d));
    posix_dirent_reset_hooks();

    /* sysctl hook modes: fail 1st, len==0, fail 2nd, pid<=0 + pid>0 */
    posix_dirent_set_sysctl_hook(NULL);
    posix_dirent_set_sysctl_hook(mock_dirent_sysctl);
    for (i = 1; i <= 4; i++) {
      g_sysctl_mode = i;
      d = posix_opendir("/proc");
      ASSERT(d != NULL);
      ASSERT_EQ(0, posix_closedir(d));
    }
    posix_dirent_reset_hooks();
#endif
  }
#endif
#endif
  PASS();
}

SUITE(suite_posix_dirent_core) {
  RUN_TEST(test_posix_dirent_get_info);
  RUN_TEST(test_posix_dirent_comprehensive);
}
