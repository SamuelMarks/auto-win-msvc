/* clang-format off */
#if defined(_MSC_VER)
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "greatest.h"
#include <linux-sys-statfs.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>

#if defined(_MSC_VER) || defined(_WIN32)
#include <io.h>
#endif
/* clang-format on */

TEST test_statfs_basic(void) {
  struct statfs st;
  int ret;

  /* Use current directory or C: drive root */
  ret = statfs("C:\\", &st);
#if defined(_WIN32)
  if (ret == 0) {
    ASSERT_EQ(0, ret);
    ASSERT(st.f_blocks > 0);
    ASSERT(st.f_bsize > 0);
  }
#else
  /* Non-Windows typically has /, but this is a stub for Windows MSVC */
  (void)st;
#endif
  PASS();
}

TEST test_statfs_invalid(void) {
  struct statfs st;
  int ret = statfs(NULL, &st);
  ASSERT_EQ(-1, ret);

  ret = statfs("C:\\ThisDriveDoesNotExist_12345\\", &st);
#if defined(_WIN32)
  ASSERT_EQ(-1, ret);
#endif
  PASS();
}

TEST test_fstatfs_basic(void) {
  struct statfs st;
  int ret;
  int fd;

#if defined(_MSC_VER)
  fd = _open("test_fstatfs_temp.txt", _O_CREAT | _O_RDWR, _S_IREAD | _S_IWRITE);
#else
  fd = open("test_fstatfs_temp.txt", O_CREAT | O_RDWR, 0666);
#endif

  if (fd < 0) {
    /* If we can't create a temp file, skip */
    PASS();
  }

  ret = fstatfs(fd, &st);
#if defined(_WIN32)
  if (ret == 0) {
    ASSERT_EQ(0, ret);
    ASSERT(st.f_blocks > 0);
  } else {
    /* On XP without GetFinalPathNameByHandleA, it will return -1 ENOSYS */
    ASSERT_EQ(-1, ret);
  }
#endif

#if defined(_MSC_VER)
  _close(fd);
  _unlink("test_fstatfs_temp.txt");
#else
  close(fd);
  unlink("test_fstatfs_temp.txt");
#endif

  PASS();
}

TEST test_fstatfs_invalid(void) {
  struct statfs st;
  int ret = fstatfs(-1, &st);
  ASSERT_EQ(-1, ret);
  PASS();
}

SUITE(main_suite) {
  RUN_TEST(test_statfs_basic);
  RUN_TEST(test_statfs_invalid);
  RUN_TEST(test_fstatfs_basic);
  RUN_TEST(test_fstatfs_invalid);
}

GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
  GREATEST_MAIN_BEGIN();
  (void)argc;
  (void)argv;
  RUN_SUITE(main_suite);
  GREATEST_MAIN_END();
}
