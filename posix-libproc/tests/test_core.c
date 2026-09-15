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
  enum posix_libproc_error_code rc;
  int info;

  info = 0;
  rc = posix_libproc_get_info(NULL);
  if (rc != POSIX_LIBPROC_ERROR_NULL_POINTER) {
    printf("Expected POSIX_LIBPROC_ERROR_NULL_POINTER, got %d\n", (int)rc);
    FAIL();
  }

  rc = posix_libproc_get_info(&info);
  if (rc != POSIX_LIBPROC_SUCCESS) {
    printf("posix_libproc_get_info failed with rc=%d\n", (int)rc);
    FAIL();
  }
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
  (void)ret;
#endif

  PASS();
}

SUITE(suite_posix_libproc_core) {
  RUN_TEST(test_posix_libproc_get_info);
  RUN_TEST(test_proc_pidpath);
}
