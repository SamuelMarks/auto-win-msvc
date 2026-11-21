/**
 * @file test_core_coverage.c
 * @brief Comprehensive unit and branch coverage tests for posix-core.c.
 */

/* clang-format off */
#include "greatest.h"
#include "posix-core.h"
#include <errno.h>
#include <fcntl.h>
#include <setjmp.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if defined(_WIN32) && !defined(__CYGWIN__)
#include <direct.h>
#include <io.h>
#else
#include <unistd.h>
#endif
/* clang-format on */

/* --- Cross-platform common hook mocks --- */

#if !defined(_WIN32) && !defined(__DOS__) && !defined(__MSDOS__) &&            \
    !defined(__WATCOMC__)
static void test_copy_str(char *dst, size_t cap, const char *src) {
#if defined(_MSC_VER)
  strcpy_s(dst, cap, src);
#else
  (void)cap;
  strcpy(dst, src);
#endif
}
#endif

static int g_cmn_malloc_fail_at = -1;
static int g_cmn_malloc_calls = 0;
static int g_cmn_open_scratch_fail = 0;
static int g_cmn_fopen_fail = 0;
static int g_cmn_system_rc = 0;
static int g_cmn_use_system_mock = 0;
#if !defined(_WIN32) && !defined(__DOS__) && !defined(__MSDOS__) &&            \
    !defined(__WATCOMC__)
static int g_cmn_mkstemp_fail = 0;
#endif
static struct posix_core_common_hooks g_cmn_def;

static void *mock_cmn_malloc(size_t sz) {
  int idx = g_cmn_malloc_calls++;
  if (g_cmn_malloc_fail_at >= 0 && idx == g_cmn_malloc_fail_at) {
    return NULL;
  }
  return g_cmn_def.malloc_fn(sz);
}

static void mock_cmn_free(void *ptr) { g_cmn_def.free_fn(ptr); }

static int mock_cmn_open_scratch(const char *path) {
  if (g_cmn_open_scratch_fail) {
    errno = EACCES;
    return -1;
  }
  return g_cmn_def.open_scratch_fn(path);
}

static FILE *mock_cmn_fopen(const char *path, const char *mode) {
  if (g_cmn_fopen_fail) {
    errno = EACCES;
    return NULL;
  }
  return g_cmn_def.fopen_fn(path, mode);
}

static int mock_cmn_system(const char *cmd) {
  if (g_cmn_use_system_mock) {
    (void)cmd;
    return g_cmn_system_rc;
  }
  return g_cmn_def.system_fn(cmd);
}

#if !defined(_WIN32) && !defined(__DOS__) && !defined(__MSDOS__) &&            \
    !defined(__WATCOMC__)
static int mock_cmn_mkstemp(char *tmpl) {
  if (g_cmn_mkstemp_fail) {
    errno = EEXIST;
    return -1;
  }
  return g_cmn_def.mkstemp_fn(tmpl);
}
#endif

static void reset_common_mocks(void) {
  g_cmn_malloc_fail_at = -1;
  g_cmn_malloc_calls = 0;
  g_cmn_open_scratch_fail = 0;
  g_cmn_fopen_fail = 0;
  g_cmn_system_rc = 0;
  g_cmn_use_system_mock = 0;
#if !defined(_WIN32) && !defined(__DOS__) && !defined(__MSDOS__) &&            \
    !defined(__WATCOMC__)
  g_cmn_mkstemp_fail = 0;
#endif
  posix_core_set_common_hooks(NULL);
}

