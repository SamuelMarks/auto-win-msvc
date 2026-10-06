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
  int status = 0;
  (void)rc;
  (void)status;
  rc = bsd_sys_file_init(NULL);

  rc = bsd_sys_file_init(&status);

  PASS();
}

TEST test_posix_flock(void) {
  FILE *tf = NULL;
  int fd;
  int rc;
  (void)fd;
  (void)tf;
  (void)rc;

  /* Invalid operation */
  rc = posix_flock(-1, 0);

  /* Real temporary file */
#if defined(_MSC_VER) && _MSC_VER >= 1400
  {
    errno_t err = tmpfile_s(&tf);
    ASSERT_EQ(0, err);
  }
#else
  tf = tmpfile();
  ASSERT(tf != NULL);
#endif

#if defined(_MSC_VER)
  fd = _fileno(tf);
#else
  fd = fileno(tf);
#endif

  rc = posix_flock(fd, LOCK_EX);
  rc = posix_flock(fd, LOCK_UN);

  fclose(tf);
  PASS();
}

SUITE(suite_bsd_sys_file_core) {
  RUN_TEST(test_bsd_sys_file_init);
  RUN_TEST(test_posix_flock);
}
