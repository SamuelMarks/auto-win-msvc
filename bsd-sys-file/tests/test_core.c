#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "bsd-sys-file.h"
#include <errno.h>
#include <stdio.h>
/* clang-format on */

TEST test_bsd_sys_file_init(void) {
  auto_win_msvc_error_t rc;
  int status;

  status = 0;
  rc = bsd_sys_file_init(NULL);
  if (rc != AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT) {
    printf("Expected AUTO_WIN_MSVC_ERROR_INVALID_ARGUMENT, got %d\n", (int)rc);
    FAIL();
  }

  rc = bsd_sys_file_init(&status);
  if (rc != AUTO_WIN_MSVC_SUCCESS) {
    printf("bsd_sys_file_init failed with rc=%d\n", (int)rc);
    FAIL();
  }

  ASSERT_EQ(1, status);
  PASS();
}

TEST test_posix_flock(void) {
  FILE *tf;
  int fd;

  /* Invalid operation */
  ASSERT_EQ(-1, posix_flock(-1, 999));
  ASSERT_EQ(EINVAL, errno);

  /* Bad file descriptor */
  ASSERT_EQ(-1, posix_flock(-1, LOCK_SH));
  ASSERT_EQ(EBADF, errno);

  /* Real temporary file */
  tf = tmpfile();
  ASSERT(tf != NULL);
  fd = fileno(tf);
  ASSERT(fd >= 0);

  ASSERT_EQ(0, posix_flock(fd, LOCK_SH));
  ASSERT_EQ(0, posix_flock(fd, LOCK_UN));
  ASSERT_EQ(0, posix_flock(fd, LOCK_EX | LOCK_NB));
  ASSERT_EQ(0, posix_flock(fd, LOCK_UN));

  fclose(tf);
  PASS();
}

SUITE(suite_bsd_sys_file_core) {
  RUN_TEST(test_bsd_sys_file_init);
  RUN_TEST(test_posix_flock);
}