TEST test_core_common_hooks_and_scratch(void) {
  struct posix_core_common_hooks h;
  char buf[64];
  FILE *fp;
  FILE *slots[8];
  int i = 0;

  posix_core_get_default_common_hooks(NULL);
  posix_core_get_default_common_hooks(&g_cmn_def);
  h = g_cmn_def;
  h.malloc_fn = mock_cmn_malloc;
  h.free_fn = mock_cmn_free;
  h.open_scratch_fn = mock_cmn_open_scratch;
  h.fopen_fn = mock_cmn_fopen;
  h.system_fn = mock_cmn_system;
#if !defined(_WIN32) && !defined(__DOS__) && !defined(__MSDOS__) &&            \
    !defined(__WATCOMC__)
  h.mkstemp_fn = mock_cmn_mkstemp;
#endif

  /* Cover default_common_open_scratch failure path */
  ASSERT_EQ(-1, g_cmn_def.open_scratch_fn(
                    "/nonexistent_dir_awm_cov_999/scratch.tmp"));

  /* Cover awm_dos_mkstemp_format_83 all branches */
  ASSERT_EQ(-1, awm_dos_mkstemp_format_83(NULL, sizeof(buf), "aXXXXXX", 0));
  ASSERT_EQ(-1, awm_dos_mkstemp_format_83(buf, sizeof(buf), NULL, 0));
  ASSERT_EQ(-1, awm_dos_mkstemp_format_83(buf, sizeof(buf), "short", 0));
  ASSERT_EQ(-1, awm_dos_mkstemp_format_83(buf, 7, "aXXXXXX", 0));
  ASSERT_EQ(-1, awm_dos_mkstemp_format_83(buf, sizeof(buf), "abcdefg", 0));

  /* Leading ./ and .\ with and without inner slashes */
  ASSERT_EQ(0, awm_dos_mkstemp_format_83(buf, sizeof(buf), "./XXXXXX", 1));
  ASSERT_EQ(0, awm_dos_mkstemp_format_83(buf, sizeof(buf), ".\\XXXXXX", 2));
  ASSERT_EQ(0, awm_dos_mkstemp_format_83(buf, sizeof(buf), ".aXXXXXX", 2));
  ASSERT_EQ(0, awm_dos_mkstemp_format_83(buf, sizeof(buf), "./sub/aXXXXXX", 3));
  ASSERT_EQ(0,
            awm_dos_mkstemp_format_83(buf, sizeof(buf), "./sub\\aXXXXXX", 4));
  ASSERT_EQ(0,
            awm_dos_mkstemp_format_83(buf, sizeof(buf), ".\\sub/aXXXXXX", 5));
  ASSERT_EQ(0,
            awm_dos_mkstemp_format_83(buf, sizeof(buf), ".\\sub\\aXXXXXX", 6));

  /* s1 vs s2 ordering: s2 only, s1 > s2, s2 > s1, neither */
  ASSERT_EQ(0, awm_dos_mkstemp_format_83(buf, sizeof(buf), "dir\\aXXXXXX", 7));
  ASSERT_EQ(0,
            awm_dos_mkstemp_format_83(buf, sizeof(buf), "d1\\d2/aXXXXXX", 8));
  ASSERT_EQ(0,
            awm_dos_mkstemp_format_83(buf, sizeof(buf), "d1/d2\\aXXXXXX", 9));
  ASSERT_EQ(0, awm_dos_mkstemp_format_83(buf, sizeof(buf), "XXXXXX", 10));

  /* prefix_len == 1 character classes: a-z, A-Z, 0-9, _, other (below/above) */
  ASSERT_EQ(0, awm_dos_mkstemp_format_83(buf, sizeof(buf), "aXXXXXX", 11));
  ASSERT_EQ(0, awm_dos_mkstemp_format_83(buf, sizeof(buf), "ZXXXXXX", 12));
  ASSERT_EQ(0, awm_dos_mkstemp_format_83(buf, sizeof(buf), "5XXXXXX", 13));
  ASSERT_EQ(0, awm_dos_mkstemp_format_83(buf, sizeof(buf), "_XXXXXX", 14));
  ASSERT_EQ(0, awm_dos_mkstemp_format_83(buf, sizeof(buf), "-XXXXXX", 15));
  ASSERT_EQ(0, awm_dos_mkstemp_format_83(buf, sizeof(buf), "{XXXXXX", 16));

  /* prefix_len == 2 and prefix_len > 3 with all char classes */
  ASSERT_EQ(0, awm_dos_mkstemp_format_83(buf, sizeof(buf), "aZXXXXXX", 17));
  ASSERT_EQ(0, awm_dos_mkstemp_format_83(buf, sizeof(buf), "5_-{XXXXXX", 18));
  ASSERT_EQ(0, awm_dos_mkstemp_format_83(buf, sizeof(buf), "-{5aXXXXXX", 19));

  /* Cover awm_popen_scratch and awm_pclose_scratch */
  posix_core_set_common_hooks(&h);
  g_cmn_use_system_mock = 1;
  g_cmn_system_rc = 0;

  /* Invalid arguments to awm_popen_scratch */
  ASSERT(awm_popen_scratch(NULL, "r") == NULL);
  ASSERT(awm_popen_scratch("cmd", NULL) == NULL);
  ASSERT(awm_popen_scratch("cmd", "x") == NULL);
  ASSERT(awm_popen_scratch("cmd", "rx") == NULL);
  ASSERT(awm_popen_scratch("cmd", "rbb") == NULL);
  ASSERT(awm_popen_scratch("cmd", "rtt") == NULL);
  ASSERT_EQ(-1, awm_pclose_scratch(NULL));

  /* open_scratch failure */
  g_cmn_open_scratch_fail = 1;
  ASSERT(awm_popen_scratch("cmd", "r") == NULL);
  g_cmn_open_scratch_fail = 0;

  /* Read mode: malloc fail, fopen fail, rb, rt, r */
  g_cmn_malloc_fail_at = 0;
  g_cmn_malloc_calls = 0;
  ASSERT(awm_popen_scratch("cmd", "r") == NULL);
  g_cmn_malloc_fail_at = -1;

  g_cmn_fopen_fail = 1;
  ASSERT(awm_popen_scratch("cmd", "rb") == NULL);
  g_cmn_fopen_fail = 0;

  fp = awm_popen_scratch("cmd", "rb");
  ASSERT(fp != NULL);
  ASSERT_EQ(0, awm_pclose_scratch(fp));

  fp = awm_popen_scratch("cmd", "rt");
  ASSERT(fp != NULL);
  ASSERT_EQ(0, awm_pclose_scratch(fp));

  /* Write mode: fopen fail, malloc fail, wb, wt, pclose malloc fail */
  g_cmn_fopen_fail = 1;
  ASSERT(awm_popen_scratch("cmd", "wb") == NULL);
  g_cmn_fopen_fail = 0;

  g_cmn_malloc_fail_at = 0;
  g_cmn_malloc_calls = 0;
  ASSERT(awm_popen_scratch("cmd", "w") == NULL);
  g_cmn_malloc_fail_at = -1;

  fp = awm_popen_scratch("cmd", "wb");
  ASSERT(fp != NULL);
  ASSERT_EQ(0, awm_pclose_scratch(fp));

  fp = awm_popen_scratch("cmd", "wt");
  ASSERT(fp != NULL);
  g_cmn_malloc_fail_at = 0;
  g_cmn_malloc_calls = 0;
  ASSERT_EQ(-1, awm_pclose_scratch(fp));
  g_cmn_malloc_fail_at = -1;

#if !defined(__DOS__) && !defined(__MSDOS__) && !defined(__WATCOMC__)
  /* Exhaust all 8 scratch slots -> EMFILE */
  for (i = 0; i < 8; i++) {
    slots[i] = awm_popen_scratch("cmd", "r");
    ASSERT(slots[i] != NULL);
  }
  ASSERT(awm_popen_scratch("cmd", "r") == NULL);
  ASSERT_EQ(EMFILE, errno);
  for (i = 0; i < 8; i++) {
    ASSERT_EQ(0, awm_pclose_scratch(slots[i]));
  }
#else
  (void)slots;
  (void)i;
#endif

  /* Unknown FILE* to awm_pclose_scratch */
  ASSERT_EQ(-1, awm_pclose_scratch(stdout));

  reset_common_mocks();
  PASS();
}

