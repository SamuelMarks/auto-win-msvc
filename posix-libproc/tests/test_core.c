#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "posix-libproc.h"
#include <stdio.h>
#include <string.h>

#if !defined(_WIN32)
#include <unistd.h>
#endif
/* clang-format on */

TEST test_posix_libproc_get_info(void) {
  auto_win_msvc_error_t rc;
  int info;

  info = 0;
  rc = posix_libproc_get_info(NULL);
  ASSERT_EQ(AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT, rc);

  rc = posix_libproc_get_info(&info);
  ASSERT_EQ(AUTO_WIN_MSVC_SUCCESS, rc);
  ASSERT_EQ(1, info);

  PASS();
}

TEST test_proc_pidpath(void) {
  char path[1024];
  int ret;
  int my_pid;

#if defined(_WIN32)
  my_pid = 0;
#else
  my_pid = (int)getpid();
#endif

  ret = proc_pidpath(my_pid, NULL, 1024);
  ASSERT_EQ(0, ret);

  ret = proc_pidpath(my_pid, path, 0);
  ASSERT_EQ(0, ret);

  ret = proc_pidpath(my_pid, path, 1);
  ASSERT_EQ(0, ret);

  ret = proc_pidpath(99999999, path, (unsigned int)sizeof(path));
  ASSERT_EQ(0, ret);

  ret = proc_pidpath(my_pid, path, (unsigned int)sizeof(path));
#if defined(_WIN32) || defined(__APPLE__)
  ASSERT(ret > 0);
  ASSERT(strlen(path) > 0);
#else
  ASSERT(ret >= 0);
#endif

  PASS();
}

TEST test_posix_proc_enum(void) {
  void *cursor;
  struct posix_proc_entry entry;
  int count = 0;

  ASSERT_EQ(0, posix_proc_next(NULL, &entry));
  posix_proc_close(NULL);

  cursor = posix_proc_open();
  ASSERT(cursor != NULL);
  ASSERT_EQ(0, posix_proc_next(cursor, NULL));

  while (posix_proc_next(cursor, &entry)) {
    ASSERT(entry.pid > 0);
    ASSERT(entry.comm[0] != '\0');
    count++;
    if (count > 5) {
      break;
    }
  }
  ASSERT(count > 0);
  posix_proc_close(cursor);

  PASS();
}

SUITE(suite_posix_libproc_core) {
  RUN_TEST(test_posix_libproc_get_info);
  RUN_TEST(test_proc_pidpath);
  RUN_TEST(test_posix_proc_enum);
}
