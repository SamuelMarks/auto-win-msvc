#ifdef _MSC_VER
#endif /* _MSC_VER */
/* clang-format off */
#include "greatest.h"
#include "bsd-sys-file.h"
#include <errno.h>
#include <stdio.h>
/* clang-format on */

TEST test_bsd_sys_file_init(void) {
  enum bsd_sys_file_error_code rc;
  int status;

  status = 0;
  rc = bsd_sys_file_init(NULL);
  if (rc != BSD_SYS_FILE_ERROR_NULL_POINTER) {
    printf("Expected BSD_SYS_FILE_ERROR_NULL_POINTER, got %d\n", (int)rc);
    FAIL();
  }

  rc = bsd_sys_file_init(&status);
  if (rc != BSD_SYS_FILE_SUCCESS) {
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
