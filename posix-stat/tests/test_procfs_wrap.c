#if !defined(_WIN32) && !defined(_MSC_VER) && !defined(__APPLE__)
#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif
#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE 700
#endif
#ifndef _DEFAULT_SOURCE
#define _DEFAULT_SOURCE 1
#endif
#endif
#if !defined(_WIN32)
#define AUTO_WIN_MSVC_EMULATE_PROCFS 1
#endif
/* clang-format off */
#include "posix-stat.h"
#include "greatest.h"
#include <string.h>
/* clang-format on */

TEST test_posix_stat_procfs_wrappers(void) {
#if !defined(_WIN32)
  struct stat st;

  memset(&st, 0, sizeof(st));
  ASSERT_EQ(0, stat(".", &st));
  ASSERT(S_ISDIR(st.st_mode));

  memset(&st, 0, sizeof(st));
  ASSERT_EQ(0, lstat(".", &st));
  ASSERT(S_ISDIR(st.st_mode));
#endif
  PASS();
}

SUITE(suite_posix_stat_procfs_wrap) {
  RUN_TEST(test_posix_stat_procfs_wrappers);
}