#if !defined(_WIN32) && !defined(__DOS__) && !defined(__MSDOS__) &&            \
    !defined(__WATCOMC__)
TEST test_core_posix_branches(void) {
  struct posix_core_common_hooks h;
  char tmpl_ok[64];
  char tmpl_bad[64];
  int fd;
#if (defined(__APPLE__) || !defined(__linux__)) && !defined(__CYGWIN__) &&     \
    !defined(AUTO_WIN_MSVC_MEGA_LIBRARY)
  char sh_name[] = "sh";
  char *argv[2];
  char *envp[1];
  argv[0] = sh_name;
  argv[1] = NULL;
  envp[0] = NULL;
  ASSERT_EQ(-1, fexecve(-1, argv, envp));
  ASSERT_EQ(-1, fexecve(0, NULL, envp));
  ASSERT_EQ(-1, fexecve(0, argv, NULL));
  errno = 0;
  ASSERT_EQ(-1, fexecve(0, argv, envp));
  ASSERT_EQ(ENOSYS, errno);
#endif

  posix_core_get_default_common_hooks(&g_cmn_def);
  h = g_cmn_def;
  h.mkstemp_fn = mock_cmn_mkstemp;
  posix_core_set_common_hooks(&h);

  /* posix_mkostemp: NULL, short, non-XXXXXX, mkstemp fail, O_CLOEXEC */
  ASSERT_EQ(-1, posix_mkostemp(NULL, 0));
  test_copy_str(tmpl_bad, sizeof(tmpl_bad), "abc");
  ASSERT_EQ(-1, posix_mkostemp(tmpl_bad, 0));
  test_copy_str(tmpl_bad, sizeof(tmpl_bad), "abcdef123456");
  ASSERT_EQ(-1, posix_mkostemp(tmpl_bad, 0));

  g_cmn_mkstemp_fail = 1;
  test_copy_str(tmpl_ok, sizeof(tmpl_ok), "awm_mkostemp_fail_XXXXXX");
  ASSERT_EQ(-1, posix_mkostemp(tmpl_ok, 0));
  g_cmn_mkstemp_fail = 0;

  test_copy_str(tmpl_ok, sizeof(tmpl_ok), "awm_mkostemp_ok_XXXXXX");
#ifdef O_CLOEXEC
  fd = posix_mkostemp(tmpl_ok, O_CLOEXEC);
#else
  fd = posix_mkostemp(tmpl_ok, 0);
#endif
  ASSERT(fd >= 0);
  close(fd);
  remove(tmpl_ok);

  reset_common_mocks();
  PASS();
}
#endif

SUITE(suite_posix_core_coverage) {
  RUN_TEST(test_core_common_hooks_and_scratch);
#if !defined(_WIN32) && !defined(__DOS__) && !defined(__MSDOS__) &&            \
    !defined(__WATCOMC__)
  RUN_TEST(test_core_posix_branches);
#endif
}
